#ifndef __MIXER_H
#define __MIXER_H

#include <linux/soundcard.h>

typedef enum {
	MIXER_CONTROL_VOL = SOUND_MIXER_VOLUME,
	MIXER_CONTROL_PCM = SOUND_MIXER_PCM 
} MixerControl;

typedef struct {
	MixerControl control;
	int volume;
} Mixer;

Mixer *mixer_new(MixerControl ctrl);
void mixer_free(Mixer *mixer);

int mixer_read(Mixer *mixer);
int mixer_change(Mixer *mixer, int diff);

#endif

