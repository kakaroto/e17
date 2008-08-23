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

Epplet_gadget       close_button, cfg_button, picture, label;
Epplet_gadget       cfg_tb_image, cfg_tb_delay, cfg_tb_ans_file;
unsigned long       idx = 0, cnt = 0;
double              delay = 2.0;
char               *image = NULL, *ans_file, **answers = NULL;
Window              config_win = None;
int                 w = 6, h = 3;
unsigned char       focus = 0;

static void         button_click_cb(void *data);
static void         button_show_cb(void *data);
static void         close_cb(void *data);
static void         in_cb(void *data, Window w);
static void         out_cb(void *data, Window w);
static int          delete_cb(void *data, Window win);
static void         apply_config(void);
static void         ok_cb(void *data);
static void         apply_cb(void *data);
static void         cancel_cb(void *data);
static void         config_cb(void *data);
static unsigned long parse_answers(char *path);
static void         parse_config(void);

static void
button_click_cb(void *data)
{
   unsigned long       last_idx = idx;

   do
     {
	idx = (unsigned long)(cnt * ((float)rand()) / (RAND_MAX + 1.0));
	if (idx >= cnt)
	  {
      /********** FIXME ***********/
	     fprintf(stderr, "Eek!\n");
	     fflush(stderr);
	     idx = last_idx;
	  }
     }
   while (idx == last_idx);

   Epplet_gadget_hide(picture);
   Epplet_gadget_hide(close_button);
   Epplet_gadget_hide(cfg_button);
   Epplet_change_label(label, answers[idx]);
   Epplet_gadget_show(label);

   Epplet_timer(button_show_cb, NULL, delay, "SHOW_BUTTON");
   return;
   data = NULL;
}

