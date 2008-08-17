/* E-ScreenShoot.c
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

#include "epplet.h"
#include "utils.h"
#include "cloak.h"

#include "E-ScreenShoot_ftp.h"

Epplet_gadget       btn_shoot, btn_conf, btn_close, btn_help, btn_ctimer;
Epplet_gadget       tog_win, btn_col, btn_stimer;
Epplet_gadget       sldr_qual;
Epplet_gadget       p, col_p, ctimer_p, stimer_p, grabber_p;
Window              win;
RGB_buf             buf;
Epplet_gadget       da;
int                 cloaked = 0;
extern int          load_val;
extern int          colors[];
static void         cb_in(void *data, Window w);
int                 invquality = 0;

int                 cloak_anims[] = {
   0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
   22, 23, 24, 25
};
int                 cloak_delays[] = {
   1, 2, 3, 4, 5, 10, 15, 20, 30, 60, 120
};
int                 shot_delays[] = {
   0, 1, 2, 3, 4, 5, 10, 15, 20, 30, 60, 120
};
int                 rand_delays[] = {
   0, 30, 60, 90, 120, 180, 240, 300, 600, 900, 1200
};

struct
{
   int                 quality;
   int                 win;
   int                 beep;
   int                 cloak_anim;
   int                 frame;
   int                 do_cloak;
   int                 rand_cloak;
   int                 run_script;
   int                 view_shot;
   int                 do_ftp;
   int                 ftp_passive;
   double              delay;
   double              cloak_delay;
   double              rand_delay;
   double              draw_interval;
   char               *dir;
   char               *file_prefix;
   char               *file_stamp;
   char               *file_type;
   char               *script;
   char               *viewer;
   char               *ftp_host;
   char               *ftp_user;
   char               *ftp_pass;
   char               *ftp_file;
   char               *ftp_temp;
   char               *ftp_dir;
   char               *grabber;
}
opt;

Window              confwin = 0;
Epplet_gadget       txt_file_prefix;
Epplet_gadget       txt_file_stamp;
Epplet_gadget       txt_file_type;
Epplet_gadget       txt_viewer;
Epplet_gadget       txt_script;
Epplet_gadget       txt_dir;

Epplet_gadget       txt_ftp_host;
Epplet_gadget       txt_ftp_user;
Epplet_gadget       txt_ftp_pass;
Epplet_gadget       txt_ftp_dir;
Epplet_gadget       txt_ftp_file;
Epplet_gadget       txt_ftp_temp;

static void
choose_random_cloak(void *data)
{
   static int          last_anim = 0;

   do
     {
	opt.cloak_anim = (int)(16 * ((float)rand()) / (RAND_MAX + 1.0)) + 1;
     }
   while (opt.cloak_anim == last_anim);	/* Don't pick the same one twice in a row. */
   last_anim = opt.cloak_anim;
   Epplet_timer(choose_random_cloak, NULL, opt.rand_delay, "RAND_TIMER");
   return;
   data = NULL;
}

