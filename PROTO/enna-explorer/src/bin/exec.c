#include <Elementary.h>
#include <Efreet.h>

#include "exec.h"
#include "file.h"
#include "enna.h"
#include "enna_config.h"

typedef struct _Enna_Mime Enna_Mime;
typedef struct _Enna_Mime_Hash Enna_Mime_Hash;
typedef struct _Enna_Mime_Hash_Item Enna_Mime_Hash_Item;

struct _Enna_Mime
{
   Eina_Hash *hash; /* Hash element type is an Eina_List of Enna_Mime_Item */
   const char *filename;
   Eet_File *ef;
   Ecore_Timer *timer;
};

struct _Enna_Mime_Hash
{
   Eina_List *items; /* Items of type Enna_Mime_Item */

};

struct _Enna_Mime_Hash_Item
{
   const char *app;   /* Application name */
   unsigned int freq; /* Number of use for this app */
   unsigned int last; /* Used last time this mime has been opened */
};

static Eet_Data_Descriptor *_mime_edd = NULL;
static Eet_Data_Descriptor *_mime_hash_edd = NULL;
static Eet_Data_Descriptor *_mime_hash_item_edd = NULL;
static Enna_Mime *_mime = NULL;

static void _mime_add(const char *mime, const char *app);

static void*
_app_exec_cb(void *data,
             Efreet_Desktop *desktop,
             char *command, int remaining)
{
    ecore_exe_run(command, NULL);
    return NULL;
}

static void
_app_selected_cb(void *data, Evas_Object *obj, void *event)
{

   Evas_Object *win = data;
   Efreet_Desktop *desk = evas_object_data_get(win, "desk");
   Enna_File *file = evas_object_data_get(win, "file");
   const char *mime = evas_object_data_get(win, "mime");
   Eina_List *files = NULL;

   files = eina_list_append(files, file->mrl);
   _mime_add(mime, desk->orig_path);
   eina_stringshare_del(mime);
   efreet_desktop_command_get(desk, files,
                              _app_exec_cb, NULL);
}

