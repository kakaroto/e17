#include "ui.h"


/* Elogin_Button Callbacks */
void
null_clicked (void *data)
{
}

void
_elogin_button_mouse_in (void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			 int _y)
{
  Elogin_Button *but;
  but = _data;
  evas_set_color (evas, but->but, 255, 255, 255, 255);
  evas_set_color (evas, but->text, 0, 0, 0, 255);
}

void
_elogin_button_mouse_out (void *_data, Evas _e, Evas_Object _o, int _b,
			  int _x, int _y)
{
  Elogin_Button *but;
  but = _data;
  evas_set_color (evas, but->but, 255, 255, 255, 128);
  evas_set_color (evas, but->text, 0, 0, 0, 128);
}

void
_elogin_button_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b,
			   int _x, int _y)
{
  Elogin_Button *but;
  void (*clicked) (void *data);
  but = _data;
  clicked = but->clicked;
  evas_set_image_file (evas, but->but,
		       PACKAGE_DATA_DIR "/elogin/data/button_clicked.png");
  elogin_button_set_size (but, but->o->h, but->o->w);
  clicked (_data);

}

void
_elogin_button_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			 int _y)
{
  Elogin_Button *but;
  but = _data;
  evas_set_image_file (evas, but->but,
		       PACKAGE_DATA_DIR "/elogin/data/button.png");
  elogin_button_set_size (but, but->o->h, but->o->w);
}

/* Elogin_Buttons Funcs */
Elogin_Button *
elogin_button_new (void)
{
  Elogin_Object *obj;
  Elogin_Button *button;

  obj = elogin_object_new ();

  button = NEW (Elogin_Button, 1);
  ZERO (button, Elogin_Button, 1);

  button->o = obj;

  button->text = evas_add_text (evas, "borzoib", 13, "");
  evas_set_layer (evas, button->text, 12);

  button->but =
    evas_add_image_from_file (evas,
			      PACKAGE_DATA_DIR "/elogin/data/button.png");
  evas_set_color (evas, button->but, 255, 255, 255, 128);
  evas_set_layer (evas, button->but, 11);

  evas_callback_add (evas, button->but, CALLBACK_MOUSE_IN,
		     _elogin_button_mouse_in, button);
  evas_callback_add (evas, button->but, CALLBACK_MOUSE_OUT,
		     _elogin_button_mouse_out, button);
  evas_callback_add (evas, button->text, CALLBACK_MOUSE_IN,
		     _elogin_button_mouse_in, button);
  evas_callback_add (evas, button->text, CALLBACK_MOUSE_OUT,
		     _elogin_button_mouse_out, button);
  evas_callback_add (evas, button->but, CALLBACK_MOUSE_DOWN,
		     _elogin_button_mouse_down, button);
  evas_callback_add (evas, button->but, CALLBACK_MOUSE_UP,
		     _elogin_button_mouse_up, button);
  evas_callback_add (evas, button->text, CALLBACK_MOUSE_DOWN,
		     _elogin_button_mouse_down, button);
  evas_callback_add (evas, button->text, CALLBACK_MOUSE_UP,
		     _elogin_button_mouse_up, button);

  button->clicked = null_clicked;

  evas_set_clip (evas, button->but, button->o->clip);
  evas_set_clip (evas, button->text, button->o->clip);
  button->login = 0;

  return button;
}


void
elogin_button_set_as_login (Elogin_Button * button)
{
  button->login = 1;
}


void
elogin_button_set_size (Elogin_Button * button, int h, int w)
{
  elogin_object_set_size (button->o, h, w);

  evas_resize (evas, button->but, button->o->h, button->o->w);
  evas_set_image_fill (evas, button->but, 0, 0, button->o->h, button->o->w);
}

void
elogin_button_set_pos (Elogin_Button * button, int x, int y)
{
  elogin_button_set_abs_pos (button, x + button->b->o->x,
			     y + button->b->o->y);

}

void
elogin_button_set_abs_pos (Elogin_Button * button, int x, int y)
{
  elogin_object_set_pos (button->o, x, y);

  evas_move (evas, button->but, button->o->x, button->o->y);
  evas_move (evas, button->text, button->o->x + 24, button->o->y + 15);

}

void
elogin_button_set_text (Elogin_Button * button, char *text)
{
  evas_set_text (evas, button->text, text);
  evas_set_color (evas, button->text, 0, 0, 0, 128);
}

void
elogin_button_add_to_box (Elogin_Box * box, Elogin_Button * button)
{
  box->buttons = evas_list_append (box->buttons, button);
  button->b = box;
  elogin_button_set_pos (button, box->o->x, box->o->y);
}


void
elogin_button_show (Elogin_Button * button)
{
  elogin_object_show (button->o);
  evas_show (evas, button->text);
  evas_show (evas, button->but);
}

void
elogin_button_hide (Elogin_Button * button)
{
  elogin_object_hide (button->o);
  evas_hide (evas, button->text);
  evas_hide (evas, button->but);
}
