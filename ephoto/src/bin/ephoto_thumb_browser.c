#include "ephoto.h"

#define ZOOM_MAX 512
#define ZOOM_MED 256
#define ZOOM_MIN 128
#define ZOOM_START 192
#define ZOOM_STEP 32
#define TODO_ITEM_MIN_BATCH 16

static void _todo_items_free(void);
static void _grid_items_free(void);
static void _entry_item_add(Ephoto_Entry *e);
static Eina_Bool _todo_items_process(void *data __UNUSED__);
static Eina_Bool _ephoto_thumb_entry_create(void *data __UNUSED__, int type __UNUSED__, void *event);
static Eina_Bool _ephoto_thumb_populate_error(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__);
static Eina_Bool _ephoto_thumb_populate_end(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__);
static Eina_Bool _ephoto_thumb_populate_start(void *data, int type __UNUSED__, void *event __UNUSED__);
static Evas_Object *_ephoto_thumbnail_icon_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__);
static char *_ephoto_thumbnail_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__);
static void _ephoto_thumbnail_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__);
static void _ephoto_change_dir(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info);
static void _ephoto_zoom_in(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_zoom_out(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_show_flow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);
static void _ephoto_show_slideshow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__);

static const Elm_Gengrid_Item_Class _ephoto_thumbnail_class = {
     "thumb",
     {
        _ephoto_thumbnail_label_get,
        _ephoto_thumbnail_icon_get,
        NULL,
        _ephoto_thumbnail_del
     }
   };

typedef struct _Ephoto_Thumb_Browser Ephoto_Thumb_Browser;
struct _Ephoto_Thumb_Browser
{
   Evas_Object *box;
   Evas_Object *dir_entry;
   Evas_Object *grid;
   Evas_Object *toolbar;
   Eina_List *handlers;
   Eina_List *todo_items;
   Eina_List *grid_items;
   Eio_File *ls;
   int thumb_size;
   struct {
      Elm_Toolbar_Item *zoom_in;
      Elm_Toolbar_Item *zoom_out;
      Elm_Toolbar_Item *view_flow;
      Elm_Toolbar_Item *slideshow;
   } action;
   struct {
     Ecore_Animator *todo_items;
   } animator;
};

Ephoto_Thumb_Browser *etb;

