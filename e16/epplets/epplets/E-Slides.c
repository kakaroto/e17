/*
 * Copyright (C) 1999-2000, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

static const char   cvs_ident[] =
   "$Id$";

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include "epplet.h"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

#define PREV_PIC()      do {if (idx == 1) idx = image_cnt - 1; else if (idx == 0) idx = image_cnt - 2; else idx -= 2;} while (0)
#define CUR_PIC()       ((idx == 0) ? (image_cnt - 1) : (idx - 1))
#define NEXT_PIC()      ((void) 0)
#define INC_PIC()       do {idx++; if (idx == image_cnt) idx = 0;} while (0)

#define AUTOBG_OFF      0
#define AUTOBG_TILED    1
#define AUTOBG_SCALED   2
#define AUTOBG_PSCALED  3

Epplet_gadget       close_button, play_button, pause_button, prev_button,
   next_button, cfg_button, cfg_popup, picture;
Epplet_gadget       cfg_tb_path, cfg_tb_delay, cfg_tb_zoom;
unsigned long       idx = 0, image_cnt = 0;
double              delay = 5.0;
char              **filenames = NULL, *path, *zoom_cmd;
unsigned char       paused = 0, randomize = 0, auto_setbg =
   AUTOBG_OFF, maintain_aspect = 0;
int                 cfg_auto_setbg = AUTOBG_OFF, cfg_maintain_aspect =
   0, cfg_randomize = 0;
Window              zoom_win = None, config_win = None;
int                 w = 3, h = 3;

static char       **dirscan(char *dir, unsigned long *num);
static char       **randomize_file_list(char **names, unsigned long num);
static char       **sort_file_list(char **names, unsigned long num);
static void         set_background(int tiled, int keep_aspect);
static void         change_image(void *data);
static void         close_cb(void *data);
static void         zoom_cb(void *data);
static void         play_cb(void *data);
static void         cfg_popup_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
static int          delete_cb(void *data, Window win);
static void         apply_config(void);
static void         ok_cb(void *data);
static void         apply_cb(void *data);
static void         cancel_cb(void *data);
static void         config_cb(void *data);
static unsigned char get_images(char *image_path);

static char       **
dirscan(char *dir, unsigned long *num)
{
   unsigned long       i, dirlen;
   DIR                *dirp;
   char              **names;
   struct dirent      *dp;
   struct stat         filestat;
   char                fullname[256];

   D(("dirscan(\"%s\", %8p) called.\n", dir, num));

   if ((!dir) || (!*dir))
     {
	*num = 0;
	return ((char **)NULL);
     }
   dirp = opendir(dir);
   if (!dirp)
     {
	*num = 0;
	return ((char **)NULL);
     }
   /* count # of entries in dir (worst case) */
   for (dirlen = 0; (dp = readdir(dirp)) != NULL; dirlen++);
   D((" -> Got %d entries.\n", dirlen));
   if (!dirlen)
     {
	closedir(dirp);
	*num = 0;
	return ((char **)NULL);
     }
   names = (char **)malloc(dirlen * sizeof(char *));
   D((" -> Storing names at %8p.\n", names));

   if (!names)
     {
	*num = 0;
	return ((char **)NULL);
     }
   rewinddir(dirp);
   for (i = 0; (dp = readdir(dirp)) != NULL;)
     {
	if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, "..")))
	  {
	     Esnprintf(fullname, sizeof(fullname), "%s/%s", dir, dp->d_name);
	     D((" -> About to stat() %s\n", fullname));
	     if (stat(fullname, &filestat))
	       {
		  D((" -> Couldn't stat() file %s -- %s\n", dp->d_name,
		     strerror(errno)));
	       }
	     else
	       {
		  if (S_ISREG(filestat.st_mode))
		    {
		       D((" -> Adding name \"%s\" at index %d (%8p)\n",
			  dp->d_name, i, names + i));
		       names[i] = strdup(dp->d_name);
		       i++;
		    }
		  else if (S_ISDIR(filestat.st_mode))
		    {
		       /* Recurse directories here at some point, maybe? */
		    }
	       }
	  }
     }

   if (i < dirlen)
     {
	dirlen = i;
     }
   if (!dirlen)
     {
	closedir(dirp);
	*num = 0;
	return ((char **)NULL);
     }
   closedir(dirp);
   *num = dirlen;
   names = (char **)realloc(names, dirlen * sizeof(char *));
   D((" -> Final directory length is %lu.  List moved to %8p\n", *num, names));

   if (randomize)
     {
	randomize_file_list(names, dirlen);
     }
   else
     {
	sort_file_list(names, dirlen);
     }
   return (names);
}