static void
save_config(void)
{
   char                buf[10];

   Esnprintf(buf, sizeof(buf), "%d", opt.quality);
   Epplet_modify_config("QUALITY", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.win);
   Epplet_modify_config("WIN_AREA", buf);
   Esnprintf(buf, sizeof(buf), "%f", opt.delay);
   Epplet_modify_config("SHOT_DELAY", buf);
   Esnprintf(buf, sizeof(buf), "%f", opt.cloak_delay);
   Epplet_modify_config("CLOAK_DELAY", buf);
   Esnprintf(buf, sizeof(buf), "%f", opt.draw_interval);
   Epplet_modify_config("DRAW_INTERVAL", buf);
   Esnprintf(buf, sizeof(buf), "%f", opt.rand_delay);
   Epplet_modify_config("RAND_DELAY", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.do_cloak);
   Epplet_modify_config("DO_CLOAK", buf);
   if (opt.rand_cloak)
     {
	strcpy(buf, "25");
     }
   else
     {
	Esnprintf(buf, sizeof(buf), "%d", opt.cloak_anim);
     }
   Epplet_modify_config("CLOAK_ANIM", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.frame);
   Epplet_modify_config("WM_FRAME", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.beep);
   Epplet_modify_config("BEEP", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.run_script);
   Epplet_modify_config("RUN_SCRIPT", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.view_shot);
   Epplet_modify_config("VIEW_SHOT", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.do_ftp);
   Epplet_modify_config("DO_FTP", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.ftp_passive);
   Epplet_modify_config("FTP_PASSIVE", buf);
   Epplet_modify_config("DIRECTORY", opt.dir);
   Epplet_modify_config("FILE_PREFIX", opt.file_prefix);
   Epplet_modify_config("FILE_STAMP", opt.file_stamp);
   Epplet_modify_config("FILE_TYPE", opt.file_type);
   Epplet_modify_config("SCRIPT_TO_RUN", opt.script);
   Epplet_modify_config("SHOT_VIEWER", opt.viewer);
   Epplet_modify_config("FTP_USER", opt.ftp_user);
   Epplet_modify_config("FTP_PASS", opt.ftp_pass);
   Epplet_modify_config("FTP_HOST", opt.ftp_host);
   Epplet_modify_config("FTP_DIR", opt.ftp_dir);
   Epplet_modify_config("FTP_FILE", opt.ftp_file);
   Epplet_modify_config("FTP_TEMP", opt.ftp_temp);
   Epplet_modify_config("SHOT_GRABBER", opt.grabber);
}

static void
load_config(void)
{
   opt.quality = atoi(Epplet_query_config_def("QUALITY", "75"));
   invquality = (100 - opt.quality);
   opt.win = atoi(Epplet_query_config_def("WIN_AREA", "0"));
   opt.do_ftp = atoi(Epplet_query_config_def("DO_FTP", "0"));
   opt.ftp_passive = atoi(Epplet_query_config_def("FTP_PASSIVE", "1"));
   opt.run_script = atoi(Epplet_query_config_def("RUN_SCRIPT", "0"));
   opt.view_shot = atoi(Epplet_query_config_def("VIEW_SHOT", "1"));
   opt.frame = atoi(Epplet_query_config_def("WM_FRAME", "1"));
   opt.do_cloak = atoi(Epplet_query_config_def("DO_CLOAK", "1"));
   opt.beep = atoi(Epplet_query_config_def("BEEP", "1"));
   opt.cloak_anim = atoi(Epplet_query_config_def("CLOAK_ANIM", "8"));
   if (opt.cloak_anim == 25)
     {
	opt.rand_cloak = 1;
	choose_random_cloak(NULL);
     }
   else
     {
	opt.rand_cloak = 0;
     }
   opt.delay = atof(Epplet_query_config_def("SHOT_DELAY", "0"));
   opt.cloak_delay = atof(Epplet_query_config_def("CLOAK_DELAY", "4"));
   opt.rand_delay = atof(Epplet_query_config_def("RAND_DELAY", "60"));
   opt.draw_interval = atof(Epplet_query_config_def("DRAW_INTERVAL", "0.1"));
   if (opt.dir)
      free(opt.dir);
   opt.dir = _Strdup(Epplet_query_config_def("DIRECTORY", "$HOME/"));
   if (opt.file_prefix)
      free(opt.file_prefix);
   opt.file_prefix = _Strdup(Epplet_query_config_def("FILE_PREFIX", "shot_"));
   if (opt.file_stamp)
      free(opt.file_stamp);
   opt.file_stamp =
      _Strdup(Epplet_query_config_def("FILE_STAMP", "`date +%d-%m-%y_%H%M%S`"));
   if (opt.file_type)
      free(opt.file_type);
   opt.file_type = _Strdup(Epplet_query_config_def("FILE_TYPE", "jpg"));
   if (opt.script)
      free(opt.script);
   opt.script =
      _Strdup(Epplet_query_config_def("SCRIPT_TO_RUN", "scrshot_script"));
   if (opt.viewer)
      free(opt.viewer);
   opt.viewer = _Strdup(Epplet_query_config_def("SHOT_VIEWER", "ee"));
   if (opt.ftp_user)
      free(opt.ftp_user);
   opt.ftp_user = _Strdup(Epplet_query_config_def("FTP_USER", "username"));
   if (opt.ftp_pass)
      free(opt.ftp_pass);
   opt.ftp_pass = _Strdup(Epplet_query_config_def("FTP_PASS", "password"));
   if (opt.ftp_host)
      free(opt.ftp_host);
   opt.ftp_host =
      _Strdup(Epplet_query_config_def("FTP_HOST", "ftp.myhost.com"));
   if (opt.ftp_dir)
      free(opt.ftp_dir);
   opt.ftp_dir = _Strdup(Epplet_query_config_def("FTP_DIR", "/images/"));
   if (opt.ftp_file)
      free(opt.ftp_file);
   opt.ftp_file = _Strdup(Epplet_query_config_def("FTP_FILE", "shot.jpg"));
   if (opt.ftp_temp)
      free(opt.ftp_temp);
   opt.ftp_temp = _Strdup(Epplet_query_config_def("FTP_TEMP", "uploading.jpg"));
   if (opt.grabber)
      free(opt.grabber);
   opt.grabber = _Strdup(Epplet_query_config_def("SHOT_GRABBER", "import"));
}

