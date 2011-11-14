#include "main.h"

static Elm_Genlist_Item_Class itc_theme;
static char *
_gl_theme_label_get(void *data, Evas_Object *obj, const char *part);
static void
_gl_theme_select_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_tag_new_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_album_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_library_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_library_delete_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_collection_new_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_collection_new_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_album_new_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_album_new_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_photo_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_photo_delete_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_album_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_album_delete_apply_cb(void *data, Evas_Object *obj, void *event_info);

static Elm_Genlist_Item_Class itc_album;
static char *
_gl_album_label_get(void *data, Evas_Object *obj, const char *part);
static void
_gl_album_sel(void *data, Evas_Object *obj, void *event_info);

static void
_bt_photo_move_album_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_photo_move_album_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_photo_save_dont_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_photo_save_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_photo_save_save_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_album_rename_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_album_rename_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_save_as_file_exists_cancel_cb(void *data, Evas_Object *obj,
                                  void *event_info);
static void
         _bt_save_as_file_exists_apply_cb(void *data, Evas_Object *obj,
                                          void *event_info);

static void
_bt_preferences_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_preferences_apply_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_login_failed_close_cb(void *data, Evas_Object *obj, void *event_info);

// TODO: rewrite with edje external
Inwin *
inwin_save_as_file_exists_new(Inwin_Del del_cb, Inwin_Apply apply_cb,
                              void *data, const char *file)
{
   Evas_Object *hbox, *fr, *bt, *tb, *lbl;
   char buf[PATH_MAX];

   ASSERT_RETURN(file != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_SAVE_AS_FILE_EXISTS;
   inwin->del_cb = del_cb;
   inwin->apply_cb = apply_cb;
   inwin->data = data;
   inwin->file = eina_stringshare_add(buf);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr, D_("Erase a photo"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, 0.0, 0.0);
   evas_object_show(tb);

   snprintf(
            buf,
            PATH_MAX,
            D_("You are about to erase the photo <br>%s. Do you want to continue ?"),
            file);
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 0.0, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, -1.0, -1.0);
   evas_object_show(lbl);
   elm_table_pack(tb, lbl, 0, 0, 2, 1);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 1, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked",
                                  _bt_save_as_file_exists_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Apply"));
   evas_object_smart_callback_add(bt, "clicked",
                                  _bt_save_as_file_exists_apply_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *
inwin_photo_save_new(Inwin_Del del_cb, Inwin_Apply apply_cb,
                     Inwin_Close close_cb, void *data, Enlil_Photo *photo)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *bt, *tb;
   char buf[PATH_MAX];

   ASSERT_RETURN(apply_cb != NULL);
   ASSERT_RETURN(close_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(photo != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PHOTO_SAVE;
   inwin->del_cb = del_cb;
   inwin->apply_cb = apply_cb;
   inwin->close_cb = close_cb;
   inwin->data = data;
   inwin->photo = photo;

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr, D_("Close a photo"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2, "");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);

   snprintf(buf, PATH_MAX, D_("Save the changes to image %s before closing?"),
            enlil_photo_file_name_get(photo));
   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Don't save"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_save_dont_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_save_cancel_cb,
                                  inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Save"));
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_save_save_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_notify_content_set(inwin->inwin, fr);

   return inwin;
}

//

Inwin *
inwin_library_delete_new(Enlil_Data *enlil_data)
{
   Evas_Object *lbl, *ly, *bt;

   ASSERT_RETURN(enlil_data != NULL);
   ASSERT_RETURN(enlil_data->library != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_LIBRARY_DELETE;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/library/delete");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   lbl = edje_object_part_external_object_get(edje,
                                              "object.win.library.delete.name");
   elm_label_label_set(lbl, enlil_library_path_get(enlil_data->library));

   bt
            = edje_object_part_external_object_get(edje,
                                                   "object.win.library.delete.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_library_delete_cancel_cb,
                                  inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.library.delete.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_library_delete_apply_cb,
                                  inwin);

   inwin->check
            = edje_object_part_external_object_get(edje,
                                                   "object.win.library.delete.delete_hard_drive");

   return inwin;
}

Inwin *
inwin_tag_new_new(Inwin_Del del_cb, void *data, Eina_List *photos)
{
   Evas_Object *bt, *ly;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(photos != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_TAG_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->photos = photos;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/tag/new");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   inwin->entry
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.tag.new.name");

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.tag.new.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_tag_new_cancel_cb, inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.tag.new.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_tag_new_apply_cb, inwin);

   return inwin;
}

Inwin *
inwin_album_tag_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *bt, *ly;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_TAG_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/album/tag/new");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   inwin->entry
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.tag.new.name");

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.tag.new.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_tag_new_cancel_cb, inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.tag.new.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_tag_new_apply_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_collection_new_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *bt, *ly;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_COLLECTION_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/album/collection/new");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   inwin->entry
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.collection.new.name");

   bt
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.collection.new.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_collection_new_cancel_cb,
                                  inwin);

   bt
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.collection.new.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_collection_new_apply_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_album_new_new(Inwin_Del del_cb, void *data)
{
   Evas_Object *bt, *ly;
   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_NEW;
   inwin->del_cb = del_cb;
   inwin->data = data;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/album/new");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   inwin->entry
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.new.name");

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.new.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_new_cancel_cb, inwin);

   bt
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.new.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_new_apply_cb, inwin);

   return inwin;
}

