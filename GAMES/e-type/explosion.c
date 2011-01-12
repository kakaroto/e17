#include "config.h"

#include <Evas.h>
#include <Edje.h>

#include "explosion.h"


struct _Explosion
{
  Evas *evas;
  char *name;
  Evas_Object *data;
};


static void
_explosion_hide_cb(void *data __UNUSED__, Evas_Object *obj, const char *emission __UNUSED__, const char *source __UNUSED__)
{
  evas_object_del(obj);
}

Explosion *
explosion_new(Evas *evas,
              const char *name,
              void *data)
{
  Explosion *e;

  e = (Explosion *)calloc(1, sizeof(Explosion));
  if (!e) return NULL;

  e->name = strdup(name);
  if (!e->name)
    goto free_e;

  e->evas = evas;
  e->data = (Evas_Object *)data;

  return e;

 free_e:
  free(e);

  return NULL;
}

Eina_Bool
explosion_launch(Explosion *e, Evas_Coord hot_x, Evas_Coord hot_y)
{
  Evas_Object *o;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord w;
  Evas_Coord h;

  if (!e) return EINA_FALSE;

  evas_object_geometry_get(e->data, &x, &y, &w, &h);
  if (((hot_x) >= x) &&
      ((hot_x) < (x + w)) &&
      ((hot_y) >= y) &&
      ((hot_y) < (y + h)))
    {
      Evas_Coord ew;
      Evas_Coord eh;

      o = edje_object_add(e->evas);
      edje_object_file_set(o, "theme.edj", e->name);
      edje_object_size_min_get(o, &ew, &eh);
      evas_object_move(o, x + w / 2 - ew / 2, y + h / 2 - eh / 2);
      evas_object_resize(o, eh, eh);
      evas_object_layer_set(o, 11);
      evas_object_show(o);
      edje_object_signal_callback_add(o, "end_explosion", "",
                                      _explosion_hide_cb, NULL);
      edje_object_signal_emit(o, "start", "");
      return EINA_TRUE;
    }

  return EINA_FALSE;
}

void
explosion_free(Explosion *e)
{
  if (!e) return;

  free(e->name);
  free(e);
}
