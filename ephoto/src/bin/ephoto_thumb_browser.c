#include "ephoto.h"

#define ZOOM_MAX 512
#define ZOOM_MIN 128
#define ZOOM_STEP 32

#define TODO_ITEM_MIN_BATCH 16

#define PARENT_DIR "Up"

typedef struct _Ephoto_Thumb_Browser Ephoto_Thumb_Browser;

struct _Ephoto_Thumb_Browser
{
   Ephoto *ephoto;
   Evas_Object *layout;
   Evas_Object *edje;
   Evas_Object *fsel;
   Evas_Object *grid;
   Evas_Object *dir_grid;
   Evas_Object *toolbar;
   Evas_Object *overlay;
   Eio_File *ls;
   Eina_List *todo_items;
   Eina_List *grid_items;
   Eina_List *dir_grid_items;
   Eina_List *handlers;
   struct {
      Elm_Toolbar_Item *zoom_in;
      Elm_Toolbar_Item *zoom_out;
      Elm_Toolbar_Item *view_flow;
      Elm_Toolbar_Item *slideshow;
   } action;
   struct {
      Ecore_Animator *todo_items;
   } animator;
   Eina_Bool layout_deleted : 1;
};

static void
_todo_items_free(Ephoto_Thumb_Browser *tb)
{
   eina_list_free(tb->todo_items);
   tb->todo_items = NULL;
}

static void
_grid_items_free(Ephoto_Thumb_Browser *tb)
{
   eina_list_free(tb->grid_items);
   tb->grid_items = NULL;
   eina_list_free(tb->dir_grid_items);
   tb->dir_grid_items = NULL;
}

static Ephoto_Entry *
_first_file_entry_find(Ephoto_Thumb_Browser *tb)
{
   const Eina_List *l;
   Ephoto_Entry *entry;
   EINA_LIST_FOREACH(tb->ephoto->entries, l, entry)
     if (!entry->is_dir) return entry;
   return NULL;
}

static char *
_ephoto_thumb_item_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Ephoto_Entry *e = data;
   return strdup(e->label);
}

static Evas_Object *
_ephoto_thumb_dir_icon_get(void *data, Evas_Object *obj, const char *part)
{
   Ephoto_Entry *e = data;
   const char *f;
   int n;

   if (strncmp(part, "elm.swallow.icon.", sizeof("elm.swallow.icon.") - 1) != 0)
     return NULL;

   n = atoi(part + sizeof("elm.swallow.icon.") - 1);
   if (n < 1)
     return NULL;
   n--;

   f = eina_list_nth(e->dir_files, n);
   if (f)
     return ephoto_thumb_add(e->ephoto, obj, f);

   if (e->dir_files_checked)
     return NULL;

   return ephoto_directory_thumb_add(obj, e);
}

static Eina_Bool
_ephoto_thumb_dir_state_get(void *data, Evas_Object *obj __UNUSED__, const char *part)
{
   Ephoto_Entry *e = data;
   int n;

   if (strcmp(part, "have_files") == 0)
     return !!e->dir_files;

   if (strncmp(part, "have_file.", sizeof("have_file.") - 1) != 0)
     return EINA_FALSE;

   n = atoi(part + sizeof("have_file.") - 1);
   if (n < 1)
     return EINA_FALSE;
   return n <= (int)eina_list_count(e->dir_files);
}

static Evas_Object *
_ephoto_thumb_file_icon_get(void *data, Evas_Object *obj, const char *part __UNUSED__)
{
   Ephoto_Entry *e = data;
   return ephoto_thumb_add(e->ephoto, obj, e->path);
}

static void
_ephoto_thumb_item_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{
   /* FIXME: the entry is already freed when changing directories
    * One solution is to take care of this cleaning when manually removing
    * some grid items
   Ephoto_Entry *e = data;
   e->item = NULL;
   */
}

static const Elm_Gengrid_Item_Class _ephoto_thumb_dir_class = {
  "album-preview",
  {
    _ephoto_thumb_item_label_get,
    _ephoto_thumb_dir_icon_get,
    _ephoto_thumb_dir_state_get,
    _ephoto_thumb_item_del
  }
};

static const Elm_Gengrid_Item_Class _ephoto_thumb_file_class = {
  "thumb",
  {
    _ephoto_thumb_item_label_get,
    _ephoto_thumb_file_icon_get,
    NULL,
    _ephoto_thumb_item_del
  }
};

static int
_entry_cmp(const void *pa, const void *pb)
{
   const Elm_Gengrid_Item *ia = pa;
   const Ephoto_Entry *a, *b = pb;

   a = elm_gengrid_item_data_get(ia);

   if (a->is_dir == b->is_dir)
     return strcoll(a->basename, b->basename);
   else if (a->is_dir)
     return -1;
   else
     return 1;
}

