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

#define DATA_DIR   EROOT "/epplet_data/E-Toolbox/"

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
  long popup;
  char *prog;
} toolbutton_t;

typedef struct tool_config_struct {
  Epplet_gadget cfg_tb_prog, cfg_tb_label,
    cfg_tb_image, cfg_tb_x, cfg_tb_y,
    cfg_tb_w, cfg_tb_h, cfg_tb_popup;
} tool_config_t;

toolbutton_t *buttons;
tool_config_t *cfg_gads;
Epplet_gadget close_button, cfg_button, cfg_popup;
unsigned long idx = 0, button_cnt = 0;
Window config_win = None, shade_win = None, main_win = None;;
int w = 6, h = 3;

static void create_shade_window(void);
static void close_cb(void *data);
static void shade_cb(void *data);
static void unshade_cb(void *data);
static int delete_cb(void *data, Window win);
static void apply_config(void);
static void ok_cb(void *data);
static void apply_cb(void *data);
static void cancel_cb(void *data);
static void config_cb(void *data);
static void save_config(void);

static void
create_shade_window(void)
{
  __attribute__((unused)) XWindowAttributes attr;

  shade_win = Epplet_create_window_borderless(16, 16, "E-Toolbox -- Shaded", 0);
  Epplet_gadget_show(Epplet_create_std_button("EJECT", 2, 2, unshade_cb, NULL));
  Epplet_window_pop_context();

  main_win = Epplet_get_main_window();
#if 0
  XGetWindowAttributes(Epplet_get_display(), main_win, &attr);
  XMoveWindow(Epplet_get_display(), shade_win, attr.x, attr.y);
#endif
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
shade_cb(void *data)
{
  Epplet_remember();
  Epplet_window_hide(main_win);
  Epplet_window_show(shade_win);
  return;
  data = NULL;
}

static void
unshade_cb(void *data)
{
  Epplet_window_hide(shade_win);
  Epplet_window_show(main_win);
  return;
  data = NULL;
}

static int
delete_cb(void *data, Window win)
{
  if (win == config_win) {
    config_win = None;
  } else if (win == shade_win) {
    close_cb(data);
  }
  return 1;
}

static void
apply_config(void)
{
  char *s;
  unsigned long i, j, n;

  for (i = 0, j = 0; i < (button_cnt + 3); i++) {
    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_prog);
    if (!(*s)) {
      continue;
    } else if (strcmp(buttons[i].prog, s)) {
      free(buttons[i].prog);
      buttons[i].prog = strdup(s);
    }
    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_label);
    if (!(*s)) {
      if (buttons[i].label) {
	free(buttons[i].label);
	buttons[i].label = NULL;
      }
    } else if (strcmp(buttons[i].label, s)) {
      free(buttons[i].label);
      buttons[i].label = strdup(s);
    }
    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_image);
    if (!(*s)) {
      if (buttons[i].image) {
	free(buttons[i].image);
	buttons[i].image = NULL;
      }
    } else if (strcmp(buttons[i].image, s)) {
      free(buttons[i].image);
      buttons[i].image = strdup(s);
    }

    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_x);
    if (*s) {
      n = strtoul(s, (char **) NULL, 10);
      buttons[i].x = n;
    }
    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_y);
    if (*s) {
      n = strtoul(s, (char **) NULL, 10);
      buttons[i].y = n;
    }
    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_w);
    if (*s) {
      n = strtoul(s, (char **) NULL, 10);
      buttons[i].w = n;
    }
    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_h);
    if (*s) {
      n = strtoul(s, (char **) NULL, 10);
      buttons[i].h = n;
    }

    s = Epplet_textbox_contents(cfg_gads[i].cfg_tb_popup);
    if (*s) {
      n = strtoul(s, (char **) NULL, 10);
      if ((n < button_cnt) && (Epplet_gadget_get_type(buttons[n].gad) == E_POPUP)) {
	buttons[i].popup = n;
      }
    }

    if (j != i) {
      buttons[j] = buttons[i];
    }
    j++;
  }
  button_cnt = j;
}

