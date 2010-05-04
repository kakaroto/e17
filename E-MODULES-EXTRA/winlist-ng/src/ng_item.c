#include "e_mod_main.h"

static void     _ngw_item_del_icon(Ngw_Item *ic);
static void     _ngw_item_border_set_icon(Ngw_Item *it);


Ngw_Item *
ngw_item_new(Ng *ng, E_Border *bd)
{
  Ngw_Item *it;

  it = E_NEW(Ngw_Item, 1);
  it->ng = ng;
  it->border = NULL;
  it->label = NULL;
  it->o_icon = NULL;
  it->scale = 1.0;
  it->desk = NULL;
  it->bd_above = NULL;
  it->was_iconified = 0;
  it->was_shaded = 0;
  it->set_state = 0;
  
  it->obj = edje_object_add(ng->win->evas);

  if (!e_theme_edje_object_set(it->obj, "base/theme/modules/ng", "e/modules/ng/icon"))
    edje_object_file_set(it->obj, ngw_config->theme_path, "e/modules/ng/icon");

  evas_object_show(it->obj);

  e_object_ref(E_OBJECT(bd));
  it->border = bd;

  _ngw_item_border_set_icon(it);
  ngw_box_item_show(ng->box, it);
  ngw_item_signal_emit(it, "inactive");
 
  return it;
}

static void
_ngw_item_border_set_icon(Ngw_Item *it)
{
  it->o_icon = e_border_icon_add(it->border, it->ng->win->evas);
  edje_object_part_swallow(it->obj, "e.swallow.content", it->o_icon);
  evas_object_pass_events_set(it->o_icon, 1);
  evas_object_show(it->o_icon);
}

void
ngw_item_remove(Ngw_Item *it)
{
  ngw_box_item_remove(it->ng->box, it);
}

void
ngw_item_free(Ngw_Item *it)
{
  it->ng->items = eina_list_remove(it->ng->items, it);

  _ngw_item_del_icon(it);
  evas_object_del(it->obj);
  if(it->label) free(it->label);

  e_object_unref(E_OBJECT(it->border));
  
  free(it);
  it = NULL;
}

static void
_ngw_item_del_icon(Ngw_Item *it)
{
  if (it->o_icon)
    {
      edje_object_part_unswallow(it->obj, it->o_icon);
      evas_object_del(it->o_icon);
      it->o_icon = NULL;
    }
}

void
ngw_item_signal_emit(Ngw_Item *it, char *sig)
{
   if (it->obj)  edje_object_signal_emit(it->obj, sig, "e");
}



