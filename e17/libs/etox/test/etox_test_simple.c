#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Evas.h>
#include <Ecore.h>
#include "../src/Etox.h"

#define MAX_EVAS_COLORS (216)
#define MAX_FONT_CACHE (512 * 1024)
#define MAX_IMAGE_CACHE (1 * (1024 * 1024))
#define FONT_DIRECTORY "./fnt/"
#define RENDER_ENGINE RENDER_METHOD_ALPHA_SOFTWARE

/* the obstacle's coords */
#define OBST_X 100.0
#define OBST_Y 100.0


#define DEBUG


double obstacle_w = -1.0, obstacle_h = -1.0, obstacle_x, obstacle_y;

static void e_idle(void *data);
static void e_window_expose(Eevent *ev);

void setup(void);

Evas evas;
Evas_Render_Method render_method = RENDER_ENGINE;

static void
e_idle(void *data)
{
  evas_render(evas);
}

static void
e_window_expose(Eevent *ev)
{
  Ev_Window_Expose *e;

  e = (Ev_Window_Expose *)ev->event;
  evas_update_rect(evas, e->x, e->y, e->w, e->h);
}

void setup(void)
{
  Window win, ewin;

  e_event_filter_handler_add(EV_WINDOW_EXPOSE, e_window_expose);
  e_event_filter_idle_handler_add(e_idle, NULL);
  win = e_window_new(0, 0, 0, 400, 400);

  evas = evas_new_all(e_display_get(), win, 0, 0, 400, 400, render_method,
		      MAX_EVAS_COLORS, MAX_FONT_CACHE, MAX_IMAGE_CACHE,
		      FONT_DIRECTORY);

  ewin = evas_get_window(evas);

  e_window_show(ewin);
  e_window_set_events(ewin, XEV_EXPOSE);
  e_window_show(win);
}

int
main(int argc, char *argv[])
{
  Etox e;
  Etox_Style s;
  Etox_Color c;
  Evas_Object bg, et_bg, obst;

  obstacle_x = OBST_X;
  obstacle_y = OBST_Y;

  {
    int i;

    for (i = 1; i < argc; i++)
      {
	if (!strcmp(argv[i], "soft") ||
	    !strcmp(argv[i], "x11")  ||
            !strcmp(argv[i], "hard") )
          {
	    if (!strcmp(argv[i], "soft"))
	      render_method = RENDER_METHOD_ALPHA_SOFTWARE;
	    if (!strcmp(argv[i], "x11"))
	      render_method = RENDER_METHOD_BASIC_HARDWARE;
	    if (!strcmp(argv[i], "hard"))
	      render_method = RENDER_METHOD_3D_HARDWARE;
	  }
	else
	  {
	    if (obstacle_w < 0.0)
	      obstacle_w = atoi(argv[i]);
	    else 
	      if (obstacle_h < 0.0)
		obstacle_h = atoi(argv[i]);
	  }
      }
  }

  if (obstacle_w < 0.0)
    obstacle_w = 100.0;
  if (obstacle_h < 0.0)
    obstacle_h = 100.0;

  e_display_init(NULL);
  e_ev_signal_init();
  e_event_filter_init();
  e_ev_x_init();

  setup();

  /* add a background */
  bg = evas_add_rectangle(evas);
  evas_resize(evas, bg, 400, 400);
  evas_move(evas, bg, 0, 0);
  evas_set_color(evas, bg, 255, 255, 255, 255);
  evas_show(evas, bg);

  /* add an etox-background */
  et_bg = evas_add_rectangle(evas);
  evas_resize(evas, et_bg, 380, 380);
  evas_move(evas, et_bg, 10, 10);
  evas_set_color(evas, et_bg, 0, 0, 255, 50);
  evas_show(evas, et_bg);

  /* draw obstacle-rect */
  obst = evas_add_rectangle(evas);
  evas_resize(evas, obst, obstacle_w, obstacle_h);
  evas_move(evas, obst, obstacle_x, obstacle_y);
  evas_set_color(evas, obst, 255, 0, 0, 50);
  evas_show(evas, obst);

  /* test the etox stuff.. */
  e = etox_new(evas, "My Etox");
  etox_move(e, 10, 10);
  etox_resize(e, 380, 380);
  etox_set_font(e, "notepad", 10);

  etox_style_add_path("./style/");

  s = etox_style_new("plain");

  obst = etox_obstacle_add(e, obstacle_x, obstacle_y,
                              obstacle_w, obstacle_h);

  c = etox_color_new();
  etox_color_set_member(c, "fg", 70, 90, 80, 255);
  etox_color_set_member(c, "sh", 70, 90, 80, 255);
  etox_color_set_member(c, "ol", 70, 90, 80, 255);

  etox_set_text(e, ET_TEXT("This is just a test string.. some lame "
	"copied stuff actually..\n\n "), ET_STYLE(s),
    ET_ALIGN(ETOX_ALIGN_TYPE_CENTER, ETOX_ALIGN_TYPE_RIGHT),
    ET_TEXT("As a result of meeting requests from users, Enlightenment over "),
    ET_TEXT("time has done some nasty hacks, but now for the development of "),
    ET_TEXT("version 0.17.0, we have moved a lot of the design and core code "),
    ET_COLOR(c),
    ET_TEXT("into various subsystems than generalize some back end and let us "),
    ET_COLOR_END,
    ET_TEXT("not only use it in Enlightenment itself, but make this work "),
    ET_TEXT("available to everyone to use as they want. The result is various "),
    ET_ALIGN_END, ET_STYLE_END, ET_FONT("cinema", 8), 
    ET_TEXT("lower level libraries that do lots of useful stuff for you and "  
    "provide a consistant api to them. Also daemons and helper programs "
    "have been written too to make life easier."), ET_END);

  etox_show(e);

  etox_set_alpha(e, 255);

  printf("Text: %s\n", etox_get_text(e));

  {
    double x, y, w, h;
    Evas_Object real_rect;

    etox_get_actual_geometry(e, &x, &y, &w, &h);

    printf("Real rect: x=%f,y=%f,w=%f,h=%f\n", x, y, w, h);

    real_rect = evas_add_rectangle(evas);
    evas_set_color(evas, real_rect, 0, 255, 0, 50);
    evas_move(evas, real_rect, x, y);
    evas_resize(evas, real_rect, w, h);
    evas_show(evas, real_rect);
  }

  /* render it! :) */
  evas_render(evas);

  etox_free(e);

  e_event_loop();

  return 0;
}
