/* E-UrlWatch.c
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
#include <stdlib.h>
#include <sys/resource.h>

#include "epplet.h"
#include "utils.h"
#include "cloak.h"
#include "E-UrlWatch.h"

static void display_string (char *string);

static void
choose_random_cloak (void *data)
{
  static int last_anim = 0;

  do
    {
      opt.cloak_anim = (int) (15 * ((float) rand ()) / (RAND_MAX + 1.0)) + 1;
    }
  while (opt.cloak_anim == last_anim);	/* Don't pick the same one twice in a row. */
  last_anim = opt.cloak_anim;
  Epplet_timer (choose_random_cloak, NULL, opt.rand_delay, "RAND_TIMER");
  return;
  data = NULL;
}

static void
save_config (void)
{
  char buf[10];
  Esnprintf (buf, sizeof (buf), "%.2f", opt.cloak_delay);
  Epplet_modify_config ("CLOAK_DELAY", buf);
  Esnprintf (buf, sizeof (buf), "%.2f", opt.draw_interval);
  Epplet_modify_config ("DRAW_INTERVAL", buf);
  Esnprintf (buf, sizeof (buf), "%.2f", opt.rand_delay);
  Epplet_modify_config ("RAND_DELAY", buf);
/* 
 *   Esnprintf (buf, sizeof (buf), "%d", opt.do_cloak);
 *   Epplet_modify_config ("DO_CLOAK", buf);
 */
  Esnprintf (buf, sizeof (buf), "%d", opt.save_urls);
  Epplet_modify_config ("SAVE_URLS", buf);
  if (opt.rand_cloak)
    {
      strcpy (buf, "16");
    }
  else
    {
      Esnprintf (buf, sizeof (buf), "%d", opt.cloak_anim);
    }
  Epplet_modify_config ("CLOAK_ANIM", buf);
  Epplet_modify_config ("WWW_COMMAND", opt.www_command);
  Epplet_modify_config ("FTP_COMMAND", opt.ftp_command);
  Epplet_modify_config ("GET_COMMAND", opt.get_command);
  Epplet_modify_config ("URL_SAVE_FILE", opt.url_save_file);
}

static void
load_config (void)
{
  opt.do_cloak = atoi (Epplet_query_config_def ("DO_CLOAK", "0"));
  opt.cloak_anim = atoi (Epplet_query_config_def ("CLOAK_ANIM", "8"));
  opt.save_urls = atoi (Epplet_query_config_def ("SAVE_URLS", "1"));
  if (opt.cloak_anim == 16)
    {
      opt.rand_cloak = 1;
      choose_random_cloak (NULL);
    }
  else
    {
      opt.rand_cloak = 0;
    }
  opt.cloak_delay = atof (Epplet_query_config_def ("CLOAK_DELAY", "4"));
  opt.rand_delay = atof (Epplet_query_config_def ("RAND_DELAY", "60"));
  opt.draw_interval = atof (Epplet_query_config_def ("DRAW_INTERVAL", "0.1"));
  if (opt.www_command)
    free (opt.www_command);
  opt.www_command =
    _Strdup (Epplet_query_config_def ("WWW_COMMAND", "gnome-moz-remote"));
  if (opt.ftp_command)
    free (opt.ftp_command);
  opt.ftp_command =
    _Strdup (Epplet_query_config_def ("FTP_COMMAND", "gnome-moz-remote"));
  if (opt.get_command)
    free (opt.get_command);
  opt.get_command =
    _Strdup (Epplet_query_config_def ("GET_COMMAND", "Eterm -O -e wget"));
  if (opt.url_save_file)
    free (opt.url_save_file);
  opt.url_save_file =
    _Strdup (Epplet_query_config_def ("URL_SAVE_FILE", "$HOME/.Urls"));
}

static void
cb_close (void *data)
{
  save_config ();
  Esync ();
  Epplet_unremember ();
  exit (0);
  data = NULL;
}

static void
cb_help (void *data)
{
  Epplet_show_about ("E-UrlWatch");
  return;
  data = NULL;
}

