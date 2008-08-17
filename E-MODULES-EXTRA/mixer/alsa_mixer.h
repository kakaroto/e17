#ifndef ALSA_MIXER_H
#define ALSA_MIXER_H

#include <alsa/asoundlib.h>
#include <Evas.h>

typedef struct _Alsa_Mixer Alsa_Mixer;

struct _Alsa_Mixer 
{
   Evas_List   *handlers;
   snd_mixer_t *handle;
   
   struct pollfd *fds;
   int update;
};

Evas_List   *alsa_get_cards         (void);
void        *alsa_get_card          (int id);
Evas_List   *alsa_card_get_channels (void *data);
void        *alsa_card_get_channel  (void *data, int channel_id);
void         alsa_free_cards        (void *data);

int          alsa_get_volume        (int card_id, int channel_id);
int          alsa_set_volume        (int card_id, int channel_id, double vol);

int          alsa_get_mute        (int card_id, int channel_id);
int          alsa_set_mute        (int card_id, int channel_id, int mute);

#endif