Inwin *
inwin_photo_delete_new(Evas_Object *win, Inwin_Del del_cb, void *data,
                       Eina_List *photos)
{
   Evas_Object *ly, *bt, *entry;
   char buf[PATH_MAX];
   char buf2[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;

   ASSERT_RETURN(photos != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PHOTO_DELETE;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->photos = photos;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/photo/delete");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   entry = edje_object_part_external_object_get(edje,
                                                "object.win.photo.delete.name");

   buf[0] = '\0';
   EINA_LIST_FOREACH(photos, l, photo)
   {
      snprintf(buf2, sizeof(buf), "%s - %s<br>", buf, enlil_photo_name_get(photo));
      strcpy(buf, buf2);
   }
   elm_scrolled_entry_entry_set(entry, buf);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.photo.delete.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_delete_cancel_cb,
                                  inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.photo.delete.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_delete_apply_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_album_delete_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *bt, *ly, *lbl;

   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_DELETE;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/album/delete");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   lbl = edje_object_part_external_object_get(edje,
                                              "object.win.album.delete.name");
   elm_label_label_set(lbl, enlil_album_name_get(album));

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.delete.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_delete_cancel_cb,
                                  inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.delete.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_delete_apply_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_album_rename_new(Inwin_Del del_cb, void *data, Enlil_Album *album)
{
   Evas_Object *lbl, *bt, *ly;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(album != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_ALBUM_RENAME;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->album = album;

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/album/rename");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   inwin->inwin = elm_notify_add(enlil_data->win->win);
   elm_notify_repeat_events_set(inwin->inwin, EINA_FALSE);
   elm_notify_orient_set(inwin->inwin, ELM_NOTIFY_ORIENT_TOP);
   evas_object_show(inwin->inwin);
   elm_notify_content_set(inwin->inwin, ly);

   inwin->entry
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.rename.name");

   lbl
            = edje_object_part_external_object_get(edje,
                                                   "object.win.album.rename.old_name");
   elm_label_label_set(lbl, enlil_album_name_get(album));

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.rename.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_rename_cancel_cb,
                                  inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.album.rename.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_album_rename_apply_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_photo_move_album_new(Inwin_Del del_cb, void *data, Eina_List *photos)
{
   Evas_Object *hbox, *fr, *fr2, *lbl, *bt, *tb, *gl;
   char buf[PATH_MAX], buf2[PATH_MAX], buf3[PATH_MAX];
   const Eina_List *l;
   Enlil_Album *album;
   Enlil_Photo *photo;

   ASSERT_RETURN(del_cb != NULL);
   ASSERT_RETURN(data != NULL);
   ASSERT_RETURN(photos != NULL);

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PHOTO_MOVE_ALBUM;
   inwin->del_cb = del_cb;
   inwin->data = data;
   inwin->photos = photos;

   inwin->inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin->inwin);

   fr = elm_frame_add(enlil_data->win->win);
   elm_frame_label_set(fr, D_("Move the photo into an album"));
   evas_object_size_hint_weight_set(fr, 1.0, 1.0);
   evas_object_size_hint_align_set(fr, 0.0, 0.0);
   evas_object_show(fr);

   tb = elm_table_add(enlil_data->win->win);
   elm_table_padding_set(tb, 10, 10);
   evas_object_size_hint_weight_set(tb, 1.0, 1.0);
   evas_object_size_hint_align_set(tb, -1.0, 0.0);
   evas_object_show(tb);

   *buf3 = '\0';
   EINA_LIST_FOREACH(photos, l, photo)
   {
      snprintf(buf2, PATH_MAX, "%s<br>%s", buf3, enlil_photo_name_get(photo));
      strncpy(buf3, buf2, PATH_MAX);
   }

   snprintf(buf, PATH_MAX,
            D_("Move %d photo(s): <br> %s <br><br>into an new album"),
            eina_list_count(photos), buf3);
   fr2 = elm_frame_add(enlil_data->win->win);
   elm_object_style_set(fr2, "outdent_top");
   elm_frame_label_set(fr2, "");
   evas_object_size_hint_weight_set(fr2, 1.0, 0.0);
   evas_object_size_hint_align_set(fr2, 0.5, 0.0);
   evas_object_show(fr2);
   elm_table_pack(tb, fr2, 0, 0, 2, 1);

   lbl = elm_label_add(enlil_data->win->win);
   elm_label_label_set(lbl, buf);
   evas_object_size_hint_weight_set(lbl, 1.0, 1.0);
   evas_object_size_hint_align_set(lbl, -1.0, 0.0);
   //elm_label_line_wrap_set(lbl, 1);
   evas_object_show(lbl);
   elm_frame_content_set(fr2, lbl);

   gl = elm_genlist_add(enlil_data->win->win);
   inwin->gl = gl;
   elm_genlist_horizontal_mode_set(gl, ELM_LIST_SCROLL);
   evas_object_size_hint_weight_set(gl, 1.0, 1.0);
   evas_object_size_hint_align_set(gl, -1.0, -1.0);
   evas_object_show(gl);
   elm_table_pack(tb, gl, 0, 1, 2, 1);

   itc_album.item_style = "default";
   itc_album.func.label_get = _gl_album_label_get;
   itc_album.func.content_get = NULL;
   itc_album.func.state_get = NULL;
   itc_album.func.del = NULL;

   EINA_LIST_FOREACH(enlil_library_albums_get(enlil_data->library), l, album)
   {
      Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
      album_data->inwin_photo_move_album = inwin;

      album_data->photo_move_album_list_album_item =
      elm_genlist_item_append(gl, &itc_album,
               album, NULL, ELM_GENLIST_ITEM_NONE, _gl_album_sel,
               album);
   }

   hbox = elm_box_add(enlil_data->win->win);
   elm_box_horizontal_set(hbox, 1);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, 0.0);
   evas_object_size_hint_align_set(hbox, 1.0, 0.0);
   evas_object_show(hbox);
   elm_table_pack(tb, hbox, 1, 2, 1, 1);

   bt = elm_button_add(enlil_data->win->win);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_smart_callback_add(bt, "clicked",
                                  _bt_photo_move_album_cancel_cb, inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   bt = elm_button_add(enlil_data->win->win);
   elm_object_disabled_set(bt, 1);
   elm_button_label_set(bt, D_("Apply"));
   inwin->bt_apply = bt;
   evas_object_smart_callback_add(bt, "clicked", _bt_photo_move_album_apply_cb,
                                  inwin);
   evas_object_size_hint_align_set(bt, 1.0, 0.0);
   evas_object_show(bt);
   elm_box_pack_end(hbox, bt);

   elm_frame_content_set(fr, tb);
   elm_win_inwin_content_set(inwin->inwin, fr);

   return inwin;
}

Inwin *
inwin_preferences_new()
{
   Evas_Object *bt, *ly, *lbl, *gl;
   Eina_List *l;
   char *str;

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_PREFERENCES;

   inwin->inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin->inwin);

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/preferences");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   elm_win_inwin_content_set(inwin->inwin, ly);

   inwin->entry
            = edje_object_part_external_object_get(edje,
                                                   "object.win.preferences.video_software");
   elm_scrolled_entry_entry_set(inwin->entry, media_player);

   //themes
   itc_theme.item_style = "default";
   itc_theme.func.label_get = _gl_theme_label_get;
   itc_theme.func.content_get = NULL;
   itc_theme.func.state_get = NULL;
   itc_theme.func.del = NULL;

   gl = edje_object_part_external_object_get(edje,
                                             "object.win.preferences.themes");

   inwin->themes = ecore_file_ls(PACKAGE_DATA_DIR "/themes/");

   EINA_LIST_FOREACH(inwin->themes, l, str)
   {
      if(!strcmp(str, "default_small.edj"))
      continue;

      Elm_Genlist_Item *item = elm_genlist_item_append(gl, &itc_theme, str, NULL, ELM_GENLIST_ITEM_NONE, _gl_theme_select_cb, str);

      int i, j = strlen(Theme) - 1;
      for(i = strlen(str) - 1; i>=0 && j >= 0; i--)
      {
         if(str[i] == Theme[j])
         {
            j--;
            if(i == 0 && j>0 && Theme[j]=='/')
            {
               elm_genlist_item_selected_set(item, EINA_TRUE);
               break;
            }
         }
         else
         break;
      }
   }
   //

   //library
   if (enlil_data->library)
   {
      edje_object_signal_emit(edje, "win,preference,library,selected", "");

      lbl
               = edje_object_part_external_object_get(edje,
                                                      "object.win.preferences.library.name");
      elm_label_label_set(lbl, enlil_library_path_get(enlil_data->library));

      inwin->entry2
               = edje_object_part_external_object_get(edje,
                                                      "object.win.preferences.library.netsync.account");
      elm_scrolled_entry_entry_set(
                                   inwin->entry2,
                                   enlil_library_netsync_account_get(
                                                                     enlil_data->library));

      inwin->entry3
               = edje_object_part_external_object_get(edje,
                                                      "object.win.preferences.library.netsync.password");
      elm_scrolled_entry_entry_set(
                                   inwin->entry3,
                                   enlil_library_netsync_password_get(
                                                                      enlil_data->library));

      inwin->entry4
               = edje_object_part_external_object_get(edje,
                                                      "object.win.preferences.library.netsync.host");
      elm_scrolled_entry_entry_set(
                                   inwin->entry4,
                                   enlil_library_netsync_host_get(
                                                                  enlil_data->library));

      inwin->entry5
               = edje_object_part_external_object_get(edje,
                                                      "object.win.preferences.library.netsync.path");
      elm_scrolled_entry_entry_set(
                                   inwin->entry5,
                                   enlil_library_netsync_path_get(
                                                                  enlil_data->library));
   }
   else
      edje_object_signal_emit(edje, "win,preference,library,no,selected", "");
   //

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.preferences.cancel");
   evas_object_smart_callback_add(bt, "clicked", _bt_preferences_cancel_cb,
                                  inwin);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.preferences.apply");
   evas_object_smart_callback_add(bt, "clicked", _bt_preferences_apply_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_login_failed_new()
{
   Evas_Object *bt, *ly, *lbl;

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_LOGIN_FAILED;

   inwin->inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin->inwin);

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/login_failed");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   elm_win_inwin_content_set(inwin->inwin, ly);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.login_failed.close");
   evas_object_smart_callback_add(bt, "clicked", _bt_login_failed_close_cb,
                                  inwin);

   return inwin;
}

Inwin *
inwin_netsync_error_new(const char *message)
{
   Evas_Object *bt, *ly, *lbl;

   Inwin *inwin = calloc(1, sizeof(Inwin));
   inwin->type = INWIN_LOGIN_FAILED;

   inwin->inwin = elm_win_inwin_add(enlil_data->win->win);
   evas_object_show(inwin->inwin);

   ly = elm_layout_add(enlil_data->win->win);
   elm_layout_file_set(ly, Theme, "win/netsync_error");
   Evas_Object *edje = elm_layout_edje_get(ly);
   evas_object_show(ly);

   elm_win_inwin_content_set(inwin->inwin, ly);

   lbl = edje_object_part_external_object_get(edje,
                                              "object.win.netsync_error.error");
   elm_label_label_set(lbl, message);

   bt = edje_object_part_external_object_get(edje,
                                             "object.win.netsync_error.close");
   evas_object_smart_callback_add(bt, "clicked", _bt_login_failed_close_cb,
                                  inwin);

   return inwin;
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
   Enlil_Album *album = (Enlil_Album *) data;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   if (album_data->inwin_photo_move_album) elm_object_disabled_set(
                                                                   album_data->inwin_photo_move_album->bt_apply,
                                                                   0);
}

static void
_bt_tag_new_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_scrolled_entry_entry_get(inwin->entry);
   if (s && strlen(s) > 0)
   {
      Eina_List *l;
      Enlil_Photo *photo;
EINA_LIST_FOREACH   (inwin->photos, l, photo)
   enlil_photo_tag_add(photo, s);
}

inwin_free(inwin);
}

