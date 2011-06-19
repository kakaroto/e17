#include "main.h"
#include "evas_object/slideshow_object.h"
#include "slideshow.h"

int APP_LOG_DOMAIN;
const char *Theme;

const char *media_player = NULL;
Enlil_Data *enlil_data = NULL;
static Tabpanel_Item *tp_list_photo;
static Tabpanel_Item *tp_menu;
Evas_Object *global_object;
Evas_Object *main_panel_object;

static const Ecore_Getopt options =
   { "Enki", NULL, VERSION, "(C) 2009 Photo manager, see AUTHORS.",
     "LGPL with advertisement, see COPYING",
     "A photo manager using the EFL !\n\n", 1,
        { ECORE_GETOPT_VERSION('V', "version"),
          ECORE_GETOPT_COPYRIGHT('R', "copyright"),
          ECORE_GETOPT_LICENSE('L', "license"),
          ECORE_GETOPT_STORE_STR('l', "library",
                                 "Specify the location of a library"),
          ECORE_GETOPT_STORE_STR('t', "theme", "Specify the edje theme"),
          ECORE_GETOPT_STORE_STR('s', "size", "Specify the window size"),
          ECORE_GETOPT_HELP('h', "help"), ECORE_GETOPT_SENTINEL } };

void
close_cb(void *data, Evas_Object *obj, void *event_info)
{
   enlil_file_manager_flush();

   slideshow_hide();
   slideshow_clear();

   photos_list_object_freeze(enlil_data->list_photo->o_list, 1);

   download_free(&(enlil_data->dl));
   upload_free(&(enlil_data->ul));
   enlil_library_free(&(enlil_data->library));
   enlil_sync_free(&(enlil_data->sync));
   if (enlil_data->load) enlil_load_free(&(enlil_data->load));

   map_free(enlil_data->map);

   tabpanel_item_del(enlil_data->library_item);
   tabpanel_del(enlil_data->tabpanel);
   free(enlil_data->list_photo);
   free(enlil_data->list_left);
   free(enlil_data->win);
   free(enlil_data);

   EINA_STRINGSHARE_DEL(media_player);

   elm_exit();
}

static void
_notify_bt_close(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *notify = data;
   evas_object_hide(notify);
}

static void
_tabpanel_select_page1_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item)
{
   //Enlil_Data *enlil_data = data;
}

static void
_photos_list_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item)
{
   Enlil_Data *enlil_data = data;
   enlil_data->list_left->is_map = EINA_FALSE;
   if (enlil_data->map) elm_map_bubbles_close(enlil_data->map->map);
}

static void
_map_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item)
{
   Enlil_Data *enlil_data = data;
   enlil_data->list_left->is_map = EINA_TRUE;
}

static void
_menu_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item)
{
   Enlil_Data *enlil_data = data;
   enlil_data->list_left->is_map = EINA_FALSE;
   if (enlil_data->map) elm_map_bubbles_close(enlil_data->map->map);
}

