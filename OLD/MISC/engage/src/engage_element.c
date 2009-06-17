
/**
 * engage_element.c: something that lives in the engage bar
 */
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "engage_element.h"

static Evas_Object *engage_element_icon_new(Evas * evas, const char *file,
                                            const char *group);
static Evas_Object *engage_element_behavior_new(Evas * evas, const char *file,
                                                const char *group);

/* smart object handlers */
static Evas_Smart *_engage_element_object_smart_get(void);
static Evas_Object *engage_element_object_new(Evas * evas);
void            _engage_element_object_add(Evas_Object * o);
void            _engage_element_object_del(Evas_Object * o);
void            _engage_element_object_layer_set(Evas_Object * o, int l);
void            _engage_element_object_raise(Evas_Object * o);
void            _engage_element_object_lower(Evas_Object * o);
void            _engage_element_object_stack_above(Evas_Object * o,
                                                   Evas_Object * above);
void            _engage_element_object_stack_below(Evas_Object * o,
                                                   Evas_Object * below);
void            _engage_element_object_move(Evas_Object * o, Evas_Coord x,
                                            Evas_Coord y);
void            _engage_element_object_resize(Evas_Object * o, Evas_Coord w,
                                              Evas_Coord h);
void            _engage_element_object_show(Evas_Object * o);
void            _engage_element_object_hide(Evas_Object * o);
void            _engage_element_object_color_set(Evas_Object * o, int r, int g,
                                                 int b, int a);
void            _engage_element_object_clip_set(Evas_Object * o,
                                                Evas_Object * clip);
void            _engage_element_object_clip_unset(Evas_Object * o);


Evas_Object    *
engage_element_new(Evas * e, const char *bfile, const char *bgroup,
                   const char *ifile, const char *igroup)
{
  char            buf[PATH_MAX];
  int             w = 48, h = 48;
  Evas_Object    *result = NULL;
  Engage_Element *data = NULL;

  if ((result = engage_element_object_new(e))) {
    if ((data = evas_object_smart_data_get(result))) {
      data->clip = evas_object_rectangle_add(e);
      evas_object_color_set(data->clip, 255, 255, 255, 255);
      evas_object_layer_set(data->clip, 0);
      evas_object_show(data->clip);

      if ((data->behavior = engage_element_behavior_new(e, bfile, bgroup))) {
        evas_object_clip_set(data->behavior, data->clip);
        if ((data->icon = engage_element_icon_new(e, ifile, igroup))) {
          if (!strcmp(evas_object_type_get(data->icon), "image")) {
            evas_object_image_size_get(data->icon, &data->iw, &data->ih);
            evas_object_image_fill_set(data->icon, 0.0, 0.0,
                                       (Evas_Coord) data->iw,
                                       (Evas_Coord) data->ih);

          }
          evas_object_clip_set(data->icon, data->clip);
        } else {
          evas_object_del(result);
          result = NULL;
        }
      }
    }
  }
  return (result);
}


/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
engage_element_object_new(Evas * evas)
{
  Evas_Object    *engage_element_object;

  engage_element_object =
    evas_object_smart_add(evas, _engage_element_object_smart_get());

  return engage_element_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_engage_element_object_smart_get(void)
{
  static Evas_Smart *smart = NULL;

  if (smart)
    return (smart);
  smart = evas_smart_new("engage_element_object",
                         _engage_element_object_add,
                         _engage_element_object_del,
                         _engage_element_object_layer_set,
                         _engage_element_object_raise,
                         _engage_element_object_lower,
                         _engage_element_object_stack_above,
                         _engage_element_object_stack_below,
                         _engage_element_object_move,
                         _engage_element_object_resize,
                         _engage_element_object_show,
                         _engage_element_object_hide,
                         _engage_element_object_color_set,
                         _engage_element_object_clip_set,
                         _engage_element_object_clip_unset, NULL);

  return smart;
}

void
_engage_element_object_add(Evas_Object * o)
{
  Engage_Element *data = NULL;

  data = malloc(sizeof(Engage_Element));
  memset(data, 0, sizeof(Engage_Element));
  evas_object_smart_data_set(o, data);
}


void
_engage_element_object_del(Evas_Object * o)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    if (data->clip)
      evas_object_del(data->clip);
    if (data->icon)
      evas_object_del(data->icon);
    if (data->behavior)
      evas_object_del(data->behavior);
    free(data);
  }
}

void
_engage_element_object_layer_set(Evas_Object * o, int l)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_layer_set(data->clip, l);
    evas_object_layer_set(data->icon, l);
    evas_object_layer_set(data->behavior, l);
  }
}

