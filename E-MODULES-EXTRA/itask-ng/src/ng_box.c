#include "e_mod_main.h"

Ngi_Box *
ngi_box_new(Ng *ng)
{
   Ngi_Box *box;

   box = E_NEW(Ngi_Box, 1);
   if (!box)
      return NULL;

   box->ng = ng;
   box->pos = 0;
   box->apps = NULL;
   box->drop_handler = NULL;
   box->item_drop = NULL;
   box->dnd_timer = NULL;
   box->items = NULL;

   box->separator = edje_object_add(ng->evas);

   switch(box->ng->cfg->orient)
     {
      case E_GADCON_ORIENT_BOTTOM:
	 ngi_object_theme_set(box->separator, "e/modules/itask-ng/separator_bottom");
	 break;
      case E_GADCON_ORIENT_TOP:
	 ngi_object_theme_set(box->separator, "e/modules/itask-ng/separator_top");
         break;
      case E_GADCON_ORIENT_LEFT:
	 ngi_object_theme_set(box->separator, "e/modules/itask-ng/separator_left");
         break;
      case E_GADCON_ORIENT_RIGHT:
	 ngi_object_theme_set(box->separator, "e/modules/itask-ng/separator_right");
         break;
     }

   const char *clip_to = edje_object_data_get(box->separator, "clip_to_background");

   if (clip_to && atoi(clip_to))
      evas_object_clip_set(box->separator, ng->bg_clip);

   evas_object_clip_set(box->separator, ng->bg_clip);

   ng->boxes = eina_list_append(ng->boxes, box);

   return box;
}

void
ngi_box_free(Ngi_Box *box)
{
   Ngi_Item *it;

   box->ng->boxes = eina_list_remove(box->ng->boxes, box);

   EINA_LIST_FREE(box->items, it)
   ngi_item_free(it);

   if (box->separator)
      evas_object_del(box->separator);

   E_FREE(box);
}

void
ngi_box_item_show(Ng *ng, Ngi_Item *it, int instant)
{
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
        ng->items_scaling = eina_list_append(ng->items_scaling, it);
     }

   evas_object_clip_set(it->obj,  ng->clip);
   evas_object_clip_set(it->over, ng->clip);

   ng->changed = 1;

   if (ng->state != hidden || !ng->cfg->autohide)
      ngi_bar_show(ng);

   //evas_object_show(ng->clip);

   ngi_animate(ng);
}

Ngi_Item *
ngi_box_item_at_position_get(Ngi_Box *box)
{
   Eina_List *l;
   Ngi_Item *it;
   Ng *ng = box->ng;
   int pos = ng->pos;

   for(l = box->items; l; l = l->next)
     {
        it = l->data;

        if(pos <= it->pos + ng->size + ng->opt.item_spacing)
          {
             if(pos + ng->opt.item_spacing < it->pos - ng->size)
                return NULL;

	     return it;
          }
     }
   return NULL;
}

