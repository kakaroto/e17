#include "ui.h"

Window win, ewin;


int max_colors = MAX_EVAS_COLORS;
Evas_Object b;
Ebits_Object *bg;

Elogin_Entry *login_e;
Elogin_Entry *passwd_e;

/* base callbacks */

void
but_clicked (void *data)
{
  printf ("login: %s\npassword: %s\n", login_e->t, passwd_e->t);
}


void
e_idle (void *data)
{
  evas_render (evas);
}


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
e_key_down (Ecore_Event * ev)
{
  Ecore_Event_Key_Down *e;

  e = ev->event;
  if ((e->win != win) && (e->win != evas_get_window (evas)))
    return;
  printf ("key: %s- %s\n", e->key, e->compose);
}

int
main (int argc, char **argv)
{
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

  ecore_event_filter_handler_add (ECORE_EVENT_WINDOW_EXPOSE, e_window_expose);
  ecore_event_filter_handler_add (ECORE_EVENT_MOUSE_MOVE, e_mouse_move);
  ecore_event_filter_handler_add (ECORE_EVENT_MOUSE_DOWN, e_mouse_down);
  ecore_event_filter_handler_add (ECORE_EVENT_MOUSE_UP, e_mouse_up);
  ecore_event_filter_handler_add (ECORE_EVENT_KEY_DOWN, e_key_down);

  ecore_event_filter_idle_handler_add (e_idle, NULL);

  win = ecore_window_new (0, 0, 0, 640, 480);
  ecore_window_set_events (win, XEV_CONFIGURE | XEV_PROPERTY);
  ecore_window_set_name_class (win, "Edb", "Main");
  ecore_window_set_min_size (win, 256, 128);
  ecore_window_set_max_size (win, 8000, 8000);

  evas =
    evas_new_all (ecore_display_get (), win, 0, 0, 640, 480, render_method,
		  max_colors, MAX_FONT_CACHE, MAX_IMAGE_CACHE,
		  FONT_DIRECTORY);

  ewin = evas_get_window (evas);

  ecore_window_set_events (ewin,
			   XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE |
			   XEV_KEY);


  ecore_window_show (ewin);
  ecore_window_show (win);

  b = evas_add_rectangle (evas);
  evas_set_color (evas, b, 0, 0, 0, 255);
  evas_resize (evas, b, 640, 480);
  //  evas_raise(evas,b);
  evas_show (evas, b);

  {
    Elogin_Box *ebox;
    Elogin_Label *title;
    Elogin_Label *login_l;
    Elogin_Label *passwd_l;

    Elogin_Button *login_b;


    ebox = elogin_box_new ();
    elogin_box_set_size (ebox, 300, 200);
    elogin_box_set_pos (ebox, 100, 100);

    title = elogin_label_new ();
    elogin_label_set_text (title, "eLogin");
    elogin_label_add_to_box (ebox, title);
    elogin_label_set_pos (title, ebox->o->h - 75, 0);

    login_l = elogin_label_new ();
    elogin_label_set_text (login_l, "Login:");
    elogin_label_add_to_box (ebox, login_l);
    elogin_label_set_pos (login_l, 20, 50);

    login_e = elogin_entry_new (0);
    elogin_entry_set_text (login_e, "");
    elogin_entry_add_to_box (ebox, login_e);
    elogin_entry_set_size (login_e, 120, 22);
    elogin_entry_set_pos (login_e, ebox->o->h - 20 - 120, 50);

    passwd_l = elogin_label_new ();
    elogin_label_set_text (passwd_l, "Password:");
    elogin_label_add_to_box (ebox, passwd_l);
    elogin_label_set_pos (passwd_l, 20, 100);

    passwd_e = elogin_entry_new (1);
    elogin_entry_set_text (passwd_e, "");
    elogin_entry_add_to_box (ebox, passwd_e);
    elogin_entry_set_size (passwd_e, 120, 22);
    elogin_entry_set_pos (passwd_e, ebox->o->h - 20 - 120, 100);

    login_b = elogin_button_new ();
    elogin_button_set_text (login_b, "Login");
    elogin_button_add_to_box (ebox, login_b);
    elogin_button_set_size (login_b, 96, 48);
    elogin_button_set_pos (login_b, ebox->o->h - 20 - 100, 140);
    login_b->clicked = but_clicked;
    elogin_button_set_as_login (login_b);

    elogin_button_show (login_b);
    elogin_label_show (login_l);
    elogin_entry_show (login_e);
    elogin_label_show (passwd_l);
    elogin_entry_show (passwd_e);
    elogin_label_show (title);
    elogin_box_show (ebox);
    {
      int h;
      int w;
      elogin_object_get_size (ELOGIN_OBJECT (ebox), &h, &w);
      printf ("%i:%i\n", h, w);
    }
  }




  ecore_event_loop ();

}