static void
ok_cb(void *data)
{
  apply_config();
  save_config();
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
  unsigned long h, y, i;

  if (config_win) {
    return;
  }

  cfg_gads = (tool_config_t *) malloc(sizeof(tool_config_t) * (button_cnt + 3));
  memset(cfg_gads, 0, sizeof(tool_config_t) * (button_cnt + 3));

  h = 30 + ((button_cnt + 3) * 30);
  config_win = Epplet_create_window_config(770, h, "E-Toolbox Configuration", ok_cb, NULL, apply_cb, NULL, cancel_cb, NULL);

  for (i = 0, y = 10; i < button_cnt; i++, y += 30) {
    int x = 0;
    Epplet_gadget tmp_gad;

    Esnprintf(buff, sizeof(buff), "Button %2lu:", i);
    tmp_gad = Epplet_create_label(4, y + 4, buff, 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_prog = Epplet_create_textbox(NULL, buttons[i].prog, x, y, 90, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_prog);
    x += 100;

    tmp_gad = Epplet_create_label(x, y + 4, "Label:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_label = Epplet_create_textbox(NULL, buttons[i].label, x, y, 90, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_label);
    x += 100;

    tmp_gad = Epplet_create_label(x, y + 4, "Image:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_image = Epplet_create_textbox(NULL, buttons[i].image, x, y, 90, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_image);
    x += 100;

    tmp_gad = Epplet_create_label(x, y + 4, "Row:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    Esnprintf(buff, sizeof(buff), "%u", (unsigned) buttons[i].x);
    cfg_gads[i].cfg_tb_x = Epplet_create_textbox(NULL, buff, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_x);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Column:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    Esnprintf(buff, sizeof(buff), "%u", (unsigned) buttons[i].y);
    cfg_gads[i].cfg_tb_y = Epplet_create_textbox(NULL, buff, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_y);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Width:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    Esnprintf(buff, sizeof(buff), "%u", (unsigned) buttons[i].w);
    cfg_gads[i].cfg_tb_w = Epplet_create_textbox(NULL, buff, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_w);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Height:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    Esnprintf(buff, sizeof(buff), "%u", (unsigned) buttons[i].h);
    cfg_gads[i].cfg_tb_h = Epplet_create_textbox(NULL, buff, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_h);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Popup:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    Esnprintf(buff, sizeof(buff), "%ld", buttons[i].popup);
    cfg_gads[i].cfg_tb_popup = Epplet_create_textbox(NULL, ((buttons[i].popup != -1) ? buff : NULL), x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_popup);
  }
  for (; i < button_cnt + 3; i++, y += 30) {
    int x = 0;
    Epplet_gadget tmp_gad;

    Esnprintf(buff, sizeof(buff), "Button %2lu:", i);
    tmp_gad = Epplet_create_label(4, y + 4, buff, 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_prog = Epplet_create_textbox(NULL, NULL, x, y, 90, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_prog);
    x += 100;

    tmp_gad = Epplet_create_label(x, y + 4, "Label:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_label = Epplet_create_textbox(NULL, NULL, x, y, 90, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_label);
    x += 100;

    tmp_gad = Epplet_create_label(x, y + 4, "Image:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_image = Epplet_create_textbox(NULL, NULL, x, y, 90, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_image);
    x += 100;

    tmp_gad = Epplet_create_label(x, y + 4, "Row:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_x = Epplet_create_textbox(NULL, NULL, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_x);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Column:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_y = Epplet_create_textbox(NULL, NULL, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_y);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Width:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_w = Epplet_create_textbox(NULL, NULL, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_w);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Height:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_h = Epplet_create_textbox(NULL, NULL, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_h);
    x += 25;

    tmp_gad = Epplet_create_label(x, y + 4, "Popup:", 2);
    Epplet_gadget_show(tmp_gad);
    x += Epplet_gadget_get_width(tmp_gad) + 8;

    cfg_gads[i].cfg_tb_popup = Epplet_create_textbox(NULL, NULL, x, y, 16, 20, 2, NULL, NULL);
    Epplet_gadget_show(cfg_gads[i].cfg_tb_popup);
  }

  Epplet_window_show(config_win);
  Epplet_window_pop_context();

  return;
  data = NULL;
}

static void
parse_config(char *argv0) {

  char *tmp, buff[40];
  int new_w, new_h;

  if (Epplet_query_config("button_0") == NULL) {
    Epplet_load_config_file(DATA_DIR "default.cfg");
  }
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
    save_config();
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
    memset(&buttons[button_cnt], 0, sizeof(toolbutton_t));

    Esnprintf(buff, sizeof(buff), "button_%lu", button_cnt);
    tmp = Epplet_query_config(buff);
    if (!tmp) {
      break;
    }
    buttons[button_cnt].prog = strdup(tmp);

    Esnprintf(buff, sizeof(buff), "button_%lu_image", button_cnt);
    buttons[button_cnt].image = strdup(Epplet_query_config_def(buff, ""));
    Esnprintf(buff, sizeof(buff), "button_%lu_label", button_cnt);
    buttons[button_cnt].label = strdup(Epplet_query_config_def(buff, ""));
    D(("label is %8p \"%s\", image is %8p \"%s\"\n", buttons[button_cnt].label,
       buttons[button_cnt].label, buttons[button_cnt].image, buttons[button_cnt].image));

    Esnprintf(buff, sizeof(buff), "button_%lu_popup", button_cnt);
    tmp = Epplet_query_config(buff);
    if (tmp) {
      buttons[button_cnt].popup = atoi(tmp);
    } else {
      buttons[button_cnt].popup = -1;
      Esnprintf(buff, sizeof(buff), "button_%lu_x", button_cnt);
      buttons[button_cnt].x = (unsigned short) atoi(Epplet_query_config_def(buff, "0"));
      Esnprintf(buff, sizeof(buff), "button_%lu_y", button_cnt);
      buttons[button_cnt].y = (unsigned short) atoi(Epplet_query_config_def(buff, "0"));
      Esnprintf(buff, sizeof(buff), "button_%lu_w", button_cnt);
      buttons[button_cnt].w = (unsigned short) atoi(Epplet_query_config_def(buff, "1"));
      Esnprintf(buff, sizeof(buff), "button_%lu_h", button_cnt);
      buttons[button_cnt].h = (unsigned short) atoi(Epplet_query_config_def(buff, "1"));
    }
  }
  save_config();
}

static void
save_config(void)
{
  char buff[25], buff2[8];
  unsigned long i;

  for (i = 0; i < button_cnt; i++) {
    Esnprintf(buff, sizeof(buff), "button_%lu", i);
    Epplet_modify_config(buff, buttons[i].prog);
    D(("label is %8p \"%s\", image is %8p \"%s\"\n", buttons[i].label, buttons[i].label,
       buttons[i].image, buttons[i].image));
    if (*(buttons[i].image)) {
      D(("Saving image\n"));
      Esnprintf(buff, sizeof(buff), "button_%lu_image", i);
      Epplet_modify_config(buff, buttons[i].image);
    }
    if (*(buttons[i].label)) {
      D(("Saving label\n"));
      Esnprintf(buff, sizeof(buff), "button_%lu_label", i);
      Epplet_modify_config(buff, buttons[i].label);
    }

    if (buttons[i].popup != -1) {
      Esnprintf(buff, sizeof(buff), "button_%lu_popup", i);
      Esnprintf(buff2, sizeof(buff2), "%d", (int) buttons[i].popup);
      Epplet_modify_config(buff, buff2);
    } else {      
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
      } else if (!strcasecmp(b->prog, "<shade>")) {
	shade_cb(NULL);
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
    char *std = NULL, *pbuff, *s = NULL;

    if (*(buttons[j].image) == '<') {
      std = strdup(buttons[j].image + 1);
      for (pbuff = std; *pbuff; pbuff++) {
        if (*pbuff == '>') {
          *pbuff = 0;
        } else {
          *pbuff = toupper(*pbuff);
        }
      }
    }
    if (*(buttons[j].image) && *(buttons[j].image) != '<' && !strchr(buttons[j].image, '/')) {
      s = buttons[j].image;
      buttons[j].image = (char *) malloc(sizeof(DATA_DIR) + strlen(s) + 1);
      sprintf(buttons[j].image, DATA_DIR "%s", s);
    }
    if (!strcasecmp(buttons[j].prog, "<popup>")) {
      buttons[j].gad = Epplet_create_popup();
      Epplet_gadget_show(Epplet_create_popupbutton(buttons[j].label, buttons[j].image,
                                                   (buttons[j].x * 16) + 2, (buttons[j].y * 16) + 2,
                                                   (buttons[j].w - 1) * 16 + 12, (buttons[j].h - 1) * 16 + 12,
                                                   std, buttons[j].gad));
    } else if (buttons[j].popup != -1) {
      if ((unsigned long) buttons[j].popup < button_cnt) {
        Epplet_add_popup_entry(buttons[buttons[j].popup].gad, buttons[j].label, buttons[j].image, button_cb, &buttons[j]);
      }
    } else {
      buttons[j].gad = Epplet_create_button(buttons[j].label, buttons[j].image,
                                            (buttons[j].x * 16) + 2, (buttons[j].y * 16) + 2,
                                            (buttons[j].w - 1) * 16 + 12, (buttons[j].h - 1) * 16 + 12,
                                            std, None, NULL, button_cb, &buttons[j]);
      Epplet_gadget_show(buttons[j].gad);
    }
    if (std) {
      free(std);
    }
    if (s) {
      free(buttons[j].image);
      buttons[j].image = s;
    }
  }

  Epplet_register_delete_event_handler(delete_cb, NULL);
  create_shade_window();
  Epplet_show();
  Epplet_Loop();

  return 0;
}
