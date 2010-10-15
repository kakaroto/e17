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
   Evas_Object *toolbar;
   Eio_File *ls;
   Eina_List *todo_items;
   struct {
      Elm_Toolbar_Item *zoom_in;
      Elm_Toolbar_Item *zoom_out;
      Elm_Toolbar_Item *view_flow;
      Elm_Toolbar_Item *slideshow;
   } action;
   struct {
      const char *path;
      void (*cb)(void *data, Ephoto_Entry *entry);
      const void *data;
   } pending;
   struct {
      Ecore_Job *change_dir;
   } job;
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
_ephoto_thumb_item_del(void *data, Evas_Object *obj __UNUSED__)
{
   Ephoto_Entry *e = data;
   e->item = NULL;
}

static const Elm_Gengrid_Item_Class _ephoto_thumb_dir_class = {
  "dir",
  {
    _ephoto_thumb_item_label_get,
    _ephoto_thumb_dir_icon_get,
    _ephoto_thumb_dir_state_get,
    _ephoto_thumb_item_del
  }
};

static const Elm_Gengrid_Item_Class _ephoto_thumb_up_class = {
  "up",
  {
    _ephoto_thumb_item_label_get,
    NULL,
    NULL,
    _ephoto_thumb_item_del
  }
};

static const Elm_Gengrid_Item_Class _ephoto_thumb_file_class = {
  "file",
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
   const Ephoto_Entry *a = pa, *b = pb;
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

   DBG("populate add '%s'", e->path);

   if (e->is_dir) ic = &_ephoto_thumb_dir_class;
   else           ic = &_ephoto_thumb_file_class;

   if (!tb->ephoto->entries)
     {
        e->item = elm_gengrid_item_append(tb->grid, ic, e, NULL, NULL);
        tb->ephoto->entries = eina_list_append(tb->ephoto->entries, e);
     }
   else
     {
        int near_cmp;
        Ephoto_Entry *near_entry;
        Elm_Gengrid_Item *near_item;
        Eina_List *near_node = eina_list_search_sorted_near_list
          (tb->ephoto->entries, _entry_cmp, e, &near_cmp);

        near_entry = near_node->data;
        near_item = near_entry->item;
        if (near_cmp < 0)
          {
             e->item = elm_gengrid_item_insert_after
               (tb->grid, ic, e, near_item, NULL, NULL);
             tb->ephoto->entries =  eina_list_append_relative_list
               (tb->ephoto->entries, e, near_node);
          }
        else
          {
             e->item = elm_gengrid_item_insert_before
               (tb->grid, ic, e, near_item, NULL, NULL);
             tb->ephoto->entries =  eina_list_prepend_relative_list
               (tb->ephoto->entries, e, near_node);
          }
     }

   if (!e->item)
     {
        ERR("could not add item to grid: path '%s'", e->path);
        ephoto_entry_free(e);
        return;
     }

   if (tb->pending.path == e->path)
     {
        tb->pending.cb((void*)tb->pending.data, e);
        tb->pending.cb = NULL;
        tb->pending.data = NULL;
        eina_stringshare_replace(&tb->pending.path, NULL);
     }
}

static void
_up_item_add_if_required(Ephoto_Thumb_Browser *tb)
{
   Ephoto_Entry *entry;
   char *parent_dir;

   if ((elm_gengrid_first_item_get(tb->grid)) ||
       (strcmp(tb->ephoto->config->directory, "/") == 0))
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
   /* does not go into entries as it is always the first - no sort! */
}

static Eina_Bool
_todo_items_process(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   Edje_Message_Int msg;
   Ephoto_Entry *entry;

   if ((tb->ls) && (eina_list_count(tb->todo_items) < TODO_ITEM_MIN_BATCH))
     return EINA_TRUE;

   _up_item_add_if_required(tb);

   tb->animator.todo_items = NULL;

   EINA_LIST_FREE(tb->todo_items, entry)
     _entry_item_add(tb, entry);

   msg.val = eina_list_count(tb->ephoto->entries);
   edje_object_message_send(tb->edje, EDJE_MESSAGE_INT, 1, &msg);

   return EINA_FALSE;
}

