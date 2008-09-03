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
}
