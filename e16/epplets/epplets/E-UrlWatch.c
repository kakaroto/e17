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
#include <sys/stat.h>
#include <sys/types.h>

#include "epplet.h"
#include "utils.h"
#include "E-UrlWatch.h"

#if 0
#define D(a) { printf("%s +%u : ",__FILE__,__LINE__); \
               printf a; fflush(stdout); }
#else
#define D(a) { /* No debug */; }
#endif

static void display_string (char *string);
static void handle_url (char *url, char *type);
static void add_url_to_popup (char *url);
static int url_in_popup (char *url);
static char *validate_url (char *url);

static void
save_config (void)
{
  char buf[10];
  Esnprintf (buf, sizeof (buf), "%d", opt.always_show_file_urls);
  Epplet_modify_config ("ALWAYS_SHOW_FILE_URLS", buf);
  Esnprintf (buf, sizeof (buf), "%d", opt.save_urls);
  Epplet_modify_config ("SAVE_URLS", buf);
  Esnprintf (buf, sizeof (buf), "%d", opt.check_url_file);
  Epplet_modify_config ("CHECK_URL_FILE", buf);
  Esnprintf (buf, sizeof (buf), "%d", opt.do_new_url_command);
  Epplet_modify_config ("RUN_COMMAND_ON_NEW_URL", buf);
  Epplet_modify_config ("WWW_COMMAND", opt.www_command);
  Epplet_modify_config ("FTP_COMMAND", opt.ftp_command);
  Epplet_modify_config ("GET_COMMAND", opt.get_command);
  Epplet_modify_config ("URL_SAVE_FILE", opt.url_save_file);
  Epplet_modify_config ("URL_CHECK_FILE", opt.url_file);
  Epplet_modify_config ("NEW_URL_COMMAND", opt.new_url_command);
}

