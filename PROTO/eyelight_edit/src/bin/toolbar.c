#include "main.h"

Elm_Menu_Item *mi_save;
Elm_Menu_Item *mi_slideshow;

static void _pres_open_cb(void *data, Evas_Object *obj, void *event_info);
static void _pres_new_cb(void *data, Evas_Object *obj, void *event_info);
static void _recent_open_cb(void *data, Evas_Object *obj, void *event_info);

static void _pres_fileselector_done(void *data, Evas_Object *obj, void *event_info);
static void _pres_new_fileselector_done(void *data, Evas_Object *obj, void *event_info);

static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info);
static void _pres_save_cb(void *data, Evas_Object *obj, void *event_info);

Evas_Object *toolbar_create()
{
    Evas_Object *tb;
    Evas_Object *menu;
    Elm_Toolbar_Item *item;
    Elm_Menu_Item *mi;
    Eina_List *l;
    L_Recent_File *l_file;
    Recent_File *file;

    tb = elm_toolbar_add(win);
    elm_toolbar_menu_parent_set(tb, win);
    elm_toolbar_homogenous_set(tb, EINA_FALSE);
    elm_toolbar_align_set(tb, 0.0);
    evas_object_size_hint_weight_set(tb, 0.0, 0.0);
    evas_object_size_hint_align_set(tb, -1.0, 0.0);

    item = elm_toolbar_item_add(tb, NULL, D_("File"), NULL, NULL);
    menu = elm_toolbar_item_menu_get(item);
    elm_menu_item_add(menu, NULL, NULL, D_("New"), _pres_new_cb, NULL);
    elm_menu_item_add(menu, NULL, NULL, D_("Open"), _pres_open_cb, NULL);
    mi = elm_menu_item_add(menu, NULL, NULL, D_("Recents"), NULL, NULL);

    l_file = recents_file_load();
    EINA_LIST_FOREACH(l_file->l, l, file)
        elm_menu_item_add(menu, mi, NULL, file->file, _recent_open_cb, NULL);
    recents_file_free(l_file);


    elm_menu_item_separator_add(menu, NULL);
    mi_save = elm_menu_item_add(menu, NULL, NULL, D_("Save"), _pres_save_cb, NULL);

    item = elm_toolbar_item_add(tb, NULL, D_("View"), NULL, NULL);
    menu = elm_toolbar_item_menu_get(item);
    mi_slideshow = elm_menu_item_add(menu, NULL, NULL, D_("Slideshow"), _slideshow_cb, NULL);

    evas_object_show(tb);
    return tb;
}

void toolbar_nopres_disabled_set(Eina_Bool disabled)
{
    elm_menu_item_disabled_set(mi_save, disabled);
    elm_menu_item_disabled_set(mi_slideshow, disabled);
}

static void _pres_new_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *inwin, *fs;

    setlocale(LC_ALL, "");

    inwin = elm_win_inwin_add(win);
    evas_object_show(inwin);

    fs = elm_fileselector_add(win);
    elm_fileselector_expandable_set(fs, EINA_FALSE);
    elm_fileselector_is_save_set(fs, EINA_TRUE);
    elm_fileselector_path_set(fs, getenv("HOME"));
    evas_object_size_hint_weight_set(fs, 1.0, 1.0);
    elm_win_resize_object_add(win, fs);
    evas_object_show(fs);
    evas_object_smart_callback_add(fs, "done", _pres_new_fileselector_done, inwin);

    elm_win_inwin_content_set(inwin, fs);
}

static void _pres_open_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *inwin, *fs;

    setlocale(LC_ALL, "");

    inwin = elm_win_inwin_add(win);
    evas_object_show(inwin);

    fs = elm_fileselector_add(win);
    elm_fileselector_expandable_set(fs, EINA_FALSE);
    elm_fileselector_path_set(fs, getenv("HOME"));
    evas_object_size_hint_weight_set(fs, 1.0, 1.0);
    elm_win_resize_object_add(win, fs);
    evas_object_show(fs);
    evas_object_smart_callback_add(fs, "done", _pres_fileselector_done, inwin);

    elm_win_inwin_content_set(inwin, fs);
}

static void _slideshow_cb(void *data, Evas_Object *obj, void *event_info)
{
     _pres_save_cb(NULL, NULL, NULL);
    slideshow_create();
}

static void _pres_save_cb(void *data, Evas_Object *obj, void *event_info)
{
    const char* pres_file = eyelight_object_presentation_file_get(pres);
    utils_save(pres_file);
}

static void _pres_new_fileselector_done(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;

   if (selected)
   {
       const char *file;
       char buf[PATH_MAX];
       L_Recent_File *l_file;
       Recent_File *r_file;

       char *dir = ecore_file_dir_get(selected);
       if(!ecore_file_exists(dir))
           ecore_file_mkpath(dir);
       EYELIGHT_FREE(dir);

       if(ecore_file_is_dir(selected))
       {
            snprintf(buf, sizeof(buf), "%s/presentation.elt", selected);
            file = eina_stringshare_add(buf);
       } 
       else
           file = eina_stringshare_add(selected);

       if(!ecore_file_exists(file))
       {
            FILE *f = fopen(file, "w");
            if(f)
                fclose(f);
            else
                return ;
       }

       utils_obj_unselect();
       eyelight_object_presentation_file_set(pres,file);
       toolbar_nopres_disabled_set(EINA_FALSE);
       slides_list_nopres_disabled_set(EINA_FALSE);
       eyelight_object_thumbnails_size_set(pres, 1024/2, 768/2);
       slides_list_update();
       slides_grid_update();

       l_file = recents_file_load();
       r_file = calloc(1, sizeof(Recent_File));
       r_file->file = eina_stringshare_add(file);
       l_file->l = eina_list_append(l_file->l, r_file);
       recents_file_save(l_file);
       recents_file_free(l_file);

       eina_stringshare_del(file);
   }

   evas_object_del(data);
}

static void _pres_fileselector_done(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;
   L_Recent_File *l_file;
   Recent_File *r_file;

   if (selected)
   {
       utils_obj_unselect();
       eyelight_object_presentation_file_set(pres,selected);
       toolbar_nopres_disabled_set(EINA_FALSE);
       slides_list_nopres_disabled_set(EINA_FALSE);
       eyelight_object_thumbnails_size_set(pres, 1024/2, 768/2);
       slides_list_update();
       slides_grid_update();

       l_file = recents_file_load();
       r_file = calloc(1, sizeof(Recent_File));
       r_file->file = eina_stringshare_add(selected);
       l_file->l = eina_list_append(l_file->l, r_file);
       recents_file_save(l_file);
       recents_file_free(l_file);
   }

   evas_object_del(data);
}

static void _recent_open_cb(void *data, Evas_Object *obj, void *event_info)
{
   Elm_Menu_Item *mi = event_info;
   L_Recent_File *l_file;
   Recent_File *r_file;

   utils_obj_unselect();
   eyelight_object_presentation_file_set(pres,elm_menu_item_label_get(mi));
   toolbar_nopres_disabled_set(EINA_FALSE);
   slides_list_nopres_disabled_set(EINA_FALSE);
   eyelight_object_thumbnails_size_set(pres, 1024/2, 768/2);
   slides_list_update();
   slides_grid_update();

   l_file = recents_file_load();
   r_file = calloc(1, sizeof(Recent_File));
   r_file->file = eina_stringshare_add(elm_menu_item_label_get(mi));
   l_file->l = eina_list_append(l_file->l, r_file);
   recents_file_save(l_file);
   recents_file_free(l_file);

   evas_object_del(data);
}

