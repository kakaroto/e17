#include "Etox_test.h"

/* globals */
Evas_Object o_bg;
Evas_Object o_logo;
Evas_Object o_panel;
Evas_Object o_showpanel;
Evas_Object o_hidepanel;
Evas_Object o_txt_paneltitle;
Evas_Object o_bg_etox;
Evas_Object clip_msg;
Evas_Object clip_test;
Evas_Object o_panel_box1;
Evas_Object o_txt_panel_box1;

Evas evas;
Evas_Render_Method render_method = RENDER_ENGINE;
int max_colors = MAX_EVAS_COLORS;

Etox *e_msg;
Etox *e_test;

int win_w = W, win_h = H;
int win_x = 0, win_y = 0;
Window main_win;

int panel_active = 0;

double
get_time (void)
{
  struct timeval timev;

  gettimeofday (&timev, NULL);
  return (double) timev.tv_sec + (((double) timev.tv_usec) / 1000000);
}

void
e_fade_logo_in (int v, void *data)
{
  static double start = 0.0;
  double duration = 1.0;
  double val;

  if (v == 0)
    start = get_time ();
  val = (get_time () - start) / duration;

  evas_set_color (evas, o_logo, 255, 255, 255, (val * 255));

  if (val < 1.0)
    ecore_add_event_timer ("e_fade_logo_in()", 0.05, e_fade_logo_in, v + 1,
			   NULL);
}

/* Navigation panel */
void
e_slide_panel_in (int v, void *data)
{
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
  double ascent, descent;

  panel_active = 1;

  if (v == 0)
    evas_set_layer (evas, o_showpanel, 180);
  if (v == 0)
    start = get_time ();
  val = (get_time () - start) / duration;

  evas_get_image_size (evas, o_panel, &w, NULL);
  px = (w * sin (val * 0.5 * 3.141592654)) - w;
  evas_move (evas, o_panel, px, 0);
  evas_move (evas, o_txt_paneltitle, px + 8, 5);
  evas_move (evas, o_panel_box1, px + 5, 40);
  evas_move (evas, o_txt_panel_box1, px + 8, 42);
  evas_text_get_max_ascent_descent (evas, o_txt_panel_box1, &ascent,
				    &descent);
  evas_resize (evas, o_panel_box1, 108, ascent - descent + 4);
  evas_set_image_fill (evas, o_panel_box1, 0, 0, 108, ascent - descent + 4);
  if (val < 1.0)
    ecore_add_event_timer ("e_slide_panel()", 0.05, e_slide_panel_in, v + 1,
			   NULL);
}

void
e_slide_panel_out (int v, void *data)
{
  static double start = 0.0;
  double duration = 0.5;
  double val;
  double px;
  int w;
  double ascent, descent;

  if (v == 0)
    evas_set_layer (evas, o_showpanel, 1000);
  if (v == 0)
    start = get_time ();
  val = (get_time () - start) / duration;

  evas_get_image_size (evas, o_panel, &w, NULL);
  px = (w * sin ((1.0 - val) * 0.5 * 3.141592654)) - w;
  evas_move (evas, o_panel, px, 0);
  evas_move (evas, o_txt_paneltitle, px + 8, 5);
  evas_move (evas, o_panel_box1, px + 5, 40);
  evas_move (evas, o_txt_panel_box1, px + 8, 42);
  evas_text_get_max_ascent_descent (evas, o_txt_panel_box1, &ascent,
				    &descent);
  evas_resize (evas, o_panel_box1, 108, ascent - descent + 4);
  evas_set_image_fill (evas, o_panel_box1, 0, 0, 108, ascent - descent + 4);
  if (val < 1.0)
    ecore_add_event_timer ("e_slide_panel()", 0.05, e_slide_panel_out, v + 1,
			   NULL);
  else
    panel_active = 0;
}

void
show_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (!panel_active)
    e_slide_panel_in (0, NULL);
}

void
hide_panel (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if (panel_active)
    e_slide_panel_out (0, NULL);
}

void
e_idle (void *data)
{
  evas_render (evas);
}

/* Events */
void
e_window_expose (Ecore_Event * ev)
{
  Ecore_Event_Window_Expose *e;

  e = (Ecore_Event_Window_Expose *) ev->event;
  if ((e->win != evas_get_window (evas)))
    return;
  evas_update_rect (evas, e->x, e->y, e->w, e->h);
}

