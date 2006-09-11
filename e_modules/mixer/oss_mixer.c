#include <e.h>
#include <sys/soundcard.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "e_mod_types.h"
#include "oss_mixer.h"

static int mixerfd;

Evas_List *
oss_get_cards() 
{
   Evas_List    *cards = NULL;
   oss_sysinfo   info;
   oss_audioinfo ainfo;
   int           i, hd;
   char          dname[256];
   
   if ((mixerfd = open("/dev/mixer0", O_RDWR, 0)) == -1) 
     return NULL;
   
   if (ioctl(mixerfd, SNDCTL_SYSINFO, &info) == -1) 
     {
	printf("Cannot open oss info: %s\n", strerror(errno));
	return NULL;
     }

   for (i = 0; i < info.numaudios; i++) 
     {
	Mixer_Card *card;
	
	ainfo.dev = i;
	if (ioctl(mixerfd, SNDCTL_AUDIOINFO, &ainfo) == -1) 
	  continue;
	if (!ainfo.caps & DSP_CAP_OUTPUT)
	  continue;
	card = E_NEW(Mixer_Card, 1);
	if (!card) continue;

	if (!(ainfo.caps & DSP_CAP_OUTPUT)) continue;
	
	card->id = ainfo.card_number;
	card->name = ainfo.name;
	card->real = ainfo.name;

	cards = evas_list_append(cards, card);
     }
   
   return cards;
}
