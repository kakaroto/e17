
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <alsa/asoundlib.h>
#include <Evas.h>
#include <math.h>

#include "alsa_mixer.h"
#define MODULE
#include "e_mixer.h"

static int mixer_error(const char *fmt, ...)
{
	va_list va;
	int r;

	va_start(va,fmt);
	fprintf(stderr,"eVolume: ALSA mixer: ");
	r = vfprintf(stderr,fmt, va);
	fprintf(stderr,"\n");
	va_end(va);
	return r;
}


Mixer_System_Name* s_get_systems()
{
	snd_mixer_t *handle;
	snd_ctl_t *control;
	snd_ctl_card_info_t *hw_info;
	Mixer_Name* name;

	Mixer_System_Name* ret;

	int err;
	int i;
	char buf[1024];
	Evas_List* names_list = NULL;

	ret = calloc(sizeof(Mixer_System_Name), 1);
	ret->system = strdup("alsa");
	ret->name = strdup("ALSA");
	ret->system_id = mixer_system_get_id(ret->name);

	if((err = snd_mixer_open(&handle, 0)) < 0)
	{
		mixer_error("Cannot open mixer: %s", snd_strerror(err));
		return NULL;
	}
		
	snd_ctl_card_info_alloca(&hw_info);

	for(i = 0; i < 32; i ++)
	{
		sprintf(buf,"hw:%d", i);
		if((err = snd_mixer_attach(handle, buf)) < 0)
			break;

		if((err = snd_mixer_detach(handle, buf)) < 0)
		{
			snd_mixer_close(handle);
			break;
		}


		if ((err = snd_ctl_open(&control, buf, 0)) < 0 )
		{
			mixer_error("Cannot control %s: %s", buf, snd_strerror(err));
			continue;
		}

		if ((err = snd_ctl_card_info(control, hw_info)) < 0)
		{
			mixer_error("Cannot get hardware info %s: %s", buf, snd_strerror(err));
			snd_ctl_close(control);
			continue;
		}

		snd_ctl_close(control);

		name = calloc(1,sizeof(Mixer_Name));
		name->card = strdup(buf);
		name->real = strdup(snd_ctl_card_info_get_name(hw_info));
		name->mixer_id = mixer_get_id(name->real);
		name->system_name = ret;

		names_list = evas_list_append(names_list, name);
	}

	ret->mixer_names = names_list;

	return ret;
}

#if 0
static int
_alsamixer_event(snd_mixer_t * __UNUSED__ m, unsigned int __UNUSED__ sign, snd_mixer_elem_t *__UNUSED__ elem)
{
	//Mixer* mixer;

	/*
	mixer = (Mixer*) snd_mixer_get_callback_private(m);

	if(mixer && mixer->callback)
		 mixer->callback(mixer->callback_private);
		 */
	return 0;
}
#endif

static int convert_to_percent(int in, int min, int max)
{
	int ret;
	int range;

	range = max - min;

	if (range == 0)
		return 0;

	in -= min;
	ret = rint(((double) in / (double) range * 100));
	return ret;
}

static int convert_from_percent(int in, int min, int max)
{
	int ret;
	int range;

	range = max - min;
	if (range == 0)
		return 0;

	ret = rint((double) range * ((double) in * .01)) + min;
	return ret;
}
	
static int 
_alsamixer_handler(void* data, Ecore_Fd_Handler* handler)
{
	unsigned short revents;
	Mixer* mixer;
	Alsa_Mixer *amixer;

	mixer = data;

	handler = handler;

	amixer = ALSAMIXER(mixer->local);

	if (snd_mixer_poll_descriptors_revents(amixer->handle,
				amixer->fds, evas_list_count(amixer->handlers), &revents) >= 0)
	{
		if (!revents)
			revents = POLLIN;
		if (revents & POLLNVAL)
			mixer_error("snd_mixer_poll_descriptors (POLLNVAL)", 0);
		if (revents & POLLERR)
			mixer_error("snd_mixer_poll_descriptors (POLLERR)", 0);
		if (revents & POLLIN)
			snd_mixer_handle_events(amixer->handle);
	}
	else
		return 1;


			
	mixer->is_changed = 1;

//	fprintf(stderr,"revents = %x\n, POLLIN, POLLNVAL, POLLERR = %x, %x, %x\n",
//			revents, POLLIN, POLLNVAL, POLLERR);
	if(revents & POLLIN && mixer->callback && !amixer->update)
		mixer->callback(mixer->callback_private);

	return 1;
}

