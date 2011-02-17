#include "e_mod_main.h"


static Ngi_Item *
_ngi_item_new(Ngi_Box *box)
{
   Ngi_Item *it;

   it = E_NEW(Ngi_Item, 1);

   it->box = box;
   it->border = NULL;
   it->app = NULL;
   it->gadcon = NULL;
   it->cfg_gadcon = NULL;
   it->label = NULL;
   it->class = NULL;
   it->o_icon = NULL;
   it->o_icon2 = NULL;
   it->scale = 1.0;
   it->mouse_down = 0;
   it->visible = 1;
   it->size = 0;

   /* it->cb_free  = _ngi_item_cb_free; */
   it->cb_mouse_in = NULL;
   it->cb_mouse_out = NULL;
   it->cb_mouse_up = NULL;
   it->cb_mouse_down = NULL;
   it->cb_drag_start = NULL;

   it->overlay_signal_timer = NULL;
   return it;
}

Ngi_Item *
ngi_item_new(Ngi_Box *box)
{
   Ngi_Item *it = _ngi_item_new(box);
   int ok = 0;

   it->obj = edje_object_add(box->ng->evas);

   switch(box->ng->cfg->orient)
     {
      case E_GADCON_ORIENT_BOTTOM:
	 ok = ngi_object_theme_set(it->obj, "e/modules/itask-ng/icon_bottom");
   	 break;
      case E_GADCON_ORIENT_TOP:
	 ok = ngi_object_theme_set(it->obj, "e/modules/itask-ng/icon_top");
   	 break;
      case E_GADCON_ORIENT_LEFT:
	 ok = ngi_object_theme_set(it->obj, "e/modules/itask-ng/icon_left");
   	 break;
      case E_GADCON_ORIENT_RIGHT:
	 ok = ngi_object_theme_set(it->obj, "e/modules/itask-ng/icon_right");
   	 break;
     }

   /* TODO remove fallback */
   if (!ok)
     {
   	if (!e_theme_edje_object_set(it->obj, "base/theme/modules/itask-ng", "e/modules/itask-ng/icon"))
   	  edje_object_file_set(it->obj, ngi_config->theme_path, "e/modules/itask-ng/icon");
     }

   it->over = edje_object_add(box->ng->evas);

   switch(box->ng->cfg->orient)
     {
      case E_GADCON_ORIENT_BOTTOM:
	 ok = ngi_object_theme_set(it->over, "e/modules/itask-ng/icon_over_bottom");
   	 break;
      case E_GADCON_ORIENT_TOP:
	 ok = ngi_object_theme_set(it->over, "e/modules/itask-ng/icon_over_top");
   	 break;
      case E_GADCON_ORIENT_LEFT:
	 ok = ngi_object_theme_set(it->over, "e/modules/itask-ng/icon_over_left");
   	 break;
      case E_GADCON_ORIENT_RIGHT:
	 ok = ngi_object_theme_set(it->over, "e/modules/itask-ng/icon_over_right");
   	 break;
     }
   if (!ok)
     {
	if (!e_theme_edje_object_set(it->over, "base/theme/modules/itask-ng", "e/modules/itask-ng/icon_overlay"))
	  edje_object_file_set(it->over, ngi_config->theme_path, "e/modules/itask-ng/icon_overlay");
     }


   evas_object_layer_set(it->over, 9999);
   evas_object_show(it->over);

   evas_object_smart_member_add(it->obj, box->ng->o_icons);
   evas_object_show(it->obj);

   return it;
}

void
ngi_item_show(Ngi_Item *it, int instant)
{
   ngi_item_signal_emit(it, "e,state,item,show");

   if (instant)
     {
        it->state = normal;
        it->scale = 1.0;
     }
   else
     {
        it->start_time = ecore_time_get();
        it->state = appearing;
        it->scale = 0.0;
        it->box->ng->items_scaling =
	  eina_list_append(it->box->ng->items_scaling, it);
     }

   ngi_animate(it->box->ng);
}

void
ngi_item_remove(Ngi_Item *it)
{
   Ng *ng = it->box->ng;

   edje_object_signal_emit(it->obj, "e,state,item,hide", "e");
   it->start_time = ecore_time_get();
   it->state = disappearing;

   if (!eina_list_data_find(ng->items_scaling, it))
      ng->items_scaling = eina_list_append(ng->items_scaling, it);

   if (it == ng->item_active)
      ng->item_active = NULL;

   if (it == ng->item_drag)
      ng->item_drag = NULL;

   ngi_animate(ng);
}

EAPI void
ngi_item_free(Ngi_Item *it)
{
   ngi_item_del_icon(it);

   if (it->obj)
     {
        evas_object_clip_unset(it->obj);
        evas_object_del(it->obj);
     }

   if (it->over)
     {
        /* evas_object_clip_unset(it->over); */
        evas_object_del(it->over);
     }

   if (it->border)
      e_object_unref(E_OBJECT(it->border));

   if (it->gadcon)
      e_object_del(E_OBJECT(it->gadcon));

   if (it->app)
      efreet_desktop_unref(it->app);

   if (it->label)
      eina_stringshare_del(it->label);

   if (it->class)
      eina_stringshare_del(it->class);

   if (it->overlay_signal_timer)
      ecore_timer_del(it->overlay_signal_timer);

   if (it->box && it->box->ng)
     {
        if (it->box->ng->item_active)
           it->box->ng->item_active = NULL;

        if (it->box->ng->item_drag)
           it->box->ng->item_drag = NULL;
     }

   E_FREE(it);
}

void
ngi_item_del_icon(Ngi_Item *it)
{
   if (it->o_icon)
     {
        edje_object_part_unswallow(it->obj, it->o_icon);
        evas_object_del(it->o_icon);
        it->o_icon = NULL;
     }

   if (it->o_icon2)
     {
        edje_object_part_unswallow(it->over, it->o_icon2);
        evas_object_del(it->o_icon2);
        it->o_icon2 = NULL;
     }
}

void
ngi_item_signal_emit(Ngi_Item *it, char *sig)
{
   edje_object_signal_emit(it->obj, sig, "e");
   edje_object_signal_emit(it->over, sig, "e");
}

void
ngi_item_mouse_in(Ngi_Item *it)
{
   if (!it) return;

   ngi_item_signal_emit(it,"e,state,mouse,in");

   if (it->cb_mouse_in)
     it->cb_mouse_in(it);
}

void
ngi_item_mouse_out(Ngi_Item *it)
{
   if (!it) return;

   ngi_item_signal_emit(it,"e,state,mouse,out");

   if (it->cb_mouse_out)
     it->cb_mouse_out(it);
}

void
ngi_item_mouse_down(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
   if (!it) return;

   ngi_item_signal_emit(it,"e,state,mouse,down");

   if (it->cb_mouse_down)
     it->cb_mouse_down(it, ev);
}

void
ngi_item_mouse_up(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
   if (!it) return;

   ngi_item_signal_emit(it,"e,state,mouse,up");

   if (it->cb_mouse_up)
     it->cb_mouse_up(it, ev);
}

