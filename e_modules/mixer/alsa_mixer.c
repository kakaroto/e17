#include <e.h>
#include "alsa_mixer.h"

static int _alsa_get_hash      (const char *name);
static int _alsa_get_system_id (const char *name);
static int _alsa_get_mixer_id  (const char *name);

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
	card->id = _alsa_get_mixer_id(card->real);
	
	cards = evas_list_append(cards, card);
     }
   
   return cards;
}

void 
alsa_free_cards(Evas_List *cards) 
{
   if (!cards) return;
   
   while (cards) 
     {
	Alsa_Card *card;
	
	card = cards->data;
	if (!card) continue;
	if (card->name) evas_stringshare_del(card->name);
	if (card->real) evas_stringshare_del(card->real);
	E_FREE(card);
	
	cards = evas_list_remove_list(cards, cards);
     }
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
_alsa_get_mixer_id(const char *name) 
{
   return _alsa_get_hash(name) << 8;
}

