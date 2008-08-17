/* E-Mixer.c
 *
 * Copyright (C) 1999-2000 Tom Gilbert, Steve Brunton
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

/* SGI Stuff skillfully implemented (and supported by) Steve Brunton
 * <brunton@dweeb.turner.com> */

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <config.h>
#include "epplet.h"

#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#elif HAVE_MACHINE_SOUNDCARD_H
#include <machine/soundcard.h>
#elif HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#elif SGI_AUDIO
#include <dmedia/audio.h>
#define MAX_CHANNELS 8
#else
#error No soundcard defenition!
#endif /* SOUNDCARD_H */

Epplet_gadget       slider, mutebtn, closebtn, helpbtn;

#ifdef SGI_AUDIO
ALport              audport;
int                 minVol;	/* to deal with SGI audio HW volume ranges */
float               adjPct;
#else
int                 mixerfd = -1;
#endif
int                 mute;
int                 vol;
int                 layout;

#define LAYOUT_CONVENTIONAL 0
#define LAYOUT_WIDE 1
#define LAYOUT_TALL 2

static void
openMixer(char *device_name)
{
#ifdef OSS_GETVERSION
   int                 res, ver;
#endif
#ifdef SGI_AUDIO
   ALparamInfo         pi;
   int                 maxVol;

   audport = alOpenPort(device_name, "w", NULL);
   if (!audport)
     {
	fprintf(stderr, "Couldn't open audio port %s\n", device_name);
	exit(1);
     }
   alGetParamInfo(alGetResource(audport), AL_GAIN, &pi);
   minVol = alFixedToDouble(pi.min.ll);
   maxVol = alFixedToDouble(pi.max.ll);
   adjPct = (maxVol - minVol) * 0.01;
#else
   mixerfd = open(device_name, O_RDWR, 0);
   if (mixerfd < 0)
     {
	fprintf(stderr, "Couldn't open mixer device %s\n", device_name);
	exit(1);
     }
#endif

   /* check driver-version */
#ifdef OSS_GETVERSION
   res = ioctl(mixerfd, OSS_GETVERSION, &ver);
   if ((res == 0) && (ver != SOUND_VERSION))
     {
	fprintf(stderr,
		"warning: compiled "
		"with a different version of\nsoundcard.h.\n");
     }
#endif
}

static int
readMixer(void)
{
#ifdef SGI_AUDIO
   int                 x;
   int                 numchan = 0;
   double              tvol = 0;
   ALpv                audpv[2];	/* audio resource paramater info */
   ALfixed             gain[MAX_CHANNELS];	/* where to store the gain information. up to 8 channels */

   audpv[0].param = AL_GAIN;
   audpv[0].value.ptr = gain;
   audpv[0].sizeIn = MAX_CHANNELS;	/* can get up to 8-channel vector back */

   if (alGetParams(alGetResource(audport), audpv, 1) < 0)
     {
	fprintf(stderr, "alGetParams failed: %d\n", oserror());
     }
   else
     {
	if (audpv[0].sizeOut < 0)
	  {
	     fprintf(stderr, "AL_GAIN was an invalid paramater\n");
	  }
	else
	  {
	     for (x = 0; x < audpv[0].sizeOut; x++)
	       {
		  tvol += alFixedToDouble(gain[x]);
		  ++numchan;
	       }
	  }
     }
   if (layout == LAYOUT_TALL)
      return 100 - ((tvol / numchan) - minVol) / adjPct;
   return ((tvol / numchan) - minVol) / adjPct;
#else
   int                 tvol, r, l;

   ioctl(mixerfd, MIXER_READ(SOUND_MIXER_VOLUME), &tvol);

   l = tvol & 0xff;
   r = (tvol & 0xff00) >> 8;

   if (layout == LAYOUT_TALL)
      return 100 - (r + l) / 2;
   return (r + l) / 2;
#endif
}

static void
setMixer(int vol)
{
   int                 tvol;

#ifdef SGI_AUDIO
   int                 x;
   ALpv                audpv[2];
   ALfixed             gain[MAX_CHANNELS];

   tvol = (vol * adjPct) + minVol;
   for (x = 0; x < MAX_CHANNELS; ++x)
     {
	gain[x] = alDoubleToFixed(tvol);
     }
   audpv[0].param = AL_GAIN;
   audpv[0].value.ptr = gain;
   audpv[0].sizeIn = MAX_CHANNELS;
   if (alSetParams(alGetResource(audport), audpv, 1) < 0)
     {
	fprintf(stderr, "alSetParams failed : %d\n", oserror());
     }
   if (audpv[0].sizeOut < 0)
     {
	fprintf(stderr, "volume - %d wasn't valid\n", tvol);
     }
#else

   tvol = (vol << 8) + vol;
   ioctl(mixerfd, MIXER_WRITE(SOUND_MIXER_VOLUME), &tvol);

#endif
}