static char       **
randomize_file_list(char **names, unsigned long len)
{
   int                 r;
   unsigned long       i;
   char               *tmp;

   for (i = 0; i < len - 1; i++)
     {
	r = (int)((len - i - 1) * ((float)rand()) / (RAND_MAX + 1.0)) + i + 1;
	tmp = names[i];
	names[i] = names[r];
	names[r] = tmp;
     }
   return (names);
}

static char       **
sort_file_list(char **names, unsigned long len)
{
   unsigned long       i;
   unsigned char       done = 0;

   while (!done)
     {
	done = 1;
	for (i = 0; i < len - 1; i++)
	  {
	     if (strcmp(names[i], names[i + 1]) > 0)
	       {
		  char               *tmp;

		  tmp = names[i];
		  names[i] = names[i + 1];
		  names[i + 1] = tmp;
		  done = 0;
	       }
	  }
     }
   return (names);
}

static void
set_background(int tiled, int keep_aspect)
{

   unsigned char       current_desk = 0;
   char               *reply, *ptr, bg_name[64], buff[255];

   Epplet_send_ipc("goto_desktop ?");
   reply = Epplet_wait_for_ipc();
   if (!reply)
      return;
   if ((ptr = strchr(reply, ':')) != NULL)
     {
	current_desk = atoi(++ptr);
     }
   free(reply);

   Esnprintf(bg_name, sizeof(bg_name), "E_SLIDES_BG_%s", filenames[CUR_PIC()]);

   Esnprintf(buff, sizeof(buff), "background %s bg.file %s/%s", bg_name, path,
	     filenames[CUR_PIC()]);
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.solid 0 0 0", bg_name);
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.tile %d", bg_name, tiled);
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.keep_aspect %d", bg_name,
	     keep_aspect);
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.xperc %d", bg_name,
	     (tiled ? 0 : 1024));
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.yperc %d", bg_name,
	     (tiled ? 0 : 1024));
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.xjust %d", bg_name,
	     (tiled ? 0 : 512));
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "background %s bg.yjust %d", bg_name,
	     (tiled ? 0 : 512));
   Epplet_send_ipc(buff);
   Esnprintf(buff, sizeof(buff), "use_bg %s %d", bg_name, current_desk);
   Epplet_send_ipc(buff);
   Esync();
}

static void
change_image(void *data)
{

   Imlib_Image        *im = NULL;
   double              ratio = 0.0;
   unsigned long       first = idx;
   int                 new_w = 0, new_h = 0, new_x = 3, new_y = 3;

   if (!filenames)
      return;

   /* Test-load each image to make sure it's a valid image file. */
   for (;
	((filenames[idx] == NULL)
	 || ((im = imlib_load_image(filenames[idx])) == NULL));)
     {
	/* It isn't, so NULL out its name. */
	filenames[idx] = NULL;
	INC_PIC();
	if (idx == first)
	  {
	     char                buff[256];

	     /* They're all NULL now.  Time to give up. */
	     Esnprintf(buff, sizeof(buff),
		       "There don't seem to be any images in \"%s\".  Please choose another directory.\n",
		       path);
	     Epplet_dialog_ok(buff);
	     Esync();
	     config_cb(NULL);
	     return;
	  }
     }
   new_w = (w * 16 - 6);
   new_h = (h * 16 - 6);
   imlib_context_set_image(im);
   if (maintain_aspect)
     {
	ratio =
	   ((double)imlib_image_get_width() / imlib_image_get_height()) /
	   ((double)new_w / new_h);
	if (ratio > 1.0)
	  {
	     new_h /= ratio;
	  }
	else if (ratio != 1.0)
	  {
	     new_w *= ratio;
	  }
     }
   imlib_free_image();		/* Destroy the image, but keep it in cache. */

   new_x = ((w * 16) / 2) - (new_w / 2);
   new_y = ((h * 16) / 2) - (new_h / 2);
   Epplet_move_change_image(picture, new_x, new_y, new_w, new_h,
			    filenames[idx]);
   INC_PIC();
   switch (auto_setbg)
     {
     case AUTOBG_TILED:
	set_background(1, 1);
	break;
     case AUTOBG_SCALED:
	set_background(0, 0);
	break;
     case AUTOBG_PSCALED:
	set_background(0, 1);
	break;
     default:
	break;
     }

   Epplet_remove_timer("CHANGE_IMAGE");
   if (!paused)
     {
	Epplet_timer(change_image, NULL, delay, "CHANGE_IMAGE");
     }
   return;
   data = NULL;
}

