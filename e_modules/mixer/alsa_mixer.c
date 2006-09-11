#include <e.h>
#include "alsa_mixer.h"

static int   _alsa_get_hash         (const char *name);
static int   _alsa_get_system_id    (const char *name);
static int   _alsa_get_card_id      (const char *name);
static int   _alsa_get_mixer_id     (const char *name); 
static void *_alsa_card_get_channel (void *data, int channel_id);

Evas_List *
alsa_get_cards() 
{
   Evas_List           *cards = NULL;
   snd_mixer_t         *handle;
   snd_ctl_t           *control;
   snd_ctl_card_info_t *hw_info;
   int                  err, i;
   char                 buf[1024];
   
   if ((err = snd_mixer_open(&handle, 0)) < 0) 
     {
	printf("Cannot open mixer: %s\n", snd_strerror(err));
	return NULL;
     }

   snd_ctl_card_info_alloca(&hw_info);
   
   for (i = 0; i < 32; i++) 
     {
	Alsa_Card *card;
	
	snprintf(buf, sizeof(buf), "hw:%d", i);
	if ((err = snd_mixer_attach(handle, buf)) < 0) break;
	if ((err = snd_mixer_detach(handle, buf)) < 0) 
	  {
	     snd_mixer_close(handle);
	     break;
	  }
	if ((err = snd_ctl_open(&control, buf, 0)) < 0) 
	  {
	     printf("Cannot control: %s: %s\n", buf, snd_strerror(err));
	     continue;
	  }
	if ((err = snd_ctl_card_info(control, hw_info)) < 0) 
	  {
	     printf("Cannot get hardware info: %s: %s\n", buf, 
		    snd_strerror(err));
	     snd_ctl_close(control);
	     continue;
	  }
	
	snd_ctl_close(control);
	
	card = E_NEW(Alsa_Card, 1);
	if (!card) continue;
	card->name = evas_stringshare_add(buf);
	card->real = evas_stringshare_add(snd_ctl_card_info_get_name(hw_info));
	card->id = _alsa_get_card_id(card->real);
	
	cards = evas_list_append(cards, card);
     }
   
   return cards;
}

void 
alsa_free_cards(void *data) 
{
   Evas_List *cards;
   
   cards = data;
   if (!cards) return;
   
   while (cards) 
     {
	Alsa_Card *card;
	
	card = cards->data;
	if (!card) continue;
	if (card->name) evas_stringshare_del(card->name);
	if (card->real) evas_stringshare_del(card->real);
	while (card->channels) 
	  {
	     Alsa_Channel *chan;
	     
	     chan = card->channels->data;
	     if (!chan) continue;
	     if (chan->name) evas_stringshare_del(chan->name);
	     card->channels = evas_list_remove_list(card->channels, card->channels);
	     E_FREE(chan);
	  }
	cards = evas_list_remove_list(cards, cards);
	E_FREE(card);
     }
}

void *
alsa_get_card(int id) 
{
   Evas_List           *cards = NULL;
   snd_mixer_t         *handle;
   snd_ctl_t           *control;
   snd_ctl_card_info_t *hw_info;
   int                  err, i;
   char                 buf[1024];

   if ((err = snd_mixer_open(&handle, 0)) < 0) 
     {
	printf("Cannot open mixer: %s\n", snd_strerror(err));
	return NULL;
     }

   snd_ctl_card_info_alloca(&hw_info);

   for (i = 0; i < 32; i++) 
     {
	Alsa_Card *card;
	
	snprintf(buf, sizeof(buf), "hw:%d", i);
	if ((err = snd_mixer_attach(handle, buf)) < 0) break;
	if ((err = snd_mixer_detach(handle, buf)) < 0) 
	  {
	     snd_mixer_close(handle);
	     break;
	  }
	if ((err = snd_ctl_open(&control, buf, 0)) < 0) 
	  {
	     printf("Cannot control: %s: %s\n", buf, snd_strerror(err));
	     continue;
	  }
	if ((err = snd_ctl_card_info(control, hw_info)) < 0) 
	  {
	     printf("Cannot get hardware info: %s: %s\n", buf, snd_strerror(err));
	     snd_ctl_close(control);
	     continue;
	  }
	
	snd_ctl_close(control);

	card = E_NEW(Alsa_Card, 1);
	if (!card) continue;
	card->name = evas_stringshare_add(buf);
	card->real = evas_stringshare_add(snd_ctl_card_info_get_name(hw_info));
	card->id = _alsa_get_card_id(card->real);
	
	if (!_alsa_get_card_id(card->real) == id) continue;
	return card;
     }
   return NULL;
}

