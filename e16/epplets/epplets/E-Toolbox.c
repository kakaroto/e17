/*
 * Copyright (C) 2000, Michael Jennings
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

static const char cvs_ident[] = "$Id$";

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include "epplet.h"

#ifndef __GNUC__
# define __attribute__(x)
#endif

#define IMAGE_DIR   EROOT "/epplet_data/E-Toolbox/"

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif
#define BEGMATCH(a, b)  (!strncasecmp((a), (b), (sizeof(b) - 1)))
#define NONULL(x)       ((x) ? (x) : (""))

typedef struct toolbutton_struct {
  Epplet_gadget gad;
  char *label, *image;
  unsigned short x, y, w, h;
  char *prog;
} toolbutton_t;

toolbutton_t *buttons;
Epplet_gadget close_button, cfg_button, cfg_popup;
unsigned long idx = 0, button_cnt = 0;
Window config_win = None;
int w = 6, h = 3;

static void close_cb(void *data);
static int delete_cb(void *data, Window win);
static void apply_config(void);
__attribute__((unused)) static void ok_cb(void *data);
__attribute__((unused)) static void apply_cb(void *data);
__attribute__((unused)) static void cancel_cb(void *data);
static void config_cb(void *data);
static void save_config(void);

static void
close_cb(void *data)
{
  Epplet_unremember();
  Esync();
  exit(0);
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
  __attribute__((unused)) char buff[1024];

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
  __attribute__((unused)) char buff[128];

  if (config_win) {
    return;
  }

  config_win = Epplet_create_window_config(200, 230, "E-Toolbox Configuration", ok_cb, NULL, apply_cb, NULL, cancel_cb, NULL);

  Epplet_window_show(config_win);
  Epplet_window_pop_context();

  return;
  data = NULL;
}

static void
parse_config(char *argv0) {

  char *tmp, buff[40];
  int new_w, new_h;

  new_w = w;
  new_h = h;
  tmp = Epplet_query_config("width");
  if (tmp) {
    new_w = atoi(tmp);
  }
  tmp = Epplet_query_config("height");
  if (tmp) {
    new_h = atoi(tmp);
  }
  if (new_w != w || new_h != h) {
    char tmp_w[8], tmp_h[8];

    Esnprintf(tmp_w, sizeof(tmp_w), "%d", new_w);
    Esnprintf(tmp_h, sizeof(tmp_h), "%d", new_h);
    Epplet_modify_config("width", tmp_w);
    Epplet_modify_config("height", tmp_h);
    Epplet_save_config();
    Epplet_unremember();
    Epplet_cleanup();
    Esync();
    execlp(argv0, argv0, "-w", tmp_w, "-h", tmp_h, NULL);
    fprintf(stderr, "execlp() failed -- %s\n", strerror(errno));
  }

  for (; 1; button_cnt++) {
    if (button_cnt) {
      buttons = (toolbutton_t *) realloc(buttons, sizeof(toolbutton_t) * (button_cnt + 1));
    } else {
      buttons = (toolbutton_t *) malloc(sizeof(toolbutton_t) * (button_cnt + 1));
    }

    Esnprintf(buff, sizeof(buff), "button_%lu", button_cnt);
    tmp = Epplet_query_config(buff);
    if (!tmp) {
      break;
    }
    buttons[button_cnt].prog = tmp;

    Esnprintf(buff, sizeof(buff), "button_%lu_image", button_cnt);
    buttons[button_cnt].image = Epplet_query_config(buff);
    Esnprintf(buff, sizeof(buff), "button_%lu_label", button_cnt);
    buttons[button_cnt].label = Epplet_query_config(buff);

    Esnprintf(buff, sizeof(buff), "button_%lu_x", button_cnt);
    buttons[button_cnt].x = (unsigned short) atoi(Epplet_query_config_def(buff, "0"));
    Esnprintf(buff, sizeof(buff), "button_%lu_y", button_cnt);
    buttons[button_cnt].y = (unsigned short) atoi(Epplet_query_config_def(buff, "0"));
    Esnprintf(buff, sizeof(buff), "button_%lu_w", button_cnt);
    buttons[button_cnt].w = (unsigned short) atoi(Epplet_query_config_def(buff, "1"));
    Esnprintf(buff, sizeof(buff), "button_%lu_h", button_cnt);
    buttons[button_cnt].h = (unsigned short) atoi(Epplet_query_config_def(buff, "1"));

  }
  if (button_cnt == 0) {
    toolbutton_t def[] = { { NULL, NULL, "<close>", 0, 0, 1, 1, "<exit>" },
			   { NULL, "efm", NULL, 1, 0, 2, 1, "efm" },
			   { NULL, "ee2", NULL, 3, 0, 2, 1, "ee2" },
			   { NULL, NULL, "<configure>", 5, 0, 1, 1, "<config>" },
			   { NULL, NULL, "eterm.png", 0, 1, 2, 2, "Eterm" },
			   { NULL, NULL, "mail.png", 2, 1, 2, 2, "Eterm -t mutt" },
			   { NULL, NULL, "netscape.png", 4, 1, 2, 2, "netscape" }
    };

    button_cnt = 7;
    buttons = (toolbutton_t *) realloc(buttons, sizeof(toolbutton_t) * (button_cnt));
    buttons[0] = def[0];
    buttons[1] = def[1];
    buttons[2] = def[2];
    buttons[3] = def[3];
    buttons[4] = def[4];
    buttons[5] = def[5];
    buttons[6] = def[6];
    save_config();
  }
}

static void
save_config(void)
{
  char buff[25], buff2[8];
  unsigned long i;

  for (i = 0; i < button_cnt; i++) {
    Esnprintf(buff, sizeof(buff), "button_%lu", i);
    Epplet_modify_config(buff, buttons[i].prog);
    if (buttons[i].image) {
      Esnprintf(buff, sizeof(buff), "button_%lu_image", i);
      Epplet_modify_config(buff, buttons[i].image);
    }
    if (buttons[i].label) {
      Esnprintf(buff, sizeof(buff), "button_%lu_label", i);
      Epplet_modify_config(buff, buttons[i].label);
    }

    D(("x == %hu, y == %hu, w == %hu, h == %hu\n", buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h));
    Esnprintf(buff, sizeof(buff), "button_%lu_x", i);
    Esnprintf(buff2, sizeof(buff2), "%d", (int) buttons[i].x);
    Epplet_modify_config(buff, buff2);
    Esnprintf(buff, sizeof(buff), "button_%lu_y", i);
    Esnprintf(buff2, sizeof(buff2), "%d", (int) buttons[i].y);
    Epplet_modify_config(buff, buff2);
    Esnprintf(buff, sizeof(buff), "button_%lu_w", i);
    Esnprintf(buff2, sizeof(buff2), "%d", (int) buttons[i].w);
    Epplet_modify_config(buff, buff2);
    Esnprintf(buff, sizeof(buff), "button_%lu_h", i);
    Esnprintf(buff2, sizeof(buff2), "%d", (int) buttons[i].h);
    Epplet_modify_config(buff, buff2);
  }
}

static void
button_cb(void *data)
{
  toolbutton_t *b = data;

  if (b && b->prog) {
    if (*(b->prog) == '<') {
      if (!strcasecmp(b->prog, "<exit>")) {
	close_cb(NULL);
	return;
      } else if (!strcasecmp(b->prog, "<config>")) {
	config_cb(NULL);
	return;
      }
    }
    Epplet_spawn_command(b->prog);
  }
}

int
main(int argc, char **argv)
{
  int j = 0;

  atexit(Epplet_cleanup);

  for (j = 1; j < argc; j++) {
    if ((!strcmp("-w", argv[j])) && (argc - j > 1)) {
      w = atoi(argv[++j]);
    } else if ((!strcmp("-h", argv[j])) && (argc - j > 1)) {
      h = atoi(argv[++j]);
    }
  }

  Epplet_Init("E-Toolbox", "0.1", "Enlightenment Toolbox Epplet", w, h, argc, argv, 0);
  Epplet_load_config();
  parse_config(argv[0]);

  for (j = 0; j < (int) button_cnt; j++) {
    if (buttons[j].image && *(buttons[j].image) == '<') {
      char *std, *pbuff;

      std = strdup(buttons[j].image + 1);
      for (pbuff = std; *pbuff; pbuff++) {
        if (*pbuff == '>') {
          *pbuff = 0;
        } else {
          *pbuff = toupper(*pbuff);
        }
      }
      buttons[j].gad = Epplet_create_button(NULL, NULL, (buttons[j].x * 16) + 2, (buttons[j].y * 16) + 2,
                                            (buttons[j].w - 1) * 16 + 12, (buttons[j].h - 1) * 16 + 12,
                                            std, None, NULL, button_cb, &buttons[j]);
      free(std);
    } else {
      char *s = NULL;

      if (buttons[j].image && !strchr(buttons[j].image, '/')) {
        s = buttons[j].image;
        buttons[j].image = (char *) malloc(sizeof(IMAGE_DIR) + strlen(s) + 1);
        sprintf(buttons[j].image, IMAGE_DIR "%s", s);
      }
      buttons[j].gad = Epplet_create_button(buttons[j].label, buttons[j].image,
                                            (buttons[j].x * 16) + 2, (buttons[j].y * 16) + 2,
                                            (buttons[j].w - 1) * 16 + 12, (buttons[j].h - 1) * 16 + 12,
                                            NULL, None, NULL, button_cb, &buttons[j]);
      if (s) {
        free(buttons[j].image);
        buttons[j].image = s;
      }
    }
    Epplet_gadget_show(buttons[j].gad);
  }

  Epplet_register_delete_event_handler(delete_cb, NULL);
  Epplet_show();
  Epplet_Loop();

  return 0;
}
