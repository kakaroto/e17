#include <e.h>
#include "config.h"
#ifdef HAVE_OSS
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "e_mod_main.h"
#include "e_mod_types.h"
#include "oss_mixer.h"

static Evas_List *
_oss_scan_devices(char *type)
{
   FILE *file;
   char buffer[256], *tmp2;
   int found = 0;
   int index = 0;
   static Evas_List *cards = NULL;
   
   if(cards)
     return cards;
   
   if ((file = fopen("/dev/sndstat", "r"))) 
     {
	while (fgets(buffer, 255, file)) {
	   if (found && buffer[0] == '\n')
	     break;
	   if (buffer[strlen(buffer) - 1] == '\n')
	     buffer[strlen(buffer) - 1] = '\0';
	   if (found) 
	     {
		if (index == 0) 
		  {
		     tmp2 = strchr(buffer, ':');
		     if (tmp2) 
		       {
			  tmp2++;
			  while (*tmp2 == ' ')
			    tmp2++;
		       }
		     else
		       tmp2 = buffer;
		     cards = evas_list_append(cards, strdup(buffer));
		  }
		else
		  cards = evas_list_append(cards, strdup(buffer));
	     }
	   if (!strcasecmp(buffer, type))
	     found = 1;	   
	}
	fclose(file);
     }
   else
     cards = evas_list_append(cards, strdup("Default"));
   
   return cards;
}   

Evas_List *
oss_get_cards() 
{   
   static Evas_List    *cards = NULL;
   Evas_List *hw_cards = NULL;
   Mixer_Card *card;

   if(cards)
     return cards;
   
   if((hw_cards = _oss_scan_devices("Installed Devices")))
     {
	Evas_List *l;
	int i = 0;
	
	for(l = hw_cards; l; l = l->next)
	  {
	     char *real;
	     int size;
	     
	     size = sizeof(char) * (strlen("/dev/mixerX") + 1);
	     real = malloc(size);
	     snprintf(real, size, "/dev/mixer%d", i);
	     card = E_NEW(Mixer_Card, 1);
	     card->name = evas_stringshare_add(l->data);
	     card->real = evas_stringshare_add(real);
	     card->id = i + 1;
	     free(real);
	     ++i;
	     cards = evas_list_append(cards, card);
	  }	     
     }
   else
     {
	card = E_NEW(Mixer_Card, 1);
	card->name = evas_stringshare_add("Default");
	card->real = evas_stringshare_add("/dev/mixer0");
	card->id = 1;
	cards = evas_list_append(cards, card);
     }
   
   return cards;
}

void *oss_get_card(int id)
{
   Evas_List *hw_cards = NULL;
   Mixer_Card *card = NULL;

   if((hw_cards = _oss_scan_devices("Installed Devices")))
     {
	int i = 0;	
	char *real;
	int size;
	char *name;
	
	if((name = evas_list_nth(hw_cards, id - 1)))
	  {
	     size = sizeof(char) * (strlen("/dev/mixerX") + 1);
	     real = malloc(size);
	     snprintf(real, size, "/dev/mixer%d", i);
	     card = E_NEW(Mixer_Card, 1);
	     card->name = evas_stringshare_add(name);
	     card->real = evas_stringshare_add(real);
	     card->id = id;
	     free(real);
	  }
     }

   if(!card)
     {   
	card = E_NEW(Mixer_Card, 1);
	card->name = evas_stringshare_add("Default");
	card->real = evas_stringshare_add("/dev/mixer0");
	card->id = 1;
     }
   
   return card;   
}

Evas_List *oss_card_get_channels(void *data)
{
   Mixer_Channel *ac;
   const char *name = "Master";
   Evas_List *channels = NULL;
   
   /* FIXME: do this properly */
   ac = E_NEW(Mixer_Channel, 1);
   
   ac->name = evas_stringshare_add(name);
   ac->id = 1;
   
   channels = evas_list_append(channels, ac);
   return channels;   
}

void *oss_card_get_channel(void *data, int channel_id)
{
   Mixer_Channel *ac;
   
   /* FIXME: do this properly */
   ac = E_NEW(Mixer_Channel, 1);
   
   ac->name = evas_stringshare_add("Master");
   ac->id = 1;
   return ac;
}

void oss_free_cards(void *data)
{
   /* FIXME: free everything here */
}

int oss_get_volume(int card_id, int channel_id)
{
   int fd, v, devs;
   long cmd;
   const char *devname;
   int r, l = 0;
   Mixer_Card *card;
   
   card = oss_get_card(card_id);
   if(!card) return 0;
   
   devname = card->real;
   fd = open(devname, O_RDONLY);
      
   if (fd != -1) 
     {
	ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devs);
	if ((devs & SOUND_MASK_PCM) /*&& (oss_cfg.use_master == 0)*/)
	  cmd = SOUND_MIXER_READ_PCM;
	else if ((devs & SOUND_MASK_VOLUME) /*&& (oss_cfg.use_master == 1)*/)
	  cmd = SOUND_MIXER_READ_VOLUME;
	else 
	  {
	     close(fd);
	     return 0;
	  }
	ioctl(fd, cmd, &v);
	/* We have the volume for both left / right, returning only one */
	r = (v & 0xFF00) >> 8;
	l = (v & 0x00FF);
	close(fd);
     }
   E_FREE(card);
   return l;
}

int oss_set_volume(int card_id, int channel_id, double vol)
{
   int fd, v, devs;
   long cmd;
   const char *devname;
   int r, l;
   Mixer_Card *card;
   
   r = l = (int)vol;   
   
   card = oss_get_card(card_id);
   if(!card) return 0;
   
   devname = card->real;
   fd = open(devname, O_RDONLY);
   
   if (fd != -1) {
      ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devs);
      if ((devs & SOUND_MASK_PCM) /*&& (oss_cfg.use_master == 0)*/)
	cmd = SOUND_MIXER_WRITE_PCM;
      else if ((devs & SOUND_MASK_VOLUME) /*&& (oss_cfg.use_master == 1)*/)
	cmd = SOUND_MIXER_WRITE_VOLUME;
      else {
	 close(fd);
	 return 0;
      }
      v = (r << 8) | l;
      ioctl(fd, cmd, &v);
      close(fd);
   }
   else
     {
	printf("oss_set_volume(): Failed to open mixer device (%s): %s",
	       devname, strerror(errno));
     }
   E_FREE(card);
   return 1;
}

int oss_get_mute(int card_id, int channel_id)
{
   if(oss_get_volume(card_id, channel_id) == 0)
     return 1;
   else
     return 0;      
}

int oss_set_mute(int card_id, int channel_id, int mute)
{   
   if(mute)
     return oss_set_volume(card_id, channel_id, 0);
   else
     /* FIXME: this is hardcoded, need to restore it */
     return oss_set_volume(card_id, channel_id, 10);
}

#endif
