#ifndef __ELOGIN_BOX_H__
#define __ELOGIN_BOX_H__

#include "ui.h"

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */


  typedef struct _elogin_box Elogin_Box;

  struct _elogin_box
  {
    Elogin_Object *o;

    Ebits_Object *b;

    Evas_List labels;
    Evas_List entries;
    Evas_List buttons;

    int move;

  };

/* callbacks */
  static void
    _elogin_box_title_down (void *_data, Ebits_Object _o,
			    char *_c, int _b, int _x, int _y,
			    int _ox, int _oy, int _ow, int _oh);
  static void
    _elogin_box_title_up (void *_data, Ebits_Object _o,
			  char *_c, int _b, int _x, int _y,
			  int _ox, int _oy, int _ow, int _oh);
  static void
    _elogin_box_title_move (void *_data, Ebits_Object _o,
			    char *_c, int _b, int _x, int _y,
			    int _ox, int _oy, int _ow, int _oh);

/* funcs */

  Elogin_Box *elogin_box_new (void);
  void elogin_box_set_size (Elogin_Box * box, int h, int w);
  void elogin_box_get_size (Elogin_Box * box, int *h, int *w);
  void elogin_box_set_pos (Elogin_Box * box, int x, int y);
  void elogin_box_get_pos (Elogin_Box * box, int *x, int *y);
  void elogin_box_show (Elogin_Box * box);
  void elogin_box_hide (Elogin_Box * box);
  void elogin_box_free (Elogin_Box * box);

#ifdef __cplusplus
}
#endif				/* __cplusplus */


#endif				/* __ELOGIN_BOX_H__ */