void
library_set(const char *library_path)
{
   //close the current library
   main_menu_nolibrary_disabled_set(EINA_TRUE);

   photos_list_object_freeze(enlil_data->list_photo->o_list, EINA_TRUE);
   enlil_thumb_clear();
   slideshow_hide();
   slideshow_clear();

   enlil_netsync_login_failed_cb_set(netsync_login_failed_cb, NULL);
   enlil_netsync_job_start_cb_set(netsync_job_start_cb, NULL);
   enlil_netsync_job_done_cb_set(netsync_job_done_cb, NULL);
   enlil_netsync_job_add_cb_set(netsync_job_add_cb, NULL);
   enlil_netsync_job_error_cb_set(netsync_job_error_cb, NULL);

   enlil_netsync_disconnect();
   if (enlil_data->sync) enlil_sync_free(&enlil_data->sync);
   if (enlil_data->load) enlil_load_free(&enlil_data->load);
   if (enlil_data->library)
   {
      enlil_library_free(&enlil_data->library);
   }

   enlil_data->sync = NULL;
   enlil_data->load = NULL;
   enlil_data->library = NULL;

   photos_list_object_freeze(enlil_data->list_photo->o_list, EINA_FALSE);
   elm_label_label_set(enlil_data->list_photo->lbl_nb_albums_photos, "");
   list_left_data_set(enlil_data->list_left, enlil_data);
   //

   if (library_path == NULL) return;

   //open the new library
   main_menu_nolibrary_disabled_set(EINA_FALSE);

   //
   Enlil_Library *library = enlil_library_new(monitor_album_new_cb,
                                              monitor_album_delete_cb,
                                              monitor_enlil_delete_cb,
                                              monitor_photo_new_cb,
                                              monitor_photo_delete_cb,
                                              monitor_photo_update_cb,
                                              collection_new_cb,
                                              collection_delete_cb,
                                              collection_album_new_cb,
                                              collection_album_delete_cb,
                                              tag_new_cb, tag_delete_cb,
                                              tag_photo_new_cb,
                                              tag_photo_delete_cb, enlil_data);
   enlil_library_path_set(library, library_path);
   enlil_library_eet_path_save(library);
   enlil_data->library = library;
   //

   //
   Enlil_Sync *sync = enlil_sync_new(enlil_library_path_get(library),
                                     sync_album_new_cb, sync_album_update_cb,
                                     sync_album_disappear_cb,
                                     sync_photo_new_cb, sync_photo_update_cb,
                                     sync_photo_disappear_cb, sync_done_cb,
                                     sync_start_cb, sync_error_cb, enlil_data);
   enlil_library_sync_set(library, sync);
   enlil_data->sync = sync;
   //

   //
   Enlil_Load *load = enlil_load_new(library, load_album_done_cb, load_done_cb,
                                     load_error_cb, enlil_data);
   enlil_library_monitor_start(library);
   enlil_data->load = load;

   loading_status_show(enlil_data, D_("  Loading ..."), EINA_TRUE);

   photos_list_object_freeze(enlil_data->list_photo->o_list, EINA_TRUE);
   enlil_load_run(load);
   //

   //netsync
   enlil_netsync_account_set(
                             enlil_library_netsync_host_get(enlil_data->library),
                             enlil_library_netsync_path_get(enlil_data->library),
                             enlil_library_netsync_account_get(
                                                               enlil_data->library),
                             enlil_library_netsync_password_get(
                                                                enlil_data->library));
   //

   //version increase
   enlil_library_album_version_header_increase_cb_set(
                                                      enlil_data->library,
                                                      album_version_header_increase_cb,
                                                      library);
   enlil_library_photo_version_header_increase_cb_set(
                                                      enlil_data->library,
                                                      photo_version_header_increase_cb,
                                                      library);
   enlil_library_photo_version_tags_increase_cb_set(
                                                    enlil_data->library,
                                                    photo_version_tags_increase_cb,
                                                    library);
   //

   //the background
   Enlil_String *s;
   Eet_Data_Descriptor *edd;
   char buf[PATH_MAX];
   edd = enlil_string_edd_new();
   snprintf(buf, PATH_MAX, "%s %s", APP_NAME " background",
            enlil_library_path_get(enlil_data->library));
   s = enlil_eet_app_data_load(edd, buf);
   eet_data_descriptor_free(edd);
   if (s)
   {
      enlil_win_bg_set(enlil_data->win, s->string);
      eina_stringshare_del(s->string);
      FREE(s);
   }
   else
      enlil_win_bg_set(enlil_data->win, NULL);
   //
   main_menu_loading_disable_set(1);
}

static double _panes_size = 0;
static void
_panes_clicked_double(void *data, Evas_Object *obj, void *event_info)
{
   if (elm_panes_content_left_size_get(obj) > 0)
   {
      _panes_size = elm_panes_content_left_size_get(obj);
      elm_panes_content_left_size_set(obj, 0.0);
   }
   else
      elm_panes_content_left_size_set(obj, _panes_size);
}

