#include "ephoto.h"

/*Ephoto Window Callbacks*/
static void _ephoto_window_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__);

/*Ephoto Thumb Population Callbacks*/
static void _ephoto_thumb_populate(void *data __UNUSED__);
static Eina_Bool _ephoto_thumb_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info);
static void _ephoto_thumb_populate_main(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info);
static void _ephoto_thumb_populate_end(void *data __UNUSED__, Eio_File *handler __UNUSED__);
static void _ephoto_thumb_populate_error(void *data __UNUSED__, Eio_File *handler __UNUSED__, int error);

/*Main Ephoto Structure*/
Ephoto *ephoto;

/*Ephoto Events*/
int EPHOTO_EVENT_ENTRY_CREATE = 0;
int EPHOTO_EVENT_POPULATE_START = 0;
int EPHOTO_EVENT_POPULATE_END = 0;
int EPHOTO_EVENT_POPULATE_ERROR = 0;

/*Ephoto Entry Listener*/
typedef struct _Ephoto_Entry_Free_Listener Ephoto_Entry_Free_Listener;
struct _Ephoto_Entry_Free_Listener
{
   void (*cb)(void *data, const Ephoto_Entry *dead);
   const void *data;
};

Evas_Object *
ephoto_window_add(const char *path)
{
   EPHOTO_EVENT_ENTRY_CREATE = ecore_event_type_new();
   EPHOTO_EVENT_POPULATE_START = ecore_event_type_new();
   EPHOTO_EVENT_POPULATE_END = ecore_event_type_new();
   EPHOTO_EVENT_POPULATE_ERROR = ecore_event_type_new();

   ephoto = calloc(1, sizeof(Ephoto));
   ephoto->client = elm_thumb_ethumb_client_get();

   ephoto->win = elm_win_add(NULL, "ephoto", ELM_WIN_BASIC);
   if (!ephoto->win)
     return NULL;
   evas_object_resize(ephoto->win, 600, 480);
   ephoto_title_set("Ephoto");
   evas_object_smart_callback_add
     (ephoto->win, "delete,request", _ephoto_window_del, NULL);
   evas_object_show(ephoto->win);

   ephoto->bg = elm_bg_add(ephoto->win);
   evas_object_size_hint_weight_set
     (ephoto->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(ephoto->bg, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->bg);
   evas_object_show(ephoto->bg);

   ephoto->pager = elm_pager_add(ephoto->win);
   elm_object_style_set(ephoto->pager, "fade");
   evas_object_size_hint_weight_set
     (ephoto->pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set
     (ephoto->pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(ephoto->win, ephoto->pager);
   evas_object_show(ephoto->pager);

   ephoto->slideshow = ephoto_slideshow_add();
   elm_pager_content_push(ephoto->pager, ephoto->slideshow);

   ephoto->flow_browser = ephoto_flow_browser_add();
   elm_pager_content_push(ephoto->pager, ephoto->flow_browser);

   ephoto->thumb_browser = ephoto_thumb_browser_add();
   elm_pager_content_push(ephoto->pager, ephoto->thumb_browser);

   elm_pager_content_promote(ephoto->pager, ephoto->thumb_browser);
   ephoto->state = EPHOTO_STATE_THUMB;

   if ((!path) || (!ecore_file_exists(path)))
     {
        char buf[PATH_MAX];

        if (getcwd(buf, sizeof(buf)))
          path = buf;
        else
          path = getenv("HOME");
     }
   if (ecore_file_is_dir(path))
        ephoto_populate(path);
   else
     {
        char *dir = ecore_file_dir_get(path);
        ephoto_populate(dir);
        free(dir);
     }
   return ephoto->win;
}

void
ephoto_populate(const char *path)
{
   eina_stringshare_replace(&ephoto->directory, path);
   if (ephoto->change_dir) ecore_job_del(ephoto->change_dir);
   ephoto->change_dir = ecore_job_add(_ephoto_thumb_populate, NULL); 
}

void
ephoto_title_set(const char *title)
{
   elm_win_title_set(ephoto->win, title);
}

void
ephoto_state_set(Ephoto_State state)
{
   ephoto->prev_state = ephoto->state;
   ephoto->state = state;
}

void
ephoto_thumb_browser_show(Ephoto_Entry *entry)
{
   ephoto_state_set(EPHOTO_STATE_THUMB);
   ephoto_flow_browser_entry_set(NULL);
   ephoto_slideshow_entry_set(NULL);
   elm_pager_content_promote(ephoto->pager, ephoto->thumb_browser);
   ephoto_thumb_browser_entry_set(entry);
}

void
ephoto_flow_browser_show(Ephoto_Entry *entry)
{
   ephoto_state_set(EPHOTO_STATE_FLOW);
   elm_pager_content_promote(ephoto->pager, ephoto->flow_browser);
   ephoto_flow_browser_entry_set(entry);
}

void
ephoto_slideshow_show(Ephoto_Entry *entry)
{
   ephoto_state_set(EPHOTO_STATE_SLIDESHOW);
   elm_pager_content_promote(ephoto->pager, ephoto->slideshow);
   ephoto_slideshow_entry_set(entry);
}

Ephoto_Entry *
ephoto_entry_new(const char *path, const char *label)
{
   Ephoto_Entry *entry;

   entry = calloc(1, sizeof(Ephoto_Entry));
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
   eina_stringshare_del(entry->path);
   eina_stringshare_del(entry->label);
   free(entry);
}

void
ephoto_entry_free_listener_add(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data)
{
   Ephoto_Entry_Free_Listener *fl;

   fl = malloc(sizeof(Ephoto_Entry_Free_Listener));
   fl->cb = cb;
   fl->data = data;
   entry->free_listeners = eina_list_append(entry->free_listeners, fl);
}

void
ephoto_entry_free_listener_del(Ephoto_Entry *entry, void (*cb)(void *data, const Ephoto_Entry *entry), const void *data)
{
   Eina_List *l;
   Ephoto_Entry_Free_Listener *fl;

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
ephoto_entries_free(void)
{
   Ephoto_Entry *entry;

   EINA_LIST_FREE(ephoto->entries, entry) ephoto_entry_free(entry);
}

static void
_ephoto_window_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event __UNUSED__)
{
   ephoto_thumb_browser_del();
   ephoto_flow_browser_del();
   ephoto_slideshow_del();
   evas_object_del(ephoto->win);
   if (ephoto->regen) 
     ecore_timer_del(ephoto->regen);
   if (ephoto->directory)
     eina_stringshare_del(ephoto->directory);
   free(ephoto);
}

static void
_ephoto_thumb_populate(void *data __UNUSED__)
{
   ephoto->change_dir = NULL;
   ephoto_entries_free();
   ephoto->ls = eio_file_stat_ls(ephoto->directory,
                                 _ephoto_thumb_populate_filter,
                                 _ephoto_thumb_populate_main,
                                 _ephoto_thumb_populate_end,
                                 _ephoto_thumb_populate_error,
                                 NULL);
   ecore_event_add(EPHOTO_EVENT_POPULATE_START, NULL, NULL, NULL);
}

static Eina_Bool
_ephoto_thumb_populate_filter(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   const char *ext;

   ext = strrchr(info->path, '.');
   if (ext)
     {
        if ((!strncasecmp(ext, ".jpeg", 5)) ||
            (!strncasecmp(ext, ".jpg", 4)) ||
            (!strncasecmp(ext, ".png", 4)) ||
            (!strncasecmp(ext, ".gif", 4)) ||
            (!strncasecmp(ext, ".svg", 4)))
              return EINA_TRUE;
     }
   return EINA_FALSE;
}

static void
_ephoto_thumb_populate_main(void *data __UNUSED__, Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   Ephoto_Entry *e;
   Ephoto_Event_Entry_Create *ev;

   e = ephoto_entry_new(info->path, info->path + info->name_start);
   ephoto->entries = eina_list_append(ephoto->entries, e);

   ev = calloc(1, sizeof(Ephoto_Event_Entry_Create));
   ev->entry = e;

   ecore_event_add(EPHOTO_EVENT_ENTRY_CREATE, ev, NULL, NULL);
}

static void
_ephoto_thumb_populate_end(void *data __UNUSED__, Eio_File *handler __UNUSED__)
{
   ephoto->ls = NULL;
   
   ecore_event_add(EPHOTO_EVENT_POPULATE_END, NULL, NULL, NULL);
}

static void
_ephoto_thumb_populate_error(void *data __UNUSED__, Eio_File *handler __UNUSED__, int error)
{
   if (error)
      printf("Error while populating images: %s\n", strerror(error));
   ecore_event_add(EPHOTO_EVENT_POPULATE_ERROR, NULL, NULL, NULL);
   _ephoto_thumb_populate_end(NULL, NULL);
}

