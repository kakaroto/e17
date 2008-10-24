#ifndef __ETOX_TEST_H__
#define __ETOX_TEST_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include <Evas.h>
#include <Evas_Engine_Software_X11.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>

#include "src/Etox.h"
#include "config.h"

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

typedef struct _panel_button Panel_Button;

struct _panel_button {
	Evas_Object *box;
	Evas_Object *label;
};

/* globals */
extern Evas_Object *o_bg_etox;
extern Evas_Object *clip_msg;
extern Evas_Object *clip_test;
extern Evas_Object *o_next_box;
extern Evas_Object *o_txt_next_box;
extern Eina_List *pbuttons;

extern Evas_Object *e_msg;
extern Evas_Object *e_test;

extern Evas *evas;
extern int render_method;
extern int max_colors;
extern int win_w;
extern int win_h;
extern int win_x;
extern int win_y;
extern Window main_win;

/* general functions */
double get_time(void);
void setup(void);

/* callbacks for evas handling */
/* when the event queue goes idle call this */
int e_idle(void *data);
/* when the window gets exposed call this */
int e_window_expose(void *data, int type, void * ev);
/* when the mouse moves in the window call this */
int e_mouse_move(void *data, int type, void * ev);
/* when a mouse button goes down in the window call this */
int e_mouse_down(void *data, int type, void * ev);
/* when a mouse button is released in the window call this */
int e_mouse_up(void *data, int type, void * ev);
/* when the mouse moves over a button */
void button_mouse_in(void *_data, Evas *_e, Evas_Object *_o,
		void *event_info);
void button_mouse_out(void *_data, Evas *_e, Evas_Object *_o, 
		void *event_info);
void button_mouse_down(void *_data, Evas *_e, Evas_Object *_o, 
		void *event_info);

/* button functions */
void button_next_new_all(Evas *_e);
void button_next_new(Evas *_e);

#include "panel.h"
#include "tests.h"

#endif				/* __ETOX_TEST_H__ */
