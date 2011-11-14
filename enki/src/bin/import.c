#include "main.h"
#include "evas_object/photos_list_object.h"
#include "evas_object/photo_object.h"

static Evas_Object *inwin, *inwin2, *fs, *photos, *gl_albums, *bt_import,
         *bt_close, *pb, *radio_copy, *radio_move;
static PL_Header_Item *header;
static PL_Header_Item_Class itc_header;
static PL_Child_Item_Class itc_child;
static Enlil_Album *album;

//list of Enlil_Photo *
static Eina_List *photos_data = NULL;
static Eina_List *current_import = NULL;
static int nb_imports;

static void
_fs_selected_cb(void *data, Evas_Object *obj, void *event_info);
static Evas_Object *
_icon_get(const void *data, Evas_Object *obj);

static void
_import_thumb_done_cb(void *data, Enlil_Photo *photo, const char *file);
static void
_import_thumb_error_cb(void *data, Enlil_Photo *photo);

void
_import_photo_data_free(Enlil_Photo *photo, void *_data);

static void
_bt_select_all_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_unselect_all_cb(void *data, Evas_Object *obj, void *event_info);

static Elm_Genlist_Item_Class itc_album;
static char *
_gl_album_label_get(void *data, Evas_Object *obj, const char *part);
static void
_gl_album_sel(void *data, Evas_Object *obj, void *event_info);
static void
_bt_album_new_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_import_cb(void *data, Evas_Object *obj, void *event_info);

static void
_import_next(void *data, Ecore_Thread *thread);
static void
_import_thread(void *data, Ecore_Thread *thread);
static void
_bt_close_cb(void *data, Evas_Object *obj, void *event_info);

typedef struct _Import_Photo_Data
{
   Enlil_Photo *photo;
   PL_Child_Item *list_photo_item;
   Eina_Bool copy_done :1;
} _Import_Photo_Data;