static void
close_cb(void *data)
{

   Epplet_unremember();
   Esync();
   exit(0);
   data = NULL;
}

static void
zoom_cb(void *data)
{

   char                buff[1024];

   Esnprintf(buff, sizeof(buff), zoom_cmd, filenames[CUR_PIC()]);
   Epplet_spawn_command(buff);
   return;
   data = NULL;
}

static void
cfg_popup_cb(void *data)
{

   int                 n = (int)data;

   switch (n)
     {
     case 0:
	set_background(0, 0);
	break;
     case 1:
	set_background(1, 1);
	break;
     case 2:
	set_background(0, 1);
	break;
     case 3:
	zoom_cb(NULL);
	break;
     case 4:
	config_cb(NULL);
	break;
     default:
	break;
     }
}

static void
play_cb(void *data)
{

   int                 op = (int)data;

   switch (op)
     {
     case -1:
	/* Previous image */
	PREV_PIC();
	change_image(NULL);
	break;
     case 0:
	/* Pause */
	Epplet_remove_timer("CHANGE_IMAGE");
	paused = 1;
	Epplet_gadget_hide(pause_button);
	Epplet_gadget_show(play_button);
	break;
     case 1:
	/* Play */
	paused = 0;
	Epplet_gadget_hide(play_button);
	Epplet_gadget_show(pause_button);
	change_image(NULL);
	break;
     case 2:
	/* Next image */
	NEXT_PIC();
	change_image(NULL);
	break;
     default:
	break;
     }
   return;
}

static void
in_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(close_button);
	Epplet_gadget_show(cfg_button);
	Epplet_gadget_show(prev_button);
	Epplet_gadget_show(next_button);
	if (paused)
	  {
	     Epplet_gadget_show(play_button);
	  }
	else
	  {
	     Epplet_gadget_show(pause_button);
	  }
     }
   return;
   data = NULL;
}

static void
out_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(close_button);
	Epplet_gadget_hide(cfg_button);
	Epplet_gadget_hide(prev_button);
	Epplet_gadget_hide(next_button);
	Epplet_gadget_hide(play_button);
	Epplet_gadget_hide(pause_button);
     }
   return;
   data = NULL;
}

static int
delete_cb(void *data, Window win)
{
   config_win = None;
   return 1;
   win = (Window) 0;
   data = NULL;
}

static void
apply_config(void)
{
   char                buff[1024];

   auto_setbg = cfg_auto_setbg;
   if (auto_setbg == AUTOBG_TILED)
     {
	Epplet_modify_config("auto_setbg", "tiled");
     }
   else if (auto_setbg == AUTOBG_SCALED)
     {
	Epplet_modify_config("auto_setbg", "scaled");
     }
   else if (auto_setbg == AUTOBG_PSCALED)
     {
	Epplet_modify_config("auto_setbg", "scaled_with_aspect");
     }
   else
     {
	Epplet_modify_config("auto_setbg", "off");
     }

   strcpy(buff, Epplet_textbox_contents(cfg_tb_path));
   if (strcmp(path, buff))
     {
	if (get_images(buff))
	  {
	     free(path);
	     path = strdup(buff);
	     Epplet_modify_config("image_dir", path);
	     idx = 0;
	  }
     }

   strcpy(buff, Epplet_textbox_contents(cfg_tb_delay));
   if ((delay = atof(buff)) != 0.0)
     {
	Epplet_modify_config("delay", buff);
     }
   else
     {
	delay = atof(Epplet_query_config_def("delay", "2.0"));
     }

   zoom_cmd = Epplet_textbox_contents(cfg_tb_zoom);
   Epplet_modify_config("zoom_prog", zoom_cmd);

   if (randomize != cfg_randomize)
     {
	if (cfg_randomize)
	  {
	     randomize_file_list(filenames, image_cnt);
	  }
	else
	  {
	     sort_file_list(filenames, image_cnt);
	  }
	idx = 0;
     }
   randomize = cfg_randomize;
   sprintf(buff, "%d", randomize);
   Epplet_modify_config("randomize", buff);

   maintain_aspect = cfg_maintain_aspect;
   sprintf(buff, "%d", maintain_aspect);
   Epplet_modify_config("maintain_aspect", buff);
   change_image(NULL);
}