Evas_Object *
ephoto_thumb_browser_add(void)
{
   etb = calloc(1, sizeof(Ephoto_Thumb_Browser));
   etb->thumb_size = ZOOM_START;

   etb->box = elm_box_add(ephoto->win);
   elm_box_horizontal_set(etb->box, EINA_FALSE);
   elm_box_homogenous_set(etb->box, EINA_FALSE);
   evas_object_size_hint_weight_set
     (etb->box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ephoto->win, etb->box);
   evas_object_show(etb->box);

   etb->toolbar = elm_toolbar_add(etb->box);
   elm_toolbar_homogenous_set(etb->toolbar, EINA_TRUE);
   elm_toolbar_mode_shrink_set(etb->toolbar, ELM_TOOLBAR_SHRINK_MENU);
   elm_toolbar_menu_parent_set(etb->toolbar, etb->box);
   evas_object_size_hint_weight_set(etb->toolbar, 0.0, 0.0);
   evas_object_size_hint_align_set(etb->toolbar, EVAS_HINT_FILL, 0.0);

   etb->action.zoom_in = elm_toolbar_item_append
     (etb->toolbar, "add", "Zoom In", _ephoto_zoom_in, NULL);
   elm_toolbar_item_priority_set(etb->action.zoom_in, 0);
   etb->action.zoom_out = elm_toolbar_item_append
     (etb->toolbar, "remove", "Zoom Out", _ephoto_zoom_out, NULL);
   elm_toolbar_item_priority_set(etb->action.zoom_out, 1);
   etb->action.view_flow = elm_toolbar_item_append
     (etb->toolbar, "image", "View Flow", _ephoto_show_flow, NULL);
   elm_toolbar_item_priority_set(etb->action.view_flow, 2);
   etb->action.slideshow = elm_toolbar_item_append
     (etb->toolbar, "media-playback-start", "Slideshow", _ephoto_show_slideshow, NULL);
   elm_toolbar_item_priority_set(etb->action.slideshow, 3);

   elm_toolbar_icon_size_set(etb->toolbar, 32);
   elm_box_pack_end(etb->box, etb->toolbar);
   evas_object_show(etb->toolbar);

   etb->dir_entry = elm_fileselector_entry_add(etb->box);
   elm_fileselector_entry_path_set(etb->dir_entry, ephoto->directory);
   elm_fileselector_entry_button_label_set(etb->dir_entry, "Choose");
   elm_fileselector_entry_is_save_set(etb->dir_entry, EINA_FALSE);
   elm_fileselector_entry_inwin_mode_set(etb->dir_entry, EINA_TRUE);
   evas_object_size_hint_weight_set(etb->dir_entry, 0.0, 0.0);
   evas_object_size_hint_align_set(etb->dir_entry, EVAS_HINT_FILL, 0.0);
   evas_object_smart_callback_add
     (etb->dir_entry, "file,chosen", _ephoto_change_dir, NULL);
   elm_box_pack_end(etb->box, etb->dir_entry);
   evas_object_show(etb->dir_entry);

   etb->grid = elm_gengrid_add(etb->box);
   elm_gengrid_align_set(etb->grid, 0.5, 0.5);
   elm_gengrid_bounce_set(etb->grid, EINA_FALSE, EINA_TRUE);
   elm_gengrid_item_size_set(etb->grid, etb->thumb_size, etb->thumb_size);
   evas_object_size_hint_align_set
     (etb->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set
     (etb->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_box_pack_end(etb->box, etb->grid);
   evas_object_smart_callback_add(etb->grid, "clicked", _ephoto_show_flow, NULL);
   evas_object_show(etb->grid);

   etb->handlers = eina_list_append
      (etb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_START, _ephoto_thumb_populate_start, NULL));


   etb->handlers = eina_list_append
      (etb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_thumb_populate_end, NULL));

   etb->handlers = eina_list_append
      (etb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_ERROR, _ephoto_thumb_populate_error, NULL));

   etb->handlers = eina_list_append
      (etb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE, _ephoto_thumb_entry_create, NULL));

   return etb->box;
}

void
ephoto_thumb_browser_entry_set(Ephoto_Entry *entry)
{
   if ((entry) && (entry->item))
     elm_gengrid_item_bring_in(entry->item);
}

void
ephoto_thumb_browser_del(void)
{
   Ecore_Event_Handler *handler;

   evas_object_del(etb->box);
   _todo_items_free();
   _grid_items_free();
   EINA_LIST_FREE(etb->handlers, handler)
     ecore_event_handler_del(handler);
   if (etb->animator.todo_items)
     {
        ecore_animator_del(etb->animator.todo_items);
        etb->animator.todo_items = NULL;
     }
   if (etb->ls)
     {
        eio_file_cancel(etb->ls);
        return;
     }
   free(etb);
}

static void
_todo_items_free()
{
   eina_list_free(etb->todo_items);
   etb->todo_items = NULL;
}

static void
_grid_items_free()
{
   eina_list_free(etb->grid_items);
   etb->grid_items = NULL;
}

static void
_entry_item_add(Ephoto_Entry *e)
{
   const Elm_Gengrid_Item_Class *ic = &_ephoto_thumbnail_class;

   e->item = elm_gengrid_item_append(etb->grid, ic, e, NULL, NULL);
   etb->grid_items = eina_list_append(etb->grid_items, e->item);

   if (e->item)
     elm_gengrid_item_data_set(e->item, e);
   else
     ephoto_entry_free(e);
}

static Eina_Bool
_todo_items_process(void *data __UNUSED__)
{
   Ephoto_Entry *entry;

   if ((etb->ls) && (eina_list_count(etb->todo_items) < TODO_ITEM_MIN_BATCH))
     return EINA_TRUE;
   etb->animator.todo_items = NULL;

   EINA_LIST_FREE(etb->todo_items, entry)
     _entry_item_add(entry);
   return EINA_FALSE;
}