static void
_bt_album_tag_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_scrolled_entry_entry_get(inwin->entry);
   const char *__s = eina_stringshare_add(s);
   if (s && strlen(s) > 0)
   {
      Enlil_Photo *photo;
      Enlil_Photo_Tag *photo_tag;
      Eina_List *l;
      const Eina_List *_l;
EINA_LIST_FOREACH   (enlil_album_photos_get(inwin->album), l, photo)
   {
      EINA_LIST_FOREACH(enlil_photo_tags_get(photo), _l, photo_tag)
      {
         if(photo_tag->name == __s)
         break;
      }

      if(!photo_tag)
      enlil_photo_tag_add(photo, __s);
   }
}

eina_stringshare_del(__s);
inwin_free(inwin);
}

static void
_bt_collection_new_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_collection_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_scrolled_entry_entry_get(inwin->entry);
   if (s && strlen(s) > 0) enlil_album_collection_add(inwin->album, s);

   inwin_free(inwin);
}

static void
_bt_album_new_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_album_new_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   const char *s = elm_scrolled_entry_entry_get(inwin->entry);
   if (s && strlen(s) > 0)
   {
      char buf[PATH_MAX];
      snprintf(buf, PATH_MAX, "%s/%s",
               enlil_library_path_get(enlil_data->library), s);
      ecore_file_mkdir(buf);
   }

   inwin_free(inwin);
}