static void
cb_close(void *data)
{
   save_config();
   Epplet_unremember();
   Esync();
   exit(0);
   data = NULL;
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-ScreenShoot");
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
	   load_val = (opt.quality / 2);
	   draw_flame();
	   break;
	}
     case 2:
	{
	   draw_radar();
	   break;
	}
     case 3:
	{
	   draw_aa_radar();
	   break;
	}
     case 4:
	{
	   draw_aa_triangle();
	   break;
	}
     case 5:
	{
	   draw_aa_star();
	   break;
	}
     case 6:
	{
	   draw_starfield();
	   break;
	}
     case 7:
	{
	   draw_aa_starfield();
	   break;
	}
     case 8:
	{
	   draw_rotator();
	   break;
	}
     case 9:
	{
	   draw_scanner();
	   break;
	}
     case 10:
	{
	   draw_colorwarp();
	   break;
	}
     case 11:
	{
	   draw_ball();
	   break;
	}
     case 12:
	{
	   draw_atoms();
	   break;
	}
     case 13:
	{
	   draw_text();
	   break;
	}
     case 14:
	{
	   draw_sine();
	   break;
	}
     case 15:
	{
	   draw_funky_rotator();
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
	Epplet_gadget_hide(btn_shoot);
	Epplet_gadget_hide(sldr_qual);
	Epplet_gadget_hide(tog_win);
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
   if (opt.cloak_anim == 25)
     {
	opt.rand_cloak = 1;
	choose_random_cloak(NULL);
     }
   else
     {
	if (opt.rand_cloak)
	  {
	     Epplet_remove_timer("RAND_TIMER");
	  }
	opt.rand_cloak = 0;
     }
   Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
   return;
   data = NULL;
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

static void
cb_shot_delay(void *data)
{
   opt.delay = *(int *)data;
   return;
   data = NULL;
}

static void
cb_grabber(void *data)
{
   if (opt.grabber)
      free(opt.grabber);
   opt.grabber = _Strdup((char *)data);
   return;
   data = NULL;
}

static void
cb_dont_cloak(void *data)
{
   opt.do_cloak = 0;
   if (cloaked)
     {
	Epplet_gadget_hide(da);
	cloaked = 0;
	Epplet_gadget_show(btn_close);
	Epplet_gadget_show(btn_conf);
	Epplet_gadget_show(btn_help);
	Epplet_gadget_show(btn_shoot);
	Epplet_gadget_show(sldr_qual);
	Epplet_gadget_show(tog_win);
     }
   Epplet_remove_timer("CLOAK_TIMER");
   return;
   data = NULL;
}

static void
cb_quality(void *data)
{
   opt.quality = (100 - invquality);
   return;
   data = NULL;
}

static void
cb_in(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	if (cloaked)
	  {
	     Epplet_gadget_hide(da);
	     cloaked = 0;
	     Epplet_gadget_show(btn_close);
	     Epplet_gadget_show(btn_conf);
	     Epplet_gadget_show(btn_help);
	     Epplet_gadget_show(btn_shoot);
	     Epplet_gadget_show(sldr_qual);
	     Epplet_gadget_show(tog_win);
	  }
	Epplet_remove_timer("CLOAK_TIMER");
	Epplet_remove_timer("DRAW_TIMER");
     }
   return;
   data = NULL;
}