static void
button_show_cb(void *data)
{
   Epplet_gadget_hide(label);
   Epplet_gadget_show(picture);
   if (focus)
     {
	Epplet_gadget_show(close_button);
	Epplet_gadget_show(cfg_button);
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
in_cb(void *data, Window w)
{

   if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(close_button);
	Epplet_gadget_show(cfg_button);
     }
   focus = 1;
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
     }
   focus = 0;
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
   char               *s;

   s = Epplet_textbox_contents(cfg_tb_image);
   if (*s && strcmp(s, image))
     {
	Epplet_modify_config("image", s);
	image = Epplet_query_config("image");
	Epplet_change_button_image(picture, image);
     }

   s = Epplet_textbox_contents(cfg_tb_ans_file);
   if (*s && strcmp(s, ans_file) && parse_answers(s))
     {
	Epplet_modify_config("answers", s);
	ans_file = Epplet_query_config("answers");
     }

   s = Epplet_textbox_contents(cfg_tb_delay);
   if (*s)
     {
	delay = atof(s);
	if (delay)
	  {
	     Epplet_modify_config("delay", s);
	  }
	else
	  {
	     delay = 2.0;
	  }
     }
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
config_cb(void *data)
{
   char                buff[128];

   if (config_win)
     {
	return;
     }

   config_win =
      Epplet_create_window_config(300, 160, "E-Magic Configuration", ok_cb,
				  NULL, apply_cb, NULL, cancel_cb, NULL);

   Epplet_gadget_show(Epplet_create_label(4, 4, "Path to Image:", 2));
   Epplet_gadget_show(cfg_tb_image =
		      Epplet_create_textbox(NULL, image, 4, 18, 292, 20, 2,
					    NULL, NULL));

   Epplet_gadget_show(Epplet_create_label(4, 50, "Path to Answers File:", 2));
   Epplet_gadget_show(cfg_tb_ans_file =
		      Epplet_create_textbox(NULL, ans_file, 4, 64, 292, 20, 2,
					    NULL, NULL));

   sprintf(buff, "%3.2f", delay);
   Epplet_gadget_show(Epplet_create_label
		      (4, 96, "Show answer for how many seconds?", 2));
   Epplet_gadget_show(cfg_tb_delay =
		      Epplet_create_textbox(NULL, buff, 4, 110, 192, 20, 2,
					    NULL, NULL));

   Epplet_window_show(config_win);
   Epplet_window_pop_context();

   return;
   data = NULL;
}

/* chomp() removes leading and trailing whitespace/quotes from a string */
static char        *
chomp(char *s)
{
   register char      *front, *back;

   for (front = s; *front && isspace(*front); front++);
   for (back = s + strlen(s) - 1; *back && isspace(*back) && back > front;
	back--);

   *(++back) = 0;
   if (front != s)
     {
	memmove(s, front, back - front + 1);
     }
   return (s);
}

static unsigned long
parse_answers(char *path)
{
   char                buff[1024];
   FILE               *fp;
   unsigned char       done = 0;
   unsigned long       len;

   fp = fopen(path, "r");
   if (fp == NULL)
     {
	return 0;
     }
   for (; !done; idx++)
     {
	for (; !(done = !(fgets(buff, sizeof(buff), fp)));)
	  {
	     /* First, skip any leading lines of whitespace. */
	     chomp(buff);
	     if (*buff)
	       {
		  break;
	       }
	  }
	if (done)
	  {
	     break;
	  }
	if (cnt)
	  {
	     answers = (char **)realloc(answers, sizeof(char *) * (++cnt));
	  }
	else
	  {
	     cnt = 1;
	     answers = (char **)malloc(sizeof(char *));
	  }
	len = strlen(buff) + 1;
	answers[idx] = (char *)malloc(len);
	strcpy(answers[idx], buff);

	for (; !(done = !(fgets(buff, sizeof(buff), fp)));)
	  {
	     /* Now, add all lines up to the next all-whitespace line. */
	     chomp(buff);
	     if (!(*buff))
	       {
		  break;
	       }
	     len += strlen(buff) + 1;
	     answers[idx] = (char *)realloc(answers[idx], len);
	     strcat(answers[idx], "\n");
	     strcat(answers[idx], buff);
	  }
     }

   fclose(fp);
   return (cnt);
}

static void
parse_config(void)
{
   char                buff[1024], *s, ss[1024];

   image = Epplet_query_config_def("image", "8ball.png");

   Esnprintf(ss, sizeof(ss), "%s/answers.txt", Epplet_data_dir());
   ans_file = Epplet_query_config_def("answers", ss);
   if (!parse_answers(ans_file) && !parse_answers(ans_file = ss))
     {
	/* Couldn't find the answers.  Exit. */
	Esnprintf(buff, sizeof(buff),
		  "E-Magic:  Missing/invalid answers file \"%s\"", ans_file);
	Epplet_dialog_ok(buff);
	Esync();
	exit(-1);
     }

   s = Epplet_query_config("delay");
   if (s != NULL)
     {
	delay = atof(s);
     }
   else
     {
	Epplet_add_config("delay", "2.0");
     }
}

int
main(int argc, char **argv)
{
   srand(getpid() ^ time(NULL));
   atexit(Epplet_cleanup);

   Epplet_Init("E-Magic", "0.1", "Enlightenment Magic Eight Ball Epplet", w, h,
	       argc, argv, 0);
   Epplet_load_config();
   parse_config();

   label = Epplet_create_label(6, 6, "", 2);
   Epplet_gadget_show(picture =
		      Epplet_create_image_button(image, 3, 3, ((w * 16) - 6),
						 ((h * 16) - 6),
						 button_click_cb, NULL));
   close_button = Epplet_create_std_button("CLOSE", 3, 3, close_cb, NULL);
   cfg_button =
      Epplet_create_std_button("CONFIGURE", ((w * 16) - 15), 3, config_cb,
			       NULL);

   Epplet_register_focus_in_handler(in_cb, NULL);
   Epplet_register_focus_out_handler(out_cb, NULL);
   Epplet_register_delete_event_handler(delete_cb, NULL);
   Epplet_show();

   Epplet_Loop();

   return 0;
}