static void
ok_cb(void *data)
{
   apply_config();
   Epplet_save_config();
   Epplet_window_destroy(config_win);
   config_win = None;
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
   Epplet_window_destroy(config_win);
   config_win = None;
   return;
   data = NULL;
}

static void
auto_popup_cb(void *data)
{
   cfg_auto_setbg = (int)data;
}

static void
config_cb(void *data)
{
   char                buff[128];
   Epplet_gadget       auto_popup;

   if (config_win)
     {
	return;
     }

   config_win =
      Epplet_create_window_config(200, 250, "E-Slides Configuration", ok_cb,
				  NULL, apply_cb, NULL, cancel_cb, NULL);

   Epplet_gadget_show(Epplet_create_label
		      (4, 4, "Directory to scan for images:", 2));
   Epplet_gadget_show(cfg_tb_path =
		      Epplet_create_textbox(NULL, path, 4, 18, 192, 20, 2, NULL,
					    NULL));

   sprintf(buff, "%3.2f", delay);
   Epplet_gadget_show(Epplet_create_label
		      (4, 50, "Delay between images (seconds):", 2));
   Epplet_gadget_show(cfg_tb_delay =
		      Epplet_create_textbox(NULL, buff, 4, 64, 192, 20, 2, NULL,
					    NULL));

   Epplet_gadget_show(Epplet_create_label
		      (4, 96, "Zoom program command line:", 2));
   Epplet_gadget_show(cfg_tb_zoom =
		      Epplet_create_textbox(NULL, zoom_cmd, 4, 110, 192, 20, 2,
					    NULL, NULL));

   auto_popup = Epplet_create_popup();
   Epplet_add_popup_entry(auto_popup, "Tiled", NULL, auto_popup_cb,
			  (void *)AUTOBG_TILED);
   Epplet_add_popup_entry(auto_popup, "Scaled", NULL, auto_popup_cb,
			  (void *)AUTOBG_SCALED);
   Epplet_add_popup_entry(auto_popup, "Scaled, Keep Aspect", NULL,
			  auto_popup_cb, (void *)AUTOBG_PSCALED);
   Epplet_add_popup_entry(auto_popup, "Off", NULL, auto_popup_cb,
			  (void *)AUTOBG_OFF);

   Epplet_gadget_show(Epplet_create_popupbutton
		      (NULL, NULL, 4, 142, 12, 12, "ARROW_UP", auto_popup));
   Epplet_gadget_show(Epplet_create_label
		      (20, 142, "Automatically set desktop", 2));
   Epplet_gadget_show(Epplet_create_label
		      (20, 156, "background when image changes?", 2));

   cfg_maintain_aspect = maintain_aspect;
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 4, 175, 12, 12, &cfg_maintain_aspect, NULL,
		       NULL));
   Epplet_gadget_show(Epplet_create_label
		      (20, 175, "Maintain aspect ratio when", 2));
   Epplet_gadget_show(Epplet_create_label
		      (20, 190, "displaying images in epplet?", 2));

   cfg_randomize = randomize;
   Epplet_gadget_show(Epplet_create_togglebutton
		      (NULL, NULL, 4, 210, 12, 12, &cfg_randomize, NULL, NULL));
   Epplet_gadget_show(Epplet_create_label(20, 210, "Randomize image list?", 2));

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

static void
parse_config(void)
{

   char                buff[1024], *s;

   path = Epplet_query_config("image_dir");
   if (path == NULL)
     {
	Esnprintf(buff, sizeof(buff), "%s/backgrounds", Epplet_e16_user_dir());
	path = strdup(buff);
	Epplet_add_config("image_dir", buff);
     }
   else
     {
	path = strdup(path);
     }
   s = Epplet_query_config("delay");
   if (s != NULL)
     {
	delay = atof(s);
     }
   else
     {
	Epplet_add_config("delay", "5.0");
     }
   zoom_cmd = Epplet_query_config_def("zoom_prog", "ee %s");
   s = Epplet_query_config_def("auto_setbg", "off");
   if (!strcasecmp(s, "tiled"))
     {
	auto_setbg = AUTOBG_TILED;
     }
   else if (!strcasecmp(s, "scaled"))
     {
	auto_setbg = AUTOBG_SCALED;
     }
   else if (!strcasecmp(s, "scaled_with_aspect"))
     {
	auto_setbg = AUTOBG_PSCALED;
     }
   maintain_aspect = atoi(Epplet_query_config_def("maintain_aspect", "0"));
   randomize = atoi(Epplet_query_config_def("randomize", "0"));
}