Evas_Object *
import_new(Evas_Object *win)
{
   Evas_Object *fr, *bt, *bx, *bx2, *vbox, *gl, *tb, *tb2;
   Eina_List *l;
   Enlil_Album * album;

   //create inwin & file selector
   inwin = elm_win_inwin_add(win);
   evas_object_show(inwin);

   tb = elm_table_add(inwin);
   evas_object_size_hint_weight_set(tb, -1.0, 0.0);
   evas_object_size_hint_align_set(tb, 1.0, 1.0);
   evas_object_show(tb);
   elm_win_inwin_content_set(inwin, tb);

   //
   fr = elm_frame_add(inwin);
   elm_frame_label_set(fr, D_("Select the photos to import"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);
   evas_object_show(fr);
   elm_table_pack(tb, fr, 0, 0, 10, 1);

   tb2 = elm_table_add(inwin);
   evas_object_size_hint_weight_set(tb2, -1.0, 0.0);
   evas_object_size_hint_align_set(tb2, 1.0, 1.0);
   evas_object_show(tb2);
   elm_frame_content_set(fr, tb2);

   photos = photos_list_object_add(inwin);
   photos_list_object_multiselect_set(photos, EINA_TRUE);
   photos_list_object_sub_items_size_set(photos, 124, 96);
   photos_list_object_header_height_set(photos, 0);
   evas_object_size_hint_weight_set(photos, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(photos, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(photos);

   itc_header.func.icon_get = NULL;
   itc_child.func.icon_get = _icon_get;
   header = photos_list_object_item_header_append(photos, &itc_header, NULL);

   fs = elm_fileselector_add(inwin);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_folder_only_set(fs, EINA_TRUE);
   evas_object_smart_callback_add(fs, "directory,open", _fs_selected_cb, NULL);
   elm_fileselector_buttons_ok_cancel_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, getenv("HOME"));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(fs);
   elm_table_pack(tb2, fs, 0, 0, 1, 1);
   //

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
   evas_object_size_hint_align_set(vbox, -1.0, -1.0);
   evas_object_show(vbox);
   elm_table_pack(tb2, vbox, 1, 0, 2, 1);

   bx = elm_box_add(inwin);
   elm_box_horizontal_set(bx, 1);
   evas_object_size_hint_weight_set(bx, 1.0, 0.0);
   evas_object_size_hint_align_set(bx, -1.0, 0.0);
   evas_object_show(bx);
   elm_box_pack_end(vbox, bx);

   elm_box_pack_end(vbox, photos);
   //

   //
   bt = elm_button_add(inwin);
   elm_button_label_set(bt, D_("Select all"));
   evas_object_smart_callback_add(bt, "clicked", _bt_select_all_cb, NULL);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.0);
   elm_box_pack_end(bx, bt);

   bt = elm_button_add(inwin);
   elm_button_label_set(bt, D_("Unselect all"));
   evas_object_smart_callback_add(bt, "clicked", _bt_unselect_all_cb, NULL);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 0.0, 0.0);
   elm_box_pack_end(bx, bt);

   vbox = elm_box_add(inwin);
   evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
   evas_object_size_hint_align_set(vbox, -1.0, -1.0);
   evas_object_show(vbox);
   elm_table_pack(tb, vbox, 10, 0, 1, 1);

   fr = elm_frame_add(inwin);
   elm_frame_label_set(fr, D_("Select an album"));
   evas_object_size_hint_weight_set(fr, 1.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);
   evas_object_show(fr);
   elm_box_pack_end(vbox, fr);

   bx2 = elm_box_add(inwin);
   evas_object_size_hint_weight_set(bx2, 1.0, 1.0);
   evas_object_size_hint_align_set(bx2, -1.0, -1.0);
   evas_object_show(bx2);
   elm_frame_content_set(fr, bx2);

   gl = elm_genlist_add(win);
   gl_albums = gl;
   elm_genlist_horizontal_mode_set(gl, ELM_LIST_SCROLL);
   evas_object_size_hint_weight_set(gl, 1.0, 1.0);
   evas_object_size_hint_align_set(gl, -1.0, -1.0);
   evas_object_show(gl);
   elm_box_pack_end(bx2, gl);

   itc_album.item_style = "default";
   itc_album.func.label_get = _gl_album_label_get;
   itc_album.func.content_get = NULL;
   itc_album.func.state_get = NULL;
   itc_album.func.del = NULL;

   EINA_LIST_FOREACH(enlil_library_albums_get(enlil_data->library), l, album)
   {
      Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
      album_data->import_list_album_item =
      elm_genlist_item_append(gl_albums, &itc_album,
               album, NULL, ELM_GENLIST_ITEM_NONE, _gl_album_sel,
               album);
   }

   bt = elm_button_add(inwin);
   bt_import = bt;
   elm_button_label_set(bt, D_("Create a new album"));
   evas_object_smart_callback_add(bt, "clicked", _bt_album_new_cb, NULL);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, 1.0);
   elm_box_pack_end(bx2, bt);

   //
   bx2 = elm_box_add(inwin);
   elm_box_horizontal_set(bx2, EINA_TRUE);
   evas_object_size_hint_weight_set(bx2, -1.0, 0.0);
   evas_object_size_hint_align_set(bx2, 0.0, 1.0);
   evas_object_show(bx2);
   elm_box_pack_end(vbox, bx2);

   radio_copy = elm_radio_add(win);
   elm_radio_state_value_set(radio_copy, 0);
   evas_object_size_hint_weight_set(radio_copy, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(radio_copy, EVAS_HINT_FILL, 0.5);
   elm_radio_label_set(radio_copy, "Copy photos");
   elm_box_pack_end(bx2, radio_copy);
   evas_object_show(radio_copy);

   radio_move = elm_radio_add(win);
   elm_radio_state_value_set(radio_move, 1);
   evas_object_size_hint_weight_set(radio_move, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(radio_move, EVAS_HINT_FILL, 0.5);
   elm_radio_group_add(radio_move, radio_copy);
   elm_radio_label_set(radio_move, "Move photos");
   elm_box_pack_end(bx2, radio_move);
   evas_object_show(radio_move);
   //

   bx2 = elm_box_add(inwin);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, -1.0, 0.0);
   evas_object_size_hint_align_set(bx2, 1.0, 1.0);
   evas_object_show(bx2);
   elm_box_pack_end(vbox, bx2);

   bt = elm_button_add(inwin);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_cancel_cb, NULL);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 1.0, 1.0);
   elm_box_pack_end(bx2, bt);

   bt = elm_button_add(inwin);
   bt_import = bt;
   elm_button_label_set(bt, D_("Import"));
   elm_object_disabled_set(bt, 1);
   evas_object_smart_callback_add(bt, "clicked", _bt_import_cb, NULL);
   evas_object_show(bt);
   evas_object_size_hint_weight_set(bt, 0.0, 1.0);
   evas_object_size_hint_align_set(bt, 1.0, 1.0);
   elm_box_pack_end(bx2, bt);

   return inwin;
}

