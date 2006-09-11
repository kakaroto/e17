#ifndef ALSA_MIXER_H
#define ALSA_MIXER_H

#include <alsa/asoundlib.h>
#include <Evas.h>

typedef struct _Alsa_Mixer Alsa_Mixer;
typedef struct _Alsa_Card Alsa_Card;
typedef struct _Alsa_Channel Alsa_Channel;

struct _Alsa_Mixer 
{
   Evas_List   *handlers;
   snd_mixer_t *handle;
   
   struct pollfd *fds;
   int update;
};

struct _Alsa_Card 
{
   const char  *name;
   const char  *real;
   int          active;
   int          id;
   
   Evas_List *channels;
};

struct _Alsa_Channel 
{
   int         id;
   int         card_id;
   
   const char *name;
};

Evas_List   *alsa_get_cards         (void);
void        *alsa_get_card          (int id);
Evas_List   *alsa_card_get_channels (void *data);
void         alsa_free_cards        (void *data);

#endif