static void
_bt_preferences_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   char *s;
   Inwin *inwin = data;

   EINA_LIST_FREE(inwin->themes, s)
   {
      FREE(s);
   }

   inwin_free(inwin);
}

static void
_bt_preferences_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   enlil_netsync_disconnect();

   const char *s = elm_scrolled_entry_entry_get(inwin->entry);
   if (s && strlen(s) > 0)
   {
      Eet_Data_Descriptor *edd;
      Enlil_String string;

      EINA_STRINGSHARE_DEL(media_player);
      media_player = eina_stringshare_add(s);

      string.string = eina_stringshare_add(s);
      edd = enlil_string_edd_new();

      enlil_eet_app_data_save(edd, APP_NAME " media_player", &string);
      eet_data_descriptor_free(edd);
      eina_stringshare_del(string.string);
   }

   if (enlil_data->library)
   {
      s = elm_scrolled_entry_entry_get(inwin->entry2);
      enlil_library_netsync_account_set(enlil_data->library, s);

      s = elm_scrolled_entry_entry_get(inwin->entry3);
      enlil_library_netsync_password_set(enlil_data->library, s);

      s = elm_scrolled_entry_entry_get(inwin->entry4);
      enlil_library_netsync_host_set(enlil_data->library, s);

      s = elm_scrolled_entry_entry_get(inwin->entry5);
      enlil_library_netsync_path_set(enlil_data->library, s);

      enlil_netsync_account_set(
                                enlil_library_netsync_host_get(
                                                               enlil_data->library),
                                enlil_library_netsync_path_get(
                                                               enlil_data->library),
                                enlil_library_netsync_account_get(
                                                                  enlil_data->library),
                                enlil_library_netsync_password_get(
                                                                   enlil_data->library));
   }

   _bt_preferences_cancel_cb(inwin, NULL, NULL);
}