static int alsamixer_set_poll_handlers(Mixer *mixer)
{
	int count, err, i;
	Alsa_Mixer *amixer;

	amixer = ALSAMIXER(mixer->local);

	if((count = snd_mixer_poll_descriptors_count(amixer->handle)) < 0)
	{
		mixer_error("snd_mixer_poll_descriptors_count");
		return 0;
	}
	
	amixer->fds = calloc(1, sizeof(struct pollfd) * count);

	if((err = snd_mixer_poll_descriptors(amixer->handle,
					amixer->fds, count)) < 0)
	{
		mixer_error("snd_mixer_poll_descriptors");
		return 0;
	}

	if( err != count )
	{
		mixer_error("poll count != polled fds");
		return 0;
	}

	amixer->handlers = NULL;
	for(i = 0; i < count; i++)
	{
		Ecore_Fd_Handler* handler;

		handler = ecore_main_fd_handler_add(amixer->fds[i].fd, ECORE_FD_READ,
				_alsamixer_handler, mixer, NULL, NULL);

		if(handler)
			amixer->handlers = evas_list_append(amixer->handlers, handler);
	}

	return 1;
}

Mixer* m_open(Mixer *mixer, Mixer_System* ms, Mixer_Name *mixer_name)
{
	int err;
	char *card;
	snd_mixer_t* handle;
	snd_ctl_t *control;
	snd_ctl_card_info_t *hw_info;

	snd_mixer_selem_id_t *sid;
	snd_mixer_elem_t *elem;
	Alsa_Mixer *amixer;


	int count, i;

	
	snd_mixer_selem_id_alloca(&sid);
	snd_ctl_card_info_alloca(&hw_info);

	card = mixer_name->card;
	if ((err = snd_ctl_open(&control, card, 0)) < 0 )
	{
		mixer_error("Cannot control %s: %s", card, snd_strerror(err));
		return NULL;
	}

	if ((err = snd_ctl_card_info(control, hw_info)) < 0)
	{
		mixer_error("Cannot get hardware info %s: %s", card, snd_strerror(err));
		snd_ctl_close(control);
		return NULL;
	}

	snd_ctl_close(control);

	if((err = snd_mixer_open(&handle, 0)) < 0)
	{
		mixer_error("Cannot open mixer: %s", snd_strerror(err));
		return NULL;
	}

	if((err = snd_mixer_attach(handle, card)) < 0)
	{
		mixer_error("Cannot attach mixer: %s", snd_strerror(err));
		snd_mixer_close(handle);
		return NULL;
	}

	if((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0)
	{
		mixer_error("Cannot register: %s", snd_strerror(err));
		snd_mixer_close(handle);
		return NULL;
	}
	

	err = snd_mixer_load(handle);
	if(err < 0)
	{
		mixer_error("Cannot load mixer: %s", snd_strerror(err));
		snd_mixer_close(handle);
		return NULL;
	}

	mixer->local = amixer = calloc(1, sizeof(Alsa_Mixer));
	snd_mixer_set_callback_private(handle, mixer);

	mixer->name = mixer_name;

	if(mixer->name == NULL)
	{
		fprintf(stderr, "Mixer %s was not listed\n", card);
	}

	amixer->handle = handle;

	mixer->system = ms;

	mixer->elems = NULL;
	
	for(i = 0, elem = snd_mixer_first_elem(handle);
			elem;
			elem = snd_mixer_elem_next(elem))
	{
		snd_mixer_selem_get_id(elem, sid);
		if(!snd_mixer_selem_is_active(elem))
			continue;

		if(snd_mixer_selem_has_playback_volume(elem))
		{
			Mixer_Elem* melem;

			melem = calloc(1, sizeof(Mixer_Elem));


			melem->name = strdup(snd_mixer_selem_id_get_name(sid));
			melem->elem_id = mixer_elem_get_id(melem->name);
				
			snd_mixer_selem_id_malloc((snd_mixer_selem_id_t**)&melem->local);
			snd_mixer_selem_get_id(elem,melem->local);

			melem->mixer = mixer;

			mixer->elems = evas_list_append(mixer->elems, melem);
		}

	}

	alsamixer_set_poll_handlers(mixer);

	return mixer;
}


int m_close(Mixer* mixer)
{
	Evas_List *l;
	Alsa_Mixer *amixer;

	amixer = ALSAMIXER(mixer->local);

	/* Wait for thread that polls information */
	for(l = amixer->handlers; l; l = evas_list_next(l))
	{
		Ecore_Fd_Handler* handler;
		handler = evas_list_data(l);

		/* First free fd */
		close(ecore_main_fd_handler_fd_get(handler));

		ecore_main_fd_handler_del(handler);
	}
	evas_list_free(amixer->handlers);

	for(l = mixer->elems; l; l = evas_list_next(l))
	{
		Mixer_Elem* melem;

		melem = evas_list_data(l);

		free(melem->name);
		free(melem->local);
		free(melem);
	}

	evas_list_free(mixer->elems);

	snd_mixer_close(amixer->handle);
	free(amixer->fds);
	free(mixer);

	return 0;
}

	
int m_get_volume(Mixer_Elem* melem, int* left, int *right)
{
	long rvol, lvol;
	long min, max;
	int err;
	Mixer	*mixer;
	Alsa_Mixer *amixer;
	snd_mixer_elem_t* elem;

	mixer = melem->mixer;
	amixer = ALSAMIXER(mixer->local);
	snd_mixer_handle_events(amixer->handle);

	//fprintf(stderr,"mixer->is_changed = %d\n", mixer->is_changed);
	if(mixer->is_changed)
	{
		snd_mixer_free(amixer->handle);
		if((err = snd_mixer_load(amixer->handle)) < 0)
		{
			mixer_error("snd_mixer_load: %s\n", snd_strerror(err));
			return 0;
		}
		mixer->is_changed = 0;
	}
		
	elem = snd_mixer_find_selem(amixer->handle, melem->local);

	snd_mixer_selem_get_playback_volume(elem, 0, &lvol);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

	if(snd_mixer_selem_is_playback_mono(elem))
		rvol = lvol;
	else
		snd_mixer_selem_get_playback_volume(elem, 1, &rvol);

	*left = convert_to_percent(lvol, min, max);
	*right = convert_to_percent(rvol, min, max);
	
	return 1;
}


int m_set_volume(Mixer_Elem* melem, int left, int right)
{
	long rvol, lvol;
	long min, max;
	snd_mixer_elem_t* elem;
	Mixer	*mixer;
	Alsa_Mixer *amixer;

	mixer = melem->mixer;
	amixer = ALSAMIXER(mixer->local);

//	fprintf(stderr,"volume = (%d,%d)\n", left, right);
	amixer->update = 1;

	elem = snd_mixer_find_selem(amixer->handle, melem->local);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

	lvol = convert_from_percent(left, min, max);
	rvol = convert_from_percent(right, min, max);
//	fprintf(stderr,"cvolume = (%ld,%ld)\n", lvol, rvol);

	snd_mixer_selem_set_playback_volume(elem, 0, lvol);

	if(!snd_mixer_selem_is_playback_mono(elem))
		snd_mixer_selem_set_playback_volume(elem, 1, rvol);

	amixer->update = 0;

	return 1;
}

int m_get_mute(Mixer_Elem* melem)
{
	Mixer *mixer;
	Alsa_Mixer* amixer;
	int mute;
	snd_mixer_elem_t* elem;

	mixer = melem->mixer;
	amixer = ALSAMIXER(mixer->local);

	snd_mixer_handle_events(amixer->handle);

	elem = snd_mixer_find_selem(amixer->handle, melem->local);

	if (snd_mixer_selem_has_playback_switch_joined(elem))
		snd_mixer_selem_get_playback_switch(elem, 0, &mute);

	return mute;
}

int m_set_mute(Mixer_Elem* melem, int mute)
{
	Mixer *mixer;
	Alsa_Mixer* amixer;
	snd_mixer_elem_t* elem;

	mixer = melem->mixer;
	amixer = ALSAMIXER(mixer->local);

	snd_mixer_handle_events(amixer->handle);

	elem = snd_mixer_find_selem(amixer->handle, melem->local);

	if (snd_mixer_selem_has_playback_switch_joined(elem))
		snd_mixer_selem_set_playback_switch_all(elem, mute);

	return 1;
}