static void
_entry_item_add(Ephoto_Thumb_Browser *tb, Ephoto_Entry *e)
{
   const Elm_Gengrid_Item_Class *ic;
   int near_cmp;
   Evas_Object *grid_of_choice;
   Elm_Gengrid_Item *near_item = NULL;
   Eina_List *near_node = NULL, *list_of_choice = NULL;
   
   
   if (e->is_dir)
     {
        ic = &_ephoto_thumb_dir_class;
        grid_of_choice = tb->dir_grid;
        list_of_choice = tb->dir_grid_items;
     }
   else
     {
        ic = &_ephoto_thumb_file_class;
        grid_of_choice = tb->grid;
        list_of_choice = tb->grid_items;
     }

   near_node = eina_list_search_sorted_near_list
     (list_of_choice, _entry_cmp, e, &near_cmp);

   if (near_node)
     near_item = near_node->data;

   if (!near_item)
     {
        e->item = elm_gengrid_item_append(grid_of_choice, ic, e, NULL, NULL);
        list_of_choice = eina_list_append(list_of_choice, e->item);
     }
   else
     {
        if (near_cmp < 0)
          {
             e->item = elm_gengrid_item_insert_after
                (grid_of_choice, ic, e, near_item, NULL, NULL);
             list_of_choice = eina_list_append_relative
                (list_of_choice, e->item, near_item);
          }
        else
          {
             e->item = elm_gengrid_item_insert_before
                (grid_of_choice, ic, e, near_item, NULL, NULL);
             list_of_choice = eina_list_prepend_relative
                (list_of_choice, e->item, near_item);
          }
     }

   if (e->item)
     elm_gengrid_item_data_set(e->item, e);
   else
     {
        ERR("could not add item to grid: path '%s'", e->path);
        ephoto_entry_free(e);
        return;
     }
}

static Eina_Bool
_todo_items_process(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   Edje_Message_Int msg;
   Ephoto_Entry *entry;

   if ((tb->ls) && (eina_list_count(tb->todo_items) < TODO_ITEM_MIN_BATCH))
     return EINA_TRUE;

   tb->animator.todo_items = NULL;

   EINA_LIST_FREE(tb->todo_items, entry)
     _entry_item_add(tb, entry);

   msg.val = eina_list_count(tb->ephoto->entries);
   edje_object_message_send(tb->edje, EDJE_MESSAGE_INT, 1, &msg);

   return EINA_FALSE;
}

static void
_ephoto_thumb_selected(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = event_info;
   Ephoto_Entry *e = elm_gengrid_item_data_get(it);

   elm_gengrid_item_selected_set(it, EINA_FALSE);

   if (e->is_dir)
     ephoto_directory_set(tb->ephoto, e->path);
   else
     evas_object_smart_callback_call(tb->layout, "view", e);
}

static void
_changed_dir(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   const char *path = event_info;
   if (!path) return;
   ephoto_directory_set(tb->ephoto, path);
}

static void
_changed_dir_text(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   const char *path = elm_fileselector_entry_path_get(tb->fsel);
   if (ecore_file_is_dir(path))
     ephoto_directory_set(tb->ephoto, path);
}

static void
_zoom_set(Ephoto_Thumb_Browser *tb, int zoom)
{
   if (zoom > ZOOM_MAX) zoom = ZOOM_MAX;
   else if (zoom < ZOOM_MIN) zoom = ZOOM_MIN;

   ephoto_thumb_size_set(tb->ephoto, zoom);
   elm_gengrid_item_size_set(tb->grid, zoom, zoom);
   elm_gengrid_item_size_set(tb->dir_grid, zoom, zoom);   

   elm_toolbar_item_disabled_set(tb->action.zoom_out, zoom == ZOOM_MIN);
   elm_toolbar_item_disabled_set(tb->action.zoom_in, zoom == ZOOM_MAX);
}

static void
_zoom_in(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   elm_toolbar_item_selected_set(tb->action.zoom_in, EINA_FALSE);
   _zoom_set(tb, tb->ephoto->config->thumb_size + ZOOM_STEP);
}

static void
_zoom_out(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   elm_toolbar_item_selected_set(tb->action.zoom_out, EINA_FALSE);
   _zoom_set(tb, tb->ephoto->config->thumb_size - ZOOM_STEP);
}

static void
_view_flow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
   Ephoto_Entry *entry;

   elm_toolbar_item_selected_set(tb->action.view_flow, EINA_FALSE);

   if (it) entry = elm_gengrid_item_data_get(it);
   else entry = _first_file_entry_find(tb);

   if (!entry) return;
   if (entry->is_dir)
     ephoto_directory_set(tb->ephoto, entry->path);
   else
     evas_object_smart_callback_call(tb->layout, "view", entry);
}