static void
cb_close(void *data)
{
   Epplet_unremember();
#ifdef SGI_AUDIO
   alClosePort(audport);
#else
   close(mixerfd);
#endif
   Esync();
   exit(0);
   data = NULL;
}

static void
mute_cb(void *data)
{
   static int          old_vol = 0;

   if (mute == 1)
     {
	old_vol = vol;
	setMixer(0);
     }
   else
     {
	vol = old_vol;
	if (layout == LAYOUT_TALL)
	   setMixer(100 - vol);
	else
	   setMixer(vol);
     }
   return;
   data = NULL;
}

static void
adj_cb(void *data)
{
   if (!mute)
     {
	if (layout == LAYOUT_TALL)
	   setMixer(100 - vol);
	else
	   setMixer(vol);
     }
   return;
   data = NULL;
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-Mixer");
   return;
   data = NULL;
}

static void
mixer_timeout_callback(void *data)
{
   vol = readMixer();
   Epplet_gadget_data_changed(slider);
   Epplet_timer(mixer_timeout_callback, NULL, 0.5, "TIMER");
   return;
   data = NULL;
}

static void
create_mixer_gadget(void)
{
   vol = readMixer();

   switch (layout)
     {
     case LAYOUT_WIDE:
	slider =
	   Epplet_create_hslider(30, 3, 48, 0, 100, 1, 25, &vol, adj_cb, NULL);
	mutebtn =
	   Epplet_create_togglebutton("M", NULL, 80, 2, 12, 12, &mute,
				      mute_cb, NULL);
	closebtn =
	   Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL,
				cb_close, NULL);
	helpbtn =
	   Epplet_create_button(NULL, NULL, 16, 2, 0, 0, "HELP", 0, NULL,
				cb_help, NULL);
	break;
     case LAYOUT_TALL:
	slider =
	   Epplet_create_vslider(3, 30, 48, 0, 100, 1, 25, &vol, adj_cb, NULL);
	mutebtn =
	   Epplet_create_togglebutton("M", NULL, 2, 80, 12, 12, &mute,
				      mute_cb, NULL);
	closebtn =
	   Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0, NULL,
				cb_close, NULL);
	helpbtn =
	   Epplet_create_button(NULL, NULL, 2, 16, 0, 0, "HELP", 0, NULL,
				cb_help, NULL);
	break;
     default:
	slider =
	   Epplet_create_hslider(4, 4, 40, 0, 100, 1, 25, &vol, adj_cb, NULL);
	mutebtn =
	   Epplet_create_togglebutton("Mute", NULL, 5, 18, 36, 12, &mute,
				      mute_cb, NULL);
	closebtn =
	   Epplet_create_button(NULL, NULL, 2, 34, 0, 0, "CLOSE", 0, NULL,
				cb_close, NULL);
	helpbtn =
	   Epplet_create_button(NULL, NULL, 34, 34, 0, 0, "HELP", 0, NULL,
				cb_help, NULL);
     }

   mute = 0;

   Epplet_gadget_show(slider);
   Epplet_gadget_show(mutebtn);
   Epplet_gadget_show(closebtn);
   Epplet_gadget_show(helpbtn);
   Epplet_timer(mixer_timeout_callback, NULL, 0.5, "TIMER");
}

int
main(int argc, char **argv)
{
   int                 i;

   atexit(Epplet_cleanup);

#ifdef SGI_AUDIO
   openMixer("audout");
#else
   openMixer("/dev/mixer");
#endif

   layout = LAYOUT_CONVENTIONAL;
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "--wide"))
	  {
	     layout = LAYOUT_WIDE;
	  }
	else if (!strcmp(argv[i], "--tall"))
	  {
	     layout = LAYOUT_TALL;
	  }
     }

   switch (layout)
     {
     case LAYOUT_WIDE:
	Epplet_Init("E-Mixer", "0.2", "Enlightenment Volume Control Epplet",
		    6, 1, argc, argv, 0);
	break;
     case LAYOUT_TALL:
	Epplet_Init("E-Mixer", "0.2", "Enlightenment Volume Control Epplet",
		    1, 6, argc, argv, 0);
	break;
     default:
	Epplet_Init("E-Mixer", "0.2", "Enlightenment Volume Control Epplet",
		    3, 3, argc, argv, 0);
     }

   create_mixer_gadget();

   Epplet_show();
   Epplet_Loop();

   return 0;
}