static void
_ephoto_populate_main(void *data, const Eina_File_Direct_Info *info)
{
   Ephoto_Thumb_Browser *tb = data;
   Ephoto_Entry *e;

   e = ephoto_entry_new(tb->ephoto, info->path, info->path + info->name_start);
   if (info->dirent->d_type == DT_DIR) e->is_dir = EINA_TRUE;
   else if (info->dirent->d_type == DT_REG) e->is_dir = EINA_FALSE;
   else e->is_dir = !_ephoto_eina_file_direct_info_image_useful(info);

   tb->todo_items = eina_list_append(tb->todo_items, e);

   if (!tb->animator.todo_items)
     tb->animator.todo_items = ecore_animator_add(_todo_items_process, tb);
}

static Eina_Bool
_ephoto_populate_filter(void *data __UNUSED__, const Eina_File_Direct_Info *info)
{
   const char *bname = info->path + info->name_start;

   if (bname[0] == '.') return EINA_FALSE;
   if (info->dirent->d_type == DT_DIR) return EINA_TRUE;
   if (info->dirent->d_type == DT_UNKNOWN)
     {
        struct stat st;
        if ((stat(info->path, &st) == 0) && (S_ISDIR(st.st_mode)))
          return EINA_TRUE;
     }

   return _ephoto_eina_file_direct_info_image_useful(info);
}

static void
_ephoto_populate_end(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   tb->ls = NULL;

   if (tb->layout_deleted)
     {
        free(tb);
        return;
     }

   if (!tb->animator.todo_items) _up_item_add_if_required(tb);

   if (tb->pending.cb)
     {
        tb->pending.cb((void*)tb->pending.data, NULL);
        tb->pending.cb = NULL;
     }
   tb->pending.data = NULL;
   eina_stringshare_replace(&tb->pending.path, NULL);
   edje_object_signal_emit(tb->edje, "populate,stop", "ephoto");
}

static void
_ephoto_populate_error(int error, void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   if (error) ERR("could not populate: %s", strerror(error));
   edje_object_signal_emit(tb->edje, "populate,error", "ephoto");
   _ephoto_populate_end(tb);
}

static void
_ephoto_populate_entries(Ephoto_Thumb_Browser *tb)
{
   /* Edje_External_Param param; */
   DBG("populate from '%s'", tb->ephoto->config->directory);

   evas_object_smart_callback_call(tb->layout, "changed,directory", NULL);

   _todo_items_free(tb);
   elm_gengrid_clear(tb->grid);
   ephoto_entries_free(tb->ephoto);

   elm_fileselector_entry_path_set(tb->fsel, tb->ephoto->config->directory);

   edje_object_signal_emit(tb->edje, "populate,start", "ephoto");
   tb->ls = eio_file_direct_ls(tb->ephoto->config->directory,
                               _ephoto_populate_filter,
                               _ephoto_populate_main,
                               _ephoto_populate_end,
                               _ephoto_populate_error,
                               tb);
}

static void
_ephoto_thumb_change_dir(void *data)
{
   Ephoto_Thumb_Browser *tb = data;
   tb->job.change_dir = NULL;
   _ephoto_populate_entries(tb);
}

static void
_ephoto_thumb_selected(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = event_info;
   Ephoto_Entry *e = elm_gengrid_item_data_get(it);

   elm_gengrid_item_selected_set(it, EINA_FALSE);

   if (e->is_dir)
     ephoto_thumb_browser_directory_set(tb->layout, e->path);
   else
     evas_object_smart_callback_call(tb->layout, "view", e);
}

static void
_changed_dir(void *data, Evas_Object *o __UNUSED__, void *event_info)
{
   Ephoto_Thumb_Browser *tb = data;
   const char *path = event_info;
   ephoto_thumb_browser_directory_set(tb->layout, path);
}

static void
_changed_dir_text(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   const char *path = elm_fileselector_entry_path_get(tb->fsel);
   if (ecore_file_is_dir(path))
     ephoto_thumb_browser_directory_set(tb->layout, path);
}

