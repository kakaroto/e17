#include "ephoto.h"

#define TODO_ITEM_MIN_BATCH 16
	
#define PARENT_DIR "Up"

typedef struct _Ephoto_List_Browser Ephoto_List_Browser;

struct _Ephoto_List_Browser
{
   Ephoto *ephoto;
   Ephoto_Entry *entry;
   Evas_Object *list;
   Eio_File *ls;
   Eina_List *todo_items;
   Eina_List *list_items;
   Eina_List *handlers;

   struct {
      Ecore_Animator *todo_items;
   } animator;
   Eina_Bool list_deleted : 1;
};

static Elm_Genlist_Item_Class _ephoto_list_item_class;
static Elm_Genlist_Item_Class _ephoto_list_up_item_class;
static Ephoto_Entry *up_entry;

static void
_todo_items_free(Ephoto_List_Browser *lb)
{
   eina_list_free(lb->todo_items);
   lb->todo_items = NULL;
}

static void
_list_items_free(Ephoto_List_Browser *lb)
{
   eina_list_free(lb->list_items);
   lb->list_items = NULL;
}

static char *
_ephoto_list_item_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Ephoto_Entry *e = data;
   return strdup(e->label);
}

static Evas_Object *
_ephoto_list_item_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   if (strcmp(part, "elm.swallow.end"))
     return ephoto_list_icon_add(e->ephoto, obj, "folder");
   else
     return NULL;
}

static Evas_Object *
_ephoto_list_item_up_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   if (strcmp(part, "elm.swallow.end"))
     return ephoto_list_icon_add(e->ephoto, obj, "go-up");
   else
     return NULL;
}

static void
_ephoto_list_item_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{

}

static void
_change_dir(void *data, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Entry *e = data;

   ephoto_directory_set(e->ephoto, e->path);
}

static int
_entry_cmp(const void *pa, const void *pb)
{
   const Ephoto_Entry *a = pa, *b = pb;
   int ret, s;
 
   s = strcmp(a->basename, b->basename);
   if (s > 0)
     ret = 1;
   else if (s < 0)
     ret = -1;
   else
     ret = 0; 
   return ret;
}

static void
_entry_item_add(Ephoto_List_Browser *lb, Ephoto_Entry *e)
{
   const Elm_Genlist_Item_Class *ic;
 
   ic = &_ephoto_list_item_class;

   if (!lb->list_items)
     {
        e->list_item = elm_genlist_item_append
                         (lb->list, ic, e, NULL, ELM_GENLIST_ITEM_NONE, _change_dir, e);
        lb->list_items = eina_list_append(lb->list_items, e);
     }
   else
     {
        int near_cmp;
        Ephoto_Entry *near_entry;
        Elm_Genlist_Item *near_item;
        Eina_List *near_node = eina_list_search_sorted_near_list
          (lb->list_items, _entry_cmp, e, &near_cmp);
       
        near_entry = near_node->data;
        near_item = near_entry->list_item;
        if (near_cmp < 0)
          {
             e->list_item = elm_genlist_item_insert_after
               (lb->list, ic, e, NULL, near_item, ELM_GENLIST_ITEM_NONE, _change_dir, e);
             lb->list_items = eina_list_append_relative_list
               (lb->list_items, e, near_node);
          }
        else
          {
             e->list_item = elm_genlist_item_insert_before
               (lb->list, ic, e, NULL, near_item, ELM_GENLIST_ITEM_NONE, _change_dir, e);
             lb->list_items = eina_list_prepend_relative_list
               (lb->list_items, e, near_node);
          }
     }
   if (e->list_item)
     elm_genlist_item_data_set(e->list_item, e);
   else
     {
        ERR("could not add item to list: path '%s'", e->path);
        ephoto_entry_free(e);
        return;
     }
}

static Eina_Bool
_todo_items_process(void *data)
{
   Ephoto_List_Browser *lb = data;
   Ephoto_Entry *entry;

   if ((lb->ls) && (eina_list_count(lb->todo_items) < TODO_ITEM_MIN_BATCH))
     return EINA_TRUE;

   lb->animator.todo_items = NULL;

   EINA_LIST_FREE(lb->todo_items, entry)
     _entry_item_add(lb, entry);

   return EINA_FALSE;
}