static void
cb_out(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_remove_timer("CLOAK_TIMER");
	if ((!cloaked) && (opt.do_cloak))
	   Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
     }
   return;
   data = NULL;
}

static void
apply_config(void)
{
   if (opt.file_prefix)
      free(opt.file_prefix);
   opt.file_prefix = _Strdup(Epplet_textbox_contents(txt_file_prefix));
   if (opt.file_stamp)
      free(opt.file_stamp);
   opt.file_stamp = _Strdup(Epplet_textbox_contents(txt_file_stamp));
   if (opt.file_type)
      free(opt.file_type);
   opt.file_type = _Strdup(Epplet_textbox_contents(txt_file_type));
   if (opt.viewer)
      free(opt.viewer);
   opt.viewer = _Strdup(Epplet_textbox_contents(txt_viewer));
   if (opt.script)
      free(opt.script);
   opt.script = _Strdup(Epplet_textbox_contents(txt_script));
   if (opt.dir)
      free(opt.dir);
   opt.dir = _Strdup(Epplet_textbox_contents(txt_dir));
   if (opt.ftp_user)
      free(opt.ftp_user);
   opt.ftp_user = _Strdup(Epplet_textbox_contents(txt_ftp_user));
   if (opt.ftp_pass)
      free(opt.ftp_pass);
   opt.ftp_pass = _Strdup(Epplet_textbox_contents(txt_ftp_pass));
   if (opt.ftp_host)
      free(opt.ftp_host);
   opt.ftp_host = _Strdup(Epplet_textbox_contents(txt_ftp_host));
   if (opt.ftp_dir)
      free(opt.ftp_dir);
   opt.ftp_dir = _Strdup(Epplet_textbox_contents(txt_ftp_dir));
   if (opt.ftp_file)
      free(opt.ftp_file);
   opt.ftp_file = _Strdup(Epplet_textbox_contents(txt_ftp_file));
   if (opt.ftp_temp)
      free(opt.ftp_temp);
   opt.ftp_temp = _Strdup(Epplet_textbox_contents(txt_ftp_temp));

   return;
}

static void
ok_cb(void *data)
{
   apply_config();
   save_config();
   Epplet_window_destroy(confwin);
   confwin = 0;

   return;
   data = NULL;
}

static void
apply_cb(void *data)
{
   apply_config();
   return;
   data = NULL;
}

static void
cancel_cb(void *data)
{
   Epplet_window_destroy(confwin);
   confwin = 0;
   load_config();

   return;
   data = NULL;
}

