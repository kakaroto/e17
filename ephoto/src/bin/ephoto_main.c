#include "ephoto.h"

int EPHOTO_EVENT_ENTRY_CREATE = 0;
int EPHOTO_EVENT_POPULATE_START = 0;
int EPHOTO_EVENT_POPULATE_END = 0;
int EPHOTO_EVENT_POPULATE_ERROR = 0;

typedef struct _Ephoto_Entry_Free_Listener Ephoto_Entry_Free_Listener;
struct _Ephoto_Entry_Free_Listener
{
   void (*cb)(void *data, const Ephoto_Entry *dead);
   const void *data;
};

static void
_ephoto_state_set(Ephoto *ephoto, Ephoto_State state)
{
   ephoto->prev_state = ephoto->state;
   ephoto->state = state;
}

static void
_ephoto_thumb_browser_show(Ephoto *ephoto, Ephoto_Entry *entry)
{
   DBG("entry '%s'", entry ? entry->path : "");

   ephoto_single_browser_entry_set(ephoto->single_browser, NULL);
   ephoto_slideshow_entry_set(ephoto->slideshow, NULL);
   elm_pager_content_promote(ephoto->pager, ephoto->thumb_browser);
   _ephoto_state_set(ephoto, EPHOTO_STATE_THUMB);

   if ((entry) && (entry->item)) elm_gengrid_item_bring_in(entry->item);
}

static void
_ephoto_single_browser_show(Ephoto *ephoto, Ephoto_Entry *entry)
{
   DBG("entry '%s'", entry->path);
   ephoto_single_browser_entry_set(ephoto->single_browser, entry);
   elm_pager_content_promote(ephoto->pager, ephoto->single_browser);
   _ephoto_state_set(ephoto, EPHOTO_STATE_SINGLE);
}

static void
_ephoto_slideshow_show(Ephoto *ephoto, Ephoto_Entry *entry)
{
   DBG("entry '%s'", entry->path);
   ephoto_slideshow_entry_set(ephoto->slideshow, entry);
   elm_pager_content_promote(ephoto->pager, ephoto->slideshow);
   _ephoto_state_set(ephoto, EPHOTO_STATE_SLIDESHOW);
}

static void
_ephoto_single_browser_back(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *entry = event_info;
   _ephoto_thumb_browser_show(ephoto, entry);
}

static void
_ephoto_slideshow_back(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *entry = event_info;
   switch (ephoto->prev_state)
     {
      case EPHOTO_STATE_SINGLE:
         _ephoto_single_browser_show(ephoto, entry);
         break;
      case EPHOTO_STATE_THUMB:
         _ephoto_thumb_browser_show(ephoto, entry);
         break;
      default:
         ERR("unhandled previous state %d", ephoto->prev_state);
     }
   elm_win_fullscreen_set(ephoto->win, EINA_FALSE);
}

static void
_ephoto_thumb_browser_view(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *entry = event_info;
   _ephoto_single_browser_show(ephoto, entry);
}

static void
_ephoto_thumb_browser_changed_directory(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto *ephoto = data;
   ephoto_single_browser_entry_set(ephoto->single_browser, NULL);
   ephoto_slideshow_entry_set(ephoto->slideshow, NULL);
}

static void
_ephoto_thumb_browser_slideshow(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *entry = event_info;
   _ephoto_slideshow_show(ephoto, entry);
}

static void
_ephoto_single_browser_slideshow(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *entry = event_info;
   _ephoto_slideshow_show(ephoto, entry);
}

static void
_win_free(void *data, Evas *e __UNUSED__, Evas_Object *o __UNUSED__, void *event_info __UNUSED__)
{
   Ephoto *ephoto = data;
   if (ephoto->timer.thumb_regen) ecore_timer_del(ephoto->timer.thumb_regen);
   free(ephoto);
}