static void
_list_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_List_Browser *lb = data;
   Ecore_Event_Handler *handler;
	
   _todo_items_free(lb);
   _list_items_free(lb);
   EINA_LIST_FREE(lb->handlers, handler)
     ecore_event_handler_del(handler);

   if (lb->animator.todo_items)
     {
        ecore_animator_del(lb->animator.todo_items);
        lb->animator.todo_items = NULL;
     }
   if (lb->ls)
     {
        lb->list_deleted = EINA_TRUE;
        eio_file_cancel(lb->ls);
        return;
     }
   free(lb);
}

static Eina_Bool
_ephoto_list_populate_start(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_List_Browser *lb = data;
   char *parent_dir;

   _todo_items_free(lb);
   _list_items_free(lb);
   elm_genlist_clear(lb->list);

   parent_dir = ecore_file_dir_get(lb->ephoto->config->directory);
   if (parent_dir && strcmp(lb->ephoto->config->directory, "/"))
     {
        Elm_Genlist_Item_Class *ic;

        if (up_entry)
          ephoto_entry_free(up_entry);
        up_entry = ephoto_entry_new(lb->ephoto, parent_dir, "Up");
        ic = &_ephoto_list_up_item_class;
        elm_genlist_item_append
           (lb->list, ic, up_entry, NULL, ELM_GENLIST_ITEM_NONE, _change_dir, up_entry);
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_list_populate_end(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_List_Browser *lb = data;

   lb->ls = NULL;
   if (lb->list_deleted)
     {
        free(lb);
        return ECORE_CALLBACK_PASS_ON;
     }

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_list_populate_error(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_list_entry_create(void *data, int type __UNUSED__, void *event)
{
   Ephoto_List_Browser *lb = data;
   Ephoto_Event_Entry_Create *ev = event;
   Ephoto_Entry *e;

   e = ev->entry;
   lb->todo_items = eina_list_append(lb->todo_items, e);

   if (!lb->animator.todo_items)
     lb->animator.todo_items = ecore_animator_add(_todo_items_process, lb);

   return ECORE_CALLBACK_PASS_ON;
}

Evas_Object *
ephoto_list_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Ephoto_List_Browser *lb;
   char *parent_dir;

   lb = calloc(1, sizeof(Ephoto_List_Browser));
   EINA_SAFETY_ON_NULL_GOTO(lb, error);
   lb->ephoto = ephoto;

   _ephoto_list_item_class.item_style = "default";
   _ephoto_list_item_class.func.label_get = _ephoto_list_item_label_get;
   _ephoto_list_item_class.func.icon_get = _ephoto_list_item_icon_get;
   _ephoto_list_item_class.func.state_get = NULL;
   _ephoto_list_item_class.func.del = _ephoto_list_item_del;

   _ephoto_list_up_item_class.item_style = "default";
   _ephoto_list_up_item_class.func.label_get = _ephoto_list_item_label_get;
   _ephoto_list_up_item_class.func.icon_get = _ephoto_list_item_up_icon_get;
   _ephoto_list_up_item_class.func.state_get = NULL;
   _ephoto_list_up_item_class.func.del = _ephoto_list_item_del;
   
   lb->list = elm_genlist_add(parent);
   elm_genlist_multi_select_set(lb->list, EINA_FALSE);
   elm_genlist_horizontal_mode_set(lb->list, EINA_FALSE);
   evas_object_data_set(lb->list, "list_browser", lb);
   evas_object_event_callback_add(lb->list, EVAS_CALLBACK_DEL, _list_del, lb);
   evas_object_show(lb->list);

   parent_dir = ecore_file_dir_get(lb->ephoto->config->directory);
   if (parent_dir)
     {
        Elm_Genlist_Item_Class *ic;

        ic = &_ephoto_list_up_item_class;
        up_entry = ephoto_entry_new(lb->ephoto, parent_dir, "Up");
        elm_genlist_item_append
           (lb->list, ic, up_entry, NULL, ELM_GENLIST_ITEM_NONE, _change_dir, up_entry);
     }
   lb->handlers = eina_list_append
      (lb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_START, _ephoto_list_populate_start, lb));

   lb->handlers = eina_list_append
      (lb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_list_populate_end, lb));

   lb->handlers = eina_list_append
      (lb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_ERROR, _ephoto_list_populate_error, lb));

   lb->handlers = eina_list_append
      (lb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE_DIR, _ephoto_list_entry_create, lb));

   return lb->list;

 error:
   evas_object_del(lb->list);
   return NULL;
}

void
ephoto_list_browser_entry_set(Evas_Object *obj, Ephoto_Entry *entry)
{
   Ephoto_List_Browser *lb = evas_object_data_get(obj, "list_browser");
   lb->entry = entry;
}