static void
cb_config(void *data)
{
   Epplet_gadget       lbl, btn_anim, btn_script, btn_view, btn_do_ftp,
      btn_ftp_passive;

   if (confwin)
      return;

   /* Save any cahnges made though the main window, so we can revert using
    * the cancel button */
   save_config();

   confwin =
      Epplet_create_window_config(390, 540, "E-ScreenShoot Config", ok_cb,
				  &confwin, apply_cb, &confwin, cancel_cb,
				  &confwin);

   Epplet_gadget_show(lbl =
		      Epplet_create_label(40, 8,
					  "Please choose a cloak animation",
					  2));
   Epplet_gadget_show(btn_anim =
		      Epplet_create_popupbutton(NULL, NULL, 20, 8, 12, 12,
						"ARROW_DOWN", p));

   Epplet_gadget_show(lbl =
		      Epplet_create_label(40, 24,
					  "Please choose a shot grabber", 2));
   Epplet_gadget_show(btn_anim =
		      Epplet_create_popupbutton(NULL, NULL, 20, 24, 12, 12,
						"ARROW_DOWN", grabber_p));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 40, "Shot Directory:", 2));
   Epplet_gadget_show(txt_dir =
		      Epplet_create_textbox(NULL, opt.dir, 20, 55, 170, 20, 2,
					    NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(200, 40, "File Prefix:", 2));
   Epplet_gadget_show(txt_file_prefix =
		      Epplet_create_textbox(NULL, opt.file_prefix, 200, 55,
					    100, 20, 2, NULL, NULL));
   Epplet_gadget_show(lbl = Epplet_create_label(310, 40, "File Type:", 2));
   Epplet_gadget_show(txt_file_type =
		      Epplet_create_textbox(NULL, opt.file_type, 310, 55, 60,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 80, "File Stamp:", 2));
   Epplet_gadget_show(txt_file_stamp =
		      Epplet_create_textbox(NULL, opt.file_stamp, 20, 95, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(btn_view =
		      Epplet_create_togglebutton(NULL, NULL, 20, 130, 12, 12,
						 &opt.view_shot, NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(40, 130, "View shot after taking?",
					  2));
   Epplet_gadget_show(lbl = Epplet_create_label(20, 145, "Image Viewer:", 2));
   Epplet_gadget_show(txt_viewer =
		      Epplet_create_textbox(NULL, opt.viewer, 20, 160, 350,
					    20, 2, NULL, NULL));
   Epplet_gadget_show(btn_script =
		      Epplet_create_togglebutton(NULL, NULL, 20, 195, 12, 12,
						 &opt.run_script, NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(40, 195,
					  "Run script/program on shot after taking?",
					  2));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(20, 210, "Script/program to run:",
					  2));
   Epplet_gadget_show(txt_script =
		      Epplet_create_textbox(NULL, opt.script, 20, 225, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(btn_do_ftp =
		      Epplet_create_togglebutton(NULL, NULL, 20, 250, 12, 12,
						 &opt.do_ftp, NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(40, 250,
					  "Transfer shot by FTP after taking shot?",
					  2));
   Epplet_gadget_show(btn_ftp_passive =
		      Epplet_create_togglebutton(NULL, NULL, 20, 265, 12, 12,
						 &opt.ftp_passive, NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(40, 265, "Use Passive FTP?", 2));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 280, "FTP Host:", 2));
   Epplet_gadget_show(txt_ftp_host =
		      Epplet_create_textbox(NULL, opt.ftp_host, 20, 295, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 320, "FTP User:", 2));
   Epplet_gadget_show(txt_ftp_user =
		      Epplet_create_textbox(NULL, opt.ftp_user, 20, 335, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 360, "FTP Password:", 2));
   Epplet_gadget_show(txt_ftp_pass =
		      Epplet_create_textbox(NULL, opt.ftp_pass, 20, 375, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 400, "FTP Directory:", 2));
   Epplet_gadget_show(txt_ftp_dir =
		      Epplet_create_textbox(NULL, opt.ftp_dir, 20, 415, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(20, 440, "FTP filename:", 2));
   Epplet_gadget_show(txt_ftp_file =
		      Epplet_create_textbox(NULL, opt.ftp_file, 20, 455, 350,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(lbl =
		      Epplet_create_label(20, 480,
					  "FTP temp filename (for uploading):",
					  2));
   Epplet_gadget_show(txt_ftp_temp =
		      Epplet_create_textbox(NULL, opt.ftp_temp, 20, 495, 350,
					    20, 2, NULL, NULL));

   Epplet_window_show(confwin);

   Epplet_window_pop_context();

   return;
   data = NULL;
}

/* Amongst all the fluff, this is the bit that does the actual work. */
static void
do_shot(void *data)
{
   char               *sys;
   char                qual_buf[5];
   char               *filename_buf;
   char                frame_buf[10];
   char                beep_buf[20];
   char                import_buf[50];
   char               *script_buf = NULL;
   char               *view_buf = NULL;
   char               *filename = NULL;

   Esnprintf(qual_buf, sizeof(qual_buf), "%d", opt.quality);

   filename =
      _Strjoin(NULL, opt.dir, opt.file_prefix, opt.file_stamp, ".",
	       opt.file_type, NULL);

   filename_buf = _Strjoin(NULL, "SCRTEMP=\"", filename, "\"", NULL);

   if (!strcmp(opt.grabber, "import"))
     {
	if ((opt.frame) && (opt.win))
	   Esnprintf(frame_buf, sizeof(frame_buf), "-frame");
	else
	   frame_buf[0] = '\0';

	if (opt.beep)
	   beep_buf[0] = '\0';
	else
	   Esnprintf(beep_buf, sizeof(beep_buf), "-silent");

	if (opt.win)
	   Esnprintf(import_buf, sizeof(import_buf), "import");
	else
	   Esnprintf(import_buf, sizeof(import_buf), "import -window root");

	if (opt.run_script)
	   script_buf = _Strjoin(" ", "&&", opt.script, "$SCRTEMP", NULL);
	else
	   script_buf = _Strdup(" ");

	if (opt.view_shot)
	   view_buf = _Strjoin(" ", "&&", opt.viewer, "$SCRTEMP", NULL);
	else
	   view_buf = _Strdup(" ");

	sys =
	   _Strjoin(" ", opt.do_ftp ? "" : "(", filename_buf, "&&", import_buf,
		    beep_buf, frame_buf, "-quality", qual_buf, "$SCRTEMP",
		    script_buf, view_buf, opt.do_ftp ? "" : ")&", NULL);
     }
   else if (!strcmp(opt.grabber, "scrot"))
     {
	char                delay_buf[20];

	if (opt.delay > 0)
	   Esnprintf(delay_buf, sizeof(delay_buf), "-d %.0f", opt.delay);
	else
	   delay_buf[0] = '\0';

	if ((opt.frame) && (opt.win))
	   Esnprintf(frame_buf, sizeof(frame_buf), "-b");
	else
	   frame_buf[0] = '\0';

	if (opt.win)
	   Esnprintf(import_buf, sizeof(import_buf), "scrot -s");
	else
	   Esnprintf(import_buf, sizeof(import_buf), "scrot");

	if (opt.run_script)
	   script_buf = _Strjoin(" ", "&&", opt.script, "$SCRTEMP", NULL);
	else
	   script_buf = _Strdup(" ");

	if (opt.view_shot)
	   view_buf = _Strjoin(" ", "&&", opt.viewer, "$SCRTEMP", NULL);
	else
	   view_buf = _Strdup(" ");

	sys =
	   _Strjoin(" ", opt.do_ftp ? "" : "(", filename_buf, "&&", import_buf,
		    delay_buf, frame_buf, "--quality", qual_buf, "$SCRTEMP",
		    script_buf, view_buf, opt.do_ftp ? "" : ")&", NULL);
     }
   else
     {
	printf("don't know how to handle grabber %s\n", opt.grabber);
	return;
     }

   system(sys);

   if (opt.do_ftp)
     {
	if (!ftp_connected)
	   ftp_connect(opt.ftp_host, opt.ftp_user, opt.ftp_pass, opt.ftp_dir);
	ftp_upload(filename, opt.ftp_file, opt.ftp_temp);
     }

   free(sys);
   free(script_buf);
   free(view_buf);
   free(filename_buf);
   free(filename);
   return;
   data = NULL;
}

static void
cb_shoot(void *data)
{
   Epplet_remove_timer("SHOOT_TIMER");
   if (opt.delay < 1)
      do_shot(NULL);
   else if (!strcmp(opt.grabber, "import"))
      Epplet_timer(do_shot, NULL, opt.delay, "SHOOT_TIMER");
   else
      do_shot(NULL);
   return;
   data = NULL;
}

static void
cb_color(void *data)
{
   int                *d;

   d = (int *)data;
   flame_col(d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
   Epplet_gadget_hide(col_p);
}

static void
create_epplet_layout(void)
{
   Epplet_gadget_show(btn_close =
		      Epplet_create_button(NULL, NULL, 2, 2, 0, 0, "CLOSE", 0,
					   NULL, cb_close, NULL));
   Epplet_gadget_show(btn_help =
		      Epplet_create_button(NULL, NULL, 18, 2, 0, 0, "HELP", 0,
					   NULL, cb_help, NULL));
   Epplet_gadget_show(btn_shoot =
		      Epplet_create_button(NULL, "E-ScreenShoot_shoot.png",
					   23, 31, 22, 13, 0, 0, NULL,
					   cb_shoot, NULL));
   Epplet_gadget_show(tog_win =
		      Epplet_create_togglebutton(NULL, "E-ScreenShoot_win.png",
						 11, 17, 10, 13, &opt.win,
						 NULL, NULL));
   Epplet_gadget_show(sldr_qual =
		      Epplet_create_vslider(2, 17, 28, 0, 100, 1, 25,
					    &invquality, cb_quality, NULL));
   p = Epplet_create_popup();
   Epplet_add_popup_entry(p, "Don't Cloak", NULL, cb_dont_cloak, NULL);
   Epplet_add_popup_entry(p, "Blank Epplet", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[0])));
   Epplet_add_popup_entry(p, "RasterFire", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[1])));
   Epplet_add_popup_entry(p, "Radar", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[2])));
   Epplet_add_popup_entry(p, "AA Radar", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[3])));
   Epplet_add_popup_entry(p, "AA Triangle", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[4])));
   Epplet_add_popup_entry(p, "AA Star", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[5])));
   Epplet_add_popup_entry(p, "Starfield", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[6])));
   Epplet_add_popup_entry(p, "AA Starfield", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[7])));
   Epplet_add_popup_entry(p, "Mesh", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[8])));
   Epplet_add_popup_entry(p, "Funky Mesh", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[15])));