static void
_bt_photo_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_login_failed_close_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_photo_delete_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   char buf[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;

   EINA_LIST_FOREACH(inwin->photos, l, photo)
   {
      snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
               enlil_photo_file_name_get(photo));
      LOG_INFO("Delete the photo %s", buf);
      remove(buf);
   }

   inwin_free(inwin);
}

static void
_bt_album_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_album_delete_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   char buf[PATH_MAX];

   snprintf(buf, PATH_MAX, "%s/%s", enlil_album_path_get(inwin->album),
            enlil_album_file_name_get(inwin->album));
   LOG_INFO("Delete the album %s", buf);
   ecore_file_recursive_rm(buf);

   inwin_free(inwin);
}

static void
_bt_album_rename_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_album_rename_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   Enlil_Album *album = inwin->album;
   Enlil_Album_Data *album_data = enlil_album_user_data_get(album);

   const char *s = elm_scrolled_entry_entry_get(inwin->entry);
   enlil_album_name_set(album, s);
   enlil_album_eet_header_save(album);

   elm_genlist_item_del(album_data->list_album_item);
   album_data->list_album_item = NULL;

   photos_list_object_header_update(album_data->list_photo_item);

   Enlil_Album *album_prev =
            enlil_library_album_prev_get(enlil_album_library_get(album), album);
   if (!album_prev)
   {
      list_left_append_relative(enlil_data->list_left, album, NULL);
      photos_list_object_header_move_after(album_data->list_photo_item, NULL);
   }
   else
   {
      Enlil_Album_Data *album_data_prev = enlil_album_user_data_get(album_prev);
      list_left_append_relative(enlil_data->list_left, album,
                                album_data_prev->list_album_item);
      photos_list_object_header_move_after(album_data->list_photo_item,
                                           album_data_prev->list_photo_item);
   }

   inwin_free(inwin);
}