Evas_List *
alsa_card_get_channels(void *data) 
{
   Alsa_Card            *card;
   Evas_List            *channels;
     
   snd_mixer_t          *handle;
   snd_ctl_t            *control;
   snd_ctl_card_info_t  *hw_info;
   snd_mixer_selem_id_t *sid;
   snd_mixer_elem_t     *elem;
   int                   err, i;

   card = data;   
   if (!card) return NULL;

   channels = NULL;
   
   snd_mixer_selem_id_alloca(&sid);
   snd_ctl_card_info_alloca(&hw_info);
   
   if ((err = snd_ctl_open(&control, card->name, 0)) < 0) 
     {
	printf("\n\nCannot Open Card: %s %s\n\n", card->name, snd_strerror(err));
	return NULL;
     }
   
   if ((err = snd_ctl_card_info(control, hw_info)) < 0) 
     {   
	printf("\n\nCannot get hardware info: %s %s\n\n", card->name, snd_strerror(err));
	snd_ctl_close(control);
	return NULL;
     }

   snd_ctl_close(control);
   
   if ((err = snd_mixer_open(&handle, 0)) < 0) 
     {
	printf("\n\nCannot Open Mixer: %s\n\n", snd_strerror(err));
	return NULL;
     }

   if ((err = snd_mixer_attach(handle, card->name)) < 0) 
     {
	printf("\n\nCannot Attach Mixer: %s\n\n", snd_strerror(err));
	snd_mixer_close(handle);
	return NULL;
     }

   if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) 
     {
	printf("\n\nCannot Register Mixer: %s\n\n", snd_strerror(err));
	snd_mixer_close(handle);
	return NULL;
     }

   if ((err = snd_mixer_load(handle)) < 0) 
     {
	printf("\n\nCannot Load Mixer: %s\n\n", snd_strerror(err));
	snd_mixer_close(handle);
	return NULL;
     }

   for (i = 0, elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem)) 
     {
	snd_mixer_selem_get_id(elem, sid);
	if (!snd_mixer_selem_is_active(elem)) continue;
	
	if (snd_mixer_selem_has_playback_volume(elem)) 
	  {
	     Alsa_Channel *ac;
	     const char *name;
	     
	     name = snd_mixer_selem_id_get_name(sid);
	     if ((!strcmp(name, "Master")) || (!strcmp(name, "PCM"))) 
	       {
		  ac = E_NEW(Alsa_Channel, 1);
		  if (!ac) continue;
	     
		  ac->name = evas_stringshare_add(name);
		  ac->id = _alsa_get_mixer_id(ac->name);
	     
		  channels = evas_list_append(channels, ac);
	       }
	  }
     }
   return channels;
}

void *
alsa_card_get_channel(void *data, int channel_id) 
{
   Alsa_Card    *card;
   Alsa_Channel *chan;
   
   card = data;
   if (!card) return NULL;

   chan = _alsa_card_get_channel(card, channel_id);
   if (!chan) return NULL;

   return chan;
}

int 
alsa_get_volume(int card_id, int channel_id) 
{
   Alsa_Card    *card;
   Alsa_Channel *chan;
   
   card = alsa_get_card(card_id);
   if (!card) return 0;

   chan = _alsa_card_get_channel(card, channel_id);
   if (!chan) return 0;

   printf("Get Volume\n");
}

int 
alsa_set_volume(int card_id, int channel_id, int vol) 
{
   Alsa_Card    *card;
   Alsa_Channel *chan;
   
   card = alsa_get_card(card_id);
   if (!card) return 0;

   chan = _alsa_card_get_channel(card, channel_id);
   if (!chan) return 0;
   
   printf("Set Volume: %i\n", vol);
}

/* Privates */
static int 
_alsa_get_hash(const char *name) 
{
   /* Ripped from evas_hash.c */
   unsigned int num = 0, i;
   const unsigned char *ptr;
   
   if (!name) return 0;
   for (i = 0, ptr = (unsigned char *)name; *ptr; ptr++, i++) 
	num ^= ((int)(*ptr) | ((int)(*ptr) << 8)) >> (i % 2);
   num = (num & 0xFF) + (num >> 4 & 0xF0) + (num >> 8 & 0x4F);
   num = num & 0xFF;
   
   return (int)num;
}

static int 
_alsa_get_system_id(const char *name) 
{
   return _alsa_get_hash(name) << 16;
}

static int 
_alsa_get_card_id(const char *name) 
{
   return _alsa_get_hash(name) << 8;
}

static int 
_alsa_get_mixer_id(const char *name) 
{
   return _alsa_get_hash(name);
}

static void *
_alsa_card_get_channel(void *data, int channel_id) 
{
   Alsa_Card    *card;
   Evas_List    *c;
   
   card = data;
   if (!card) return NULL;

   if (!card->channels)
     card->channels = alsa_card_get_channels(card);

   if (!card->channels) return NULL;
   
   for (c = card->channels; c; c = c->next) 
     {
	Alsa_Channel *chan;
	
	chan = c->data;
	if (!chan) continue;
	if (chan->id == channel_id) return chan;
     }
   return NULL;
}