Evas_Object *
ephoto_window_add(const char *path)
{
   Ephoto *ephoto = calloc(1, sizeof(Ephoto));
   Ethumb_Client *client = elm_thumb_ethumb_client_get();
   char buf[PATH_MAX];
   EINA_SAFETY_ON_NULL_RETURN_VAL(ephoto, NULL);

   EPHOTO_EVENT_ENTRY_CREATE = ecore_event_type_new();
   EPHOTO_EVENT_POPULATE_START = ecore_event_type_new();
   EPHOTO_EVENT_POPULATE_END = ecore_event_type_new();
   EPHOTO_EVENT_POPULATE_ERROR = ecore_event_type_new();

   ephoto->win = elm_win_add(NULL, "ephoto", ELM_WIN_BASIC);
   if (!ephoto->win)
     {
        free(ephoto);
        return NULL;
     }

   evas_object_event_callback_add
     (ephoto->win, EVAS_CALLBACK_FREE, _win_free, ephoto);

   elm_win_autodel_set(ephoto->win, EINA_TRUE);

   if (!ephoto_config_init(ephoto))
     {
        evas_object_del(ephoto->win);
        return NULL;
     }

   if ((ephoto->config->thumb_gen_size != 128) &&
       (ephoto->config->thumb_gen_size != 256) &&
       (ephoto->config->thumb_gen_size != 512))
     ephoto_thumb_size_set(ephoto, ephoto->config->thumb_size);
   else if (client)
     ethumb_client_size_set
       (client, ephoto->config->thumb_gen_size, ephoto->config->thumb_gen_size);

   ephoto->bg = elm_bg_add(ephoto->win);
   evas_object_size_hint_weight_set
     (ephoto->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ephoto->bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->bg);
   evas_object_show(ephoto->bg);

   ephoto->pager = elm_pager_add(ephoto->win);
   elm_object_style_set(ephoto->pager, "fade_invisible");
   evas_object_size_hint_weight_set
     (ephoto->pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (ephoto->pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->pager);
   evas_object_show(ephoto->pager);

   ephoto->thumb_browser = ephoto_thumb_browser_add(ephoto, ephoto->pager);
   if (!ephoto->thumb_browser)
     {
        ERR("could not add thumb browser");
        evas_object_del(ephoto->win);
        return NULL;
     }
   elm_pager_content_push(ephoto->pager, ephoto->thumb_browser);
   evas_object_smart_callback_add
     (ephoto->thumb_browser, "view", _ephoto_thumb_browser_view, ephoto);
   evas_object_smart_callback_add
     (ephoto->thumb_browser, "changed,directory",
      _ephoto_thumb_browser_changed_directory, ephoto);
   evas_object_smart_callback_add
     (ephoto->thumb_browser, "slideshow",
      _ephoto_thumb_browser_slideshow, ephoto);

   ephoto->single_browser = ephoto_single_browser_add(ephoto, ephoto->pager);
   if (!ephoto->single_browser)
     {
        ERR("could not add single browser");
        evas_object_del(ephoto->win);
        return NULL;
     }
   elm_pager_content_push(ephoto->pager, ephoto->single_browser);
   evas_object_smart_callback_add
     (ephoto->single_browser, "back", _ephoto_single_browser_back, ephoto);
   evas_object_smart_callback_add
     (ephoto->single_browser, "slideshow",
      _ephoto_single_browser_slideshow, ephoto);

   ephoto->slideshow = ephoto_slideshow_add(ephoto, ephoto->pager);
   if (!ephoto->slideshow)
     {
        ERR("could not add single browser");
        evas_object_del(ephoto->win);
        return NULL;
     }
   elm_pager_content_push(ephoto->pager, ephoto->slideshow);
   evas_object_smart_callback_add
     (ephoto->slideshow, "back", _ephoto_slideshow_back, ephoto);

   if ((!path) || (!ecore_file_exists(path)))
     {
        path = ephoto->config->directory;
        if ((path) && (!ecore_file_exists(path))) path = NULL;
        if (!path)
          {
             if (getcwd(buf, sizeof(buf)))
               path = buf;
             else
               path = getenv("HOME");
          }
     }

   if (ecore_file_is_dir(path))
     {
        ephoto_directory_set(ephoto, path);
        _ephoto_thumb_browser_show(ephoto, NULL);
     }
   else
     {
        char *dir = ecore_file_dir_get(path);
        ephoto_directory_set(ephoto, dir);
        free(dir);
        ephoto_single_browser_path_pending_set(ephoto->single_browser, path);

        elm_pager_content_promote(ephoto->pager, ephoto->single_browser);
        ephoto->state = EPHOTO_STATE_SINGLE;
     }

   /* TODO restore size from last run as well? */
   evas_object_resize(ephoto->win, 900, 600);
   evas_object_show(ephoto->win);

   return ephoto->win;
}

void
ephoto_title_set(Ephoto *ephoto, const char *title)
{
   char buf[1024] = "Ephoto";

   if (title) snprintf(buf, sizeof(buf), "%s - Ephoto", title);
   elm_win_title_set(ephoto->win, buf);
}

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
_ephoto_populate_main(void *data, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *e;
   Ephoto_Event_Entry_Create *ev;

   e = ephoto_entry_new(ephoto, info->path, info->path + info->name_start);
   if (info->type == EINA_FILE_DIR) e->is_dir = EINA_TRUE;
   else if (info->type == EINA_FILE_REG) e->is_dir = EINA_FALSE;
   else e->is_dir = !_ephoto_eina_file_direct_info_image_useful(info);

   if (!ephoto->entries)
     ephoto->entries = eina_list_append(ephoto->entries, e);
   else
     {
        int near_cmp;
        Eina_List *near_node = eina_list_search_sorted_near_list
          (ephoto->entries, _entry_cmp, e, &near_cmp);

        if (near_cmp < 0)
          ephoto->entries =  eina_list_append_relative_list
             (ephoto->entries, e, near_node);
        else
          ephoto->entries =  eina_list_prepend_relative_list
             (ephoto->entries, e, near_node);
     }

   ev = calloc(1, sizeof(Ephoto_Event_Entry_Create));
   ev->entry = e;

   ecore_event_add(EPHOTO_EVENT_ENTRY_CREATE, ev, NULL, NULL);
}

static Eina_Bool
_ephoto_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   const char *bname = info->path + info->name_start;

   if (bname[0] == '.') return EINA_FALSE;
   if (info->type == EINA_FILE_DIR) return EINA_TRUE;

   return _ephoto_eina_file_direct_info_image_useful(info);
}

