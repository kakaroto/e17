#ifndef __ELOGIN_BUTTON_H__
#define __ELOGIN_BUTTON_H__

#include "ui.h"

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

  typedef struct
  {
    Elogin_Object *o;
    Elogin_Box *b;

    Evas_Object text;
    Evas_Object but;

    int login;
    void (*clicked) (void *data);

  }
  Elogin_Button;


  /* callbacks */
  void _elogin_button_mouse_in (void *_data, Evas _e, Evas_Object _o, int _b,
				int _x, int _y);
  void _elogin_button_mouse_out (void *_data, Evas _e, Evas_Object _o, int _b,
				 int _x, int _y);
  void _elogin_button_mouse_down (void *_data, Evas _e, Evas_Object _o,
				  int _b, int _x, int _y);
  void _elogin_button_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b,
				int _x, int _y);

  /* funcs */
  Elogin_Button *elogin_button_new (void);
  void elogin_button_set_size (Elogin_Button * button, int h, int w);
  void elogin_button_set_abs_pos (Elogin_Button * button, int x, int y);
  void elogin_button_set_pos (Elogin_Button * button, int x, int y);
  void elogin_button_set_text (Elogin_Button * button, char *text);
  void elogin_button_add_to_box (Elogin_Box * box, Elogin_Button * button);
  void elogin_button_show (Elogin_Button * button);
  void elogin_button_hide (Elogin_Button * button);


#ifdef __cplusplus
}
#endif				/* __cplusplus */


#endif				/* __ELOGIN_BUTTON_H__ */