void
_engage_element_object_raise(Evas_Object * o)
{
  Engage_Element *data;

  data = evas_object_smart_data_get(o);

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_raise(data->clip);
    evas_object_raise(data->icon);
    evas_object_raise(data->behavior);
  }
}

void
_engage_element_object_lower(Evas_Object * o)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_lower(data->behavior);
    evas_object_lower(data->icon);
    evas_object_lower(data->clip);
  }
}

void
_engage_element_object_stack_above(Evas_Object * o, Evas_Object * above)
{
  Engage_Element *data;

  data = evas_object_smart_data_get(o);

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_stack_above(data->clip, above);
    evas_object_stack_above(data->icon, above);
    evas_object_stack_above(data->behavior, above);
  }
}

void
_engage_element_object_stack_below(Evas_Object * o, Evas_Object * below)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_stack_below(data->clip, below);
    evas_object_stack_below(data->icon, below);
    evas_object_stack_below(data->behavior, below);
  }
}

void
_engage_element_object_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
  Engage_Element *data;

  data = evas_object_smart_data_get(o);

  if ((data = evas_object_smart_data_get(o))) {
    data->x = x;
    data->y = y;
    evas_object_move(data->clip, x, y);
    evas_object_move(data->icon, x, y);
    evas_object_resize(data->icon, data->w, data->h);
  }
}

void
_engage_element_object_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
  double          fit = 0.0;
  Evas_Coord      iw = w, ih = h;
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    data->iw = w;
    data->ih = h;
    evas_object_resize(data->clip, w, h);
    evas_object_resize(data->behavior, w, h);
    if (!strcmp(evas_object_type_get(data->icon), "image")) {
      if (data->iw < data->ih)
        iw =
          (Evas_Coord) ((double) h * ((double) data->iw / (double) data->ih));
      else
        ih =
          (Evas_Coord) ((double) w * ((double) data->ih / (double) data->iw));

      evas_object_resize(data->icon, iw, ih);
      evas_object_image_fill_set(data->icon, 0.0, 0.0, iw, ih);
      evas_object_move(data->icon, data->x - ((iw - data->w) / 2),
                       data->y - ((ih - data->h) / 2));
    } else {
      evas_object_resize(data->icon, w, h);
    }
  }
}

void
_engage_element_object_show(Evas_Object * o)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_show(data->clip);
    evas_object_show(data->icon);
    evas_object_show(data->behavior);
  }
}

void
_engage_element_object_hide(Evas_Object * o)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_hide(data->clip);
    evas_object_hide(data->icon);
    evas_object_hide(data->behavior);
  }
}

void
_engage_element_object_color_set(Evas_Object * o, int r, int g, int b, int a)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_color_set(data->clip, r, g, b, a);
  }
}

void
_engage_element_object_clip_set(Evas_Object * o, Evas_Object * clip)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_clip_set(data->clip, clip);
  }
}

void
_engage_element_object_clip_unset(Evas_Object * o)
{
  Engage_Element *data;

  if ((data = evas_object_smart_data_get(o))) {
    evas_object_clip_unset(data->clip);
  }
}

static Evas_Object *
engage_element_behavior_new(Evas * evas, const char *file, const char *group)
{
  Evas_Object    *result = NULL;

  if (file && group && file[0] == '/') {
    result = edje_object_add(evas);
    if (edje_object_file_set(result, file, group) > 0) {
      evas_object_move(result, -9999, -9999);
      evas_object_resize(result, 4, 4);
    } else {
      evas_object_del(result);
      result = NULL;
    }
  }
  return (result);
}
static Evas_Object *
engage_element_icon_new(Evas * evas, const char *file, const char *group)
{
  int             iw, ih;
  Evas_Object    *result = NULL;

  /* edje */
  if (group) {
    if (file && file[0] == '/') {
      result = edje_object_add(evas);
      if (edje_object_file_set(result, file, group) > 0) {
        evas_object_move(result, -9999, -9999);
        evas_object_resize(result, 4, 4);
      }
    } else {
      evas_object_del(result);
      result = NULL;
    }
  }
  /* image */
  else {
    if (file && file[0] == '/') {
      result = evas_object_image_add(evas);
      evas_object_image_file_set(result, file, NULL);
      switch (evas_object_image_load_error_get(result)) {
      case EVAS_LOAD_ERROR_NONE:
        evas_object_move(result, -9999, -9999);
        evas_object_resize(result, 4, 4);
        break;
      default:
        evas_object_del(result);
        result = NULL;
      }
    } else {
      evas_object_del(result);
      result = NULL;
    }
  }
  return (result);
}