static void
load_config (void)
{
  char *home = getenv ("HOME");
  char buf[256];
  opt.save_urls = atoi (Epplet_query_config_def ("SAVE_URLS", "1"));
  opt.check_url_file = atoi (Epplet_query_config_def ("CHECK_URL_FILE", "1"));
  opt.always_show_file_urls =
    atoi (Epplet_query_config_def ("ALWAYS_SHOW_FILE_URLS", "0"));
  opt.do_new_url_command =
    atoi (Epplet_query_config_def ("RUN_COMMAND_ON_NEW_URL", "1"));
  if (opt.www_command)
    free (opt.www_command);
  opt.www_command =
    _Strdup (Epplet_query_config_def
	     ("WWW_COMMAND", "gnome-moz-remote --newwin"));
  if (opt.ftp_command)
    free (opt.ftp_command);
  opt.ftp_command =
    _Strdup (Epplet_query_config_def
	     ("FTP_COMMAND", "gnome-moz-remote --newwin"));
  if (opt.get_command)
    free (opt.get_command);
  opt.get_command =
    _Strdup (Epplet_query_config_def ("GET_COMMAND", "Eterm -O -e wget"));
  if (opt.url_save_file)
    free (opt.url_save_file);
  Esnprintf (buf, sizeof (buf), "%s/.Urls", home);
  opt.url_save_file =
    _Strdup (Epplet_query_config_def ("URL_SAVE_FILE", buf));
  if (opt.url_file)
    free (opt.url_file);
  Esnprintf (buf, sizeof (buf), "%s/.te/.urls", home);
  opt.url_file = _Strdup (Epplet_query_config_def ("URL_CHECK_FILE", buf));
  if (opt.new_url_command)
    free (opt.new_url_command);
  opt.new_url_command =
    _Strdup (Epplet_query_config_def
	     ("NEW_URL_COMMAND",
	      "esdplay " EROOT "/epplet_data/E-UrlWatch/wooeep.wav &"));
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
cb_url_listp (void *data)
{
  char *url = (char *) data;
  D (("In cb_url_listp: About to call handle_url on -->%s<--\n", url));
  handle_url (url, "www");
}

static void
build_url_list (void)
{
  int i, j;

  j = num_urls - 1;

  D (("In build_url_list: num_urls = -->%d<--\n", num_urls));

  url_p = Epplet_create_popup ();

  for (i = 0; i < 10; i++)
    {
      if (urllist[j])
	{
	  Epplet_add_popup_entry (url_p, urllist[j], NULL,
				  cb_url_listp, (void *) urllist[j]);
	  D (("In build_url_list: adding = -->%s<--\n", urllist[j]));
	  --j;
	  if (j < 0)
	    j = 9;
	}
      else
	break;
    }
  Epplet_change_popbutton_popup (btn_urllist, url_p);
}

static void
add_url_to_url_list (char *url)
{
  if (num_urls == 10)
    num_urls = 0;

  D (("In add_url_to_url_list: adding = -->%s<--\n", url));

  if (urllist[num_urls])
    {
      free (urllist[num_urls]);
      urllist[num_urls] = NULL;
    }

  urllist[num_urls] = _Strdup (url);

  num_urls++;
}

static void
add_url_to_popup (char *url)
{
  D (("In add_url_to_popup: adding = -->%s<--\n", url));

  add_url_to_url_list (url);

  build_url_list ();

  return;
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
  FILE *fp;
  static char buf[256];
  int j = 0;

  D (("In get_url_from_file_list\n"));

  if ((fp = fopen (file, "r")) == NULL)
    {
      fprintf (stderr, "Unable to open file -->%s<-- for reading\n", file);
      return "Error opening url file!";
    }

  if (position == -1)
    {
      while (!feof (fp))
	{
	  j++;
	  fgets (buf, sizeof (buf), fp);
	}
      D (("In get_url_from_file_list: There are %d urls in the file\n", j));
    }
  else
    {
      while (!feof (fp) && j < position)
	{
	  j++;
	  fgets (buf, sizeof (buf), fp);
	}
      D (
	 ("In get_url_from_file_list: Asked for url %d returning url %d\n",
	  position, j));
    }

  D (("In get_url_from_file_list: buf is -->%s<--\n", buf));

  fclose (fp);

  return buf;
}

static void
display_url_from_file (char *url)
{
  char *validurl;

  if ((validurl = validate_url (url)) == NULL)
    return;

  /* Perform new url command (eg play a sound) */
  if (opt.do_new_url_command && opt.new_url_command)
    system (opt.new_url_command);

  if (opt.always_show_file_urls)
    handle_url (validurl, "www");
  else
    {
      display_string (validurl);
      Epplet_gadget_show (btn_file_url);
    }

  if (!url_in_popup (validurl))
    add_url_to_popup (validurl);
}

/* Examine the file for changes */
static void
check_url_file (void *data)
{
  static off_t lastfilesize = 0;
  off_t filesize = 0;
  struct stat filestat;

  D (("In check_url_file, url_file -->%s<--\n", opt.url_file));

  if (stat (opt.url_file, &filestat) != 0)
    {
      D (("stat() failed on -->%s<--\n", opt.url_file));
    }
  else
    {
      filesize = filestat.st_size;

      if ((filesize != lastfilesize) && (lastfilesize != 0))
	display_url_from_file (get_url_from_file_list (opt.url_file, -1));

      lastfilesize = filesize;
    }

  if (opt.check_url_file)
    Epplet_timer (check_url_file, NULL, 1, "URLCHECK_TIMER");

  return;
  data = NULL;
}

static char *
validate_url (char *url)
{
  char *p;
  char *ret = NULL;
  static char *orig_ret = NULL;

  D (("In validate_url: url -->%s<--\n", url));

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

  /* If just www.blah, add the http:// to avoid confusing nutscrape */
  if (*ret == 'w' && *(ret + 1) == 'w' && *(ret + 2) == 'w')
    {
      char *temp = _Strjoin (NULL, "http://", ret, NULL);
      free (orig_ret);
      ret = _Strdup (temp);
      free (temp);
    }

  D (("In validate_url: ret -->%s<--\n", ret));

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

  if ((fp = fopen (opt.url_save_file, "a")) != NULL)
    {
      fprintf (fp, "%s\n", url);
      fclose (fp);
    }
}

static void
reset_string (void *data)
{
  display_string ("E-UrlWatch");
  Epplet_gadget_hide (btn_file_url);
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
    Epplet_change_label (lbl_url, dtext.str);

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

static int
url_in_popup (char *url)
{
  int i;

  D (("In url_in_popup: url is -->%s<--\n", url));

  for (i = 0; i < 10; i++)
    {
      if ((urllist[i]) && (!strcmp (urllist[i], url)))
	{
	  D (("In url_in_popup: A match! Returning TRUE\n"));
	  return 1;
	}
      else
	{
	  D (
	     ("In url_in_popup: No match between list item -->%s<-- and url -->%s<--\n",
	      urllist[i], url));
	}
    }
  return 0;
}

static void
display_string (char *string)
{
  D (("In display_string: String -->%s<--\n", string));

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

  if (url == NULL)
    return;

  D (("In handle_url: url -->%s<--\n", url));

  if (!strcmp (type, "www"))
    {
      sys = _Strjoin (NULL, opt.www_command, " \"", url, "\" &", NULL);
    }
  else if (!strcmp (type, "ftp"))
    {
      sys = _Strjoin (NULL, opt.ftp_command, " \"", url, "\" &", NULL);
    }
  else if (!strcmp (type, "get"))
    {
      sys = _Strjoin (NULL, opt.get_command, " \"", url, "\" &", NULL);
    }
  else
    {
      sys = _Strjoin (NULL, opt.www_command, " \"", url, "\" &", NULL);
    }

  D (("In handle_url: About to system() -->%s<--\n", sys));

  system (sys);

  if (!url_in_popup (url))
    add_url_to_popup (url);

  free (sys);

  if (opt.save_urls)
    save_url (url);
}

/* Amongst all the fluff, this is the bit that does the actual work. */
static void
cb_shoot (void *data)
{
  char *url;
  char *validurl;

  url = get_url_from_paste_buffer ();

  D (("In cb_shoot: url -->%s<--\n", url));

  if ((validurl = validate_url (url)) == NULL)
    return;

  D (("In cb_shoot: valid url -->%s<--\n", validurl));

  display_string (validurl);

  handle_url (validurl, data);

  return;
}

static void
cb_btn_file_url (void *data)
{
  if (dtext.str && strcmp (dtext.str, "E-UrlWatch"))
    {
      handle_url (dtext.str, "www");
    }
  return;
  data = NULL;
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
  btn_file_url = Epplet_create_button ("New Url!", NULL,
				       32, 2, 48, 12,
				       0, 0, NULL, cb_btn_file_url, NULL);
  url_p = Epplet_create_popup ();
  Epplet_add_popup_entry (url_p, "Url List is currently empty", NULL, NULL,
			  NULL);
  Epplet_gadget_show (btn_urllist =
		      Epplet_create_popupbutton (NULL, NULL, 16, 2, 12, 12,
						 "ARROW_DOWN", url_p));
  if (opt.check_url_file)
    Epplet_timer (check_url_file, NULL, 1, "URLCHECK_TIMER");
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
  Epplet_Init ("E-UrlWatch", "0.1",
	       "Enlightenment Url Watcher Epplet", 6, 3, argc, argv, 0);
  Epplet_load_config ();
  load_config ();
  create_epplet_layout ();
  Epplet_show ();
  Epplet_Loop ();
  return 0;
}
