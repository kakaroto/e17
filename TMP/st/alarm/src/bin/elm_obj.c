#include <Elementary.h>
#include "elm_priv.h"

static void
_elm_obj_ref(Elm_Obj *obj)
{
   obj->ref++;
}

static void
_elm_obj_unref(Elm_Obj *obj)
{
   obj->ref--;
   if (obj->ref > 0) return;
   if (!obj->delete_me)
     {
	obj->delete_me = 1;
	_elm_callback_call(obj, ELM_CALLBACK_DEL, NULL);
     }
   if (obj->parent)
     obj->parent->children = evas_list_remove(obj->parent->children, obj);
   while (obj->callbacks)
     {
	((Elm_Obj *)obj->callbacks->data)->parent = NULL;
	((Elm_Obj *)obj->callbacks->data)->unref(obj->callbacks->data);
	obj->callbacks = evas_list_remove_list(obj->callbacks, obj->callbacks);
     }
   while (obj->children)
     {
	_elm_callback_call(obj, ELM_CALLBACK_CHILD_DEL, obj->children->data);
	((Elm_Obj *)obj->children->data)->parent = NULL;
	((Elm_Obj *)obj->children->data)->unref(obj->children->data);
	obj->children = evas_list_remove_list(obj->children, obj->children);
     }
   free(obj);
}

static void
_elm_obj_del(Elm_Obj *obj)
{
   if (!obj->delete_me)
     {
	obj->delete_me = 1;
	_elm_callback_call(obj, ELM_CALLBACK_DEL, NULL);
     }
   _elm_obj_unref(obj);
}

static Elm_Callback *
_elm_obj_callback_add(Elm_Obj *obj, Elm_Callback_Type type, Elm_Callback_Func func, void *data)
{
   Elm_Callback *cb;
   
   cb = _elm_callback_new();
   cb->type = type;
   cb->func = func;
   cb->data = data;
   cb->parent = obj;
   obj->callbacks = evas_list_append(obj->callbacks, cb);
}

static void
_elm_obj_child_add(Elm_Obj *obj, Elm_Obj *child)
{
   obj->children = evas_list_append(obj->children, child);
   child->parent = obj;
   _elm_callback_call(obj, ELM_CALLBACK_CHILD_DEL, child);
}

void
_elm_obj_init(Elm_Obj *obj)
{
   obj->del = _elm_obj_del;
   obj->ref = _elm_obj_ref;
   obj->unref = _elm_obj_unref;
   obj->callback_add = _elm_obj_callback_add;
   obj->child_add = _elm_obj_child_add;
   obj->refs = 1;
}
