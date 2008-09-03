#include <Elementary.h>
#include "elm_priv.h"

Elm_Callback *
_elm_callback_new(void)
{
   Elm_Callback *cb;
   
   cb = ELM_NEW(Elm_Callback);
   _elm_obj_init(ELM_OBJ(cb));
   return cb;
}

void
_elm_callback_call(Elm_Obj *obj, Elm_Callback_Type type, void *info)
{
   // FIXME: call - safely. keep bitmask as to if a callback type is there
   // or not for efficiency
   Evas_List *l;
   
   for (l = obj->callbacks; l; l = l->next)
     {
	Elm_Callback *cb;
	
	cb = l->data;
	if (cb->delete_me) continue;
	if (cb->type == type)
	  {
	     // FIXME: fi callback deletes cb or obj or a parent obj... what
	     // then?
	     if (cb->func) cb->func(cb->data, obj, type, info);
	     if (cb->callbacks) _elm_callback_call(cb, type, info);
	  }
     }
}
