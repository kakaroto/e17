#ifndef __ELOGIN_ENTRY_H__
#define __ELOGIN_ENTRY_H__

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
    Evas_Object bg;
    Evas_Object cursor;

    Evas_Object fx;

    int selected;
    int passwd;



    /* cursor pos */
    int cx, cy, cp;

    int l;
    char *ch;
    char t[1024];

  }
  Elogin_Entry;

  Elogin_Entry *selected;


  /* callbacks */
  void _elogin_entry_get_key (int v, void *data);
  void _elogin_entry_mouse_in (void *_data, Evas _e, Evas_Object _o, int _b,
			       int _x, int _y);
  void _elogin_entry_mouse_out (void *_data, Evas _e, Evas_Object _o, int _b,
				int _x, int _y);

  /* funcs */

  /* base funcs */
  Elogin_Entry *elogin_entry_new (int passwd);
  void elogin_entry_set_abs_pos (Elogin_Entry * entry, int x, int y);
  void elogin_entry_set_pos (Elogin_Entry * entry, int x, int y);
  void elogin_entry_set_size (Elogin_Entry * entry, int h, int w);
  void elogin_entry_add_to_box (Elogin_Box * box, Elogin_Entry * entry);
  void elogin_entry_show (Elogin_Entry * entry);
  void elogin_entry_hide (Elogin_Entry * entry);

  /* selection funcs */
  void elogin_entry_set_next_selected (Elogin_Entry * entry);
  void elogin_entry_set_normal (Elogin_Entry * entry);
  void elogin_entry_set_hilited (Elogin_Entry * entry);
  void elogin_entry_set_selected (Elogin_Entry * entry);

  /* text funcs */
  int elogin_entry_get_text_len (Elogin_Entry * entry);
  void elogin_entry_get_char_width (Elogin_Entry * entry, int index,
				    double *w);
  void elogin_entry_set_text (Elogin_Entry * entry, char *text);
  char *elogin_get_text (Elogin_Entry * entry);

  /* cursor funcs */
  void elogin_entry_set_cursor_pos (Elogin_Entry * entry, int x, int y);
  void elogin_entry_cursor_from_text (Elogin_Entry * entry);
  void elogin_entry_update_cursor (Elogin_Entry * entry);
  void elogin_entry_update_text (Elogin_Entry * entry);




#ifdef __cplusplus
}
#endif				/* __cplusplus */


#endif				/* __ELOGIN_ENTRY_H__ */