void _mime_selection_window_show(Enna_File *file)
{
   const char *mime;
   Eina_List *apps, *l;
   Efreet_Desktop *desk = NULL;

   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *fr;
   Evas_Object *list;
   Evas_Object *en;
   Evas_Object *bx;
   Evas_Object *btn_bx;
   Evas_Object *btn_ok;
   Evas_Object *btn_cancel;


   win = elm_win_add(enna->win, NULL, ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(win, _("Open File"));
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_data_set(win, "file", file);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);
   evas_object_size_hint_min_set(bg, 400, 300);

   fr = elm_frame_add(win);
   elm_object_style_set(fr, "pad_medium");
   evas_object_show(fr);

   elm_win_resize_object_add(win, fr);

   bx = elm_box_add(win);
   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(bx);
   elm_box_homogeneous_set(bx, EINA_FALSE);
   elm_frame_content_set(fr, bx);
   elm_box_padding_set(bx, 4, 4);

   fr = elm_frame_add(win);
   elm_object_text_set(fr, "Choose a program to open the file");
   evas_object_show(fr);
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);

   list = elm_list_add(win);
   evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(list);
   elm_list_mode_set(list, ELM_LIST_LIMIT);


   /* Display application associated to text/plain if apps list is void */
   mime = efreet_mime_type_get(file->uri);
   if (!mime)
       apps = efreet_util_desktop_mime_list("text/plain");
   else
     {
        apps = efreet_util_desktop_mime_list(mime);
        if (!apps)
          apps = efreet_util_desktop_mime_list("text/plain");
     }

   EINA_LIST_FOREACH(apps, l, desk)
     {
        //Eina_List *files = NULL;
        Evas_Object *ic;

        //files = eina_list_append(files, file->mrl);
        ic = elm_icon_add(win);
        evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        evas_object_size_hint_min_set(ic, 24, 24);
        elm_icon_standard_set(ic, desk->icon);
        evas_object_data_set(win, "desk", desk);
        evas_object_data_set(win, "file", file);
        evas_object_data_set(win, "mime", eina_stringshare_add(mime));
        elm_list_item_append(list, desk->orig_path, ic, NULL, _app_selected_cb, win);


     }

   elm_frame_content_set(fr, list);
   elm_box_pack_end(bx, fr);


   fr = elm_frame_add(win);
   elm_object_text_set(fr, "Specific program");
   evas_object_show(fr);
   evas_object_size_hint_weight_set(fr, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(fr, EVAS_HINT_FILL, EVAS_HINT_FILL);

   en = elm_entry_add(win);
   elm_entry_single_line_set(en, EINA_TRUE);
   evas_object_size_hint_weight_set(en, EVAS_HINT_EXPAND,0);
   evas_object_size_hint_align_set(en, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(en, file->label);
   evas_object_show(en);
   evas_object_data_set(win, "entry", en);

   elm_frame_content_set(fr, en);
   elm_box_pack_end(bx, fr);

   btn_bx = elm_box_add(win);
   elm_box_horizontal_set(btn_bx, EINA_TRUE);
   evas_object_size_hint_weight_set(btn_bx, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(btn_bx, EVAS_HINT_FILL, 1.0);
   evas_object_show(btn_bx);
   elm_box_padding_set(btn_bx, 8, 2);

   btn_ok = elm_button_add(win);
   elm_object_text_set(btn_ok, _("Open it"));
   evas_object_show(btn_ok);
   evas_object_size_hint_weight_set(btn_ok, EVAS_HINT_EXPAND, 0);
   evas_object_size_hint_align_set(btn_ok, 0.0, EVAS_HINT_FILL);
   //evas_object_smart_callback_add(btn_ok, "clicked",
   //                               _dialog_rename_ok_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_ok);

   btn_cancel = elm_button_add(win);
   elm_object_text_set(btn_cancel, _("Cancel"));
   evas_object_show(btn_cancel);
   evas_object_size_hint_weight_set(btn_cancel, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(btn_cancel, 1.0, EVAS_HINT_FILL);
   //evas_object_smart_callback_add(btn_cancel, "clicked",
   //                               _dialog_cancel_clicked_cb, priv);
   elm_box_pack_end(btn_bx, btn_cancel);

   elm_box_pack_end(bx, btn_bx);

   evas_object_show(win);
}

static Enna_Mime_Hash_Item*
_mime_is_known(const char *mime)
{
   Enna_Mime_Hash *mime_hash = NULL;

   /* Find the list of apps attached to this mime type */
   mime_hash = eina_hash_find(_mime->hash, mime);
   if (mime_hash)
     return eina_list_nth(mime_hash->items, 0);
   else
     return NULL;
}

void
enna_exec(Enna_File *file)
{
   Enna_Mime_Hash_Item *it;
   if (!file || ENNA_FILE_IS_BROWSABLE(file))
     return;

   if (!file->mime)
     file->mime = eina_stringshare_add(efreet_mime_type_get(file->mrl));

   /* Do we know preferred executable for this mime ? */
   if ((it = _mime_is_known(file->mime)))
     {
        Efreet_Desktop *desk;
        Eina_List *files = NULL;
        desk = efreet_desktop_get(it->app);
        if (!desk)
          {
             printf("Error: unable to find desktop file %s\n", it->app);
             _mime_selection_window_show(file);
          }
        else
          {
             files = eina_list_append(files, file->mrl);
             efreet_desktop_command_get(desk, files,
                                        _app_exec_cb, NULL);
          }
     }
   else
     /* No : display the application selector window */
     _mime_selection_window_show(file);

}

static Eina_Bool
_mime_save(void *data)
{
   eet_sync(_mime->ef);
   eet_close(_mime->ef);

   _mime->ef = NULL;
   _mime->timer = NULL;

   return ECORE_CALLBACK_CANCEL;
}


void
enna_exec_mime_flush(void)
{
   if (!_mime->ef)
        _mime->ef = eet_open(_mime->filename, EET_FILE_MODE_READ_WRITE);

   eet_data_write(_mime->ef, _mime_edd, "mime", _mime, 1);

   if ( (_mime->ef) && !(_mime->timer))
        _mime->timer = ecore_timer_add(3.0, _mime_save, NULL);
}

static int
_sort_cb(const void *d1, const void *d2)
{
   const Enna_Mime_Hash_Item *it1 = d1;
   const Enna_Mime_Hash_Item *it2 = d2;

   if (it1->freq == it2->freq)
     if (it1->last)
       return 1;
     else if (it2->last)
       return -1;
     else
       return strcasecmp(it1->app, it2->app);
   else
     if (it1->freq > it2->freq)
       return 1;
     else
       return -1;
}

static void
_mime_add(const char *mime, const char *app)
{
   Enna_Mime_Hash *mime_hash;

   /* Find the list of apps attached to this mime type */
   mime_hash = eina_hash_find(_mime->hash, mime);
   /* No items found */
   if (!mime_hash)
     {

        /* Create a new entry add it to list and add list to hash*/
        Enna_Mime_Hash_Item *item;
        mime_hash = calloc(1, sizeof(Enna_Mime_Hash));
        mime_hash->items = NULL;
        item = calloc(1, sizeof(Enna_Mime_Hash_Item));
        item->app = eina_stringshare_add(app);
        item->freq = 1;
        item->last = 1;
        mime_hash->items = eina_list_append(mime_hash->items, item);
        eina_hash_add(_mime->hash, mime, mime_hash);
     }
   else
     {
        /* Mime already saved */
        Enna_Mime_Hash_Item *item;
        Eina_List *l;
        Eina_Bool found = EINA_FALSE;

         EINA_LIST_FOREACH(mime_hash->items, l, item)
          {
             item->last = 0;
             if (!strcmp(app, item->app))
               {
                  /* Increment frequency */
                  item->freq++;
                  item->last = 1;
                  found = EINA_TRUE;
               }
          }
        if (!found)
          {
             /* app not found in list add it */
             item = calloc(1, sizeof(Enna_Mime_Hash_Item));
             item->app = eina_stringshare_add(app);
             item->freq = 1;
             item->last = 1;
             mime_hash->items = eina_list_append(mime_hash->items, item);
          }
        else
          {
             /* Sort list of used apps */
             mime_hash->items = eina_list_sort(mime_hash->items,
                                               eina_list_count(mime_hash->items),
                                               _sort_cb);
          }
     }
   enna_exec_mime_flush();
}

void
_mime_hash_item_del_cb(void *data)
{
   Enna_Mime_Hash *hash_item = data;
   Enna_Mime_Hash_Item *it;

   if (!data)
     return;

   EINA_LIST_FREE(hash_item->items, it)
     {
        eina_stringshare_del(it->app);
        free(it);
     }
   eina_list_free(hash_item->items);
   hash_item->items = NULL;
}

int
enna_exec_init(void)
{
   Eet_File *ef;
   char mime_file[4096];

   if (!eet_init())
       return 0;

   snprintf(mime_file, sizeof(mime_file), "%s/.config/enna-explorer/", getenv("HOME"));

   if (!ecore_file_exists(mime_file))
       ecore_file_mkdir(mime_file);

   snprintf(mime_file, sizeof(mime_file), "%s/.config/enna-explorer/mime.eet", getenv("HOME"));

#undef T
#undef D
#define T Enna_Mime_Hash_Item
#define D _mime_hash_item_edd
   D =  ENNA_CONFIG_DD_NEW("Enna_Mime_Hash_Item", T);
   ENNA_CONFIG_VAL(D, T, app, STR);
   ENNA_CONFIG_VAL(D, T, freq, INT);
   ENNA_CONFIG_VAL(D, T, last, INT);
#undef T
#undef D

#define T Enna_Mime_Hash
#define D _mime_hash_edd
   D =  ENNA_CONFIG_DD_NEW("Enna_Mime_Hash", T);
   ENNA_CONFIG_LIST(D, T, items, _mime_hash_item_edd);
#undef T
#undef D

#define T Enna_Mime
#define D _mime_edd
   D =  ENNA_CONFIG_DD_NEW("Enna_Mime", T);
   ENNA_CONFIG_HASH(D, T, hash, _mime_hash_edd);
#undef T
#undef D

   ef = eet_open(mime_file, EET_FILE_MODE_READ_WRITE);

   _mime = eet_data_read(ef, _mime_edd, "mime");
   if(!_mime)
     _mime = calloc(1, sizeof(Enna_Mime));

   if (!_mime->hash)
     _mime->hash = eina_hash_string_superfast_new(_mime_hash_item_del_cb);

   eet_close(ef);
   _mime->filename = strdup(mime_file);
   _mime->ef = NULL;

   return 1;
}

void
enna_exec_shutdown(void)
{
   if (!_mime->ef)
        _mime->ef = eet_open(_mime->filename, EET_FILE_MODE_READ_WRITE);

   if (_mime->timer)
     ecore_timer_del(_mime->timer);
   _mime->timer = NULL;
   eet_data_write(_mime->ef, _mime_edd, "mime", _mime, 1);
   _mime_save(NULL);
   eet_close(_mime->ef);
   _mime->ef = NULL;

   /* TODO free hash items */
   /* EINA_HASH_FOREACH */
}
