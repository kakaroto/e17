/*
 * $Id$
 */

#include <config.h>
#include <stdio.h>
#include <assert.h>
#include "../../plugin.h"

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

static snd_pcm_t *pcm = NULL;
static snd_mixer_t *mixer = NULL;
static snd_mixer_elem_t *master_ctrl = NULL;
static int frame_size = 0;

void alsa_shutdown() {
	if (pcm) {
		snd_pcm_close(pcm);
		pcm = NULL;
	}

	if (mixer) {
		snd_mixer_close(mixer);
		mixer = NULL;
	}
}

int alsa_configure(int channels, int rate, int bits) {
	snd_pcm_hw_params_t *params;
	snd_pcm_access_mask_t *access;
	int tmp;

	/* we need to close and open the device every time we're
	 * changing pcm settings
	 */
	if (pcm) {
		snd_pcm_close(pcm);
		pcm = NULL;
	}

	if (snd_pcm_open(&pcm, "default",
	                 SND_PCM_STREAM_PLAYBACK, 0))
		return 0;

	frame_size = bits * channels / 8;

	/* initialize stuff */
	snd_pcm_hw_params_alloca(&params);
	snd_pcm_access_mask_alloca(&access);

	/* get current params */
	snd_pcm_hw_params_any(pcm, params);

	/* set access rights */
	snd_pcm_access_mask_none(access);
	snd_pcm_access_mask_set(access, SND_PCM_ACCESS_RW_INTERLEAVED); 
	snd_pcm_hw_params_set_access_mask(pcm, params, access);

	/* set channels */
	snd_pcm_hw_params_set_channels(pcm, params, channels);

	/* set format */
	snd_pcm_hw_params_set_format(pcm, params,
	                             bits == 8 ? SND_PCM_FORMAT_S8
	                                       : SND_PCM_FORMAT_S16);

	/* set rate */
	tmp = rate;
	snd_pcm_hw_params_set_rate_near(pcm, params, &tmp, 0);

	if (tmp != rate)
		printf("ALSA: Requested samplerate = %i, using %i\n",
		       rate, tmp);

	/* apply changes */
	snd_pcm_hw_params(pcm, params);

	return 1;
}

int alsa_play(unsigned char *data, int len) {
	int frames = len / frame_size, written;
	
	assert(pcm);

	/* there's still data to write */
	while (frames > 0) {
		if ((written = snd_pcm_writei(pcm, data, frames)) > 0) {
			/* success: play the next frame */
			frames -= written;
			data += written * frame_size;
		} else if (written == -EAGAIN)
			continue;
		else if (written == -EPIPE)
			snd_pcm_prepare(pcm);
		else if (written == -ESTRPIPE) {
			/* wait until the suspend flag is released */
			while ((written = snd_pcm_resume(pcm)) == -EAGAIN)
				sleep(1);
	
			if (written < 0) {
				/* can't wake up device, reset it */
				snd_pcm_prepare(pcm);
				return 0;
			}
		}
	}

	return 1;
}

int alsa_volume_get(int *left, int *right) {
	assert(master_ctrl);

	/* make sure changes made to the mixer levels w/ other applications
	 * (like alsamixer) are reflected here
	 */
	snd_mixer_handle_events(mixer);

	snd_mixer_selem_get_playback_volume(master_ctrl,
	                                    SND_MIXER_SCHN_FRONT_LEFT,
	                                    (long *) left);
	
	snd_mixer_selem_get_playback_volume(master_ctrl,
	                                    SND_MIXER_SCHN_FRONT_RIGHT,
	                                    (long *) right);
	
	return 1;
}

int alsa_volume_set(int left, int right) {
	assert(master_ctrl);

	snd_mixer_selem_set_playback_volume(master_ctrl,
	                                    SND_MIXER_SCHN_FRONT_LEFT,
	                                    left);

	snd_mixer_selem_set_playback_volume(master_ctrl,
	                                    SND_MIXER_SCHN_FRONT_RIGHT,
	                                    right);

	return 1;
}

static snd_mixer_elem_t *get_master() {
	snd_mixer_elem_t *master;
	snd_mixer_selem_id_t *id;

	snd_mixer_selem_id_malloc(&id);
	
	if (!(master = snd_mixer_first_elem(mixer)))
		return NULL;

	for (snd_mixer_selem_get_id(master, id);
		strcmp(snd_mixer_selem_id_get_name(id), "Master");
		master = snd_mixer_elem_next(master))
		snd_mixer_selem_get_id(master, id);

	return master;
}

static void init_master() {
	long min = 0, max = 0;
	int vol[2] = {0};
	
	snd_mixer_selem_get_playback_volume_range(master_ctrl, &min, &max);
	snd_mixer_selem_set_playback_volume_range(master_ctrl, 0, 100);

	if (alsa_volume_get(vol, &vol[1]))
		alsa_volume_set(vol[0] * 100 / max, vol[1] * 100 / max);
}

static snd_mixer_t *open_mixer() {
	snd_mixer_t *mixer = NULL;
	
	if (snd_mixer_open(&mixer, 0) < 0)
		return NULL;

	if (snd_mixer_attach(mixer, "hw:0") < 0)
		return NULL;
	
	if (snd_mixer_selem_register(mixer, NULL, NULL) < 0)
		return NULL;

	if (snd_mixer_load(mixer) < 0)
		return NULL;
	
	return mixer;
}

int plugin_init(OutputPlugin *op) {
	op->name = strdup("ALSA");

	op->configure = alsa_configure;
	op->play = alsa_play;
	op->volume_get = alsa_volume_get;
	op->volume_set = alsa_volume_set;
	op->shutdown = alsa_shutdown;

	if (!(mixer = open_mixer()) || !(master_ctrl = get_master()))
		return 0;

	init_master();

	return 1;
}