/* 
 *   Epplet_add_popup_entry (p, "Bendy Mesh", NULL, cb_cloak_anim,
 * 			  (void *) (&(cloak_anims[16])));
 */
   Epplet_add_popup_entry(p, "Scanner", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[9])));
   Epplet_add_popup_entry(p, "ColorShift", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[10])));
   Epplet_add_popup_entry(p, "Bouncy Ball", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[11])));
   Epplet_add_popup_entry(p, "Atoms", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[12])));
   Epplet_add_popup_entry(p, "Banner", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[13])));
   Epplet_add_popup_entry(p, "SineWave", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[14])));
   Epplet_add_popup_entry(p, "Random", NULL, cb_cloak_anim,
			  (void *)(&(cloak_anims[25])));

   col_p = Epplet_create_popup();
   Epplet_add_popup_entry(col_p, "Flame Colors", NULL, NULL, NULL);
   Epplet_add_popup_entry(col_p, "Funky", NULL, cb_color,
			  (void *)(&(colors[0 * 9])));
   Epplet_add_popup_entry(col_p, "Turquoise", NULL, cb_color,
			  (void *)(&(colors[1 * 9])));
   Epplet_add_popup_entry(col_p, "Fire", NULL, cb_color,
			  (void *)(&(colors[2 * 9])));
   Epplet_add_popup_entry(col_p, "Copper", NULL, cb_color,
			  (void *)(&(colors[3 * 9])));
   Epplet_add_popup_entry(col_p, "Violet", NULL, cb_color,
			  (void *)(&(colors[4 * 9])));
   Epplet_add_popup_entry(col_p, "Night", NULL, cb_color,
			  (void *)(&(colors[5 * 9])));
   Epplet_add_popup_entry(col_p, "Sunrise", NULL, cb_color,
			  (void *)(&(colors[6 * 9])));
   Epplet_add_popup_entry(col_p, "Sunset", NULL, cb_color,
			  (void *)(&(colors[7 * 9])));

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

   stimer_p = Epplet_create_popup();
   Epplet_add_popup_entry(stimer_p, "Shot Delay", NULL, NULL, NULL);
   Epplet_add_popup_entry(stimer_p, "No Delay", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[0])));
   Epplet_add_popup_entry(stimer_p, "1 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[1])));
   Epplet_add_popup_entry(stimer_p, "2 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[2])));
   Epplet_add_popup_entry(stimer_p, "3 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[3])));
   Epplet_add_popup_entry(stimer_p, "4 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[4])));
   Epplet_add_popup_entry(stimer_p, "5 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[5])));
   Epplet_add_popup_entry(stimer_p, "10 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[6])));
   Epplet_add_popup_entry(stimer_p, "15 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[7])));
   Epplet_add_popup_entry(stimer_p, "20 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[8])));
   Epplet_add_popup_entry(stimer_p, "30 Sec", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[9])));
   Epplet_add_popup_entry(stimer_p, "1 min", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[10])));
   Epplet_add_popup_entry(stimer_p, "2 mins", NULL, cb_shot_delay,
			  (void *)(&(shot_delays[11])));

   grabber_p = Epplet_create_popup();
   Epplet_add_popup_entry(grabber_p, "import", NULL, cb_grabber,
			  _Strdup("import"));
   Epplet_add_popup_entry(grabber_p, "scrot", NULL, cb_grabber,
			  _Strdup("scrot"));

   Epplet_gadget_show(btn_conf =
		      Epplet_create_button(NULL, NULL, 34, 2, 12, 12,
					   "CONFIGURE", 0, NULL, cb_config,
					   NULL));

   Epplet_gadget_show(btn_col =
		      Epplet_create_popupbutton(NULL, "E-ScreenShoot_col.png",
						11, 31, 10, 13, NULL, col_p));
   Epplet_gadget_show(btn_ctimer =
		      Epplet_create_popupbutton(NULL,
						"E-ScreenShoot_minitime.png",
						23, 17, 10, 13, NULL,
						ctimer_p));

   Epplet_gadget_show(btn_stimer =
		      Epplet_create_popupbutton(NULL,
						"E-ScreenShoot_minitime2.png",
						35, 17, 10, 13, NULL,
						stimer_p));

   da = Epplet_create_drawingarea(2, 2, 44, 44);
   win = Epplet_get_drawingarea_window(da);
   buf = Epplet_make_rgb_buf(40, 40);

   cloaked = 0;
   set_flame_col(0);
   if (opt.do_cloak)
      Epplet_timer(cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");

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

   Epplet_Init("E-ScreenShoot", "0.7", "Enlightenment Screen Shootin' Epplet",
	       3, 3, argc, argv, 0);
   Epplet_load_config();

   load_config();
   create_epplet_layout();

   if (opt.do_ftp)
      ftp_init(opt.ftp_passive);

   Epplet_show();
   Epplet_Loop();

   return 0;
}