static Eina_Bool
_ephoto_thumb_populate_start(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   _todo_items_free();
   _grid_items_free();
   elm_gengrid_clear(etb->grid);
   elm_fileselector_entry_path_set(etb->dir_entry, ephoto->directory);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_populate_end(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   etb->ls = NULL;
   
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_populate_error(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_entry_create(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ephoto_Event_Entry_Create *ev = event;
   Ephoto_Entry *e;

   e = ev->entry;
   etb->todo_items = eina_list_append(etb->todo_items, e);

   if (!etb->animator.todo_items)
     etb->animator.todo_items = ecore_animator_add(_todo_items_process, NULL);

   return ECORE_CALLBACK_PASS_ON;
}

static Evas_Object *
_ephoto_thumbnail_icon_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Ephoto_Entry *entry = data;
   Evas_Object *o;
   const char *ext = strrchr(entry->path, '.');

   if (!strncmp(part, "elm.swallow.icon.", sizeof("elm.swallow.icon.") - 1) != 0)
     return NULL;

   if (ext)
     {
        if ((!strcasecmp(ext, "jpg")) || (!strcasecmp(ext, "jpeg")))
          ethumb_client_format_set(ephoto->client, ETHUMB_THUMB_JPEG);
     }
   else
     ethumb_client_format_set(ephoto->client, ETHUMB_THUMB_FDO);

   ethumb_client_size_set(ephoto->client, etb->thumb_size, etb->thumb_size);

   o = elm_thumb_add(ephoto->win);
   elm_object_style_set(o, "noframe");
   elm_thumb_file_set(o, entry->path, NULL);
   evas_object_show(o);

   return o;
}

static char *
_ephoto_thumbnail_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Ephoto_Entry *e = data;

   return strdup(e->label);
}

static void
_ephoto_thumbnail_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{

}

static void 
_ephoto_change_dir(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   const char *path = event_info;

   if (!path) return;

   ephoto_populate(path);
}

static void 
_ephoto_zoom_in(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set(etb->action.zoom_in, EINA_FALSE);
   etb->thumb_size += ZOOM_STEP;
   if (etb->thumb_size >= ZOOM_MAX)
     etb->thumb_size = ZOOM_MAX;
   elm_gengrid_item_size_set(etb->grid, etb->thumb_size, etb->thumb_size);
   elm_toolbar_item_disabled_set(etb->action.zoom_in, etb->thumb_size == ZOOM_MAX);
   elm_toolbar_item_disabled_set(etb->action.zoom_out, etb->thumb_size == ZOOM_MIN);
}

static void 
_ephoto_zoom_out(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   elm_toolbar_item_selected_set(etb->action.zoom_out, EINA_FALSE);
   etb->thumb_size -= ZOOM_STEP;
   if (etb->thumb_size <= ZOOM_MIN)
     etb->thumb_size = ZOOM_MIN;
   elm_gengrid_item_size_set(etb->grid, etb->thumb_size, etb->thumb_size);
   elm_toolbar_item_disabled_set(etb->action.zoom_out, etb->thumb_size == ZOOM_MIN);
   elm_toolbar_item_disabled_set(etb->action.zoom_in, etb->thumb_size == ZOOM_MAX);
}

static void
_ephoto_show_flow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Elm_Gengrid_Item *egi;
   Ephoto_Entry *entry;

   elm_toolbar_item_selected_set(etb->action.view_flow, EINA_FALSE);

   elm_object_unfocus(etb->grid);

   egi = elm_gengrid_selected_item_get(etb->grid);
   if (egi)
     entry = elm_gengrid_item_data_get(egi);
   else
     entry = eina_list_data_get
       (eina_list_nth_list(ephoto->entries, 0));
   if (!entry)
     return;

   printf("%s\n", entry->path);
   ephoto_flow_browser_show(entry); 
}

static void
_ephoto_show_slideshow(void *data __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Elm_Gengrid_Item *egi;
   Ephoto_Entry *entry;

   elm_toolbar_item_selected_set(etb->action.slideshow, EINA_FALSE);

   elm_object_unfocus(etb->grid);

   egi = elm_gengrid_selected_item_get(etb->grid);
   if (egi)
     entry = elm_gengrid_item_data_get(egi);
   else
     entry = eina_list_data_get
       (eina_list_nth_list(ephoto->entries, 0));
   if (!entry)
     return;

   ephoto_slideshow_show(entry);
}
