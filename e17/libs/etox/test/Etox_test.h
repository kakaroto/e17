#ifndef __ETOX_TEST_H__
#define __ETOX_TEST_H__

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <Evas.h>
#include <Ecore.h>
#include <Imlib2.h>

#include "src/Etox.h"
#include "etox-config.h"

#define MAX_EVAS_COLORS (216)
#define MAX_FONT_CACHE  (512 * 1024)
#define MAX_IMAGE_CACHE (16 * (1024 * 1024))
#define FONT_DIRECTORY  PACKAGE_DATA_DIR"/fonts/"
#define IMAGE_DIRECTORY PACKAGE_DATA_DIR"/images/"
#define FN              FONT_DIRECTORY
#define IM              IMAGE_DIRECTORY
#define W               600
#define H               500
#define RENDER_ENGINE   RENDER_METHOD_ALPHA_SOFTWARE
/* #define RENDER_ENGINE   RENDER_METHOD_BASIC_HARDWARE */
/* #define RENDER_ENGINE   RENDER_METHOD_3D_HARDWARE */

#include "tests.h"

/* globals */
extern Evas_Object o_bg;
extern Evas_Object o_logo;
extern Evas_Object o_panel;
extern Evas_Object o_showpanel;
extern Evas_Object o_hidepanel;
extern Evas_Object o_txt_paneltitle;
extern Evas_Object o_panel_box1;
extern Evas_Object o_txt_panel_box1;
extern Evas_Object o_bg_etox;
extern Evas_Object clip_msg;
extern Evas_Object clip_test;

extern Etox *e_msg;
extern Etox *e_test;

extern Evas evas;
extern Evas_Render_Method render_method;
extern int max_colors;
extern int win_w;
extern int win_h;
extern int win_x;
extern int win_y;
extern Window main_win;

extern int panel_active;

/* handler functions */
void e_handle_resize(void);

/* general functions */
double get_time (void);
void setup(void);

/* callbacks for evas handling */
/* when the event queue goes idle call this */
void e_idle(void *data);
/* when the window gets exposed call this */
void e_window_expose(Ecore_Event * ev);
/* when the mouse moves in the window call this */
void e_mouse_move(Ecore_Event * ev);
/* when a mouse button goes down in the window call this */
void e_mouse_down(Ecore_Event * ev);
/* when a mouse button is released in the window call this */
void e_mouse_up(Ecore_Event * ev);
/* when the window gets moved or resized */
void e_window_configure(Ecore_Event * ev);

/* panel functions */
void e_slide_panel_in(int v, void *data);
void e_slide_panel_out(int v, void *data);
void show_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
void hide_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

#endif /* __ETOX_TEST_H__ */