int
elm_main(int argc, char **argv)
{
   Evas_Object *panels, *ly, *edje;
   unsigned char exit_option = 0;
   char *library_path = NULL;
   int w = 1024, h = 768;
   char *size = NULL;
   char *theme = NULL;

   enlil_init();
   ecore_file_init();

   LOG_DOMAIN = eina_log_domain_register("Enki", "\033[34;1m");
   Theme = THEME_BIG;

   //ecore_getopt
   Ecore_Getopt_Value values[] =
      { ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_STR(library_path), ECORE_GETOPT_VALUE_STR(theme),
        ECORE_GETOPT_VALUE_STR(size), ECORE_GETOPT_VALUE_BOOL(exit_option), };
   ecore_app_args_set(argc, (const char **) argv);
   int nonargs = ecore_getopt_parse(&options, values, argc, argv);
   if (nonargs < 0)
      return 1;
   else if (nonargs != argc)
   {
      fputs("Invalid non-option argument", stderr);
      ecore_getopt_help(stderr, &options);
      return 1;
   }

   if (exit_option) return 0;
   //


   if (theme)
   {
      Theme = theme;
      LOG_INFO("Use custom theme %s", theme);
   }
   else
   {
      //get the theme from the conf file
      Enlil_String *s;
      Eet_Data_Descriptor *edd;
      char buf[PATH_MAX];
      edd = enlil_string_edd_new();
      snprintf(buf, PATH_MAX, "%s", APP_NAME " theme");
      s = enlil_eet_app_data_load(edd, buf);
      eet_data_descriptor_free(edd);
      if (s)
      {
         Theme = eina_stringshare_add(s->string);
         eina_stringshare_del(s->string);
         FREE(s);
      }
      //

      if (!ecore_file_exists(Theme))
      {
         eina_stringshare_del(Theme);
Theme      = THEME_BIG;
   }
}

elm_theme_extension_add(NULL, Theme);
elm_theme_overlay_add(NULL, Theme);

//
enlil_data = calloc(1, sizeof(Enlil_Data));
//

//
Enlil_Win *win = enlil_win_new();
enlil_data->win = win;
evas_object_smart_callback_add(win->win, "delete,request", close_cb, NULL);

//
ly = elm_layout_add(win->win);
elm_layout_file_set(ly, Theme, "main");
evas_object_size_hint_weight_set(ly, -1.0, -1.0);
evas_object_size_hint_align_set(ly, -1.0, -1.0);
elm_win_resize_object_add(win->win, ly);
evas_object_show(ly);

edje = elm_layout_edje_get(ly);
global_object = edje;
//

Evas_Object *menu = edje_object_part_external_object_get(edje, "object.menu");
enlil_data->tabpanel = tabpanel_add_with_edje(edje, menu);

panels = tabpanel_panels_obj_get(enlil_data->tabpanel);
evas_object_size_hint_weight_set(panels, 1.0, 1.0);
evas_object_size_hint_align_set(panels, -1.0, -1.0);
evas_object_show(panels);
edje_object_part_swallow(edje, "object.pages", panels);

//
ly = elm_layout_add(win->win);
elm_layout_file_set(ly, Theme, "main_page");
evas_object_size_hint_weight_set(ly, -1.0, -1.0);
evas_object_size_hint_align_set(ly, -1.0, -1.0);
evas_object_show(ly);

edje = elm_layout_edje_get(ly);
main_panel_object = edje;
evas_object_smart_callback_add(edje, "clicked,double", _panes_clicked_double, enlil_data);
enlil_data->library_item =
tabpanel_item_add(enlil_data->tabpanel, D_("Library"), ly, _tabpanel_select_page1_cb, enlil_data);

List_Left *list_album = list_left_new(edje);
list_left_data_set(list_album, enlil_data);

//
main_menu_new(edje);

if(!edje_object_part_external_object_get(edje, "object.menu.bt.preferences"))
tp_menu = tabpanel_item_add_with_signal(list_album->tb_liste_map, D_("Menu"), edje, "main_panel,menu,show", _menu_select_cb, enlil_data);

List_Photo *list_photo = list_photo_new(edje);
list_photo_data_set(list_photo, enlil_data);
edje_object_part_swallow(edje, "object.photos.swallow", list_photo->bx);
tp_list_photo = tabpanel_item_add_with_signal(list_album->tb_liste_map, D_("Photos"), edje, "main_panel,photos,show", _photos_list_select_cb, enlil_data);

Map *map = map_new(edje);
enlil_data->map = map;
tabpanel_item_add_with_signal(list_album->tb_liste_map, D_("Map"), edje, "main_panel,map,show", _map_select_cb, enlil_data);

if(tp_menu)
tabpanel_item_select(tp_menu);
else if(tp_list_photo)
tabpanel_item_select(tp_list_photo);
//

//
enlil_data->dl = download_new(win->win);
enlil_data->ul = upload_new(win->win);
//

//the media player
Enlil_String *s;
Eet_Data_Descriptor *edd;
edd = enlil_string_edd_new();
s = enlil_eet_app_data_load(edd, APP_NAME " media_player");
eet_data_descriptor_free(edd);
if(s)
{
   media_player = s->string;
   FREE(s);
}
else
media_player = eina_stringshare_add("vlc");
//

main_menu_nolibrary_disabled_set(EINA_TRUE);
if(library_path)
library_set(library_path);

evas_object_resize(win->win, w, h);
evas_object_show(win->win);

elm_run();

enlil_file_manager_flush();

enlil_shutdown();
eina_log_domain_unregister(LOG_DOMAIN);
elm_shutdown();

return 0;
}

void
sync_status_show(Enlil_Data *enlil_data, const char *msg, Eina_Bool loading)
{
   if (loading)
      edje_object_signal_emit(main_panel_object, "status,sync,show,loading", "");
   else
      edje_object_signal_emit(main_panel_object, "status,sync,show,done", "");

   Evas_Object *lbl =
            edje_object_part_external_object_get(main_panel_object,
                                                 "object.status.sync");
   if (lbl)
   {
      elm_label_label_set(lbl, msg);
   }
}