static void
cloak_draw (void *data)
{
  switch (opt.cloak_anim)
    {
    case 0:
      {
	blank_buf ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 1:
      {
	load_val = (opt.quality / 2);
	draw_flame ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 2:
      {
	draw_radar ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 3:
      {
	draw_aa_radar ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 4:
      {
	draw_aa_triangle ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 5:
      {
	draw_aa_star ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 6:
      {
	draw_starfield ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 7:
      {
	draw_aa_starfield ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 8:
      {
	draw_rotator ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 9:
      {
	draw_scanner ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 10:
      {
	draw_colorwarp ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 11:
      {
	draw_ball ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 12:
      {
	draw_atoms ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 13:
      {
	draw_text ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 14:
      {
	draw_sine ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    case 15:
      {
	draw_funky_rotator ();
	Epplet_timer (cloak_draw, NULL, opt.draw_interval, "DRAW_TIMER");
	break;
      }
    default:
      {
	blank_buf ();
	break;
      }
    }
  Epplet_paste_buf (buf, win, 0, 0);
  return;
  data = NULL;
}

static void
cloak_epplet (void *data)
{
  if (!cloaked)
    {
      Epplet_gadget_hide (btn_close);
      Epplet_gadget_hide (btn_conf);
      Epplet_gadget_hide (btn_help);
      Epplet_gadget_hide (btn_www);
      Epplet_gadget_hide (btn_ctimer);
      Epplet_gadget_show (da);
      cloak_draw (NULL);
      cloaked = 1;
    }
  return;
  data = NULL;
}

static void
cb_cloak_anim (void *data)
{
  cb_in (NULL, 0);
  opt.do_cloak = 1;
  opt.cloak_anim = *((int *) data);
  if (opt.cloak_anim == 16)
    {
      opt.rand_cloak = 1;
      choose_random_cloak (NULL);
    }
  else
    {
      if (opt.rand_cloak)
	{
	  Epplet_remove_timer ("RAND_TIMER");
	}
      opt.rand_cloak = 0;
    }
  Epplet_timer (cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
  return;
  data = NULL;
}

static void
cb_cloak_delay (void *data)
{
  Epplet_remove_timer ("CLOAK_TIMER");
  opt.cloak_delay = *(int *) data;
  Epplet_timer (cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
  return;
  data = NULL;
}

static void
cb_save_delay (void *data)
{
  opt.delay = *(int *) data;
  return;
  data = NULL;
}

static void
cb_dont_cloak (void *data)
{
  opt.do_cloak = 0;
  Epplet_remove_timer ("CLOAK_TIMER");
  return;
  data = NULL;
}

static void
cb_in (void *data, Window w)
{
  if (cloaked)
    {
      Epplet_gadget_hide (da);
      cloaked = 0;
      Epplet_gadget_show (btn_close);
      Epplet_gadget_show (btn_conf);
      Epplet_gadget_show (btn_help);
      Epplet_gadget_show (btn_www);
      Epplet_gadget_show (btn_ctimer);
    }
  Epplet_remove_timer ("CLOAK_TIMER");
  Epplet_remove_timer ("DRAW_TIMER");
  return;
  data = NULL;
  w = (Window) 0;
}

static void
cb_out (void *data, Window w)
{
  Epplet_remove_timer ("CLOAK_TIMER");
  if ((!cloaked) && (opt.do_cloak))
    Epplet_timer (cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
  return;
  data = NULL;
  w = (Window) 0;
}

static char *
get_url_from_paste_buffer (void)
{
  static char *url;
  Display *thedisplay;
  int len = 0;

  thedisplay = Epplet_get_display ();

  if (url)
    free (url);

  url = _Strdup (XFetchBuffer (thedisplay, &len, 0));

  return url;
}

static char *
get_url_from_file_list (char *file, int position)
{
  static char url[] = "http://www.enlighenment.org";

  return url;
  file = NULL;
  position = 0;
}

static char *
validate_url (char *url)
{
  char *p;
  char *ret = NULL;
  static char *orig_ret = NULL;

  D(("In validate_url: url -->%s<--\n",url));
  
  if (orig_ret)
    free (orig_ret);

  ret = orig_ret = _Strdup (url);

  /* First, try searching for http://, in case there is a lot of text,
   * with an embedded url somewhere inside... */
  p = strstr (ret, "http://");
  if (p != NULL)
    ret = p;
  else
    {
      /* Ok. No "http://", maybe a "www." ? */
      p = strstr (ret, "http://");
      if (p != NULL)
	ret = p;
    }

  /* Kill at end of line */
  p = strchr (ret, '\n');
  if (p != NULL)
    *p = '\0';

  /* Skip first spaces */
  p = ret;
  while (*p == ' ')
    p++;
  ret = p;

  /* Kill at next space */
  p = strchr (ret, ' ');
  if (p != NULL)
    *p = '\0';

  /* If just www.blah, add the http:// */
  if (*ret == 'w' && *(ret + 1) == 'w' && *(ret + 2) == 'w')
    {
      char *temp = _Strjoin (NULL, "http://", ret, NULL);
      free (orig_ret);
      ret = _Strdup (temp);
      free (temp);
    }

  D(("In validate_url: ret -->%s<--\n",ret));

  /* Now some checks */
  if (strlen (ret) < 1)
    return 0;
  if (*ret == '-')
    return 0;

  return ret;
}

static void
save_url (char *url)
{
  FILE *fp;

  char *home = getenv ("HOME");
  char filename[256];

  Esnprintf (filename, sizeof (filename), "%s/%s", home, ".Urls");

  if ((fp = fopen (filename, "a")) != NULL)
    {
      fprintf (fp, "%s\n", url);
      fclose (fp);
    }
}

static void
reset_string (void *data)
{
  display_string ("E-UrlWatch");
  return;
  data = NULL;
}

static void
scroll_string (void *data)
{
  char buf[20];
  static int back = 0;
  static int pause = 0;

  if (dtext.len > 19)
    {
      if (dtext.pos > (dtext.len - 19))
	{
	  dtext.pos = dtext.len - 19;
	  back = 1;
	  pause = 1;
	}
      else if (dtext.pos == 0)
	{
	  back = 0;
	  pause = 1;
	}

      Esnprintf (buf, sizeof (buf), "%s", dtext.str + dtext.pos);

      if (!back)
	dtext.pos += 1;
      else
	dtext.pos -= 1;

      Epplet_change_label (lbl_url, buf);
    }
  else
    {
      Epplet_change_label (lbl_url, dtext.str);
    }

  if (pause)
    {
      Epplet_timer (scroll_string, NULL, 1.0, "SCROLL_TIMER");
      pause = 0;
    }
  else
    Epplet_timer (scroll_string, NULL, 0.3, "SCROLL_TIMER");

  Esync ();

  return;
  data = NULL;
}

static void
display_string (char *string)
{
  if (dtext.str)
    free (dtext.str);
  dtext.str = _Strdup (string);
  dtext.len = strlen (string);
  dtext.pos = 0;
  Epplet_timer (scroll_string, NULL, 0.1, "SCROLL_TIMER");
  Epplet_timer (reset_string, NULL, 20, "RESET_TIMER");
}

static void
handle_url (char *url, char *type)
{
  char *sys;
  char *validurl;

  if (url == NULL)
    return;

  D(("In handle_url: url -->%s<--\n", url));

  if ((validurl = validate_url (url)) == NULL)
    return;

  display_string (validurl);

  D (("In handle_url: valid url -->%s<--\n", validurl));  

  if (!strcmp (type, "www"))
    {
      sys = _Strjoin (" ", opt.www_command, validurl, "&", NULL);
    }
  else if (!strcmp (type, "ftp"))
    {
      sys = _Strjoin (" ", opt.ftp_command, validurl, "&", NULL);
    }
  else if (!strcmp (type, "get"))
    {
      sys = _Strjoin (" ", opt.get_command, validurl, "&", NULL);
    }
  else
    {
      sys = _Strjoin (" ", opt.www_command, validurl, "&", NULL);
    }

  D (("In handle_url: About to system() -->%s<--\n", sys));

  system (sys);

  free (sys);

  if (opt.save_urls)
    save_url (validurl);
}

/* Amongst all the fluff, this is the bit that does the actual work. */
static void
cb_shoot (void *data)
{
  char *url;

  url = get_url_from_paste_buffer ();
  
  D(("In cb_shoot: url -->%s<--\n",url));

  handle_url (url, data);

  return;
}

static void
cb_color (void *data)
{
  int *d;

  d = (int *) data;
  flame_col (d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);
  Epplet_gadget_hide (col_p);
}

static void
create_epplet_layout (void)
{
  Epplet_gadget_show (lbl_url = Epplet_create_label (2, 34, " ", 0));

  Epplet_gadget_show (btn_close =
		      Epplet_create_button (NULL, NULL, 2,
					    2, 0, 0,
					    "CLOSE", 0,
					    NULL, cb_close, NULL));
  Epplet_gadget_show (btn_help =
		      Epplet_create_button (NULL, NULL,
					    82, 2, 0, 0,
					    "HELP", 0, NULL, cb_help, NULL));
  Epplet_gadget_show (btn_www =
		      Epplet_create_button ("WWW", NULL,
					    2, 17, 28, 13,
					    0, 0, NULL, cb_shoot, "www"));
  Epplet_gadget_show (btn_ftp =
		      Epplet_create_button ("FTP", NULL,
					    32, 17, 28, 13,
					    0, 0, NULL, cb_shoot, "ftp"));
  Epplet_gadget_show (btn_wget =
		      Epplet_create_button ("GET", NULL,
					    62, 17, 28, 13,
					    0, 0, NULL, cb_shoot, "get"));
  p = Epplet_create_popup ();
  Epplet_add_popup_entry (p, "Don't Cloak", NULL, cb_dont_cloak, NULL);
  Epplet_add_popup_entry (p, "Blank Epplet", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[0])));
  Epplet_add_popup_entry (p, "RasterFire", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[1])));
  Epplet_add_popup_entry (p, "Radar", NULL, cb_cloak_anim,
			  (void *) (&(cloak_anims[2])));
  Epplet_add_popup_entry (p, "AA Radar", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[3])));
  Epplet_add_popup_entry (p, "AA Triangle", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[4])));
  Epplet_add_popup_entry (p, "AA Star", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[5])));
  Epplet_add_popup_entry (p, "Starfield", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[6])));
  Epplet_add_popup_entry (p, "AA Starfield", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[7])));
  Epplet_add_popup_entry (p, "Mesh", NULL, cb_cloak_anim,
			  (void *) (&(cloak_anims[8])));
  Epplet_add_popup_entry (p, "Funky Mesh", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[15])));
  Epplet_add_popup_entry (p, "Scanner", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[9])));
  Epplet_add_popup_entry (p, "ColorShift", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[10])));
  Epplet_add_popup_entry (p, "Bouncy Ball", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[11])));
  Epplet_add_popup_entry (p, "Atoms", NULL, cb_cloak_anim,
			  (void *) (&(cloak_anims[12])));
  Epplet_add_popup_entry (p, "Banner", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[13])));
  Epplet_add_popup_entry (p, "SineWave", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[14])));
  Epplet_add_popup_entry (p, "Random", NULL,
			  cb_cloak_anim, (void *) (&(cloak_anims[16])));
  ctimer_p = Epplet_create_popup ();
  Epplet_add_popup_entry (ctimer_p, "Cloak Delay", NULL, NULL, NULL);
  Epplet_add_popup_entry (ctimer_p, "1 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[0])));
  Epplet_add_popup_entry (ctimer_p, "2 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[1])));
  Epplet_add_popup_entry (ctimer_p, "3 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[2])));
  Epplet_add_popup_entry (ctimer_p, "4 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[3])));
  Epplet_add_popup_entry (ctimer_p, "5 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[4])));
  Epplet_add_popup_entry (ctimer_p, "10 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[5])));
  Epplet_add_popup_entry (ctimer_p, "15 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[6])));
  Epplet_add_popup_entry (ctimer_p, "20 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[7])));
  Epplet_add_popup_entry (ctimer_p, "30 Sec", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[8])));
  Epplet_add_popup_entry (ctimer_p, "1 min", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[9])));
  Epplet_add_popup_entry (ctimer_p, "2 mins", NULL,
			  cb_cloak_delay, (void *) (&(cloak_delays[10])));
/*  Epplet_gadget_show (btn_conf =
		      Epplet_create_popupbutton (NULL,
						 NULL, 34,
						 2, 12, 12, "CONFIGURE", p));
  Epplet_gadget_show (btn_ctimer =
		      Epplet_create_popupbutton (NULL,
						 EROOT
						 "/epplet_data/E-ScreenShoot/E-ScreenShoot_minitime.png",
						 50, 2,
						 12, 12, NULL, ctimer_p));
 */
  da = Epplet_create_drawingarea (2, 2, 44, 44);
  win = Epplet_get_drawingarea_window (da);
  buf = Epplet_make_rgb_buf (40, 40);
  cloaked = 0;
  set_flame_col (0);
  if (opt.do_cloak)
    Epplet_timer (cloak_epplet, NULL, opt.cloak_delay, "CLOAK_TIMER");
  Epplet_register_mouse_enter_handler (cb_in, (void *) win);
  Epplet_register_mouse_leave_handler (cb_out, NULL);
  display_string ("Welcome to E-UrlWatch ;-)");
}

static void
clean_exit (void)
{
  save_config ();
  Epplet_cleanup ();
}

int
main (int argc, char **argv)
{
  int prio;
  prio = getpriority (PRIO_PROCESS, getpid ());
  setpriority (PRIO_PROCESS, getpid (), prio + 10);
  atexit (clean_exit);
  /* Initialise random numbers */
  srand (time (0));
  Epplet_Init ("E-UrlWatch", "0.1",
	       "Enlightenment Url Watcher Epplet", 6, 3, argc, argv, 0);
  Epplet_load_config ();
  load_config ();
  create_epplet_layout ();
  Epplet_show ();
  Epplet_Loop ();
  return 0;
}
