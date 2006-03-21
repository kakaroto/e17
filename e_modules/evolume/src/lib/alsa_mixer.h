
#ifndef ALSA_MIXER_H_INCLUDED
#define ALSA_MIXER_H_INCLUDED

typedef struct _Alsa_Mixer Alsa_Mixer;

struct _Alsa_Mixer
{
	snd_mixer_t *handle;
	Evas_List* handlers;

	struct pollfd			*fds;
	int			update;
};

#define ALSAMIXER(a) ((Alsa_Mixer*)a)

#endif // ALSA_MIXER_H_INCLUDED