static void
_ephoto_populate_end(void *data, Eio_File *handler __UNUSED__)
{
   Ephoto *ephoto = data;
   ephoto->ls = NULL;

   ecore_event_add(EPHOTO_EVENT_POPULATE_END, NULL, NULL, NULL);
}

static void
_ephoto_populate_error(void *data, Eio_File *handler, int error)
{
   Ephoto *ephoto = data;
   if (error) ERR("could not populate: %s", strerror(error));

   /* XXX: Perhaps it would be better to _not_ emit POPULATE_END here */
   ecore_event_add(EPHOTO_EVENT_POPULATE_ERROR, NULL, NULL, NULL);
   _ephoto_populate_end(ephoto, handler);
}

static void
_ephoto_populate_entries(Ephoto *ephoto)
{
   /* Edje_External_Param param; */
   DBG("populate from '%s'", ephoto->config->directory);

   ephoto_entries_free(ephoto);

   ephoto->ls = eio_file_stat_ls(ephoto->config->directory,
                                 _ephoto_populate_filter,
                                 _ephoto_populate_main,
                                 _ephoto_populate_end,
                                 _ephoto_populate_error,
                                 ephoto);

   ecore_event_add(EPHOTO_EVENT_POPULATE_START, NULL, NULL, NULL);
}

static void
_ephoto_change_dir(void *data)
{
   Ephoto *ephoto = data;
   ephoto->job.change_dir = NULL;
   _ephoto_populate_entries(ephoto);
}

void
ephoto_directory_set(Ephoto *ephoto, const char *path)
{
   EINA_SAFETY_ON_NULL_RETURN(ephoto);

   ephoto_title_set(ephoto, path);
   eina_stringshare_replace(&ephoto->config->directory, path);
   if (ephoto->job.change_dir) ecore_job_del(ephoto->job.change_dir);
   ephoto->job.change_dir = ecore_job_add(_ephoto_change_dir, ephoto);
}

static Eina_Bool
_thumb_gen_size_changed_timer_cb(void *data)
{
   Ephoto *ephoto = data;
   Ethumb_Client *client;
   const Eina_List *l;
   Evas_Object *o;

   if (ephoto->config->thumb_gen_size == ephoto->thumb_gen_size) goto end;

   INF("thumbnail generation size changed from %d to %d",
       ephoto->config->thumb_gen_size, ephoto->thumb_gen_size);

   client = elm_thumb_ethumb_client_get();
   if (!client)
     {
        DBG("no client yet, try again later");
        return EINA_TRUE;
     }

   ephoto->config->thumb_gen_size = ephoto->thumb_gen_size;
   ethumb_client_size_set
     (client, ephoto->thumb_gen_size, ephoto->thumb_gen_size);

   EINA_LIST_FOREACH(ephoto->thumbs, l, o)
     {
        Ethumb_Thumb_Format format;
        format = (long)evas_object_data_get(o, "ephoto_format");
        if (format)
          {
             ethumb_client_format_set(client, format);
             elm_thumb_reload(o);
          }
     }

 end:
   ephoto->timer.thumb_regen = NULL;
   return EINA_FALSE;
}

void
ephoto_thumb_size_set(Ephoto *ephoto, int size)
{
   if (ephoto->config->thumb_size != size)
     {
        INF("thumbnail display size changed from %d to %d",
            ephoto->config->thumb_size, size);
        ephoto->config->thumb_size = size;
        ephoto_config_save(ephoto, EINA_FALSE);
     }

   if (size <= 128)      ephoto->thumb_gen_size = 128;
   else if (size <= 256) ephoto->thumb_gen_size = 256;
   else                  ephoto->thumb_gen_size = 512;

   if (ephoto->timer.thumb_regen) ecore_timer_del(ephoto->timer.thumb_regen);
   ephoto->timer.thumb_regen = ecore_timer_add
     (0.1, _thumb_gen_size_changed_timer_cb, ephoto);
}

