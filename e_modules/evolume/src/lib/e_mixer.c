
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <alsa/asoundlib.h>
#include <Evas.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <dirent.h>

#include "e_mixer.h"
#include "config.h"

extern char* module_root;

#if 0
static int mixer_error(const char *fmt, ...)
{
	va_list va;
	int r;

	va_start(va,fmt);
	fprintf(stderr,"e_vol_module: ");
	r = vfprintf(stderr,fmt, va);
	fprintf(stderr,"\n");
	va_end(va);
	return r;
}
#endif

/* This function returns list with Mixer_System_Name */
Evas_List* mixer_system_get_list()
{
	Evas_List* l;
	DIR* d;
	struct dirent* de;
	char buf[1024];

	l = NULL;

	snprintf(buf, 1024, "%s/%s/systems/",
			module_root, MODULE_ARCH);

	d = opendir(buf);
	if(!d)
		return NULL;

	while((de = readdir(d)) != NULL)
	{
		char *ptr;
		ptr = de->d_name;

		if(ptr[0] == '.')
			continue;

		if(strncmp(ptr + strlen(ptr) - 3, ".so", 3) != 0)
			continue;


		if((ptr = strstr(ptr,"module_")))
		{
			void *handle;
			Mixer_System_Name* msn;
			Mixer_System_Name* (*s_get_systems)(void);
			
			snprintf(buf, 1024, "%s/%s/systems/%s",
					module_root, MODULE_ARCH, ptr);

			fprintf(stderr,"module_path = %s\n", buf);
			handle = dlopen(buf, RTLD_NOW);
			if(!handle)
				continue;

			s_get_systems = dlsym(handle, "s_get_systems");

			if(!s_get_systems)
			{
				dlclose(handle);
				continue;
			}

			msn = s_get_systems();

			/*
			fprintf(stderr,"msn = %p\n", msn);
			fprintf(stderr,"msn = { \"%s\", \"%s\", %p }\n",
					msn->system, msn->real, msn->mixer_names);
			*/

			dlclose(handle);

			l = evas_list_append(l, msn);
			
		}

		/*
		   char name[128];

		   strncpy(name, ptr + strlen("module_"), 128);
		   ms = calloc(sizeof(Mixer_System), 1);
		   ms->name = strdup(name);
		   mixer_system_load(ms);


		   mixer_system_unload(ms);
		   l = evas_list_append(l, ms);
		   */

	}

	closedir(d);

	return l;
}

int mixer_set_callback(Mixer* mixer, mixer_callback callback, void *data)
{
	if (!mixer)
		return 0;

	mixer->callback = callback;
	mixer->callback_private = data;

	return 1;
}

int mixer_system_load(Mixer_System* ms)
{
	Mixer_System_Name *msn;
	char buf[1024];
	msn = ms->name;

	if (ms->handle)
	{
		ms->ref ++;
		return 0;
	}

	snprintf(buf, 1024, "%s/%s/systems/module_%s.so",
			module_root, MODULE_ARCH, msn->system);

	ms->handle = dlopen(buf, RTLD_NOW);


	if (!ms->handle)
	{
		fprintf(stderr,"Cannot load mixer system: %s: %s\n", msn->system,
				dlerror());
		return 1;
	}

	ms->ref ++;
	return 0;
}

int mixer_system_unload(Mixer_System* ms)
{
	ms->ref --;

	if (ms->ref)
		return 0;

	dlclose(ms->handle);
	return 0;
}


Mixer* mixer_system_getentries( Mixer_System* ms, Mixer_System_Name *msn )
{
	Mixer* mixer;

	if(ms->handle == NULL)
		return NULL;

	mixer = calloc(1, sizeof(Mixer));

	mixer->system = ms;

	mixer->open = dlsym(ms->handle, "m_open");
	mixer->close = dlsym(ms->handle, "m_close");
	mixer->get_volume = dlsym(ms->handle, "m_get_volume");
	mixer->set_volume = dlsym(ms->handle, "m_set_volume");

	mixer->get_mute = dlsym(ms->handle, "m_get_mute");
	mixer->set_mute = dlsym(ms->handle, "m_set_mute");

	if (!mixer->open || !mixer->close ||
		!mixer->get_volume || !mixer->set_volume)
	{
		fprintf(stderr,"Cannot get mixer entry points: %s: %s\n",
				msn->system, dlerror());
		mixer_system_unload(ms);

		free(mixer);
		return NULL;
	}

	return mixer;
}

