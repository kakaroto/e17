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
   Evas_Object *main;
   Evas_Object *bar;
   Evas_Object *fsel;
   Evas_Object *grid;
   Eio_File *ls;
   Eina_List *todo_items;
   Eina_List *grid_items;
   Eina_List *handlers;
   struct {
      Elm_Toolbar_Item *zoom_in;
      Elm_Toolbar_Item *zoom_out;
      Elm_Toolbar_Item *view_single;
      Elm_Toolbar_Item *slideshow;
   } action;
   struct {
      Ecore_Animator *todo_items;
   } animator;
   Eina_Bool main_deleted : 1;
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
     {
        Evas_Object *o;
        o = ephoto_thumb_add(e->ephoto, obj, f);
        //elm_object_style_set(o, "default");
        return o;
     }

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

static const Elm_Gengrid_Item_Class _ephoto_thumb_up_class = {
  "ephoto-up",
  {
    _ephoto_thumb_item_label_get,
    NULL,
    NULL,
    _ephoto_thumb_item_del
  }
};

static const Elm_Gengrid_Item_Class _ephoto_thumb_dir_class = {
  "ephoto-album-preview",
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
   Elm_Gengrid_Item *near_item = NULL;
   Eina_List *near_node = NULL;
   
   near_node = eina_list_search_sorted_near_list
     (tb->grid_items, _entry_cmp, e, &near_cmp);

   if (near_node)
     near_item = near_node->data;

   if (e->is_dir) ic = &_ephoto_thumb_dir_class;
   else           ic = &_ephoto_thumb_file_class;

   if (!near_item)
     {
        e->item = elm_gengrid_item_append(tb->grid, ic, e, NULL, NULL);
        tb->grid_items = eina_list_append(tb->grid_items, e->item);
     }
   else
     {
        if (near_cmp < 0)
          {
             e->item = elm_gengrid_item_insert_after
                (tb->grid, ic, e, near_item, NULL, NULL);
             tb->grid_items = eina_list_append_relative
                (tb->grid_items, e->item, near_item);
          }
        else
          {
             e->item = elm_gengrid_item_insert_before
                (tb->grid, ic, e, near_item, NULL, NULL);
             tb->grid_items = eina_list_prepend_relative
                (tb->grid_items, e->item, near_item);
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

static void
_up_item_add_if_required(Ephoto_Thumb_Browser *tb)
{
   Ephoto_Entry *entry;
   char *parent_dir;

   if ((strcmp(tb->ephoto->config->directory, "/") == 0))
     return;

   parent_dir = ecore_file_dir_get(tb->ephoto->config->directory);
   if (!parent_dir) return;

   entry = ephoto_entry_new(tb->ephoto, parent_dir, PARENT_DIR);
   free(parent_dir);
   EINA_SAFETY_ON_NULL_RETURN(entry);
   entry->is_up = EINA_TRUE;
   entry->is_dir = EINA_TRUE;
   entry->item = elm_gengrid_item_append
     (tb->grid, &_ephoto_thumb_up_class, entry, NULL, NULL);
}

static Eina_Bool
_todo_items_process(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   Ephoto_Entry *entry;

   if ((tb->ls) && (eina_list_count(tb->todo_items) < TODO_ITEM_MIN_BATCH))
     return EINA_TRUE;

   _up_item_add_if_required(tb);

   tb->animator.todo_items = NULL;

   EINA_LIST_FREE(tb->todo_items, entry)
     _entry_item_add(tb, entry);

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
     evas_object_smart_callback_call(tb->main, "view", e);
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
}

static void
_zoom_in(void *data, Evas_Object *o, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Evas_Object *min = evas_object_data_get(o, "min");
   int zoom = tb->ephoto->config->thumb_size + ZOOM_STEP;
   _zoom_set(tb, zoom);
   if (zoom >= ZOOM_MAX) elm_object_disabled_set(o, EINA_TRUE);
   if (zoom > ZOOM_MIN) elm_object_disabled_set(min, EINA_FALSE);
}

static void
_zoom_out(void *data, Evas_Object *o, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Evas_Object *max = evas_object_data_get(o, "max");
   int zoom = tb->ephoto->config->thumb_size - ZOOM_STEP;
   _zoom_set(tb, zoom);
   if (zoom <= ZOOM_MIN) elm_object_disabled_set(o, EINA_TRUE);
   if (zoom < ZOOM_MAX) elm_object_disabled_set(max, EINA_FALSE);
}

static void
_view_single(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
   Ephoto_Entry *entry;

   if (it) entry = elm_gengrid_item_data_get(it);
   else entry = _first_file_entry_find(tb);

   if (!entry) return;
   if (entry->is_dir)
     ephoto_directory_set(tb->ephoto, entry->path);
   else
     evas_object_smart_callback_call(tb->main, "view", entry);
}

static void
_slideshow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
   Ephoto_Entry *entry;

   if (it) entry = elm_gengrid_item_data_get(it);
   else entry = _first_file_entry_find(tb);

   if (!entry) return;
   evas_object_smart_callback_call(tb->main, "slideshow", entry);
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
          evas_object_smart_callback_call(tb->main, "slideshow", entry);
     }
}


static void
_main_del(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
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
        tb->main_deleted = EINA_TRUE;
        eio_file_cancel(tb->ls);
        return;
     }
   free(tb);
}

