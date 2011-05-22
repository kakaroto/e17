#include "ephoto.h"

int EPHOTO_EVENT_ENTRY_CREATE_DIR = 0;
int EPHOTO_EVENT_ENTRY_CREATE_THUMB = 0;
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
_back_clicked(void *data, Evas_Object *o __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   Ephoto *ephoto = data;

   switch (ephoto->state)
     {
        case EPHOTO_STATE_SINGLE : ephoto_promote_thumb_browser(ephoto); break;
        case EPHOTO_STATE_THUMB : ephoto_promote_list_browser(ephoto); break;
        case EPHOTO_STATE_LIST : break;
        default : break;
     }
   ephoto_title_set(ephoto, ephoto->config->directory);
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
   Evas_Object *o;
   char buf[PATH_MAX];
   int w, h;
   EINA_SAFETY_ON_NULL_RETURN_VAL(ephoto, NULL);

   EPHOTO_EVENT_ENTRY_CREATE_DIR = ecore_event_type_new();
   EPHOTO_EVENT_ENTRY_CREATE_THUMB = ecore_event_type_new();
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

   ephoto->overlay = elm_bg_add(ephoto->bg);
   evas_object_size_hint_weight_set
     (ephoto->overlay, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ephoto->overlay, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_bg_color_set(ephoto->overlay, 0, 0, 0);

   elm_win_resize_object_add(ephoto->win, ephoto->bg);
   evas_object_show(ephoto->bg);

   ephoto->layout = elm_layout_add(ephoto->win);
   if (!ephoto->layout)
     {
      evas_object_del(ephoto->win);
      return NULL;
     }
   if (!elm_layout_theme_set
          (ephoto->layout, "layout", "application", "content-back"))
     {
        ERR("Content-back");
        evas_object_del(ephoto->win);
        return NULL;
     }
   evas_object_size_hint_weight_set
     (ephoto->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ephoto->layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->layout);
   ephoto->edje = elm_layout_edje_get(ephoto->layout);  
   edje_object_signal_callback_add(ephoto->edje, "elm,action,back", "", 
     _back_clicked, ephoto);
   evas_object_show(ephoto->layout);

   o = edje_object_part_external_object_get(ephoto->edje, "back");
   evas_object_geometry_get(o, 0, 0, &w, &h);

   ephoto->pager = elm_pager_add(ephoto->win);
   elm_object_style_set(ephoto->pager, "fade_invisible");
   evas_object_size_hint_weight_set
     (ephoto->pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (ephoto->pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(ephoto->pager);
   elm_layout_content_set(ephoto->layout, "elm.swallow.content", ephoto->pager);

   ephoto->help_but = elm_button_add(ephoto->win);
   o = elm_icon_add(ephoto->help_but);
   elm_icon_standard_set(o, "help-browser");
   evas_object_size_hint_aspect_set(o, EVAS_ASPECT_CONTROL_BOTH, 1, 1);   
   elm_button_icon_set(ephoto->help_but, o);
   evas_object_size_hint_weight_set
     (ephoto->help_but, 0.0, 0.0);
   evas_object_size_hint_fill_set
     (ephoto->help_but, 0.0, 0.0);
   evas_object_size_hint_min_set(ephoto->help_but, w, h);
   evas_object_size_hint_max_set(ephoto->help_but, w, h);
   evas_object_show(ephoto->help_but);
   elm_layout_content_set(ephoto->layout, "elm.swallow.end", ephoto->help_but);


   ephoto->list_browser = ephoto_list_browser_add(ephoto, ephoto->pager);
   if (!ephoto->list_browser)
     {
        ERR("could not add list browser");
        evas_object_del(ephoto->win);
        return NULL;
     }
   elm_pager_content_push(ephoto->pager, ephoto->list_browser);

   ephoto->thumb_browser = ephoto_thumb_browser_add(ephoto, ephoto->pager);
   if (!ephoto->thumb_browser)
     {
        ERR("could not add thumb browser");
        evas_object_del(ephoto->win);
        return NULL;
     }
   elm_pager_content_push(ephoto->pager, ephoto->thumb_browser);

   ephoto->single_browser = ephoto_single_browser_add(ephoto, ephoto->pager);
   if (!ephoto->single_browser)
     {
        ERR("could not add single browser");
        evas_object_del(ephoto->win);
        return NULL;
     }
   elm_pager_content_push(ephoto->pager, ephoto->single_browser);

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
        ephoto_promote_list_browser(ephoto);
        ephoto_directory_set(ephoto, path);
     }
   else
     {
        ephoto_single_browser_path_pending_set(ephoto->single_browser, path);
        ephoto_promote_single_browser(ephoto, NULL);
        char *dir = ecore_file_dir_get(path);
        ephoto_directory_set(ephoto, dir);
        free(dir);
     }

   /* TODO restore size from last run as well? */
   evas_object_resize(ephoto->win, 800, 600);
   evas_object_show(ephoto->win);

   return ephoto->win;
}

void
ephoto_title_set(Ephoto *ephoto, const char *title)
{
   char buf[1024] = "Ephoto";

   if (title) snprintf(buf, sizeof(buf), "Ephoto - %s", title);
   elm_win_title_set(ephoto->win, buf);
   edje_object_part_text_set(ephoto->edje, "elm.text.title", title);
}

void
ephoto_promote_list_browser(Ephoto *ephoto)
{
   elm_pager_content_promote(ephoto->pager, ephoto->list_browser);
   edje_object_signal_emit(ephoto->edje, "elm,back,hide", "elm");
   _ephoto_state_set(ephoto, EPHOTO_STATE_LIST);
}

void
ephoto_promote_thumb_browser(Ephoto *ephoto)
{
   if (elm_bg_overlay_get(ephoto->bg))
     {
        evas_object_hide(ephoto->overlay);
        elm_bg_overlay_unset(ephoto->bg);
     }
   elm_pager_content_promote(ephoto->pager, ephoto->thumb_browser);
   edje_object_signal_emit(ephoto->edje, "elm,back,show", "elm");
   _ephoto_state_set(ephoto, EPHOTO_STATE_THUMB);
}

void
ephoto_promote_single_browser(Ephoto *ephoto, Ephoto_Entry *e)
{
   elm_pager_content_promote(ephoto->pager, ephoto->single_browser);
   elm_bg_overlay_set(ephoto->bg, ephoto->overlay);
   if (e)
     ephoto_single_browser_entry_set(ephoto->single_browser, e);
   _ephoto_state_set(ephoto, EPHOTO_STATE_SINGLE);
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
_ephoto_populate_main(void *data, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   Ephoto *ephoto = data;
   Ephoto_Entry *e;
   Ephoto_Event_Entry_Create *ev;

   e = ephoto_entry_new(ephoto, info->path, info->path + info->name_start);
   
   ev = calloc(1, sizeof(Ephoto_Event_Entry_Create));
   ev->entry = e;

   if (ecore_file_is_dir(info->path))
     ecore_event_add(EPHOTO_EVENT_ENTRY_CREATE_DIR, ev, NULL, NULL);
   else
     {
        ephoto->entries = eina_list_sorted_insert(ephoto->entries, _entry_cmp, e);
        ecore_event_add(EPHOTO_EVENT_ENTRY_CREATE_THUMB, ev, NULL, NULL);
        if (ephoto->state == EPHOTO_STATE_LIST)
          ephoto_promote_thumb_browser(ephoto);
     }
}

static Eina_Bool
_ephoto_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   const char *bname = info->path + info->name_start;

   if (bname[0] == '.') return EINA_FALSE;

   if (ecore_file_is_dir(info->path))
     return EINA_TRUE;
   else
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
        ethumb_client_format_set(client, format);
        elm_thumb_reload(o);
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
   const char *ext = strrchr(path, '.');
   if (ext)
     {
        ext++;
        if ((strcasecmp(ext, "jpg") == 0) ||
            (strcasecmp(ext, "jpeg") == 0))
          format = ETHUMB_THUMB_JPEG; /* faster! */
     }

   ethumb_client_format_set(elm_thumb_ethumb_client_get(), format);
   evas_object_data_set(o, "ephoto_format", (void*)(long)format);
   elm_thumb_file_set(o, path, NULL);
}

Evas_Object *
ephoto_list_icon_add(Ephoto *ephoto, Evas_Object *parent, const char *standard)
{
   Evas_Object *o;

   EINA_SAFETY_ON_NULL_RETURN_VAL(parent, NULL);

   o = elm_icon_add(parent);
   if (!o) return NULL;

   if (standard) elm_icon_standard_set(o, standard);
   evas_object_size_hint_aspect_set(o, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   ephoto->dirs = eina_list_append(ephoto->dirs, o);
   
   return o;
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
   Ephoto_Entry_Free_Listener *fl;

   EINA_LIST_FREE(entry->free_listeners, fl)
     {
        fl->cb((void *)fl->data, entry);
        free(fl);
     }
   EINA_SAFETY_ON_NULL_RETURN(entry);
   eina_stringshare_del(entry->path);
   eina_stringshare_del(entry->label);
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