static void
_fs_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l, *_l;
   char *file;
   Enlil_Photo *photo;
   char buf[PATH_MAX];

   LOG_INFO("Selected directory : %s", (char *)event_info);

   //delete all list
   EINA_LIST_FREE(photos_data, photo)
   enlil_photo_free(&photo);
   //

   l = ecore_file_ls(event_info);

EINA_LIST_FOREACH   (l, _l, file)
   {
      snprintf(buf, PATH_MAX, "%s/%s", (char *)event_info, file);
      if(enlil_photo_is(buf)
               || enlil_video_is(buf)
               || enlil_gpx_is(buf))
      {
         photo = enlil_photo_new();
         enlil_photo_path_set(photo, event_info);
         enlil_photo_file_name_set(photo, file);
         enlil_photo_eet_save_set(photo, 0);

         char *name = ecore_file_strip_ext(file);
         enlil_photo_name_set(photo, name);
         free(name);

         if(enlil_photo_is(buf))
         enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_PHOTO);
         else if(enlil_video_is(buf))
         enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_VIDEO);
         else if(enlil_gpx_is(buf))
         enlil_photo_type_set(photo, ENLIL_PHOTO_TYPE_GPX);

         _Import_Photo_Data *photo_data = calloc(1, sizeof(_Import_Photo_Data));
         photo_data->photo = photo;
         enlil_photo_user_data_set(photo, photo_data, _import_photo_data_free);

         photos_data = eina_list_append(photos_data, photo);
         photo_data->list_photo_item =
         photos_list_object_item_append(photos, &itc_child, header, photo);
      }
   }

   EINA_LIST_FREE(l, file)
   FREE(file);
}

static Evas_Object *
_icon_get(const void *data, Evas_Object *obj)
{
   const char *s;
   Enlil_Photo *photo = (Enlil_Photo *) data;
   _Import_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   Evas_Object *o = photo_object_add(obj);
   photo_object_theme_file_set(o, Theme, "photo/import");

   if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO
            || enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO)
   {
      s = enlil_thumb_photo_get(photo, Enlil_THUMB_FDO_NORMAL,
                                _import_thumb_done_cb, _import_thumb_error_cb,
                                NULL);

      if (s)
         photo_object_file_set(o, s, NULL);
      else
         photo_object_progressbar_set(o, EINA_TRUE);

      if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO) photo_object_camera_set(
                                                                                         o,
                                                                                         EINA_TRUE);
   }
   else //GPX
   {
      photo_object_gpx_set(o);
   }

   photo_object_text_set(o, enlil_photo_name_get(photo));

   if (photo_data->copy_done) photo_object_done_set(o, EINA_TRUE);

   evas_object_show(o);

   return o;
}

static void
_bt_select_all_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l;
   Enlil_Photo *photo;
   _Import_Photo_Data *photo_data;

