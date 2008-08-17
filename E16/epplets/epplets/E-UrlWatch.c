/* E-UrlWatch.c
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

static void         display_string(char *string);
static void         handle_url(char *url, char *type);
static void         add_url_to_popup(char *url);
static int          url_in_popup(char *url);
static char        *validate_url(char *url);
static void         check_url_file(void *data);
static void         out_cb(void *data, Window w);
static void         work_out_cb(void *data, Window w);

static void
save_config(void)
{
   /* This saves our options. Numbers are printed into a buffer to make
    * strings. */
   char                buf[10];

   Esnprintf(buf, sizeof(buf), "%d", opt.always_show_file_urls);
   Epplet_modify_config("ALWAYS_SHOW_FILE_URLS", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.save_urls);
   Epplet_modify_config("SAVE_URLS", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.check_url_file);
   Epplet_modify_config("CHECK_URL_FILE", buf);
   Esnprintf(buf, sizeof(buf), "%d", opt.do_new_url_command);
   Epplet_modify_config("RUN_COMMAND_ON_NEW_URL", buf);
   Epplet_modify_config("WWW_COMMAND", opt.www_command);
   Epplet_modify_config("FTP_COMMAND", opt.ftp_command);
   Epplet_modify_config("GET_COMMAND", opt.get_command);
   Epplet_modify_config("URL_SAVE_FILE", opt.url_save_file);
   Epplet_modify_config("URL_CHECK_FILE", opt.url_file);
   Epplet_modify_config("NEW_URL_COMMAND", opt.new_url_command);
}

static void
load_config(void)
{
   /* This reloads our config. */
   char               *home = getenv("HOME");
   char                buf[1024];

   opt.save_urls = atoi(Epplet_query_config_def("SAVE_URLS", "1"));
   opt.check_url_file = atoi(Epplet_query_config_def("CHECK_URL_FILE", "1"));
   opt.always_show_file_urls =
      atoi(Epplet_query_config_def("ALWAYS_SHOW_FILE_URLS", "0"));
   opt.do_new_url_command =
      atoi(Epplet_query_config_def("RUN_COMMAND_ON_NEW_URL", "1"));
   /* If reloading, free the old string before assigning a new one */
   if (opt.www_command)
      free(opt.www_command);
   opt.www_command =
      _Strdup(Epplet_query_config_def
	      ("WWW_COMMAND", "gnome-moz-remote --newwin \"%s\" &"));
   if (opt.ftp_command)
      free(opt.ftp_command);
   opt.ftp_command =
      _Strdup(Epplet_query_config_def
	      ("FTP_COMMAND", "gnome-moz-remote --newwin \"%s\" &"));
   if (opt.get_command)
      free(opt.get_command);
   opt.get_command =
      _Strdup(Epplet_query_config_def
	      ("GET_COMMAND", "Eterm -O -e wget \"%s\" &"));
   if (opt.url_save_file)
      free(opt.url_save_file);
   Esnprintf(buf, sizeof(buf), "%s/.Urls", home);
   opt.url_save_file = _Strdup(Epplet_query_config_def("URL_SAVE_FILE", buf));
   if (opt.url_file)
      free(opt.url_file);
   Esnprintf(buf, sizeof(buf), "%s/.te/.urls", home);
   opt.url_file = _Strdup(Epplet_query_config_def("URL_CHECK_FILE", buf));
   if (opt.new_url_command)
      free(opt.new_url_command);
   Esnprintf(buf, sizeof(buf), "esdplay %s/wooeep.wav &", Epplet_data_dir());
   opt.new_url_command =
      _Strdup(Epplet_query_config_def("NEW_URL_COMMAND", buf));
}

static void
cb_close(void *data)
{
   /* save options */
   save_config();
   /* User closed, so forget session restart */
   Epplet_unremember();
   /* Sync drawing and ipc to eesh */
   Esync();
   exit(0);
   data = NULL;
}

static void
cb_help(void *data)
{
   Epplet_show_about("E-UrlWatch");
   return;
   data = NULL;
}

