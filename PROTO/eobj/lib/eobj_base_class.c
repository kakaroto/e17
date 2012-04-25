#include <Eina.h>

#include "Eobj.h"
#include "eobj_private.h"

#include "config.h"

EAPI Eobj_Op EOBJ_BASE_BASE_ID = EOBJ_NOOP;

typedef struct
{
   Eina_Inlist *generic_data;
} Private_Data;

typedef struct
{
   EINA_INLIST;
   Eina_Stringshare *key;
   void *data;
   eobj_base_data_free_func free_func;
} Eobj_Generic_Data_Node;

static void
_eobj_generic_data_node_free(Eobj_Generic_Data_Node *node)
{
   eina_stringshare_del(node->key);
   if (node->free_func)
      node->free_func(node->data);
   free(node);
}

static void
_eobj_generic_data_del_all(Private_Data *pd)
{
   Eina_Inlist *nnode;
   Eobj_Generic_Data_Node *node;

   EINA_INLIST_FOREACH_SAFE(pd->generic_data, nnode, node)
     {
        pd->generic_data = eina_inlist_remove(pd->generic_data,
              EINA_INLIST_GET(node));

        _eobj_generic_data_node_free(node);
     }
}

static void
_data_set(Eobj *obj, void *class_data, va_list *list)
{
   Private_Data *pd = class_data;
   const char *key = va_arg(*list, const char *);
   const void *data = va_arg(*list, const void *);
   eobj_base_data_free_func free_func = va_arg(*list, eobj_base_data_free_func);

   Eobj_Generic_Data_Node *node;

   if (!key) return;

   eobj_do(obj, eobj_base_data_del(key));

   node = malloc(sizeof(Eobj_Generic_Data_Node));
   node->key = eina_stringshare_add(key);
   node->data = (void *) data;
   node->free_func = free_func;
   pd->generic_data = eina_inlist_prepend(pd->generic_data,
         EINA_INLIST_GET(node));
}

static void
_data_get(const Eobj *obj EINA_UNUSED, const void *class_data, va_list *list)
{
   /* We don't really change it... */
   Private_Data *pd = (Private_Data *) class_data;
   const char *key = va_arg(*list, const char *);
   void **data = va_arg(*list, void **);
   Eobj_Generic_Data_Node *node;

   if (!data) return;
   *data = NULL;

   if (!key) return;

   EINA_INLIST_FOREACH(pd->generic_data, node)
     {
        if (!strcmp(node->key, key))
          {
             pd->generic_data =
                eina_inlist_promote(pd->generic_data, EINA_INLIST_GET(node));
             *data = node->data;
             return;
          }
     }
}

static void
_data_del(Eobj *obj EINA_UNUSED, void *class_data, va_list *list)
{
   Private_Data *pd = class_data;
   const char *key = va_arg(*list, const char *);

   Eobj_Generic_Data_Node *node;

   if (!key) return;

   EINA_INLIST_FOREACH(pd->generic_data, node)
     {
        if (!strcmp(node->key, key))
          {
             pd->generic_data = eina_inlist_remove(pd->generic_data,
                   EINA_INLIST_GET(node));
             _eobj_generic_data_node_free(node);
             return;
          }
     }
}

/* Weak reference. */
static Eina_Bool
_eobj_weak_ref_cb(void *data, Eobj *obj EINA_UNUSED, const Eobj_Event_Description *desc EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Eobj **wref = data;
   *wref = NULL;

   return EOBJ_CALLBACK_CONTINUE;
}

static void
_wref_add(const Eobj *obj, const void *class_data EINA_UNUSED, va_list *list)
{
   Eobj **wref = va_arg(*list, Eobj **);

   *wref = (Eobj *) obj;
   /* FIXME: The cast and the one in the next func are both bad and should be
    * fixed once the event callback functions are fixed. */
   eobj_event_callback_add((Eobj *) obj, EOBJ_EV_DEL, _eobj_weak_ref_cb, wref);
}

static void
_wref_del(const Eobj *obj, const void *class_data EINA_UNUSED, va_list *list)
{
   Eobj **wref = va_arg(*list, Eobj **);
   if (*wref != obj)
     {
        ERR("Wref is a weak ref to %p, while this function was called on %p.",
              *wref, obj);
        return;
     }
   eobj_event_callback_del((Eobj *) obj, EOBJ_EV_DEL, _eobj_weak_ref_cb, wref);
}

