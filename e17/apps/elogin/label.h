#ifndef __ELOGIN_LABEL_H__
#define __ELOGIN_LABEL_H__

#include "ui.h"

#ifdef __cplusplus
extern "C"
{
#endif				/* __cplusplus */

  typedef struct
  {
    Elogin_Object *o;
    Elogin_Box *b;

    Evas_Object t;

  }
  Elogin_Label;


  Elogin_Label *elogin_label_new (void);
  void elogin_label_set_text (Elogin_Label * label, char *text);
  void elogin_label_add_to_box (Elogin_Box * box, Elogin_Label * label);
  void elogin_label_set_abs_pos (Elogin_Label * label, int x, int y);
  void elogin_label_set_pos (Elogin_Label * label, int x, int y);
  void elogin_label_show (Elogin_Label * label);
  void elogin_label_hide (Elogin_Label * label);

#ifdef __cplusplus
}
#endif				/* __cplusplus */


#endif				/* __ELOGIN_LABEL_H__ */