static unsigned char
get_images(char *image_path)
{
   char              **temp;
   unsigned long       cnt;

   temp = dirscan(image_path, &cnt);
   if (cnt == 0)
     {
	char                err[255];

	Esnprintf(err, sizeof(err), "Unable to find any files in %s!",
		  image_path);
	Epplet_dialog_ok(err);
	Esync();
	return 0;
     }
   else if (idx >= cnt)
     {
	idx = 0;
     }
   chdir(image_path);

   if (filenames)
     {
	unsigned int        i;

	for (i = 0; i < image_cnt; i++)
	  {
	     if (filenames[i])
	       {
		  free(filenames[i]);
	       }
	  }
	free(filenames);
     }
   image_cnt = cnt;
   filenames = temp;
   return 1;
}

int
main(int argc, char **argv)
{

   int                 prio, j = 0;

   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   atexit(Epplet_cleanup);
   srand(getpid() * time(NULL) % ((unsigned int)-1));

   for (j = 1; j < argc; j++)
     {
	if ((!strcmp("-w", argv[j])) && (argc - j > 1))
	  {
	     w = atoi(argv[++j]);
	     if (w < 3)
	       {
		  w = 3;
	       }
	  }
	else if ((!strcmp("-h", argv[j])) && (argc - j > 1))
	  {
	     h = atoi(argv[++j]);
	     if (h < 3)
	       {
		  h = 3;
	       }
	  }
     }

   Epplet_Init("E-Slides", "0.3", "Enlightenment Slideshow Epplet", w, h, argc,
	       argv, 0);
   Epplet_load_config();
   parse_config();

   cfg_popup = Epplet_create_popup();
   Epplet_add_popup_entry(cfg_popup, "Set Background", NULL, cfg_popup_cb,
			  (void *)0);
   Epplet_add_popup_entry(cfg_popup, "Tile as Background", NULL, cfg_popup_cb,
			  (void *)1);
   Epplet_add_popup_entry(cfg_popup, "Set Background, Keep Aspect", NULL,
			  cfg_popup_cb, (void *)2);
   Epplet_add_popup_entry(cfg_popup, "Open Image Viewer", NULL, cfg_popup_cb,
			  (void *)3);
   Epplet_add_popup_entry(cfg_popup, "Configure E-Slides", NULL, cfg_popup_cb,
			  (void *)4);

   close_button =
      Epplet_create_button(NULL, NULL, 3, 3, 0, 0, "CLOSE", 0, NULL, close_cb,
			   NULL);
   prev_button =
      Epplet_create_button(NULL, NULL, 3, ((16 * h) - 15), 0, 0, "PREVIOUS", 0,
			   NULL, play_cb, (void *)(-1));
   play_button =
      Epplet_create_button(NULL, NULL, ((16 * w / 2) - 6), ((16 * h) - 15), 0,
			   0, "PLAY", 0, NULL, play_cb, (void *)(1));
   pause_button =
      Epplet_create_button(NULL, NULL, ((16 * w / 2) - 6), ((16 * h) - 15), 0,
			   0, "PAUSE", 0, NULL, play_cb, (void *)(0));
   cfg_button =
      Epplet_create_popupbutton(NULL, NULL, ((16 * w) - 15), 3, 0, 0,
				"CONFIGURE", cfg_popup);
   next_button =
      Epplet_create_button(NULL, NULL, ((16 * w) - 15), ((16 * h) - 15), 0, 0,
			   "NEXT", 0, NULL, play_cb, (void *)(2));
   Epplet_gadget_show(picture =
		      Epplet_create_image(3, 3, ((w * 16) - 6), ((h * 16) - 6),
					  NULL));
   Epplet_show();
   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_register_delete_event_handler(delete_cb, NULL);

   if (get_images(path))
     {
	change_image(NULL);
     }
   else
     {
	config_cb(NULL);
     }
   Epplet_Loop();

   return 0;
}