EINA_LIST_FOREACH(photos_data, l, photo)
{
   photo_data = enlil_photo_user_data_get(photo);
   photos_list_object_item_selected_set(photo_data->list_photo_item, EINA_TRUE);
}
}

static void
_bt_unselect_all_cb(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l;
   Enlil_Photo *photo;
   _Import_Photo_Data *photo_data;

EINA_LIST_FOREACH(photos_data, l, photo)
{
   photo_data = enlil_photo_user_data_get(photo);
   photos_list_object_item_selected_set(photo_data->list_photo_item, EINA_FALSE);
}
}

static void
_import_thumb_done_cb(void *data, Enlil_Photo *photo, const char *file)
{
   _Import_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photos_list_object_item_update(photo_data->list_photo_item);
}

static void
_import_thumb_error_cb(void *data, Enlil_Photo *photo)
{
}

static char *
_gl_album_label_get(void *data, Evas_Object *obj, const char *part)
{
   Enlil_Album *album = (Enlil_Album *) data;

   return strdup(enlil_album_name_get(album));
}

static void
_gl_album_sel(void *data, Evas_Object *obj, void *event_info)
{
   //Enlil_Album *album = (Enlil_Album *)data;
   //Enlil_Album_Data *enlil_album_data = (Enlil_Album_Data*) enlil_album_user_data_get(album);

   elm_object_disabled_set(bt_import, 0);
}

static void
_bt_album_new_cb(void *data, Evas_Object *obj, void *event_info)
{
   inwin_album_new_new(NULL, NULL);
}

static void
_bt_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Enlil_Photo *photo;

   EINA_LIST_FREE(photos_data, photo)
   enlil_photo_free(&photo);

   evas_object_del(inwin);
   if (inwin2) evas_object_del(inwin2);
   inwin2 = NULL;
   inwin = NULL;
}

static void
_bt_import_cb(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *tb, *bt, *fr, *_pb;
   Enlil_Photo *photo;
   Eina_List *l, *l_next, *l2;
   PL_Child_Item *pl_item;
   Eina_List *items;
   Elm_Genlist_Item *item = elm_genlist_selected_item_get(gl_albums);
   ASSERT_RETURN_VOID(item != NULL);
   album = (Enlil_Album *) elm_genlist_item_data_get(item);

   items = photos_list_object_selected_get(photos);
   EINA_LIST_FOREACH_SAFE(photos_data, l, l_next, photo)
   {
      Eina_Bool find = EINA_FALSE;
      EINA_LIST_FOREACH(items, l2, pl_item)
      {
         if(photos_list_object_item_data_get(pl_item) == photo)
         {
            find = EINA_TRUE;
            break;
         }
      }
      if(!find)
      {
         photos_data = eina_list_remove(photos_data, photo);
         enlil_photo_free(&photo);
      }
   }
   photos_list_object_top_goto(photos);

   inwin2 = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin2, EINA_FALSE);
   elm_notify_orient_set(inwin2, ELM_NOTIFY_ORIENT_TOP);
   elm_notify_parent_set(inwin2, enlil_data->win->win);
   evas_object_show(inwin2);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr, D_("Copying photos"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, -1.0, -1.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(tb, -1.0, 1.0);
   evas_object_size_hint_align_set(tb, 1.0, -1.0);
   evas_object_show(tb);

   pb = elm_progressbar_add(enlil_data->win->win);
   evas_object_size_hint_weight_set(pb, 1.0, 0.0);
   evas_object_size_hint_align_set(pb, -1.0, 0.0);
   evas_object_size_hint_min_set(pb, 150, -1);
   evas_object_show(pb);
   elm_table_pack(tb, pb, 0, 0, 5, 1);

   _pb = elm_progressbar_add(enlil_data->win->win);
   elm_object_style_set(_pb, "wheel");
   elm_progressbar_label_set(_pb, "");
   elm_progressbar_pulse(_pb, EINA_TRUE);
   evas_object_size_hint_weight_set(_pb, 1.0, 0.0);
   evas_object_size_hint_align_set(_pb, -1.0, 0.5);
   evas_object_show(_pb);
   elm_table_pack(tb, _pb, 6, 0, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   bt_close = bt;
   elm_button_label_set(bt, D_("Close"));
   evas_object_size_hint_weight_set(bt, -1.0, 0.0);
   evas_object_size_hint_align_set(bt, 1.0, 1.0);
   evas_object_show(bt);
   evas_object_smart_callback_add(bt, "clicked", _bt_close_cb, NULL);
   elm_table_pack(tb, bt, 5, 1, 2, 1);
   elm_object_disabled_set(bt, 1);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin2, fr);

   enlil_album_monitor_stop(album);
   nb_imports = -1;
   _import_next(NULL, NULL);
}