/* EOF Weak reference. */


/* EOBJ_BASE_CLASS stuff */
#define MY_CLASS EOBJ_BASE_CLASS

/* FIXME: Set proper type descriptions. */
EAPI const Eobj_Event_Description _EOBJ_EV_CALLBACK_ADD =
   EOBJ_EVENT_DESCRIPTION("callback,add", "?", "A callback was added.");
EAPI const Eobj_Event_Description _EOBJ_EV_CALLBACK_DEL =
   EOBJ_EVENT_DESCRIPTION("callback,del", "?", "A callback was deleted.");
EAPI const Eobj_Event_Description _EOBJ_EV_FREE =
   EOBJ_EVENT_DESCRIPTION("free", "", "Obj is being freed.");
EAPI const Eobj_Event_Description _EOBJ_EV_DEL =
   EOBJ_EVENT_DESCRIPTION("del", "", "Obj is being deleted.");

static void
_constructor(Eobj *obj, void *class_data EINA_UNUSED)
{
   DBG("%p - %s.", obj, eobj_class_name_get(MY_CLASS));
}

static void
_destructor(Eobj *obj, void *class_data)
{
   DBG("%p - %s.", obj, eobj_class_name_get(MY_CLASS));

   _eobj_generic_data_del_all(class_data);
}

static void
_class_constructor(Eobj_Class *klass)
{
   const Eobj_Op_Func_Description func_desc[] = {
        EOBJ_OP_FUNC(EOBJ_BASE_ID(EOBJ_BASE_SUB_ID_DATA_SET), _data_set),
        EOBJ_OP_FUNC_CONST(EOBJ_BASE_ID(EOBJ_BASE_SUB_ID_DATA_GET), _data_get),
        EOBJ_OP_FUNC(EOBJ_BASE_ID(EOBJ_BASE_SUB_ID_DATA_DEL), _data_del),
        EOBJ_OP_FUNC_CONST(EOBJ_BASE_ID(EOBJ_BASE_SUB_ID_WREF_ADD), _wref_add),
        EOBJ_OP_FUNC_CONST(EOBJ_BASE_ID(EOBJ_BASE_SUB_ID_WREF_DEL), _wref_del),
        EOBJ_OP_FUNC_SENTINEL
   };

   eobj_class_funcs_set(klass, func_desc);
}

static const Eobj_Op_Description op_desc[] = {
     EOBJ_OP_DESCRIPTION(EOBJ_BASE_SUB_ID_DATA_SET, "?", "Set data for key."),
     EOBJ_OP_DESCRIPTION_CONST(EOBJ_BASE_SUB_ID_DATA_GET, "?", "Get data for key."),
     EOBJ_OP_DESCRIPTION(EOBJ_BASE_SUB_ID_DATA_DEL, "?", "Del key."),
     EOBJ_OP_DESCRIPTION_CONST(EOBJ_BASE_SUB_ID_WREF_ADD, "?", "Add a weak ref to the object."),
     EOBJ_OP_DESCRIPTION_CONST(EOBJ_BASE_SUB_ID_WREF_DEL, "?", "Delete the weak ref."),
     EOBJ_OP_DESCRIPTION_SENTINEL
};

static const Eobj_Event_Description *event_desc[] = {
     EOBJ_EV_CALLBACK_ADD,
     EOBJ_EV_CALLBACK_DEL,
     EOBJ_EV_FREE,
     EOBJ_EV_DEL,
     NULL
};

static const Eobj_Class_Description class_desc = {
     "Eobj Base",
     EOBJ_CLASS_TYPE_REGULAR_NO_INSTANT,
     EOBJ_CLASS_DESCRIPTION_OPS(&EOBJ_BASE_BASE_ID, op_desc, EOBJ_BASE_SUB_ID_LAST),
     event_desc,
     sizeof(Private_Data),
     _constructor,
     _destructor,
     _class_constructor,
     NULL
};

EOBJ_DEFINE_CLASS(eobj_base_class_get, &class_desc, NULL, NULL)
