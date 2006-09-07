#ifndef ALSA_MIXER_H
#define ALSA_MIXER_H

#include <alsa/asoundlib.h>
#include <Evas.h>

typedef struct _Alsa_Mixer Alsa_Mixer;
typedef struct _Alsa_Card Alsa_Card;

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
};

Evas_List   *alsa_get_cards();
void         alsa_free_cards(Evas_List *cards);

#endif