static void
_zoom_set(Ephoto_Thumb_Browser *tb, int zoom)
{
   if (zoom > ZOOM_MAX) zoom = ZOOM_MAX;
   else if (zoom < ZOOM_MIN) zoom = ZOOM_MIN;

   ephoto_thumb_size_set(tb->ephoto, zoom);
   elm_gengrid_item_size_set(tb->grid, zoom, zoom);

   elm_toolbar_item_disabled_set(tb->action.zoom_out, zoom == ZOOM_MIN);
   elm_toolbar_item_disabled_set(tb->action.zoom_in, zoom == ZOOM_MAX);
}

static void
_zoom_in(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   elm_toolbar_item_unselect(tb->action.zoom_in);
   _zoom_set(tb, tb->ephoto->config->thumb_size + ZOOM_STEP);
}

static void
_zoom_out(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   elm_toolbar_item_unselect(tb->action.zoom_out);
   _zoom_set(tb, tb->ephoto->config->thumb_size - ZOOM_STEP);
}

static void
_view_flow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
   Ephoto_Entry *entry;

   elm_toolbar_item_unselect(tb->action.view_flow);

   if (it) entry = elm_gengrid_item_data_get(it);
   else entry = _first_file_entry_find(tb);

   if (!entry) return;
   if (entry->is_dir)
     ephoto_thumb_browser_directory_set(tb->layout, entry->path);
   else
     evas_object_smart_callback_call(tb->layout, "view", entry);
}

static void
_slideshow(void *data, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto_Thumb_Browser *tb = data;
   Elm_Gengrid_Item *it = elm_gengrid_selected_item_get(tb->grid);
   Ephoto_Entry *entry;

   elm_toolbar_item_unselect(tb->action.slideshow);

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
                    ephoto_thumb_browser_directory_set(tb->layout, parent);
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

   _todo_items_free(tb);

   if (tb->pending.cb)
     {
        tb->pending.cb((void*)tb->pending.data, NULL);
        tb->pending.cb = NULL;
        tb->pending.data = NULL;
     }
   eina_stringshare_replace(&tb->pending.path, NULL);
   if (tb->job.change_dir)
     {
        ecore_job_del(tb->job.change_dir);
        tb->job.change_dir = NULL;
     }
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
   Elm_Toolbar_Item *item = elm_toolbar_item_add(tb->toolbar, icon, label, cb, tb);
   elm_toolbar_item_priority_set(item, priority);
   return item;
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

   tb->grid = elm_gengrid_add(layout);
   EINA_SAFETY_ON_NULL_GOTO(tb->grid, error);
   evas_object_size_hint_weight_set
     (tb->grid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(tb->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_style_set(tb->grid, "ephoto");

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
   elm_layout_box_append(tb->layout, "elm.box.content", tb->grid);

   return layout;

 error:
   evas_object_del(layout);
   return NULL;
}

void
ephoto_thumb_browser_directory_set(Evas_Object *obj, const char *path)
{
   Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser");
   EINA_SAFETY_ON_NULL_RETURN(tb);

   eina_stringshare_replace(&tb->pending.path, NULL);
   tb->pending.cb = NULL;
   tb->pending.data = NULL;

   ephoto_title_set(tb->ephoto, path);

   eina_stringshare_replace(&tb->ephoto->config->directory, path);
   if (tb->job.change_dir) ecore_job_del(tb->job.change_dir);
   tb->job.change_dir = ecore_job_add(_ephoto_thumb_change_dir, tb);
}

void
ephoto_thumb_browser_path_pending_set(Evas_Object *obj, const char *path, void (*cb)(void *data, Ephoto_Entry *entry), const void *data)
{
   Ephoto_Thumb_Browser *tb = evas_object_data_get(obj, "thumb_browser");
   EINA_SAFETY_ON_NULL_RETURN(tb);
   eina_stringshare_replace(&tb->pending.path, path);
   tb->pending.cb = cb;
   tb->pending.data = data;
}
