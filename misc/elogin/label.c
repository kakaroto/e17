#include "ui.h"


Elogin_Label *
elogin_label_new (void)
{
  Elogin_Object *obj;
  Elogin_Label *label;

  obj = elogin_object_new ();
  label = NEW (Elogin_Label, 1);
  ZERO (label, Elogin_Label, 1);

  label->o = obj;

  label->t = evas_add_text (evas, "borzoib", 15, "");
  /*  evas_set_clip(evas,label->t,label->o->clip); */

  return label;
}

void
elogin_label_set_text (Elogin_Label * label, char *text)
{
  evas_set_text (evas, label->t, text);
  evas_set_color (evas, label->t, 0, 0, 0, 255);
}

void
elogin_label_add_to_box (Elogin_Box * box, Elogin_Label * label)
{
  box->labels = evas_list_append (box->labels, label);
  evas_set_layer (evas, label->t, 10);
  label->b = box;
  elogin_label_set_pos (label, box->o->x, box->o->y);
}


void
elogin_label_set_abs_pos (Elogin_Label * label, int x, int y)
{
  elogin_object_set_pos (label->o, x, y);
  evas_move (evas, label->t, label->o->x, label->o->y);
}

void
elogin_label_set_pos (Elogin_Label * label, int x, int y)
{
  elogin_label_set_abs_pos (label, x + label->b->o->x, y + label->b->o->y);
}


void
elogin_label_show (Elogin_Label * label)
{
  evas_show (evas, label->t);
  elogin_object_show (label->o);
}

void
elogin_label_hide (Elogin_Label * label)
{
  evas_hide (evas, label->t);
  elogin_object_hide (label->o);
}