static void
_import_next(void *data, Ecore_Thread *thread)
{
   char buf[PATH_MAX];

   if (!current_import)
      current_import = photos_data;
   else
   {
      Enlil_Photo *photo = eina_list_data_get(current_import);
      _Import_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
      photo_data->copy_done = EINA_TRUE;
      const Evas_Object *o =
               photos_list_object_item_object_get(photo_data->list_photo_item);
      if (o) photo_object_done_set((Evas_Object *) o, EINA_TRUE);
      current_import = eina_list_next(current_import);
      if (current_import)
      {
         photo = eina_list_data_get(current_import);
         photo_data = enlil_photo_user_data_get(photo);
         photos_list_object_item_bring_in(photo_data->list_photo_item);
      }
   }
   nb_imports++;

   snprintf(buf, PATH_MAX, D_("%d / %d"), nb_imports,
            eina_list_count(photos_data));

   elm_progressbar_label_set(pb, buf);
   elm_progressbar_value_set(pb, nb_imports
            / (float) eina_list_count(photos_data));
   evas_render(evas_object_evas_get(pb));

   if (!current_import)
   {
      //done
      elm_object_disabled_set(bt_close, 0);
      enlil_album_monitor_start(album);
      monitor_album_update_cb(enlil_data, enlil_data->library, album);
   }
   else
      ecore_thread_run(_import_thread, _import_next, NULL, current_import);
}

static void
_import_thread(void *data, Ecore_Thread *thread)
{
   char path[PATH_MAX], path2[PATH_MAX], path3[PATH_MAX];
   Enlil_Photo *photo = eina_list_data_get(data);

   snprintf(path, PATH_MAX, "%s/%s", enlil_album_path_get(album),
            enlil_album_file_name_get(album));
   snprintf(path2, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
            enlil_photo_file_name_get(photo));
   snprintf(path3, PATH_MAX, "%s/%s", path, enlil_photo_file_name_get(photo));
   LOG_INFO("Copy %s into %s", path2, path3);
   if (!elm_radio_value_get(radio_copy))
      ecore_file_cp(path2, path3);
   else
      ecore_file_mv(path2, path3);
}

static void
_bt_close_cb(void *data, Evas_Object *obj, void *event_info)
{
   _bt_cancel_cb(NULL, NULL, NULL);
}

void
_import_photo_data_free(Enlil_Photo *photo, void *_data)
{
   Eina_List *l;
   Enlil_Album * album;
   Enlil_Album_Data *album_data;
   _Import_Photo_Data *data = _data;
   //Enlil_Data *enlil_data = data->enlil_data;

   EINA_LIST_FOREACH(enlil_library_albums_get(enlil_data->library), l, album)
   {
      album_data = enlil_album_user_data_get(album);
      album_data->import_list_album_item = NULL;
   }

   enlil_thumb_photo_clear(photo);
   photos_list_object_item_del(data->list_photo_item);

   free(data);
}

void
import_album_new(Enlil_Album *album)
{
   if (!inwin) return;

   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
   album_data->import_list_album_item
            = elm_genlist_item_append(gl_albums, &itc_album, album, NULL,
                                      ELM_GENLIST_ITEM_NONE, _gl_album_sel,
                                      album);
}