Mixer* mixer_open(Mixer_System** ms, Mixer_System_Name* msn,
		int mixer_id)
{
	Mixer* mixer;
	Mixer_Name *name;
	Evas_List *l;

	if(!ms || !msn)
		return NULL;

	if(*ms == NULL)
	{
//		fprintf(stderr,"Allocating new Mixer_System\n");
		*ms = calloc(sizeof(Mixer_System), 1);
		(*ms)->handle = NULL;
		(*ms)->name = msn;
	}


	mixer_system_load(*ms);

	mixer = mixer_system_getentries(*ms, msn);

//	fprintf(stderr,"mixer->system = %p, name = %s\n", mixer->system,
//			mixer->system->name->system);

	if (!mixer)
		return NULL;

	for(l = msn->mixer_names; l; l = evas_list_next(l))
	{
		name = evas_list_data(l);
		if(name->mixer_id == mixer_id)
			break;
	}

	if(!l)
		return NULL;

	return mixer->open(mixer, *ms, name);
}

int mixer_ref(Mixer* mixer)
{
	if(!mixer)
		return 0;

	mixer->ref++;
	fprintf(stderr,"mixer_ref = %d\n",mixer->ref);

	return mixer->ref;
}


int mixer_unref(Mixer* mixer)
{
	if(!mixer)
		return 0;

	mixer->ref--;
	fprintf(stderr,"mixer_ref = %d\n", mixer->ref);

	return mixer->ref;
}

int mixer_unref_close(Mixer* mixer)
{

	if(mixer_unref(mixer) <= 0)
	{
		mixer_close(mixer);
		return 0;
	}
	else
		return mixer->ref;
}

int mixer_close(Mixer* mixer)
{

	int ret;

	if (mixer->ref > 0)
		return 0;

	if (!mixer || !mixer->close)
		return 0;
	
	fprintf(stderr,"Mixer unloading: %s, %s\n",
			mixer->name->real, mixer->name->card);

	ret = mixer->close(mixer, mixer->system);

	mixer_system_unload(mixer->system);
	return ret;
}

int mixer_get_volume(Mixer_Elem* melem, int* left, int *right)
{
	Mixer	*mixer;

	mixer = melem->mixer;

	return mixer->get_volume(melem, left, right);
}

int mixer_set_volume(Mixer_Elem* melem, int left, int right)
{
	Mixer	*mixer;

	mixer = melem->mixer;

	return mixer->set_volume(melem, left, right);
}

int mixer_get_mute(Mixer_Elem* melem)
{
	Mixer* mixer;

	mixer = melem->mixer;
	if(mixer->get_mute)
		return mixer->get_mute(melem);
	else
		return -ENOSYS;
}

int mixer_set_mute(Mixer_Elem* melem, int mute)
{
	Mixer* mixer;

	mixer = melem->mixer;
	if(mixer->set_mute)
		return mixer->set_mute(melem, mute);
	else
		return -ENOSYS;
}

#define HASH_DEBUG
/* Code from evas/src/lib/data/evas_hash.c */
static int _mixer_hash(const char *key) 
{
	unsigned int hash_num = 0, i;
	const unsigned char *ptr;
	if (!key) return 0;
	for (i = 0, ptr = (unsigned char *)key;
			*ptr;
			ptr++, i++) hash_num ^= ((int)(*ptr) | ((int)(*ptr) << 8)) >> (i % 2);
	hash_num = (hash_num & 0xFF) + (hash_num >> 4 & 0xF0) + (hash_num >> 8 & 0x4F);
	hash_num = hash_num & 0xFF;

#ifdef HASH_DEBUG
	fprintf(stderr,"%s: key = %s, hash = %d\n", __FUNCTION__,
			key, hash_num);
#endif

	return (int)hash_num;
}

int mixer_system_get_id(const char *name)
{
	return _mixer_hash(name) << 16;
}

int mixer_get_id(const char *name)
{
	return _mixer_hash(name) << 8;
}

int mixer_elem_get_id(const char *name)
{
	return _mixer_hash(name);
}

