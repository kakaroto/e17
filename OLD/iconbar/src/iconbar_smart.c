#include "iconbar.h"
#include "icon.h"

/* smart object handlers */
void iconbar_add(Evas_Object *o);
void iconbar_del(Evas_Object *o);
void iconbar_layer_set(Evas_Object *o, int l);
void iconbar_raise(Evas_Object *o);
void iconbar_lower(Evas_Object *o);
void iconbar_stack_above(Evas_Object *o, Evas_Object *above);
void iconbar_stack_below(Evas_Object *o, Evas_Object *below);
void iconbar_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
void iconbar_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
void iconbar_show(Evas_Object *o);
void iconbar_hide(Evas_Object *o);
void iconbar_color_set(Evas_Object *o, int r, int g, int b, int a);
void iconbar_clip_set(Evas_Object *o, Evas_Object *clip);
void iconbar_clip_unset(Evas_Object *o);

/* keep this global, so it only has to be created once */
static Evas_Smart *smart = NULL;

Evas_Smart *
iconbar_smart_get()
{
  if (smart) return smart;

  smart = evas_smart_new ("iconbar",
                          iconbar_add,
                          iconbar_del,
                          iconbar_layer_set,
                          iconbar_raise,
                          iconbar_lower,
                          iconbar_stack_above,
                          iconbar_stack_below,
                          iconbar_move,
                          iconbar_resize,
                          iconbar_show,
                          iconbar_hide,
                          iconbar_color_set,
                          iconbar_clip_set,
                          iconbar_clip_unset,
                          NULL
                          );

  return smart;

}


/*********** smart object functions **************/
void
iconbar_add(Evas_Object *o)
{
  Iconbar *ib;
  Evas *evas;

  ib = (Iconbar *)malloc(sizeof(Iconbar));
  memset(ib, 0, sizeof(Iconbar));

  evas_object_smart_data_set(o, ib);
  ib->obj = o;
  
  evas = evas_object_evas_get(o);

  ib->cont = esmart_container_new(evas);
  esmart_container_direction_set(ib->cont, 1);
  esmart_container_spacing_set(ib->cont, 5);
  esmart_container_alignment_set(ib->cont, CONTAINER_ALIGN_CENTER);
  esmart_container_fill_policy_set(ib->cont, CONTAINER_FILL_POLICY_FILL_X |
                                        CONTAINER_FILL_POLICY_KEEP_ASPECT);
  esmart_container_move_button_set(ib->cont, 2);
}


void
iconbar_del(Evas_Object *o)
{
  Iconbar *ib;
  Evas_List *l;

  ib = evas_object_smart_data_get(o);
  /* FIXME free everything up!! */
  

  for (l = esmart_container_elements_get(ib->cont); l; l = l->next)
  {
    Icon *ic = evas_object_data_get(l->data, "Icon");
    iconbar_icon_free(ic);
  }

  edje_object_part_unswallow(ib->gui, ib->cont);
  evas_object_del(ib->cont);
  evas_object_del(ib->clip);
  evas_object_del(ib->grabber);
  evas_object_del(ib->gui);

  if (ib->path) free(ib->path);
  
}

void
iconbar_layer_set(Evas_Object *o, int l)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_layer_set(ib->gui, l);
}

void
iconbar_raise(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_raise(ib->gui);
}

void
iconbar_lower(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_lower(ib->gui);
}

void
iconbar_stack_above(Evas_Object *o, Evas_Object *above)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_stack_above(ib->gui, above);
}

void
iconbar_stack_below(Evas_Object *o, Evas_Object *below)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_stack_below(ib->gui, below);
}

void
iconbar_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_move(ib->gui, x, y);
//  iconbar_icons_fix(ib);
}

void
iconbar_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_resize(ib->gui, w, h);

  if (w > h)
  {
    esmart_container_direction_set(ib->cont, 0);
    esmart_container_fill_policy_set(ib->cont, CONTAINER_FILL_POLICY_FILL_Y |
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
    esmart_container_padding_set(ib->cont, 5, 5, 11, 11);
  }
  else
  {
    esmart_container_direction_set(ib->cont, 1);
    esmart_container_fill_policy_set(ib->cont, CONTAINER_FILL_POLICY_FILL_X |
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
    esmart_container_padding_set(ib->cont, 11, 11, 5, 5);
  }

}

void
iconbar_show(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);

  /* show the iconbar */
  if (ib->gui)
    evas_object_show(ib->gui);

  evas_object_show(ib->cont);
}

void
iconbar_hide(Evas_Object *o)
{
  Iconbar *ib;
//Evas_List *l;

  ib = evas_object_smart_data_get(o);

  if (ib->gui)
    evas_object_hide(ib->gui);

  evas_object_hide(ib->cont);
}

void
iconbar_color_set(Evas_Object *o, int r, int g, int b, int a)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_color_set(ib->gui, r, g, b, a);
}

void
iconbar_clip_set(Evas_Object *o, Evas_Object *clip)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_clip_set(ib->gui, clip);
}

void
iconbar_clip_unset(Evas_Object *o)
{
  Iconbar *ib;

  ib = evas_object_smart_data_get(o);
  evas_object_clip_unset(ib->gui);
}
