/* E-Mixer.c
 *
 * Copyright (C) 1999 Tom Gilbert
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

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <config.h>
#include <epplet.h>

#ifdef HAVE_LINUX_SOUNDCARD_H
#include <linux/soundcard.h>
#elif HAVE_MACHINE_SOUNDCARD_H
#include <machine/soundcard.h>
#elif HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>
#else
#error No soundcard defenition!
#endif /* SOUNDCARD_H */

Epplet_gadget slider, mutebtn, closebtn, helpbtn;

int mixerfd = -1;
int mute;
int vol;

static void
openMixer (char *device_name)
{
  int res, ver;

  mixerfd = open (device_name, O_RDWR, 0);
  if (mixerfd < 0)
    {
      fprintf (stderr, "Couldn't open mixer device %s\n", device_name);
      exit (1);
    }

  /* check driver-version */
#ifdef OSS_GETVERSION
  res = ioctl (mixerfd, OSS_GETVERSION, &ver);
  if ((res == 0) && (ver != SOUND_VERSION))
    {
      fprintf (stderr, "warning: compiled "
	       "with a different version of\nsoundcard.h.\n");
    }
#endif
}

static int
readMixer (void)
{
  int tvol, r, l;

  ioctl (mixerfd, MIXER_READ (SOUND_MIXER_VOLUME), &tvol);

  l = tvol & 0xff;
  r = (tvol & 0xff00) >> 8;

  return (r + l) / 2;
}

static void
setMixer (int vol)
{
  int tvol;

  tvol = (vol << 8) + vol;
  ioctl (mixerfd, MIXER_WRITE (SOUND_MIXER_VOLUME), &tvol);
}

static void
cb_close (void *data)
{
  Epplet_unremember ();
  close (mixerfd);
  exit (0);
  data=NULL;
}

static void
mute_cb (void *data)
{
  if (mute == 1)
    setMixer (0);
  else
    setMixer (vol);
  return;
  data=NULL;
}

static void
adj_cb (void *data)
{
  if (!mute)
    setMixer (vol);
  return;
  data=NULL;
}

static void
cb_help (void *data)
{
  Epplet_show_about ("E-Mixer");
  return;
  data=NULL;
}

static void
mixer_timeout_callback (void *data)
{
  vol = readMixer ();
  Epplet_gadget_data_changed (slider);
  return;
  data=NULL;
}

static void
create_mixer_gadget (void)
{
  vol = readMixer ();

  slider =
    Epplet_create_hslider (30, 3, 48, 0, 100, 1, 25, &vol, adj_cb, NULL);
  mutebtn =
    Epplet_create_togglebutton ("M", NULL, 80, 2, 12, 12, &mute, mute_cb,
				NULL);
  closebtn = Epplet_create_button (NULL, NULL, 2, 2, 0, 0, "CLOSE", 0,
				   NULL, cb_close, NULL);
  helpbtn = Epplet_create_button (NULL, NULL, 16, 2, 0, 0, "HELP", 0,
				  NULL, cb_help, NULL);
  mute = 0;

  Epplet_gadget_show (slider);
  Epplet_gadget_show (mutebtn);
  Epplet_gadget_show (closebtn);
  Epplet_gadget_show (helpbtn);
  Epplet_timer (mixer_timeout_callback, NULL, 0.5, "TIMER");

  Epplet_timer (mixer_timeout_callback, NULL, 0.5, "TIMER");

}

int
main (int argc, char **argv)
{
  openMixer ("/dev/mixer");
  atexit (Epplet_cleanup);
  Epplet_Init ("E-Mixer", "0.2", "Enlightenment Volume Control Epplet", 6, 1,
	       argc, argv, 0);

  create_mixer_gadget ();

  Epplet_show ();
  Epplet_Loop ();

  return 0;
}