static void
apply_config(void)
{
   /* Apply options from settings dialog */
   if (opt.new_url_command)
      free(opt.new_url_command);
   opt.new_url_command = _Strdup(Epplet_textbox_contents(txt_new_url_command));
   if (opt.url_save_file)
      free(opt.url_save_file);
   opt.url_save_file = _Strdup(Epplet_textbox_contents(txt_url_save_file));
   if (opt.url_file)
      free(opt.url_file);
   opt.url_file = _Strdup(Epplet_textbox_contents(txt_url_file));
   if (opt.www_command)
      free(opt.www_command);
   opt.www_command = _Strdup(Epplet_textbox_contents(txt_www_command));
   if (opt.ftp_command)
      free(opt.ftp_command);
   opt.ftp_command = _Strdup(Epplet_textbox_contents(txt_ftp_command));
   if (opt.get_command)
      free(opt.get_command);
   opt.get_command = _Strdup(Epplet_textbox_contents(txt_get_command));
   if (opt.check_url_file)
      Epplet_timer(check_url_file, NULL, 1, "URLCHECK_TIMER");

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
   /* close config window */
   Epplet_window_destroy(confwin);
   confwin = 0;
   load_config();

   return;
   data = NULL;
}

static void
cb_config(void *data)
{
   Epplet_gadget       lbl, btn_check_file, btn_save, btn_do_new_url_command,
      btn_always_show_file_urls;

   if (confwin)
      return;

   /* Save any cahnges made though the main window, so we can revert using
    * the cancel button */
   save_config();

   /* Create the window using my lovely config window api ;) */
   confwin =
      Epplet_create_window_config(440, 360, "E-UrlWatch Config", ok_cb,
				  &confwin, apply_cb, &confwin, cancel_cb,
				  &confwin);

   /* Put stuff in the window */
   Epplet_gadget_show(lbl = Epplet_create_label(10, 10, "WWW Command:", 2));
   Epplet_gadget_show(txt_www_command =
		      Epplet_create_textbox(NULL, opt.www_command, 10, 25,
					    420, 20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(10, 50, "FTP Command:", 2));
   Epplet_gadget_show(txt_ftp_command =
		      Epplet_create_textbox(NULL, opt.ftp_command, 10, 65,
					    420, 20, 2, NULL, NULL));

   Epplet_gadget_show(lbl = Epplet_create_label(10, 90, "GET Command:", 2));
   Epplet_gadget_show(txt_get_command =
		      Epplet_create_textbox(NULL, opt.get_command, 10, 105,
					    420, 20, 2, NULL, NULL));

   Epplet_draw_line(confwin, 130, 0, 130, 440, 0, 0, 0);

   Epplet_gadget_show(btn_save =
		      Epplet_create_togglebutton(NULL, NULL, 10, 135, 12, 12,
						 &opt.save_urls, NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(30, 135,
					  "Save all urls handled?", 2));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(10, 150, "File to save urls in:", 2));
   Epplet_gadget_show(txt_url_save_file =
		      Epplet_create_textbox(NULL, opt.url_save_file, 10, 165,
					    420, 20, 2, NULL, NULL));

   Epplet_gadget_show(btn_check_file =
		      Epplet_create_togglebutton(NULL, NULL, 10, 195, 12, 12,
						 &opt.check_url_file, NULL,
						 NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(30, 195,
					  "Watch a file for new urls? (Eg. Third Eye Url Catcher)",
					  2));
   Epplet_gadget_show(lbl = Epplet_create_label(20, 210, "File to watch:", 2));
   Epplet_gadget_show(txt_url_file =
		      Epplet_create_textbox(NULL, opt.url_file, 10, 225, 420,
					    20, 2, NULL, NULL));

   Epplet_gadget_show(btn_do_new_url_command =
		      Epplet_create_togglebutton(NULL, NULL, 10, 255, 12, 12,
						 &opt.do_new_url_command,
						 NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(30, 255,
					  "Run command when a new url is detected?",
					  2));
   Epplet_gadget_show(lbl = Epplet_create_label(10, 270, "Command to run:", 2));
   Epplet_gadget_show(txt_new_url_command =
		      Epplet_create_textbox(NULL, opt.new_url_command, 10,
					    285, 420, 20, 2, NULL, NULL));

   Epplet_gadget_show(btn_always_show_file_urls =
		      Epplet_create_togglebutton(NULL, NULL, 10, 315, 12, 12,
						 &opt.always_show_file_urls,
						 NULL, NULL));
   Epplet_gadget_show(lbl =
		      Epplet_create_label(30, 315,
					  "Automatically launch every new url in watched file?",
					  2));

   Epplet_window_show(confwin);

   Epplet_window_pop_context();

   return;
   data = NULL;
}

/* A click on a url in the url list. Launch the url */
static void
cb_url_listp(void *data)
{
   char               *url = (char *)data;

   D(("In cb_url_listp: About to call handle_url on -->%s<--\n", url));
   handle_url(url, "www");
}

/* Create the url list popup */
static void
build_url_list(void)
{
   int                 i, j;

   j = num_urls - 1;

   D(("In build_url_list: num_urls = -->%d<--\n", num_urls));

   url_p = Epplet_create_popup();

   for (i = 0; i < 10; i++)
     {
	if (urllist[j])
	  {
	     Epplet_add_popup_entry(url_p, urllist[j], NULL,
				    cb_url_listp, (void *)urllist[j]);
	     D(("In build_url_list: adding = -->%s<--\n", urllist[j]));
	     --j;
	     if (j < 0)
		j = 9;
	  }
	else
	   break;
     }
   Epplet_change_popbutton_popup(btn_urllist, url_p);
}

/* Stick a new url in the list. Don't rebuild the popup yet though. */
static void
add_url_to_url_list(char *url)
{
   if (num_urls == 10)
      num_urls = 0;

   D(("In add_url_to_url_list: adding = -->%s<--\n", url));

   if (urllist[num_urls])
     {
	free(urllist[num_urls]);
	urllist[num_urls] = NULL;
     }

   urllist[num_urls] = _Strdup(url);

   num_urls++;
}

/* Add a url then rebuild the popup */
static void
add_url_to_popup(char *url)
{
   D(("In add_url_to_popup: adding = -->%s<--\n", url));

   add_url_to_url_list(url);

   build_url_list();

   return;
}

/* Extract a url from the X cut buffer. This is currently technically
 * broken, as it only examines buffer 0 */
static char        *
get_url_from_paste_buffer(void)
{
   static char        *url;
   Display            *thedisplay;
   int                 len = 0;

   thedisplay = Epplet_get_display();

   if (url)
      XFree(url);

   /* This needs to be XFreed at some point */
   url = XFetchBuffer(thedisplay, &len, 0);

   return url;
}

/* Pull a url from a file. I have not used the position parameter yet, so
 * that part may not work. (I think it will though ;) */
static char        *
get_url_from_file_list(char *file, int position)
{
   FILE               *fp;
   static char         buf[256];
   int                 j = 0;

   D(("In get_url_from_file_list\n"));

   if ((fp = fopen(file, "r")) == NULL)
     {
	fprintf(stderr, "Unable to open file -->%s<-- for reading\n", file);
	return "Error opening url file!";
     }

   if (position == -1)
     {
	while (!feof(fp))
	  {
	     j++;
	     fgets(buf, sizeof(buf), fp);
	  }
	D(("In get_url_from_file_list: There are %d urls in the file\n", j));
     }
   else
     {
	while (!feof(fp) && j < position)
	  {
	     j++;
	     fgets(buf, sizeof(buf), fp);
	  }
	D(("In get_url_from_file_list: Asked for url %d returning url %d\n",
	   position, j));
     }

   D(("In get_url_from_file_list: buf is -->%s<--\n", buf));

   fclose(fp);

   return buf;
}

/* New file in url file. Show the "New!" button */
static void
display_url_from_file(char *url)
{
   char               *validurl;

   if ((validurl = validate_url(url)) == NULL)
      return;

   /* Perform new url command (eg play a sound) */
   if (opt.do_new_url_command && opt.new_url_command)
      system(opt.new_url_command);

   if (opt.always_show_file_urls)
      handle_url(validurl, "www");
   else
     {
	display_string(validurl);
	Epplet_gadget_show(btn_file_url);
     }

   if (!url_in_popup(validurl))
      add_url_to_popup(validurl);
}

/* Examine the file for changes */
static void
check_url_file(void *data)
{
   static off_t        lastfilesize = 0;
   off_t               filesize = 0;
   struct stat         filestat;

   D(("In check_url_file, url_file -->%s<--\n", opt.url_file));

   if (stat(opt.url_file, &filestat) != 0)
     {
	D(("stat() failed on -->%s<--\n", opt.url_file));
     }
   else
     {
	filesize = filestat.st_size;

	if ((filesize != lastfilesize) && (lastfilesize != 0))
	   display_url_from_file(get_url_from_file_list(opt.url_file, -1));

	lastfilesize = filesize;
     }

   if (opt.check_url_file)
      Epplet_timer(check_url_file, NULL, 1, "URLCHECK_TIMER");

   return;
   data = NULL;
}

/* Search string for url. Fix up url if needed. */
static char        *
validate_url(char *url)
{
   char               *p;
   char               *ret = NULL;
   static char        *orig_ret = NULL;

   D(("In validate_url: url -->%s<--\n", url));

   if (!url)
      return NULL;

   if (orig_ret)
      free(orig_ret);

   ret = orig_ret = _Strdup(url);

   /* First, try searching for http://, in case there is a lot of text,
    * with an embedded url somewhere inside... */
   p = strstr(ret, "http://");
   if (p != NULL)
      ret = p;
   else
     {
	/* Ok. No "http://", maybe a "www." ? */
	p = strstr(ret, "http://");
	if (p != NULL)
	   ret = p;
     }

   /* Kill at end of line */
   p = strchr(ret, '\n');
   if (p != NULL)
      *p = '\0';

   /* Skip first spaces */
   p = ret;
   while (*p == ' ')
      p++;
   ret = p;

   /* Kill at next space */
   p = strchr(ret, ' ');
   if (p != NULL)
      *p = '\0';

   /* If just www.blah, add the http:// to avoid confusing nutscrape */
   if (*ret == 'w' && *(ret + 1) == 'w' && *(ret + 2) == 'w')
     {
	char               *temp = _Strjoin(NULL, "http://", ret, NULL);

	free(orig_ret);
	ret = _Strdup(temp);
	free(temp);
     }

   D(("In validate_url: ret -->%s<--\n", ret));

   /* Now some checks */
   if (strlen(ret) < 1)
      return 0;
   if (*ret == '-')
      return 0;

   return ret;
}

/* Save url in file */
static void
save_url(char *url)
{
   FILE               *fp;

   if ((fp = fopen(opt.url_save_file, "a")) != NULL)
     {
	fprintf(fp, "%s\n", url);
	fclose(fp);
     }
}

/* Set scrolly string to default */
static void
reset_string(void *data)
{
   display_string("E-UrlWatch");
   Epplet_gadget_hide(btn_file_url);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_gadget_hide(lbl_url);
   return;
   data = NULL;
}

/* Scroll the string around */
static void
scroll_string(void *data)
{
   char                buf[20];
   static int          back = 0;
   static int          pause = 0;

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

	Esnprintf(buf, sizeof(buf), "%s", dtext.str + dtext.pos);

	if (!back)
	   dtext.pos += 1;
	else
	   dtext.pos -= 1;

	Epplet_change_label(lbl_url, buf);
     }
   else
      Epplet_change_label(lbl_url, dtext.str);

   if (pause)
     {
	Epplet_timer(scroll_string, NULL, 1.0, "SCROLL_TIMER");
	pause = 0;
     }
   else
      Epplet_timer(scroll_string, NULL, 0.3, "SCROLL_TIMER");

   Esync();

   return;
   data = NULL;
}

/* Check to see if the url is already in the list. */
static int
url_in_popup(char *url)
{
   int                 i;

   D(("In url_in_popup: url is -->%s<--\n", url));

   for (i = 0; i < 10; i++)
     {
	if ((urllist[i]) && (!strcmp(urllist[i], url)))
	  {
	     D(("In url_in_popup: A match! Returning TRUE\n"));
	     return 1;
	  }
	else
	  {
	     D(("In url_in_popup: No match between list item -->%s<-- and url -->%s<--\n", urllist[i], url));
	  }
     }
   return 0;
}

/* Stick a string in the scrolly display */
static void
display_string(char *string)
{
   D(("In display_string: String -->%s<--\n", string));

   if (dtext.str)
      free(dtext.str);
   dtext.str = _Strdup(string);
   dtext.len = strlen(string);
   dtext.pos = 0;
   Epplet_timer(scroll_string, NULL, 0.1, "SCROLL_TIMER");
   Epplet_timer(reset_string, NULL, 20, "RESET_TIMER");
}

/* Do something with the url. Launch it, prolly. */
static void
handle_url(char *url, char *type)
{
   char               *sys = NULL;
   int                 len = 0;

   if (url == NULL)
      return;

   D(("In handle_url: url -->%s<--\n", url));

   if (!strcmp(type, "www"))
     {
	/* This should be close enough :) */
	len = strlen(opt.www_command) + strlen(url) + 2;
	sys = malloc(len);
	Esnprintf(sys, len, opt.www_command, url);
     }
   else if (!strcmp(type, "ftp"))
     {
	/* This should be close enough :) */
	len = strlen(opt.ftp_command) + strlen(url) + 2;
	sys = malloc(len);
	Esnprintf(sys, len, opt.ftp_command, url);
     }
   else if (!strcmp(type, "get"))
     {
	/* This should be close enough :) */
	len = strlen(opt.get_command) + strlen(url) + 2;
	sys = malloc(len);
	Esnprintf(sys, len, opt.get_command, url);
     }
   else
     {
	/* This should be close enough :) */
	len = strlen(opt.www_command) + strlen(url) + 2;
	sys = malloc(len);
	Esnprintf(sys, len, opt.www_command, url);
     }

   D(("In handle_url: About to system() -->%s<--\n", sys));

   system(sys);

   if (!url_in_popup(url))
      add_url_to_popup(url);

   free(sys);

   if (opt.save_urls)
      save_url(url);
}

/* Amongst all the fluff, this is the bit that does the actual work. */
static void
cb_shoot(void *data)
{
   char               *url;
   char               *validurl;

   url = get_url_from_paste_buffer();

   D(("In cb_shoot: url -->%s<--\n", url));

   if ((validurl = validate_url(url)) == NULL)
      return;

   D(("In cb_shoot: valid url -->%s<--\n", validurl));

   Epplet_register_focus_out_handler(work_out_cb, NULL);
   display_string(validurl);	/*richlowe is a hamster */
   handle_url(validurl, data);
   return;
}

/* Someone clicked on the "New!" button */
static void
cb_btn_file_url(void *data)
{
   if (dtext.str && strcmp(dtext.str, "E-UrlWatch"))
     {
	handle_url(dtext.str, "www");
     }
   return;
   data = NULL;
}

static void
create_epplet_layout(void)
{
   Epplet_gadget_show(lbl_url = Epplet_create_label(2, 34, " ", 0));

   Epplet_gadget_show(btn_close =
		      Epplet_create_std_button("CLOSE", 2, 2, cb_close, NULL));
   Epplet_gadget_show(btn_help =
		      Epplet_create_std_button("HELP", 16, 2, cb_help, NULL));
   Epplet_gadget_show(btn_www =
		      Epplet_create_text_button("WWW",
						4, 17, 28, 13,
						cb_shoot, "www"));
   Epplet_gadget_show(btn_ftp =
		      Epplet_create_text_button("FTP",
						34, 17, 28, 13,
						cb_shoot, "ftp"));
   Epplet_gadget_show(btn_wget =
		      Epplet_create_text_button("GET",
						64, 17, 28, 13,
						cb_shoot, "get"));
   btn_file_url = Epplet_create_text_button("New!",
					    30, 2, 34, 12,
					    cb_btn_file_url, NULL);
   Epplet_gadget_show(btn_conf =
		      Epplet_create_std_button("CONFIGURE",
					       82, 2, cb_config, NULL));
   url_p = Epplet_create_popup();
   Epplet_add_popup_entry(url_p, "Url List is currently empty", NULL, NULL,
			  NULL);
   Epplet_gadget_show(btn_urllist =
		      Epplet_create_popupbutton(NULL, NULL, 68, 2, 12, 12,
						"ARROW_DOWN", url_p));
   if (opt.check_url_file)
      Epplet_timer(check_url_file, NULL, 1, "URLCHECK_TIMER");
   display_string("Welcome to E-UrlWatch ;-)");
}

static void
clean_exit(void)
{
   save_config();
   Epplet_cleanup();
}

static void
in_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(btn_urllist);
	Epplet_gadget_show(btn_conf);
	Epplet_gadget_show(btn_help);
	Epplet_gadget_show(btn_close);
	Epplet_gadget_show(lbl_url);
     }
   return;
   data = NULL;
}

static void
out_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(btn_close);
	Epplet_gadget_hide(btn_help);
	Epplet_gadget_hide(btn_conf);
	Epplet_gadget_hide(btn_urllist);
	Epplet_gadget_hide(lbl_url);
     }
   return;
   data = NULL;
}

static void
work_out_cb(void *data, Window w)
{
   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(btn_close);
	Epplet_gadget_hide(btn_help);
	Epplet_gadget_hide(btn_conf);
	Epplet_gadget_hide(btn_urllist);
     }
   return;
   data = NULL;
}

int
main(int argc, char **argv)
{
   int                 prio;

   /* Lower priority to save cpu cycles */
   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(clean_exit);
   Epplet_Init("E-UrlWatch", "0.1",
	       "Enlightenment Url Watcher Epplet", 6, 3, argc, argv, 0);
   Epplet_load_config();
   load_config();
   create_epplet_layout();
   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_show();
   Epplet_Loop();
   return 0;
}