static void
_slideshow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
   Ephoto_Entry *entry;

   elm_toolbar_item_selected_set(tb->action.slideshow, EINA_FALSE);

   if (it) entry = elm_gengrid_item_data_get(it);
   else entry = _first_file_entry_find(tb);

   if (!entry) return;
   evas_object_smart_callback_call(tb->layout, "slideshow", entry);
}

static void
_key_down(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   Evas_Event_Key_Down *ev = event_info;
   Eina_Bool alt = evas_key_modifier_is_set(ev->modifiers, "Alt");
   const char *k = ev->keyname;

   if (alt)
     {
        if (!strcmp(k, "Up"))
          {
             if (strcmp(tb->ephoto->config->directory, "/") != 0)
               {
                  char *parent = ecore_file_dir_get
                    (tb->ephoto->config->directory);
                  if (parent)
                    ephoto_directory_set(tb->ephoto, parent);
                  free(parent);
               }
          }

        return;
     }

   if (!strcmp(k, "F5"))
     {
        Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
        Ephoto_Entry *entry;
        if (it) entry = elm_gengrid_item_data_get(it);
        else entry = _first_file_entry_find(tb);

        if (entry)
          evas_object_smart_callback_call(tb->layout, "slideshow", entry);
     }
}


static void
_layout_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Ecore_Event_Handler *handler;

   _todo_items_free(tb);
   _grid_items_free(tb);
   EINA_LIST_FREE(tb->handlers, handler)
      ecore_event_handler_del(handler);

   if (tb->animator.todo_items)
     {
        ecore_animator_del(tb->animator.todo_items);
        tb->animator.todo_items = NULL;
     }
   if (tb->ls)
     {
        tb->layout_deleted = EINA_TRUE;
        eio_file_cancel(tb->ls);
        return;
     }
   free(tb);
}

static Elm_Toolbar_Item *
_toolbar_item_add(Ephoto_Thumb_Browser *tb, const char *icon, const char *label, int priority, Evas_Smart_Cb cb)
{
   Elm_Toolbar_Item *item = elm_toolbar_item_append(tb->toolbar, icon, label,
                                                    cb, tb);
   elm_toolbar_item_priority_set(item, priority);
   return item;
}