static void
_thumb_del(void *data, Evas *e __UNUSED__, Evas_Object *o, void *event_info __UNUSED__)
{
   Ephoto *ephoto = data;
   ephoto->thumbs = eina_list_remove(ephoto->thumbs, o);
}

Evas_Object *
ephoto_thumb_add(Ephoto *ephoto, Evas_Object *parent, const char *path)
{
   Evas_Object *o;
   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);

   if (path)
     {
        const char *ext = strrchr(path, '.');
        if (ext)
          {
             ext++;
             if ((strcasecmp(ext, "edj") == 0))
               o = elm_icon_add(parent);
             else
               o = elm_thumb_add(parent);
          }
        else
          o = elm_thumb_add(parent);
     }
   else
     o = elm_thumb_add(parent);
   if (!o) return NULL;

   if (path) ephoto_thumb_path_set(o, path);
   elm_object_style_set(o, "noframe");
   ephoto->thumbs = eina_list_append(ephoto->thumbs, o);
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, _thumb_del, ephoto);
   return o;
}

void
ephoto_thumb_path_set(Evas_Object *o, const char *path)
{
   Ethumb_Thumb_Format format = ETHUMB_THUMB_FDO;
   const char *group = NULL;
   const char *ext = strrchr(path, '.');
   if (ext)
     {
        ext++;
        if ((strcasecmp(ext, "jpg") == 0) ||
            (strcasecmp(ext, "jpeg") == 0))
          format = ETHUMB_THUMB_JPEG; /* faster! */
        else if ((strcasecmp(ext, "edj") == 0))
          {
             if (edje_file_group_exists(path, "e/desktop/background"))
               group = "e/desktop/background";
             else
               {
                  Eina_List *g = edje_file_collection_list(path);
                  group = eina_list_data_get(g);
                  edje_file_collection_list_free(g);
               }
             elm_icon_file_set(o, path, group);
             evas_object_data_set(o, "ephoto_format", NULL);
             return;
          }
     }
   ethumb_client_format_set(elm_thumb_ethumb_client_get(), format);
   evas_object_data_set(o, "ephoto_format", (void*)(long)format);
   elm_thumb_file_set(o, path, group);
}

Ephoto_Entry *
ephoto_entry_new(Ephoto *ephoto, const char *path, const char *label)
{
   Ephoto_Entry *entry;
   EINA_SAFETY_ON_NULL_RETURN_VAL(path, NULL);
   entry = calloc(1, sizeof(Ephoto_Entry));
   EINA_SAFETY_ON_NULL_RETURN_VAL(entry, NULL);
   entry->ephoto = ephoto;
   entry->path = eina_stringshare_add(path);
   entry->basename = ecore_file_file_get(entry->path);
   entry->label = eina_stringshare_add(label);
   return entry;
}

void
ephoto_entry_free(Ephoto_Entry *entry)
{
   const char *s;
   Ephoto_Entry_Free_Listener *fl;

   EINA_LIST_FREE(entry->free_listeners, fl)
     {
        fl->cb((void *)fl->data, entry);
        free(fl);
     }
   EINA_SAFETY_ON_NULL_RETURN(entry);
   eina_stringshare_del(entry->path);
   eina_stringshare_del(entry->label);
   EINA_LIST_FREE(entry->dir_files, s) eina_stringshare_del(s);
   free(entry);
}

void
ephoto_entry_free_listener_add(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data)
{
   Ephoto_Entry_Free_Listener *fl;
   EINA_SAFETY_ON_NULL_RETURN(entry);
   EINA_SAFETY_ON_NULL_RETURN(cb);
   fl = malloc(sizeof(Ephoto_Entry_Free_Listener));
   EINA_SAFETY_ON_NULL_RETURN(fl);
   fl->cb = cb;
   fl->data = data;
   entry->free_listeners = eina_list_append(entry->free_listeners, fl);
}

void
ephoto_entry_free_listener_del(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data)
{
   Eina_List *l;
   Ephoto_Entry_Free_Listener *fl;
   EINA_SAFETY_ON_NULL_RETURN(entry);
   EINA_SAFETY_ON_NULL_RETURN(cb);
   EINA_LIST_FOREACH(entry->free_listeners, l, fl)
     {
        if ((fl->cb == cb) && (fl->data == data))
          {
             entry->free_listeners = eina_list_remove_list
               (entry->free_listeners, l);
             break;
          }
     }
}

void
ephoto_entries_free(Ephoto *ephoto)
{
   Ephoto_Entry *entry;
   EINA_LIST_FREE(ephoto->entries, entry) ephoto_entry_free(entry);
}
