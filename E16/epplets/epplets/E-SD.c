/* E-SD.c
 *
 * Copyright (C) 1999-2000 Tom Gilbert
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
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

#include "epplet.h"
#include <esd.h>
#include "utils.h"
#include "esdcloak.h"

Epplet_gadget       btn_conf, btn_close, btn_help, btn_ctimer;
Epplet_gadget       p, ctimer_p;
Window              win;
RGB_buf             buf;
Epplet_gadget       da;
int                 cloaked = 0;
extern int          load_val;
extern int          load_r;
extern int          load_l;
extern int          colors[];
static void         cb_in(void *data, Window w);

int                 cloak_anims[] = {
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
};
int                 cloak_delays[] = {
   1, 2, 3, 4, 5, 10, 15, 20, 30, 60, 120
};

struct
{
   int                 fd;
   int                 vol_l;
   int                 vol_r;
   int                 vol_ave;
   int                 standby;
} esd;

Epplet_gadget       lbar, rbar, standby;

struct
{
   int                 cloak_anim;
   int                 do_cloak;
   double              cloak_delay;
   double              draw_interval;
   char               *dir;
}
opt;

/* RATE  is sample rate (samples/sec) */
#define BUFS   8		/* number of audio buffers (lag compensation) */
#define RATE   44100

int                 nsamp;	/* number of samples */

/*
short *incoming_buf[BUFS];	
int last_is_full;	
volatile int curbuf;
int pos;
int to_get;
int lag;
*/

static void
save_config(void)
{
   char                buf[10];

   Esnprintf(buf, sizeof(buf), "%f", opt.cloak_delay);
   Epplet_modify_config("CLOAK_DELAY", buf);
   Esnprintf(buf, sizeof(buf), "%f", opt.draw_interval);
   Epplet_modify_config("DRAW_INTERVAL", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.do_cloak);
   Epplet_modify_config("DO_CLOAK", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.cloak_anim);
   Epplet_modify_config("CLOAK_ANIM", buf);
   Epplet_modify_config("DIRECTORY", opt.dir);
}

static void
load_config(void)
{
   opt.do_cloak = atoi(Epplet_query_config_def("DO_CLOAK", "1"));
   opt.cloak_anim = atoi(Epplet_query_config_def("CLOAK_ANIM", "4"));
   opt.cloak_delay = atof(Epplet_query_config_def("CLOAK_DELAY", "3"));
   opt.draw_interval = atof(Epplet_query_config_def("DRAW_INTERVAL", "0.05"));
   if (opt.dir)
      free(opt.dir);
   opt.dir = _Strdup(Epplet_query_config_def("DIRECTORY", "~/"));
}

static void
cb_close(void *data)
{
   save_config();
   Epplet_unremember();
   esd_close(esd.fd);
   Esync();
   exit(0);
   data = NULL;
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-SD");
   return;
   data = NULL;
}

