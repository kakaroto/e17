#include <Elementary.h>
#include "elm_priv.h"

static void
_elm_cb_del(Elm_Obj *obj)
{
   if (_elm_obj_del_defer(obj)) return;
   if (obj->parent) /* callbacks are special children */
     {
	obj->parent->cbs = evas_list_remove(obj->parent->cbs, obj);
	obj->parent = NULL;
     }
   /* chain the original object type  - we keep the basic object class */
   ((Elm_Obj_Class *)(obj->clas))->del(obj);
}
    
Elm_Cb *
_elm_cb_new(void)
{
   Elm_Cb *cb;
   
   cb = ELM_NEW(Elm_Cb);
   _elm_obj_init(ELM_OBJ(cb));
   cb->type = ELM_OBJ_CB;

   cb->del = _elm_cb_del;

   return cb;
}

void
_elm_cb_call(Elm_Obj *obj, Elm_Cb_Type type, void *info)
{
   Evas_List *l;
   
   _elm_obj_nest_push();
   for (l = obj->cbs; l; l = l->next)
     {
	Elm_Cb *cb;
	
	cb = l->data;
	if (cb->delete_me) continue;
	if (cb->cb_type == type)
	  {
	     if (cb->func) cb->func(cb->data, obj, type, info);
	     if (cb->cbs) _elm_cb_call(cb, type, info);
	  }
     }
   _elm_obj_nest_pop();
}