void
loading_status_show(Enlil_Data *enlil_data, const char *msg, Eina_Bool loading)
{
   if (loading)
      edje_object_signal_emit(main_panel_object, "status,loading,show,loading",
                              "");
   else
      edje_object_signal_emit(main_panel_object, "status,loading,show,done", "");

   Evas_Object *lbl =
            edje_object_part_external_object_get(main_panel_object,
                                                 "object.status.loading");
   if (lbl)
   {
      elm_label_label_set(lbl, msg);
   }
}

void
enlil_album_data_free(Enlil_Album *album, void *_data)
{
   Enlil_Album_Data *data = _data;
   Enlil_Data *enlil_data = data->enlil_data;
   Enlil_NetSync_Job *job;

   if (data->import_list_album_item) elm_genlist_item_del(
                                                          data->import_list_album_item);
   if (data->photo_move_album_list_album_item) elm_genlist_item_del(
                                                                    data->photo_move_album_list_album_item);

   list_left_remove(enlil_data->list_left, album);
   photos_list_object_header_del(data->list_photo_item);

   EINA_LIST_FREE(data->netsync.jobs, job)
   {
      enlil_netsync_job_del(job);
   }

   free(data);
}

void
enlil_photo_data_free(Enlil_Photo *photo, void *_data)
{
   Enlil_Photo_Data *data = _data;
   Slideshow_Item *item;
   //Enlil_Data *enlil_data = data->enlil_data;
   Enlil_NetSync_Job *job;

   enlil_thumb_photo_clear(photo);
   if (data->list_photo_item) photos_list_object_item_del(data->list_photo_item);
   if (data->slideshow_item) elm_slideshow_item_del(data->slideshow_item);
   if (data->panel_image) panel_image_free(&(data->panel_image));
   if (data->marker) map_photo_remove(enlil_data->map, photo);
   if (data->exif_job) enlil_exif_job_del(data->exif_job);
   if (data->iptc_job) enlil_iptc_job_del(data->iptc_job);

   EINA_LIST_FREE(data->slideshow_object_items, item)
   {
      slideshow_object_item_del(item);
   }

   EINA_LIST_FREE(data->netsync.jobs, job)
   {
      enlil_netsync_job_del(job);
   }

   free(data);
}

void
enlil_collection_data_free(Enlil_Collection *col, void *_data)
{
   Enlil_Collection_Data *data = _data;
   elm_genlist_item_del(data->list_col_item);
   data->list_col_item = NULL;
   free(data);
}

void
enlil_tag_data_free(Enlil_Tag *tag, void *_data)
{
   Enlil_Tag_Data *data = _data;
   elm_genlist_item_del(data->list_tag_item);
   data->list_tag_item = NULL;
   free(data);
}

void
enlil_geocaching_data_free(Enlil_Geocaching *gp, void *_data)
{
   Geocaching_Data *data = _data;

   if (data->panel_geocaching) panel_geocaching_free(&(data->panel_geocaching));

   if (data->marker) map_geocaching_remove(enlil_data->map, gp);
   free(data);
}

const char *
album_netsync_edje_signal_get(Enlil_Album *album)
{
   ASSERT_RETURN(album != NULL);
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   Eina_List *l;
   Enlil_Photo *photo;

   if (!album_data) return "uptodate";

   if (album_data->netsync.album_netsync_notuptodate
            || album_data->netsync.album_notinnetsync
            || album_data->netsync.album_local_notuptodate
            || album_data->netsync.photos_notinlocal) return "update";

   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      if (photo_data &&
               (photo_data->netsync.state != PHOTO_NETSYNC_NONE
                        || photo_data->netsync.state_tags != PHOTO_NETSYNC_NONE))
      return "update";
   }

   return "uptodate";
}

const char *
photo_netsync_edje_signal_get(Photo_NetSync_Enum e)
{
   switch (e)
   {
   case PHOTO_NETSYNC_NONE:
      return "uptodate";

   case PHOTO_NETSYNC_NOTUPTODATE:
      return "update";

   case PHOTO_NETSYNC_NETSYNCNOTUPTODATE:
      return "update";

   case PHOTO_NETSYNC_NOTINNETSYNC:
      return "update";

   case PHOTO_NETSYNC_TAGS_NETSYNCNOTUPTODATE:
      return "update";

   case PHOTO_NETSYNC_TAGS_NOTUPTODATE:
      return "update";
   }
   return NULL;
}

void
select_list_photo()
{
   tabpanel_item_select(tp_list_photo);
   edje_object_signal_emit(main_panel_object, "main_panel,photos,show", "");
   edje_object_signal_emit(global_object, "main_panel,photos,show", "");
}

void
select_menu()
{
   tabpanel_item_select(tp_menu);
   edje_object_signal_emit(main_panel_object, "main_panel,menu,show", "");
   edje_object_signal_emit(global_object, "main_panel,menu,show", "");
}

ELM_MAIN()