static Eina_Bool
_ephoto_thumb_populate_start(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;

   evas_object_smart_callback_call(tb->layout, "changed,directory", NULL);

   _todo_items_free(tb);
   _grid_items_free(tb);
   elm_gengrid_clear(tb->grid);
   elm_gengrid_clear(tb->dir_grid);
   elm_fileselector_entry_path_set(tb->fsel, tb->ephoto->config->directory);

   edje_object_signal_emit(tb->edje, "populate,start", "ephoto");

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_populate_end(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;

   tb->ls = NULL;
   if (tb->layout_deleted)
     {
        free(tb);
        return ECORE_CALLBACK_PASS_ON;
     }

   edje_object_signal_emit(tb->edje, "populate,stop", "ephoto");

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_populate_error(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;

   edje_object_signal_emit(tb->edje, "populate,error", "ephoto");

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_entry_create(void *data, int type __UNUSED__, void *event)
{
   Ephoto_Thumb_Browser *tb = data;
   Ephoto_Event_Entry_Create *ev = event;
   Ephoto_Entry *e;

   e = ev->entry;
   tb->todo_items = eina_list_append(tb->todo_items, e);

   if (!tb->animator.todo_items)
     tb->animator.todo_items = ecore_animator_add(_todo_items_process, tb);

   return ECORE_CALLBACK_PASS_ON;
}

static void
_ephoto_up_clicked(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Ephoto_Thumb_Browser *tb = data;
   char *parent_dir;

   parent_dir = ecore_file_dir_get(tb->ephoto->config->directory);
   if (!parent_dir)
     return;
   ephoto_directory_set(tb->ephoto, parent_dir);
}

static void
_ephoto_dir_clicked(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Ephoto_Thumb_Browser *tb = data;

   evas_object_show(tb->dir_grid);
}

Evas_Object *
ephoto_thumb_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *layout = elm_layout_add(parent);
   Ephoto_Thumb_Browser *tb;

   EINA_SAFETY_ON_NULL_RETURN_VAL(layout, NULL);

   tb = calloc(1, sizeof(Ephoto_Thumb_Browser));
   EINA_SAFETY_ON_NULL_GOTO(tb, error);
   tb->ephoto = ephoto;
   tb->layout = layout;
   tb->edje = elm_layout_edje_get(layout);
   evas_object_event_callback_add(layout, EVAS_CALLBACK_DEL, _layout_del, tb);
   evas_object_event_callback_add
     (layout, EVAS_CALLBACK_KEY_DOWN, _key_down, tb);
   evas_object_data_set(layout, "thumb_browser", tb);

   if (!elm_layout_theme_set
       (layout, "layout", "application", "toolbar-vbox"))
     {
        ERR("could not load style 'toolbar-vbox' from theme");
        goto error;
     }

   tb->toolbar = edje_object_part_external_object_get
     (tb->edje, "elm.external.toolbar");
   if (!tb->toolbar)
     {
        ERR("no toolbar in layout!");
        goto error;
     }
   elm_toolbar_homogenous_set(tb->toolbar, EINA_FALSE);
   elm_toolbar_mode_shrink_set(tb->toolbar, ELM_TOOLBAR_SHRINK_MENU);
   elm_toolbar_menu_parent_set(tb->toolbar, parent);

   tb->action.slideshow = _toolbar_item_add
     (tb, "media-playback-start", "Slideshow", 100, _slideshow);
   tb->action.zoom_in = _toolbar_item_add
     (tb, "zoom-in", "Zoom In", 50, _zoom_in);
   tb->action.zoom_out = _toolbar_item_add
     (tb, "zoom-out", "Zoom Out", 50, _zoom_out);
   tb->action.view_flow = _toolbar_item_add
     (tb, "image", "Larger", 50, _view_flow);

   tb->fsel = elm_fileselector_entry_add(layout);
   EINA_SAFETY_ON_NULL_GOTO(tb->fsel, error);
   evas_object_size_hint_weight_set(tb->fsel, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(tb->fsel, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_fileselector_entry_button_label_set(tb->fsel, "Choose...");
   elm_fileselector_entry_folder_only_set(tb->fsel, EINA_TRUE);
   evas_object_smart_callback_add
     (tb->fsel, "file,chosen", _changed_dir, tb);
   evas_object_smart_callback_add
     (tb->fsel, "activated", _changed_dir_text, tb);

   evas_object_show(tb->fsel);
   elm_layout_box_append(layout, "elm.box.content", tb->fsel);

   tb->overlay = elm_layout_add(layout);
   elm_layout_file_set
      (tb->overlay, PACKAGE_DATA_DIR "/themes/default/ephoto.edj", 
                                            "ephoto,thumb,grid");
   evas_object_size_hint_weight_set
     (tb->overlay, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set
     (tb->overlay, EVAS_HINT_FILL, EVAS_HINT_FILL);
   edje_object_signal_callback_add
     (elm_layout_edje_get(tb->overlay), "ephoto.signal.up", "ephoto", 
                      _ephoto_up_clicked, tb);
   edje_object_signal_callback_add
     (elm_layout_edje_get(tb->overlay), "ephoto.signal.dir", "ephoto", 
                      _ephoto_dir_clicked, tb);
   evas_object_show(tb->overlay);
   elm_layout_box_append(tb->layout, "elm.box.content", tb->overlay);

   tb->grid = elm_gengrid_add(tb->overlay);
   EINA_SAFETY_ON_NULL_GOTO(tb->grid, error);
   elm_gengrid_align_set(tb->grid, 0.5, 0.5);
   elm_gengrid_bounce_set(tb->grid, EINA_FALSE, EINA_TRUE);
   evas_object_size_hint_align_set
     (tb->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set
     (tb->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_smart_callback_add
     (tb->grid, "selected", _ephoto_thumb_selected, tb);
   _zoom_set(tb, tb->ephoto->config->thumb_size);
   evas_object_show(tb->grid);
   elm_layout_content_set
     (tb->overlay, "ephoto.swallow.content.thumb", tb->grid);

   tb->dir_grid = elm_gengrid_add(tb->overlay);
   EINA_SAFETY_ON_NULL_GOTO(tb->dir_grid, error);
   elm_gengrid_align_set(tb->dir_grid, 0.5, 0.5);
   elm_gengrid_bounce_set(tb->dir_grid, EINA_FALSE, EINA_TRUE);
   evas_object_size_hint_align_set
     (tb->dir_grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set
     (tb->dir_grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_smart_callback_add
     (tb->dir_grid, "selected", _ephoto_thumb_selected, tb);
   _zoom_set(tb, tb->ephoto->config->thumb_size);
   evas_object_show(tb->dir_grid);
   elm_layout_content_set
     (tb->overlay, "ephoto.swallow.content.dir", tb->dir_grid);

   tb->handlers = eina_list_append
      (tb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_START, _ephoto_thumb_populate_start, tb));

   tb->handlers = eina_list_append
      (tb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_END, _ephoto_thumb_populate_end, tb));

   tb->handlers = eina_list_append
      (tb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_POPULATE_ERROR, _ephoto_thumb_populate_error, tb));

   tb->handlers = eina_list_append
      (tb->handlers, ecore_event_handler_add
       (EPHOTO_EVENT_ENTRY_CREATE, _ephoto_thumb_entry_create, tb));

   return layout;

 error:
   evas_object_del(layout);
   return NULL;
}
