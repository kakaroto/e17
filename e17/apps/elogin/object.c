#include "ui.h"

/* Elogin_Objects functions */

Elogin_Object *
elogin_object_new (void)
{
  Elogin_Object *object;
  object = NEW (Elogin_Object, 1);
  ZERO (object, Elogin_Object, 1);

  object->x = 0;
  object->y = 0;
  object->h = 0;
  object->w = 0;

  object->clip = evas_add_rectangle (evas);
  evas_set_color (evas, object->clip, 255, 255, 255, 255);

  return object;
}

void
elogin_object_set_size (Elogin_Object * object, int h, int w)
{
  object->h = h;
  object->w = w;
  evas_resize (evas, object->clip, object->h, object->w);
}

void
elogin_object_get_size (Elogin_Object * object, int *h, int *w)
{
  *h = object->h;
  *w = object->w;
}

void
elogin_object_set_pos (Elogin_Object * object, int x, int y)
{
  object->x = x;
  object->y = y;
  evas_move (evas, object->clip, object->x, object->y);
}

void
elogin_object_get_pos (Elogin_Object * object, int *x, int *y)
{
  *x = object->x;
  *y = object->y;
}

void
elogin_object_show (Elogin_Object * object)
{
  evas_show (evas, object->clip);
}

void
elogin_object_hide (Elogin_Object * object)
{
  evas_hide (evas, object->clip);
}

void
elogin_object_free (Elogin_Object * object)
{
  FREE (object);
}