static void
_bt_library_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   inwin_free(inwin);
}

static void
_bt_library_delete_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if (elm_check_state_get(inwin->check))
   {
      //delete file on hard drive
      eio_dir_unlink(enlil_library_path_get(enlil_data->library), NULL, NULL,
                     NULL);
   }

   enlil_library_eet_path_delete(enlil_data->library);
   library_set(NULL);
   main_menu_update_libraries_list();
   select_menu();

   inwin_free(inwin);
}

static void
_bt_photo_move_album_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   Eina_List *l;
   Enlil_Album *album;

   EINA_LIST_FOREACH(enlil_library_albums_get(enlil_data->library), l, album)
   {
      Enlil_Album_Data *album_data = enlil_album_user_data_get(album);
      album_data->inwin_photo_move_album = NULL;
      album_data->photo_move_album_list_album_item = NULL;
   }

   inwin_free(inwin);
}

static void
_bt_photo_move_album_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;
   Elm_Genlist_Item *item = elm_genlist_selected_item_get(inwin->gl);
   char buf[PATH_MAX], buf2[PATH_MAX];
   Eina_List *l;
   Enlil_Photo *photo;
   if (!item) return;
   Enlil_Album *album = (Enlil_Album *) elm_genlist_item_data_get(item);
   ASSERT_RETURN_VOID(album != NULL);

   EINA_LIST_FOREACH(inwin->photos, l, photo)
   {
      snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
               enlil_photo_file_name_get(photo));

      snprintf(buf2, PATH_MAX, "%s/%s/%s", enlil_album_path_get(album), enlil_album_file_name_get(album),
               enlil_photo_file_name_get(photo));

      ecore_file_mv(buf, buf2);
   }

   _bt_photo_move_album_cancel_cb(inwin, NULL, NULL);
}

static void
_bt_photo_save_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if (inwin->del_cb) inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void
_bt_photo_save_dont_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if (inwin->close_cb) inwin->close_cb(inwin->data);

   if (inwin->del_cb) inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void
_bt_photo_save_save_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if (inwin->apply_cb) inwin->apply_cb(inwin->data);

   if (inwin->del_cb) inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void
_bt_save_as_file_exists_cancel_cb(void *data, Evas_Object *obj,
                                  void *event_info)
{
   Inwin *inwin = data;

   if (inwin->del_cb) inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

static void
_bt_save_as_file_exists_apply_cb(void *data, Evas_Object *obj, void *event_info)
{
   Inwin *inwin = data;

   if (inwin->apply_cb) inwin->apply_cb(inwin->data);

   if (inwin->del_cb) inwin->del_cb(inwin->data);

   inwin_free(inwin);
}

void
inwin_free(Inwin *inwin)
{
   ASSERT_RETURN_VOID(inwin != NULL);

   if (inwin->del_cb) inwin->del_cb(inwin->data);

   EINA_STRINGSHARE_DEL(inwin->file);
   evas_object_del(inwin->inwin);
   FREE(inwin);
}

static char *
_gl_theme_label_get(void *data, Evas_Object *obj, const char *part)
{
   return strdup(data);
}

static void
_gl_theme_select_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   snprintf(buf, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s", (char*)data);
   s.string = eina_stringshare_add(buf);
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s", APP_NAME " theme");
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
   eina_stringshare_del(s.string);
}