static void
cloak_draw(void *data)
{
   switch (opt.cloak_anim)
     {
     case 0:
	{
	   blank_buf();
	   break;
	}
     case 1:
	{
	   draw_flame();
	   break;
	}
     case 2:
	{
	   draw_aa_triangle();
	   break;
	}
     case 3:
	{
	   draw_aa_star();
	   break;
	}
     case 4:
	{
	   draw_scanner();
	   break;
	}
     case 5:
	{
	   draw_colorwarp();
	   break;
	}
     case 6:
	{
	   draw_ball();
	   break;
	}
     case 7:
	{
	   draw_atoms();
	   break;
	}
     case 8:
	{
	   draw_text();
	   break;
	}
     case 9:
	{
	   draw_history();
	   break;
	}
     case 10:
	{
	   draw_history_bar();
	   break;
	}
     default:
	{
	   blank_buf();
	   break;
	}
     }
   Epplet_paste_buf(buf, win, 0, 0);
   Epplet_timer(cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
   return;
   data = NULL;
}

static void
cloak_epplet(void *data)
{
   if (!cloaked)
     {
	Epplet_gadget_hide(btn_close);
	Epplet_gadget_hide(btn_conf);
	Epplet_gadget_hide(btn_help);
	Epplet_gadget_show(da);
	cloak_draw(NULL);
	cloaked = 1;
     }
   return;
   data = NULL;
}

static void
cb_cloak_anim(void *data)
{
   cb_in(NULL, 0);
   opt.do_cloak = 1;
   opt.cloak_anim = *((int *)data);
   Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
   return;
   data = NULL;
}

static void
cb_dont_cloak(void *data)
{
   opt.do_cloak = 0;
   Epplet_remove_timer("CLOAK_TIMER");
   if (cloaked)
     {
	Epplet_gadget_hide(da);
	cloaked = 0;
	Epplet_gadget_show(btn_close);
	Epplet_gadget_show(btn_conf);
	Epplet_gadget_show(btn_help);
     }
   return;
   data = NULL;
}

static void
cb_in(void *data, Window w)
{
   if (cloaked)
     {
	Epplet_gadget_hide(da);
	cloaked = 0;
	Epplet_gadget_show(btn_close);
	Epplet_gadget_show(btn_conf);
	Epplet_gadget_show(btn_help);
     }
   Epplet_remove_timer("CLOAK_TIMER");
   Epplet_remove_timer("DRAW_TIMER");
   return;
   data = NULL;
   w = (Window) 0;
}

static void
cb_out(void *data, Window w)
{
   Epplet_remove_timer("CLOAK_TIMER");
   if ((!cloaked) && (opt.do_cloak))
      Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
   return;
   data = NULL;
   w = (Window) 0;
}

static void
cb_cloak_delay(void *data)
{
   Epplet_remove_timer("CLOAK_TIMER");
   opt.cloak_delay = *(int *)data;
   Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
   return;
   data = NULL;
}

/* This method was *really* laggy */

/*
static void
handle_data (void)
{
  int buf;
  short *ptr;
  int count;
  register int i;
  register short val_l = 0, val_r = 0;
  unsigned short bigl = 0, bigr = 0;

  if (last_is_full)
    {
      curbuf++;
      if (curbuf >= BUFS)
	curbuf = 0;
    }
  count = read (esd.fd, incoming_buf[curbuf] + pos, to_get);
  if (count < 0)
    {
      printf ("count < 0\n");
      exit (1);
    }
  else
    {
      pos += count;
      to_get -= count;
    }
  if (to_get <= 0)
    {
      to_get = nsamp;
      pos = 0;
      last_is_full = 1;
      buf = ((BUFS * 2) + curbuf - lag) % BUFS;

      bigl = bigr = 0;
      for (i = 0; i < nsamp; i++)
	{
	  val_l = abs (*(incoming_buf[buf] + i));
	  i++;
	  val_r = abs (*(incoming_buf[buf] + i));
	  bigl = (val_l > bigl) ? val_l : bigl;
	  bigr = (val_r > bigr) ? val_r : bigr;
	}
      bigl /= (nsamp / 8);
      bigr /= (nsamp / 8);

      esd.vol_l = bigl;
      esd.vol_r = bigr;
      esd.vol_ave = (bigl + bigr) / 2;

      if (!cloaked)
	{
	  Epplet_gadget_data_changed (lbar);
	  Epplet_gadget_data_changed (rbar);
	}
    }
  else
    {
      last_is_full = 0;
    }
}
*/

static void
esd_timer(void *data)
{
   fd_set              rfds;
   struct timeval      tv;
   int                 retval;

   register short      val_l = 0, val_r = 0;
   unsigned short      bigl = 0, bigr = 0;
   int                 count, i;
   short               aubuf[8192] = { 0 };
   FD_ZERO(&rfds);
   FD_SET(esd.fd, &rfds);
   tv.tv_sec = 0;
   tv.tv_usec = 0;

   /* tv is basically 0, so this should be a non-blocking select() */
   /* It should immediately return whether there is anything in the stream */
   retval = select(esd.fd + 1, &rfds, NULL, NULL, &tv);

   if (retval)
     {
	count = read(esd.fd, aubuf, nsamp * 2);
	if (count < 0)
	  {
	     printf("WARNING: Couldn't read EsounD monitor stream!\n");
	     return;
	  }
	for (i = 0; i < count;)
	  {
	     val_r = abs(aubuf[i++]);
	     val_l = abs(aubuf[i++]);
	     bigl = (val_l > bigl) ? val_l : bigl;
	     bigr = (val_r > bigr) ? val_r : bigr;
	  }
	bigl /= 327;
	bigr /= 327;

	esd.vol_l = bigl;
	esd.vol_r = bigr;
	esd.vol_ave = (bigl + bigr) / 2;
	load_val = esd.vol_ave;
	load_r = esd.vol_r;
	load_l = esd.vol_l;

	if (!cloaked)
	  {
	     Epplet_gadget_data_changed(lbar);
	     Epplet_gadget_data_changed(rbar);
	  }
     }
   else
     {
	if (esd.vol_ave != 0)
	  {
	     esd.vol_r = 0;
	     esd.vol_l = 0;
	     esd.vol_ave = 0;
	     load_val = esd.vol_ave;
	     load_r = esd.vol_r;
	     load_l = esd.vol_l;

	     if (!cloaked)
	       {
		  Epplet_gadget_data_changed(lbar);
		  Epplet_gadget_data_changed(rbar);
	       }
	  }
     }

/*
  if (retval)
    {
      handle_data ();
    }
  else
    {
      if (esd.vol_ave != 0)
	{
	  esd.vol_r = 0;
	  esd.vol_l = 0;
	  esd.vol_ave = 0;
	  Epplet_gadget_data_changed (lbar);
	  Epplet_gadget_data_changed (rbar);
	}
    }
*/
   Epplet_timer(esd_timer, NULL, 0.05, "ESD_TIMER");
   return;
   data = NULL;
}

static void
standby_cb(void *data)
{
   int                 fd = esd_open_sound(NULL);

   if (esd.standby)
     {
	esd_resume(fd);
     }
   else
     {
	esd_standby(fd);
     }

   esd_close(fd);

   return;
   data = NULL;
}

static void
create_epplet_layout(void)
{

   Epplet_gadget_show(btn_close = Epplet_create_button(NULL, NULL,
						       2, 2, 0, 0, "CLOSE",
						       0, NULL, cb_close,
						       NULL));
   Epplet_gadget_show(btn_help =
		      Epplet_create_button(NULL, NULL, 18, 2, 0, 0, "HELP",
					   0, NULL, cb_help, NULL));

   Epplet_gadget_show(lbar = Epplet_create_vbar(2, 18, 12, 28, 1, &esd.vol_l));
   Epplet_gadget_show(rbar = Epplet_create_vbar(34, 18, 12, 28, 1, &esd.vol_r));
   Epplet_gadget_show(standby =
		      Epplet_create_togglebutton(NULL, "E-SD_standby.png",
						 18, 18, 12, 12,
						 &esd.standby, standby_cb,
						 NULL));

   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "Don't Cloak", NULL, cb_dont_cloak, NULL);
   Epplet_add_popup_entry(p, "Blank Epplet", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[0])));
   Epplet_add_popup_entry(p, "RasterFire", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[1])));
   Epplet_add_popup_entry(p, "AA Triangle", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[2])));
   Epplet_add_popup_entry(p, "AA Star", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[3])));
   Epplet_add_popup_entry(p, "Scanner", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[4])));
   Epplet_add_popup_entry(p, "ColorShift", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[5])));
   Epplet_add_popup_entry(p, "Bouncy Ball", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[6])));
   Epplet_add_popup_entry(p, "Atoms", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[7])));
   Epplet_add_popup_entry(p, "Banner", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[8])));
   Epplet_add_popup_entry(p, "History", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[9])));
   Epplet_add_popup_entry(p, "History Bar", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[10])));

   ctimer_p = Epplet_create_popup();
   Epplet_add_popup_entry(ctimer_p, "Cloak Delay", NULL, NULL, NULL);
   Epplet_add_popup_entry(ctimer_p, "1 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[0])));
   Epplet_add_popup_entry(ctimer_p, "2 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[1])));
   Epplet_add_popup_entry(ctimer_p, "3 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[2])));
   Epplet_add_popup_entry(ctimer_p, "4 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[3])));
   Epplet_add_popup_entry(ctimer_p, "5 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[4])));
   Epplet_add_popup_entry(ctimer_p, "10 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[5])));
   Epplet_add_popup_entry(ctimer_p, "15 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[6])));
   Epplet_add_popup_entry(ctimer_p, "20 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[7])));
   Epplet_add_popup_entry(ctimer_p, "30 Sec", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[8])));
   Epplet_add_popup_entry(ctimer_p, "1 min", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[9])));
   Epplet_add_popup_entry(ctimer_p, "2 mins", NULL, cb_cloak_delay,
			  (void *)(&(cloak_delays[10])));

   Epplet_gadget_show(btn_ctimer =
		      Epplet_create_popupbutton(NULL, "E-SD_minitime.png",
						18, 31, 12, 12, NULL,
						ctimer_p));

   Epplet_gadget_show(btn_conf = Epplet_create_popupbutton(NULL, NULL,
							   34, 2, 12, 12,
							   "CONFIGURE", p));

   da = Epplet_create_drawingarea(2, 2, 44, 44);
   win = Epplet_get_drawingarea_window(da);
   buf = Epplet_make_rgb_buf(40, 40);

   cloaked = 0;
   if (opt.do_cloak)
      Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");

   Epplet_timer(esd_timer, NULL, 0.05, "ESD_TIMER");

   Epplet_register_mouse_enter_handler(cb_in, (void *)win);
   Epplet_register_mouse_leave_handler(cb_out, NULL);
}

static void
clean_exit(void)
{
   save_config();
   Epplet_cleanup();
}

int
main(int argc, char **argv)
{
   int                 prio;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);

   atexit(clean_exit);

   /* Initialise random numbers */
   srand(time(0));

   Epplet_Init("E-SD", "0.4", "Enlightenment ESD Monitor", 3, 3, argc, argv, 0);
   Epplet_load_config();

   esd.fd =
      esd_monitor_stream(ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_PLAY,
			 RATE, NULL, "E-SD");
   if (esd.fd < 0)
     {
	printf("No ESD - exiting!\n");
	exit(1);
     }

   fcntl(esd.fd, F_SETFL, O_NONBLOCK);

   if (esd_get_standby_mode(esd.fd) == ESM_RUNNING)
      esd.standby = 0;
   else
      esd.standby = 1;

   nsamp = 4096;
/*
  last_is_full = 1;	    
  pos = 0;
  to_get = nsamp;
  lag = 0;		    
  for (i = 0; i < BUFS; i++)
    {
      incoming_buf[i] = malloc (nsamp * sizeof (short));
      if (incoming_buf[i] == NULL)
	{
	  printf
	    ("Audio Buffer Memory could NOT be allocated!!!!, Exiting now!\n");
	  exit (2);
	}
    }
*/

   set_flame_col(0);

   load_config();
   create_epplet_layout();

   Epplet_show();
   Epplet_Loop();

   return 0;
}
