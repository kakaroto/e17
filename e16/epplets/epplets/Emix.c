/*
 Emix - A small audio mixer Epplet
 Roberto Machorro <rmach@bigfoot.com>
 This is a GPL Licensed program, you know the deal.
 */

#include "epplet.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#ifdef __FreeBSD__
#include <machine/soundcard.h>
#else
#include <sys/soundcard.h>
#endif

Epplet_gadget       vs_master, vs_pcm, vs_lin, vs_cda, b_close, b_help;

int                 open_mixer(void);
int                 read_volume(int);
void                set_volume(void *);
void                initialize_channels(void);

#define MIXER_DEV	"/dev/mixer"
#if 0
# define debug
#endif

typedef struct _MIX_CHANNEL
{
   int                 id;
   int                 value;
} MIX_CHANNEL;

int                 fd_mixer = 0;
int                 msk_mixerdevs = 0;
MIX_CHANNEL         channels[4];
int                 mute_status = 0;
Epplet_gadget       b_mute;

static void
cb_close(void *data)
{
   Epplet_unremember();
   close(fd_mixer);
   Esync();
   data = NULL;
   exit(0);
}

static void
cb_help(void *data)
{
   data = NULL;
   Epplet_show_about("Emix");
}

static void
toggle_mute(void *data)
{
   unsigned char       lrvl[4];
   MIX_CHANNEL        *dev = (MIX_CHANNEL *) data;

   if (mute_status)
     {
	lrvl[0] = lrvl[1] = 0;
     }
   else
     {
	lrvl[0] = (20 - (unsigned char)dev->value) * 5;
	lrvl[1] = lrvl[0];
     }
   lrvl[2] = lrvl[3] = 0;
   ioctl(fd_mixer, MIXER_WRITE(dev->id), lrvl);
}

int
open_mixer(void)
{
   if ((fd_mixer = open(MIXER_DEV, O_RDWR)) < 0)
     {
	fprintf(stderr, "Mixer device open failed...\n");
	return 0;
     }
   if (ioctl(fd_mixer, SOUND_MIXER_READ_DEVMASK, &msk_mixerdevs) == -1)
     {
	fprintf(stderr, "Mixer device mask read failed...\n");
	return 0;
     }
   return 1;
}

int
read_volume(int channel)
{
   unsigned char       lrvl[4];

   if (ioctl(fd_mixer, MIXER_READ(channels[channel].id), lrvl) == -1)
     {
#ifdef debug
	fprintf(stderr, "Error reading volume for channel #%d\n", channel);
#endif
	return 0;
     }
   channels[channel].value = (100 - (lrvl[0] + lrvl[1]) / 2) / 5;
   return channels[channel].value;
}

void
set_volume(void *data)
{
   unsigned char       lrvl[4];
   MIX_CHANNEL        *dev = (MIX_CHANNEL *) data;

   if (mute_status)
     {
	mute_status = 0;
	toggle_mute(&channels[0]);
	Epplet_gadget_data_changed(b_mute);
     }
   lrvl[0] = (20 - (unsigned char)dev->value) * 5;
   lrvl[1] = lrvl[0];
   lrvl[2] = lrvl[3] = 0;
   ioctl(fd_mixer, MIXER_WRITE(dev->id), lrvl);
}

void
initialize_channels(void)
{
   channels[0].id = SOUND_MIXER_VOLUME;
   channels[0].value = read_volume(0);
   channels[1].id = SOUND_MIXER_SYNTH;
   channels[1].value = read_volume(1);
   channels[2].id = SOUND_MIXER_PCM;
   channels[2].value = read_volume(2);
   channels[3].id = SOUND_MIXER_LINE;
   channels[3].value = read_volume(3);
}

/* callback_function to update the volumes with a timer */
static void
update_volumes_callback(void *data)
{
   channels[0].value = read_volume(0);
   channels[1].value = read_volume(1);
   channels[2].value = read_volume(2);
   channels[3].value = read_volume(3);
   Epplet_gadget_data_changed(vs_master);
   Epplet_gadget_data_changed(vs_pcm);
   Epplet_gadget_data_changed(vs_lin);
   Epplet_gadget_data_changed(vs_cda);
   Epplet_timer(update_volumes_callback, NULL, 0.5, "TIMER");
}

int
main(int argc, char *argv[])
{

   if (!open_mixer())
      return 1;
   initialize_channels();

   atexit(Epplet_cleanup);
   Epplet_Init("Emix", "0.9", "Audio mixer Epplet", 3, 3, argc, argv, 0);
   vs_master = Epplet_create_vslider(2, 2, 44, 0, 20, 1, 5,
				     &channels[0].value, set_volume,
				     &channels[0]);
   vs_pcm =
      Epplet_create_vslider(10, 2, 44, 0, 20, 1, 5, &channels[1].value,
			    set_volume, &channels[1]);
   vs_lin =
      Epplet_create_vslider(18, 2, 44, 0, 20, 1, 5, &channels[2].value,
			    set_volume, &channels[2]);
   vs_cda =
      Epplet_create_vslider(26, 2, 44, 0, 20, 1, 5, &channels[3].value,
			    set_volume, &channels[3]);
   b_close =
      Epplet_create_button(NULL, NULL, 34, 3, 0, 0, "CLOSE", 0, NULL, cb_close,
			   NULL);
   b_mute =
      Epplet_create_togglebutton("M", NULL, 34, 18, 12, 12, &mute_status,
				 toggle_mute, &channels[0]);
   b_help =
      Epplet_create_button(NULL, NULL, 34, 33, 0, 0, "HELP", 0, NULL, cb_help,
			   NULL);

   Epplet_gadget_show(vs_master);
   Epplet_gadget_show(vs_pcm);
   Epplet_gadget_show(vs_lin);
   Epplet_gadget_show(vs_cda);
   Epplet_gadget_show(b_close);
   Epplet_gadget_show(b_mute);
   Epplet_gadget_show(b_help);

   Epplet_show();
   Epplet_timer(update_volumes_callback, NULL, 0.5, "TIMER");
   Epplet_Loop();
   return 0;
}