void
e_mouse_move (Ecore_Event * ev)
{
  Ecore_Event_Mouse_Move *e;

  e = (Ecore_Event_Mouse_Move *) ev->event;
  if ((e->win != evas_get_window (evas)))
    return;
  evas_event_move (evas, e->x, e->y);
}

void
e_mouse_down (Ecore_Event * ev)
{
  Ecore_Event_Mouse_Down *e;

  e = (Ecore_Event_Mouse_Down *) ev->event;
  if ((e->win != evas_get_window (evas)))
    return;
  evas_event_button_down (evas, e->x, e->y, e->button);
}

void
e_mouse_up (Ecore_Event * ev)
{
  Ecore_Event_Mouse_Up *e;

  e = (Ecore_Event_Mouse_Up *) ev->event;
  if ((e->win != evas_get_window (evas)))
    return;
  evas_event_button_up (evas, e->x, e->y, e->button);
}

void
mouse_in (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if ((_e = evas) && ((_o == o_txt_panel_box1)))
    evas_set_color (_e, _o, 0, 0, 0, 255);
}

void
mouse_out (void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  if ((_e = evas) && ((_o == o_txt_panel_box1)))
    evas_set_color (_e, _o, 0, 0, 0, 160);
}

void
setup (void)
{
  Window win, ewin;
  int w, h;
  int i, j;
  Evas_Object o;

  /* setup callbacks for events */
  ecore_event_filter_handler_add (ECORE_EVENT_WINDOW_EXPOSE, e_window_expose);
  ecore_event_filter_handler_add (ECORE_EVENT_MOUSE_MOVE, e_mouse_move);
  ecore_event_filter_handler_add (ECORE_EVENT_MOUSE_DOWN, e_mouse_down);
  ecore_event_filter_handler_add (ECORE_EVENT_MOUSE_UP, e_mouse_up);
  /* handler for when the event queue goes idle */
  ecore_event_filter_idle_handler_add (e_idle, NULL);
  /* create a 500x400 toplevel window */
  win = ecore_window_new (0, 0, 0, win_w, win_h);
  ecore_window_set_events (win, XEV_CONFIGURE);
  ecore_window_set_min_size (win, 600, 500);
  ecore_window_set_max_size (win, 600, 500);
  main_win = win;

  /* create a 500x400 evas rendering in software - conveience function that */
  /* also creates the window for us in the right colormap & visual */
  evas =
    evas_new_all (ecore_display_get (), win, 0, 0, win_w, win_h,
		  render_method, max_colors, MAX_FONT_CACHE, MAX_IMAGE_CACHE,
		  FONT_DIRECTORY);
  /* get the window ID for the evas created for us */
  ewin = evas_get_window (evas);
  /* show the evas window */
  ecore_window_show (ewin);
  /* set the events this window accepts */
  ecore_window_set_events (ewin, XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE);
  /* show the toplevel */
  ecore_window_show (win);

  /* Create interface */

  /* Background */
  o_bg = evas_add_image_from_file (evas, IM "bg.png");
  evas_resize (evas, o_bg, win_w, win_h);
  evas_move (evas, o_bg, 0, 0);
  evas_set_layer (evas, o_bg, 0);
  evas_show (evas, o_bg);

  /* Logo */
  /*FIXME
     o_logo = evas_add_image_from_file(evas, IM"logo.png");
     evas_set_color(evas, o_logo, 255, 255, 255, 0);
     evas_show(evas, o_logo);
     evas_get_image_size(evas, o_logo, &w, &h);
     evas_move(evas, o_logo, (win_w - w) / 2, (win_h - h) / 2);
     evas_resize(evas, o_logo, w, h);
     evas_set_layer(evas, o_logo, 100);
   */

  /* Panel background */
  o_panel = evas_add_image_from_file (evas, IM "panel.png");
  o_showpanel = evas_add_rectangle (evas);
  o_hidepanel = evas_add_rectangle (evas);
  evas_set_color (evas, o_showpanel, 0, 0, 0, 0);
  evas_set_color (evas, o_hidepanel, 0, 0, 0, 0);
  evas_get_image_size (evas, o_panel, &w, NULL);
  if (!panel_active)
    evas_move (evas, o_panel, -w, 0);
  evas_resize (evas, o_panel, w, win_h);
  evas_set_layer (evas, o_panel, 200);
  evas_resize (evas, o_showpanel, 64, win_h);
  if (panel_active)
    evas_set_layer (evas, o_showpanel, 180);
  else
    evas_set_layer (evas, o_showpanel, 1000);
  evas_move (evas, o_hidepanel, 128, 0);
  evas_resize (evas, o_hidepanel, win_w - 128, win_h);
  evas_set_layer (evas, o_hidepanel, 1000);
  evas_move (evas, o_showpanel, 0, 0);
  evas_show (evas, o_panel);
  evas_show (evas, o_showpanel);
  evas_show (evas, o_hidepanel);

  /* Panel title */
  o_txt_paneltitle = evas_add_text (evas, "notepad", 17, "Etox Test");
  evas_set_color (evas, o_txt_paneltitle, 255, 255, 255, 255);
  evas_set_layer (evas, o_txt_paneltitle, 250);
  evas_show (evas, o_txt_paneltitle);

  /* Panel box 1 */
  o = evas_add_image_from_file (evas, IM "panel_button1.png");
  evas_set_image_border (evas, o, 3, 3, 3, 3);
  evas_set_layer (evas, o, 250);
  evas_show (evas, o);
  o_panel_box1 = o;
  o_txt_panel_box1 = evas_add_text (evas, "andover", 24, "Basic");
  evas_set_color (evas, o_txt_panel_box1, 0, 0, 0, 160);
  evas_set_layer (evas, o_txt_panel_box1, 250);
  evas_show (evas, o_txt_panel_box1);

  /* Clip rectangle for bounding where the message text is drawn */
  clip_msg = evas_add_rectangle (evas);
  evas_show (evas, clip_msg);
  evas_set_color (evas, clip_msg, 255, 0, 255, 255);
  evas_move (evas, clip_msg, 40, 40);
  evas_resize (evas, clip_msg, 520, 140);

  /* Message etox */
  e_msg = etox_new_all (evas, 40, 40, 520, 140, 255, ETOX_ALIGN_RIGHT);
  etox_context_set_align (e_msg, ETOX_ALIGN_RIGHT);
  etox_context_set_font (e_msg, "andover", 24);
  etox_context_set_style (e_msg, "plain");
  etox_context_set_color (e_msg, 255, 255, 255, 255);
  etox_set_clip (e_msg, clip_msg);
  etox_set_alpha (e_msg, 255);
  etox_set_layer (e_msg, 1000);

  e_slide_panel_out (0, NULL);

  /* Callbacks */
  evas_callback_add (evas, o_showpanel, CALLBACK_MOUSE_IN, show_panel, NULL);
  evas_callback_add (evas, o_hidepanel, CALLBACK_MOUSE_IN, hide_panel, NULL);
  evas_callback_add (evas, o_txt_panel_box1, CALLBACK_MOUSE_DOWN, test_basic,
		     NULL);
  evas_callback_add (evas, o_txt_panel_box1, CALLBACK_MOUSE_UP, test_basic,
		     NULL);
  evas_callback_add (evas, o_txt_panel_box1, CALLBACK_MOUSE_IN, mouse_in,
		     NULL);
  evas_callback_add (evas, o_txt_panel_box1, CALLBACK_MOUSE_OUT, mouse_out,
		     NULL);
  /*FIXME
     evas_callback_add(evas, o_logo, CALLBACK_MOUSE_DOWN, show_panel, NULL);
     evas_callback_add(evas, o_logo, CALLBACK_MOUSE_UP, show_panel, NULL);
   */
}

int
main (int argc, char **argv)
{
  /* init X */
  if (!ecore_display_init (NULL))
    {
      if (getenv ("DISPLAY"))
	{
	  printf ("Cannot initialize default display:\n");
	  printf ("DISPLAY=%s\n", getenv ("DISPLAY"));
	}
      else
	{
	  printf ("No DISPLAY variable set!\n");
	}
      printf ("Exit.\n");
      exit (-1);
    }
  /* setup handlers for system signals */
  ecore_event_signal_init ();
  /* setup the event filter */
  ecore_event_filter_init ();
  /* setup the X event internals */
  ecore_event_x_init ();
  /* program does its data setup here */
  setup ();
  /* call the animator once to start it up */
  e_fade_logo_in (0, NULL);
  /* and now loop forever handling events */
  ecore_event_loop ();

  etox_free (e_msg);
  etox_free (e_test);
  evas_free (evas);

  return 0;
}