static Eina_Bool
_ephoto_thumb_populate_start(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;

   evas_object_smart_callback_call(tb->main, "changed,directory", NULL);

   _todo_items_free(tb);
   _grid_items_free(tb);
   elm_gengrid_clear(tb->grid);
   elm_fileselector_entry_path_set(tb->fsel, tb->ephoto->config->directory);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_populate_end(void *data, int type __UNUSED__, void *event __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;

   tb->ls = NULL;
   if (tb->main_deleted)
     {
        free(tb);
        return ECORE_CALLBACK_PASS_ON;
     }

   if (!tb->animator.todo_items) _up_item_add_if_required(tb);

   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool
_ephoto_thumb_populate_error(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
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

Evas_Object *
ephoto_thumb_browser_add(Ephoto *ephoto, Evas_Object *parent)
{
   Evas_Object *box = elm_box_add(parent);
   Evas_Object *ic, *but, *min, *max;
   Ephoto_Thumb_Browser *tb;

   EINA_SAFETY_ON_NULL_RETURN_VAL(box, NULL);

   tb = calloc(1, sizeof(Ephoto_Thumb_Browser));
   EINA_SAFETY_ON_NULL_GOTO(tb, error);

   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/themes/default/ephoto.edj");

   tb->ephoto = ephoto;
   tb->main = box;
   elm_box_horizontal_set(tb->main, EINA_FALSE);
   elm_box_homogeneous_set(tb->main, EINA_FALSE);
   evas_object_event_callback_add(tb->main, EVAS_CALLBACK_DEL, _main_del, tb);
   evas_object_event_callback_add
     (tb->main, EVAS_CALLBACK_KEY_DOWN, _key_down, tb);
   evas_object_data_set(tb->main, "thumb_browser", tb);

   tb->bar = elm_box_add(tb->main);
   elm_box_horizontal_set(tb->bar, EINA_TRUE);
   elm_box_homogeneous_set(tb->bar, EINA_FALSE);
   evas_object_size_hint_weight_set(tb->bar, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(tb->bar, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(tb->bar);
   elm_box_pack_end(tb->main, tb->bar);

   but = elm_button_add(tb->bar);

   ic = elm_icon_add(but);
   elm_icon_file_set(ic, PACKAGE_DATA_DIR "/images/single.png", NULL);

   elm_button_icon_set(but, ic);
   evas_object_size_hint_align_set(but, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(but, "clicked", _view_single, tb);
   elm_box_pack_end(tb->bar, but);
   evas_object_show(but);

   but = elm_button_add(tb->bar);

   ic = elm_icon_add(but);
   elm_icon_file_set(ic, PACKAGE_DATA_DIR "/images/slideshow.png", NULL);

   elm_button_icon_set(but, ic);
   evas_object_size_hint_align_set(but, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(but, "clicked", _slideshow, tb);
   elm_box_pack_end(tb->bar, but);
   evas_object_show(but);

   tb->fsel = elm_fileselector_entry_add(tb->bar);
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
   elm_box_pack_end(tb->bar, tb->fsel);

   but = elm_button_add(tb->bar);

   ic = elm_icon_add(but);
   elm_icon_file_set(ic, PACKAGE_DATA_DIR "/images/zoom-in.png", NULL);

   elm_button_icon_set(but, ic);
   evas_object_size_hint_align_set(but, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(but, "clicked", _zoom_in, tb);
   elm_box_pack_end(tb->bar, but);
   evas_object_show(but);
   max = but;

   but = elm_button_add(tb->bar);

   ic = elm_icon_add(but);
   elm_icon_file_set(ic, PACKAGE_DATA_DIR "/images/zoom-out.png", NULL);

   elm_button_icon_set(but, ic);
   evas_object_size_hint_align_set(but, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_smart_callback_add(but, "clicked", _zoom_out, tb);
   elm_box_pack_end(tb->bar, but);
   evas_object_show(but);
   min = but;

   evas_object_data_set(max, "min", min);
   evas_object_data_set(min, "max", max);

   tb->grid = elm_gengrid_add(tb->main);
   EINA_SAFETY_ON_NULL_GOTO(tb->grid, error);
   evas_object_size_hint_weight_set
     (tb->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);

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
   elm_box_pack_end(tb->main, tb->grid);

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

   return tb->main;

 error:
   evas_object_del(tb->main);
   return NULL;
}
