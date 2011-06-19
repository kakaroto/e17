#include "main.h"
#include "evas_object/slideshow_object.h"
#include "evas_object/photo_object.h"

static Elm_Genlist_Item_Class itc_exifs;
static char *
_gl_exifs_label_get(void *data, Evas_Object *obj, const char *part);

static Elm_Genlist_Item_Class itc_iptcs;
static char *
_gl_iptcs_label_get(void *data, Evas_Object *obj, const char *part);
static void
_slideshow_selected_cb(void *data, Evas_Object *obj, void *event_info);

static Slideshow_Item_Class itc_slideshow;
static Evas_Object *
_slideshow_icon_get(const void *data, Evas_Object *obj);

static void
_panel_image_photo_set(Panel_Image *panel_image, Enlil_Photo *photo);

static void
_entry_name_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void
_entry_description_changed_cb(void *data, Evas_Object *obj, void *event_info);
static void
_entry_author_changed_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_1_1_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_fit_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_fill_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photocam_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj,
                         void *event_info);
static void
_photocam_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void
_slider_photocam_zoom_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photocam_move_resize_cb(void *data, Evas *e, Evas_Object *obj,
                         void *event_info);

static void
_bt_rotate_180_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_rotate_90_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_rotate_R90_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_flip_vertical_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_flip_horizontal_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_blur_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_sharpen_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_grayscale_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_sepia_cb(void *data, Evas_Object *obj, void *event_info);
static void
_end_trans_cb(void *data, Enlil_Trans_Job *job, const char *file);

static void
_notify_trans_item_add(Panel_Image *panel_image, Evas_Object *item);
static void
_notify_trans_item_first_del(Panel_Image *panel_image);
static void
_bt_notify_trans_cancel_cb(void *data, Evas_Object *obj, void *event_info);

static void
_update_undo_redo(Panel_Image *panel_image);
static void
_menu_history_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_save_as_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_save_as_done_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_save_cb(void *data, Evas_Object *obj, void *event_info);
static void
_inwin_save_as_apply_cb(void *data);
static void
_bt_close_cb(void *data, Evas_Object *obj, void *event_info);

static void
_bt_menu_undo_open_cb(void *data, Evas_Object *obj, void *event_info);
static void
_bt_menu_redo_open_cb(void *data, Evas_Object *obj, void *event_info);

static void
_close_without_save_cb(void *data);
static void
_close_save_cb(void *data);

void
_panel_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item);

static Eina_Bool
_save_description_name_timer(void *data);
static void
_save_description_name(Panel_Image *panel_image);

static void
_photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_wall_set_cb(void *data, Evas_Object *obj, void *event_info);
static void
_photo_delete_cb(void *data, Evas_Object *obj, void *event_info);

static void
_photocal_loaded_cb(void *data, Evas_Object *obj, void *event);

Panel_Image *
panel_image_new(Evas_Object *obj, Enlil_Photo *photo)
{
   Evas_Object *ph, *bx, *bx2, *bt, *rect, *sl, *pb, *gl, *entry, *main_obj;
   Tabpanel_Item *tp_item;
   Eina_List *l;
   Enlil_Photo *_photo;

   Panel_Image *panel_image = calloc(1, sizeof(Panel_Image));
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   photo_data->panel_image = panel_image;
   panel_image->photo = photo;

   main_obj = edje_object_add(evas_object_evas_get(obj));
   edje_object_file_set(main_obj, Theme, "panel/image");
   evas_object_show(main_obj);

   //
   panel_image->tabpanel
            = tabpanel_add_with_edje(
                                     main_obj,
                                     edje_object_part_external_object_get(
                                                                          main_obj,
                                                                          "object.panel.image.menu"));
   tp_item = tabpanel_item_add_with_signal(panel_image->tabpanel, D_("Info."),
                                           main_obj,
                                           "panel,image,menu,info,show", NULL,
                                           panel_image);
   tabpanel_item_add_with_signal(panel_image->tabpanel, D_("Exifs."), main_obj,
                                 "panel,image,menu,exifs,show", NULL,
                                 panel_image);
   tabpanel_item_add_with_signal(panel_image->tabpanel, D_("Iptcs."), main_obj,
                                 "panel,image,menu,iptcs,show", NULL,
                                 panel_image);
   tabpanel_item_add_with_signal(panel_image->tabpanel, D_("Edition"),
                                 main_obj, "panel,image,menu,edition,show",
                                 NULL, panel_image);

   tabpanel_item_select(tp_item);
   //

   //
   bt = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.close");
   evas_object_smart_callback_add(bt, "clicked", _bt_close_cb, panel_image);

   bt = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.save");
   evas_object_smart_callback_add(bt, "clicked", _bt_save_cb, panel_image);

   bt = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.save_as");
   evas_object_smart_callback_add(bt, "clicked", _bt_save_as_cb, panel_image);
   //

   //
   entry = edje_object_part_external_object_get(main_obj,
                                                "object.panel.image.name");
   panel_image->entry_name = entry;
   evas_object_smart_callback_add(entry, "changed", _entry_name_changed_cb,
                                  panel_image);
   //

   //
   entry
            = edje_object_part_external_object_get(main_obj,
                                                   "object.panel.image.description");
   panel_image->entry_description = entry;
   evas_object_smart_callback_add(entry, "changed",
                                  _entry_description_changed_cb, panel_image);
   //

   //
   entry = edje_object_part_external_object_get(main_obj,
                                                "object.panel.image.author");
   panel_image->entry_author = entry;
   evas_object_smart_callback_add(entry, "changed", _entry_author_changed_cb,
                                  panel_image);
   //

   //
   panel_image->lbl_file_size
            = edje_object_part_external_object_get(main_obj,
                                                   "object.panel.image.file_size");
   panel_image->exifs.size
            = edje_object_part_external_object_get(main_obj,
                                                   "object.panel.image.photo_size");
   //

   //
   gl = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.exifs");
   panel_image->exifs.gl = gl;
   itc_exifs.item_style = "default_style";
   itc_exifs.func.label_get = _gl_exifs_label_get;
   itc_exifs.func.icon_get = NULL;
   itc_exifs.func.state_get = NULL;
   itc_exifs.func.del = NULL;

   gl = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.iptcs");
   panel_image->iptcs.gl = gl;
   itc_iptcs.item_style = "default_style";
   itc_iptcs.func.label_get = _gl_iptcs_label_get;
   itc_iptcs.func.icon_get = NULL;
   itc_iptcs.func.state_get = NULL;
   itc_iptcs.func.del = NULL;
   //

   //right panel
   ph = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.photo");
   panel_image->photocam = ph;

   bt = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.undo");
   evas_object_smart_callback_add(bt, "clicked", _bt_menu_undo_open_cb,
                                  panel_image);
   panel_image->undo.undo = elm_menu_add(enlil_data->win->win);

   bt = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.redo");
   evas_object_smart_callback_add(bt, "clicked", _bt_menu_redo_open_cb,
                                  panel_image);
   panel_image->undo.redo = elm_menu_add(enlil_data->win->win);

   bt
            = edje_object_part_external_object_get(main_obj,
                                                   "object.panel.image.1_1");
   evas_object_smart_callback_add(bt, "clicked", _bt_1_1_cb, panel_image);

   bt
            = edje_object_part_external_object_get(main_obj,
                                                   "object.panel.image.fit");
   evas_object_smart_callback_add(bt, "clicked", _bt_fit_cb, panel_image);

   bt = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.fill");
   evas_object_smart_callback_add(bt, "clicked", _bt_fill_cb, panel_image);

   panel_image->tb
            = edje_object_part_external_object_get(main_obj,
                                                   "object.panel.image.edition");
   Evas_Object *ic;
#define ITEM(LABEL, ICON, CB)         \
  ic = elm_icon_add(panel_image->tb); \
  elm_icon_file_set(ic, Theme, ICON); \
  evas_object_show(ic);               \
  elm_list_item_append(panel_image->tb, D_(LABEL), NULL, ic, CB, panel_image);

   ITEM("Rotate 90°", "icons/rotate/90", _bt_rotate_90_cb);
   ITEM("Rotate -90°", "icons/rotate/90/reverse", _bt_rotate_R90_cb);
   ITEM("Rotate 180°", "icons/rotate/180", _bt_rotate_180_cb);
   ITEM("Flip Horizontal", "icons/flip/horizontal", _bt_flip_horizontal_cb);
   ITEM("Flip Vertical", "icons/flip/vertical", _bt_flip_vertical_cb);
   ITEM("Effect Blur", "icons/blur", _bt_blur_cb);
   ITEM("Effect Sharpen", "icons/sharpen", _bt_sharpen_cb);
   ITEM("Effect Sepia", "icons/sepia", _bt_sepia_cb);
   ITEM("Effect Grayscale", "icons/grayscale", _bt_grayscale_cb);
   ITEM("", NULL, NULL);
   ITEM(D_("Set as application's wallpaper"), "icons/logo", _photo_wall_set_cb);
   ITEM("", NULL, NULL);
   ITEM(D_("Recreate thumbnail"), "icons/logo", _photo_thumb_reload_cb);
   ITEM(D_("Reload exifs data"), "icons/logo", _photo_exif_reload_cb);
   ITEM(D_("Reload IPTCs data"), "icons/logo", _photo_iptc_reload_cb);
   ITEM("", NULL, NULL);
   ITEM("", NULL, NULL);
   ITEM(D_("Delete the photo"), "icons/logo", _photo_delete_cb);

   //rectangle on top of the photocam which retrieves the mouse wheel
   rect = evas_object_rectangle_add(evas_object_evas_get(obj));
   panel_image->rect = rect;
   evas_object_color_set(rect, 0, 0, 0, 0);
   evas_object_repeat_events_set(rect, 1);
   evas_object_show(rect);
   evas_object_smart_member_add(rect, ph);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_WHEEL,
                                  _photocam_mouse_wheel_cb, panel_image);
   evas_object_raise(rect);

   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP,
                                  _photocam_mouse_up_cb, panel_image);

   evas_object_event_callback_add(ph, EVAS_CALLBACK_RESIZE,
                                  _photocam_move_resize_cb, panel_image);
   evas_object_event_callback_add(ph, EVAS_CALLBACK_MOVE,
                                  _photocam_move_resize_cb, panel_image);
   //

   //slideshow
   Evas_Object *slideshow = slideshow_object_add(obj);
   panel_image->slideshow.slideshow = slideshow;
   evas_object_size_hint_min_set(slideshow, 100, 80);
   slideshow_object_file_set(slideshow, Theme, "slideshow");
   evas_object_smart_callback_add(slideshow, "selected",
                                  _slideshow_selected_cb, panel_image);
   evas_object_show(slideshow);
   edje_object_part_swallow(main_obj, "object.panel.image.slideshow", slideshow);

   itc_slideshow.icon_get = _slideshow_icon_get;
   EINA_LIST_FOREACH(enlil_album_photos_get(enlil_photo_album_get(photo)), l, _photo)
   {
      if(enlil_photo_type_get(_photo) == ENLIL_PHOTO_TYPE_PHOTO)
      {
         Enlil_Photo_Data *_photo_data = enlil_photo_user_data_get(_photo);
         Slideshow_Item *_item = slideshow_object_item_append(slideshow, &itc_slideshow, _photo);
         _photo_data->slideshow_object_items = eina_list_append(_photo_data->slideshow_object_items, _item);
      }
   }

   Slideshow_Item
            *_item =
                     eina_list_data_get(
                                        eina_list_last(
                                                       photo_data->slideshow_object_items));
   slideshow_object_item_select(slideshow, _item);
   //

   //zoom
   sl = edje_object_part_external_object_get(main_obj,
                                             "object.panel.image.zoom");
   panel_image->sl = sl;
   evas_object_smart_callback_add(sl, "delay,changed",
                                  _slider_photocam_zoom_cb, panel_image);
   elm_slider_value_set(
                        photo_data->panel_image->sl,
                        elm_photocam_zoom_get(photo_data->panel_image->photocam));
   //

   //transformations notification window
   panel_image->notify_trans = elm_notify_add(ph);
   elm_notify_orient_set(panel_image->notify_trans,
                         ELM_NOTIFY_ORIENT_BOTTOM_RIGHT);
   evas_object_size_hint_weight_set(panel_image->notify_trans, -1.0, -1.0);
   evas_object_size_hint_align_set(panel_image->notify_trans, -1.0, -1.0);

   bx = elm_box_add(obj);
   panel_image->notify_trans_bx = bx;
   evas_object_size_hint_weight_set(bx, 1.0, 1.0);
   evas_object_size_hint_align_set(bx, -1.0, 0.0);
   evas_object_show(bx);
   elm_notify_content_set(panel_image->notify_trans, bx);

   bx2 = elm_box_add(obj);
   elm_box_horizontal_set(bx2, 1);
   evas_object_size_hint_weight_set(bx2, 1.0, 1.0);
   evas_object_size_hint_align_set(bx2, -1.0, 0.0);
   evas_object_show(bx2);
   elm_box_pack_end(bx, bx2);

   pb = elm_progressbar_add(obj);
   elm_object_style_set(pb, "wheel");
   elm_progressbar_label_set(pb, "");
   elm_progressbar_pulse(pb, EINA_TRUE);
   evas_object_size_hint_weight_set(pb, 1.0, 0.0);
   evas_object_size_hint_align_set(pb, -1.0, 0.5);
   evas_object_show(pb);
   elm_box_pack_end(bx2, pb);

   bt = elm_button_add(obj);
   elm_button_label_set(bt, D_("Cancel"));
   evas_object_size_hint_weight_set(bt, 1.0, 0.0);
   evas_object_size_hint_align_set(bt, -1.0, 0.0);
   evas_object_smart_callback_add(bt, "clicked", _bt_notify_trans_cancel_cb,
                                  panel_image);
   evas_object_show(bt);
   elm_box_pack_end(bx2, bt);
   //

   panel_image->tabpanel_item
            = tabpanel_item_add(photo_data->enlil_data->tabpanel,
                                enlil_photo_name_get(photo), main_obj,
                                _panel_select_cb, panel_image);

   _panel_image_photo_set(panel_image, photo);

   return panel_image;
}

static void
_panel_image_photo_set(Panel_Image *panel_image, Enlil_Photo *photo)
{
   Enlil_Trans_Job *job;
   Elm_Menu_Item *mi_item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   char buf[PATH_MAX];

   if (panel_image->photo)
   {
      Enlil_Photo_Data *_data = enlil_photo_user_data_get(panel_image->photo);
      _data->panel_image = NULL;
   }

   panel_image->photo = photo;
   photo_data->panel_image = panel_image;

   const char *name = enlil_photo_name_get(photo);
   if (!name || !strcmp(name, "")) name = "no name";
   tabpanel_item_label_set(panel_image->tabpanel_item, name);

   elm_scrolled_entry_entry_set(panel_image->entry_name,
                                enlil_photo_name_get(photo));
   elm_scrolled_entry_entry_set(panel_image->entry_description,
                                enlil_photo_description_get(photo));
   elm_scrolled_entry_entry_set(panel_image->entry_author,
                                enlil_photo_author_get(photo));

   snprintf(buf, sizeof(buf), "%f mo", enlil_photo_size_get(photo) / 1024.
            / 1024.);
   elm_label_label_set(panel_image->lbl_file_size, buf);

   elm_label_label_set(panel_image->exifs.size, D_("Unknown"));

   snprintf(buf, sizeof(buf), "%s/%s", enlil_photo_path_get(photo),
            enlil_photo_file_name_get(photo));
   elm_photocam_file_set(panel_image->photocam, buf);
   evas_object_smart_callback_add(panel_image->photocam, "loaded",
                                  _photocal_loaded_cb, NULL);
   elm_photocam_paused_set(panel_image->photocam, EINA_TRUE);

   panel_image->save.save = EINA_FALSE;

   EINA_LIST_FREE( panel_image->jobs_trans, job)
   enlil_trans_job_del(job);

   EINA_LIST_FREE(panel_image->undo.items_undo, mi_item);
   EINA_LIST_FREE(panel_image->undo.items_redo, mi_item);

   enlil_trans_history_free(panel_image->history);
   panel_image->history = enlil_trans_history_new(buf);

   panel_image_exifs_update(photo);
   panel_image_iptcs_update(photo);

   elm_photocam_zoom_mode_set(panel_image->photocam,
                              ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
}

void
panel_image_free(Panel_Image **_panel_image)
{
   Enlil_Trans_Job *job;
   Eina_List *l;
   Slideshow_Item *item;
   Elm_Menu_Item *mi_item;
   Panel_Image *panel_image = *_panel_image;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(panel_image->photo);

   photo_data->panel_image = NULL;

   EINA_LIST_FREE( panel_image->jobs_trans, job)
   enlil_trans_job_del(job);

   EINA_LIST_FREE(panel_image->undo.items_undo, mi_item);
   EINA_LIST_FREE(panel_image->undo.items_redo, mi_item);

   enlil_trans_history_free(panel_image->history);
   if (panel_image->tabpanel_item)
   {
      evas_object_event_callback_del_full(panel_image->photocam,
                                          EVAS_CALLBACK_RESIZE,
                                          _photocam_move_resize_cb, panel_image);
      evas_object_event_callback_del_full(panel_image->photocam,
                                          EVAS_CALLBACK_MOVE,
                                          _photocam_move_resize_cb, panel_image);
      tabpanel_item_del(panel_image->tabpanel_item);
      panel_image->tabpanel_item = NULL;
      evas_object_del(panel_image->rect);
   }

   if (panel_image->timer_description_name) ecore_timer_del(
                                                            panel_image->timer_description_name);

   EINA_LIST_FOREACH(slideshow_object_items_get(panel_image->slideshow.slideshow), l, item)
   {
      Enlil_Photo *_photo;
      Eina_List *l2;
      EINA_LIST_FOREACH(enlil_album_photos_get(enlil_photo_album_get(panel_image->photo)), l2, _photo)
      {
         Enlil_Photo_Data *_photo_data = enlil_photo_user_data_get(_photo);
         _photo_data->slideshow_object_items = eina_list_remove(_photo_data->slideshow_object_items, item);
      }
   }

   evas_object_del(panel_image->slideshow.slideshow);
   FREE(panel_image);
}

void
panel_image_exifs_update(Enlil_Photo *photo)
{
   char buf[PATH_MAX];
   const Eina_List *l;
   Enlil_Exif *exif;
   ASSERT_RETURN_VOID(photo != NULL);

   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   if (!photo_data->panel_image) return;

   snprintf(buf, PATH_MAX, "%d x %d", enlil_photo_size_w_get(photo),
            enlil_photo_size_h_get(photo));
   elm_label_label_set(photo_data->panel_image->exifs.size, buf);

   elm_genlist_clear(photo_data->panel_image->exifs.gl);

   if (!enlil_photo_exif_loaded_get(photo))
   {
      photo_data->clear_exif_data = EINA_FALSE;
      enlil_exif_job_prepend(photo, exif_load_done, photo);
   }
   else
   {
EINA_LIST_FOREACH   (enlil_photo_exifs_get(photo), l, exif)
   elm_genlist_item_append(photo_data->panel_image->exifs.gl, &itc_exifs,
            exif, NULL, ELM_GENLIST_ITEM_NONE, NULL, exif);
}
}

static void
_photocal_loaded_cb(void *data, Evas_Object *obj, void *event)
{
   elm_photocam_paused_set(obj, EINA_FALSE);
}

static char *
_gl_exifs_label_get(void *data, Evas_Object *obj, const char *part)
{
   Enlil_Exif *exif = (Enlil_Exif *) data;
   char buf[PATH_MAX];
   snprintf(buf, PATH_MAX, "<b>%s</b>  :  %s", enlil_exif_tag_get(exif),
            enlil_exif_value_get(exif));
   return strdup(buf);
}

void
panel_image_iptcs_update(Enlil_Photo *photo)
{
   const Eina_List *l;
   Enlil_IPTC *iptc;
   ASSERT_RETURN_VOID(photo != NULL);

   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   if (!photo_data->panel_image) return;

   elm_genlist_clear(photo_data->panel_image->iptcs.gl);

   elm_scrolled_entry_entry_set(photo_data->panel_image->entry_name,
                                enlil_photo_name_get(photo));
   elm_scrolled_entry_entry_set(photo_data->panel_image->entry_description,
                                enlil_photo_description_get(photo));

   if (!enlil_photo_iptc_loaded_get(photo))
   {
      photo_data->clear_iptc_data = EINA_FALSE;
      enlil_iptc_job_prepend(photo, iptc_load_done, photo);
   }
   else
   {
EINA_LIST_FOREACH   (enlil_photo_iptcs_get(photo), l, iptc)
   elm_genlist_item_append(photo_data->panel_image->iptcs.gl, &itc_iptcs,
            iptc, NULL, ELM_GENLIST_ITEM_NONE, NULL, iptc);
}
}

static char *
_gl_iptcs_label_get(void *data, Evas_Object *obj, const char *part)
{
   Enlil_IPTC *iptc = (Enlil_IPTC *) data;
   char buf[PATH_MAX];
   snprintf(buf, PATH_MAX, "<b>%s</b>  :  %s", enlil_iptc_title_get(iptc),
            enlil_iptc_value_get(iptc));
   return strdup(buf);
}

void
panel_image_1_1(Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   elm_slider_value_set(photo_data->panel_image->sl, 1);
   elm_photocam_zoom_mode_set(photo_data->panel_image->photocam,
                              ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
   elm_photocam_zoom_set(photo_data->panel_image->photocam, 1);
}

void
panel_image_fit(Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   elm_photocam_zoom_mode_set(photo_data->panel_image->photocam,
                              ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
   elm_slider_value_set(
                        photo_data->panel_image->sl,
                        elm_photocam_zoom_get(photo_data->panel_image->photocam));
}

void
panel_image_fill(Enlil_Photo *photo)
{
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   elm_photocam_zoom_mode_set(photo_data->panel_image->photocam,
                              ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL);
   elm_slider_value_set(
                        photo_data->panel_image->sl,
                        elm_photocam_zoom_get(photo_data->panel_image->photocam));
}

void
panel_image_rotation_90(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Rotation 90°"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_ROTATE_90, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_rotation_R90(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Rotation -90°"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_ROTATE_R90, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_rotation_180(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Rotation 180°"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_ROTATE_180, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_flip_vertical(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Flip Vertical"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_FLIP_VERTICAL, _end_trans_cb,
                                  photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_flip_horizontal(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Flip Horizontal"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_FLIP_HORIZONTAL, _end_trans_cb,
                                  photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_blur(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Blur"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_BLUR, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_sharpen(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Sharpen"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_SHARPEN, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_sepia(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Sepia"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_SEPIA, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_grayscale(Enlil_Photo *photo)
{
   Evas_Object *item;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Enlil_Trans_Job *job;

   item = elm_label_add(photo_data->panel_image->photocam);
   elm_label_label_set(item, D_("Grayscale"));
   evas_object_show(item);
   _notify_trans_item_add(photo_data->panel_image, item);

   job
            = enlil_trans_job_add(
                                  photo_data->panel_image->history,
                                  elm_photocam_file_get(
                                                        photo_data->panel_image->photocam),
                                  Enlil_TRANS_GRAYSCALE, _end_trans_cb, photo);
   photo_data->panel_image->jobs_trans
            = eina_list_append(photo_data->panel_image->jobs_trans, job);

   photo_data->panel_image->save.save = EINA_TRUE;
}

void
panel_image_save_as(Enlil_Photo *photo)
{
   Evas_Object *inwin, *fs, *vbox;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Panel_Image *panel_image = photo_data->panel_image;

   //create inwin & file selector
   inwin = elm_win_inwin_add(photo_data->enlil_data->win->win);
   panel_image->inwin = inwin;
   evas_object_show(inwin);

   vbox = elm_box_add(photo_data->enlil_data->win->win);
   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(vbox);
   elm_win_inwin_content_set(inwin, vbox);

   fs = elm_fileselector_add(photo_data->enlil_data->win->win);
   elm_fileselector_is_save_set(fs, EINA_TRUE);
   elm_fileselector_expandable_set(fs, EINA_FALSE);
   elm_fileselector_path_set(fs, enlil_photo_path_get(photo));
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_box_pack_end(vbox, fs);
   evas_object_show(fs);

   evas_object_smart_callback_add(fs, "done", _bt_save_as_done_cb, panel_image);

   panel_image->save.save = EINA_FALSE;
}

void
panel_image_save(Enlil_Photo *photo)
{
   char buf[PATH_MAX];
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Panel_Image *panel_image = photo_data->panel_image;

   _save_description_name(panel_image);

   const Enlil_Trans_History_Item *item =
            enlil_trans_history_current_get(panel_image->history);

   if (item)
   {
      snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
               enlil_photo_file_name_get(photo));
      enlil_photo_copy_exif_in_file(photo,
                                    enlil_trans_history_item_file_get(item));
      enlil_photo_save_iptc_in_custom_file(
                                           photo,
                                           enlil_trans_history_item_file_get(
                                                                             item));
      ecore_file_cp(enlil_trans_history_item_file_get(item), buf);
   }

   panel_image->save.save = EINA_FALSE;
}

void
_panel_select_cb(void *data, Tabpanel *tabpanel, Tabpanel_Item *item)
{
   Panel_Image *panel_image = (Panel_Image *) data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(panel_image->photo);
   _update_undo_redo(photo_data->panel_image);

   elm_map_bubbles_close(enlil_data->map->map);
}

static void
_bt_1_1_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = (Panel_Image *) data;
   panel_image_1_1(panel_image->photo);
}

static void
_bt_fit_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = (Panel_Image *) data;
   panel_image_fit(panel_image->photo);
}

static void
_bt_fill_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = (Panel_Image *) data;
   panel_image_fill(panel_image->photo);
}

static void
_slider_photocam_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = (Panel_Image *) data;
   int zoom = 1;
   int i;
   int val = elm_slider_value_get(panel_image->sl);

   for (i = 0; i < val - 1; i++)
      zoom *= 2;

   elm_photocam_zoom_mode_set(panel_image->photocam,
                              ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
   elm_photocam_zoom_set(panel_image->photocam, zoom);
}

static void
_photocam_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj,
                         void *event_info)
{
   Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel *) event_info;
   Panel_Image *panel_image = data;
   int zoom;
   double val;
   //unset the mouse wheel
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   zoom = elm_photocam_zoom_get(panel_image->photocam);
   if (ev->z < 0 && zoom == 1) return;

   if (ev->z < 0)
      zoom /= 2;
   else
      zoom *= 2;

   val = 1;
   int _zoom = zoom;
   while (_zoom > 1)
   {
      _zoom /= 2;
      val++;
   }

   if (val > 10) return;

   elm_photocam_zoom_mode_set(panel_image->photocam,
                              ELM_PHOTOCAM_ZOOM_MODE_MANUAL);
   if (zoom >= 1) elm_photocam_zoom_set(panel_image->photocam, zoom);

   elm_slider_value_set(panel_image->sl, val);
}

static void
_photocam_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *) event_info;
   Panel_Image *panel_image = data;

   if (ev->button != 3) return;
   //unset the mouse up
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   elm_menu_move(panel_image->menu, ev->output.x, ev->output.y);
   evas_object_show(panel_image->menu);
}

static void
_photocam_move_resize_cb(void *data, Evas *e, Evas_Object *obj,
                         void *event_info)
{
   Panel_Image *panel_image = (Panel_Image *) data;
   int x, y, w, h;

   evas_object_geometry_get(panel_image->photocam, &x, &y, &w, &h);
   evas_object_resize(panel_image->rect, w, h);
   evas_object_move(panel_image->rect, x, y);

   evas_object_resize(panel_image->notify_trans, w, h);
   evas_object_move(panel_image->notify_trans, x, y);
}

static void
_bt_rotate_180_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_rotation_180(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_rotate_90_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_rotation_90(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_rotate_R90_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_rotation_R90(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_flip_vertical_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_flip_vertical(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_flip_horizontal_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_flip_horizontal(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_blur_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_blur(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_sharpen_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_sharpen(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_grayscale_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_grayscale(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_bt_sepia_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_sepia(panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_end_trans_cb(void *data, Enlil_Trans_Job *job, const char *file)
{
   Enlil_Photo *photo = data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   elm_photocam_file_set(photo_data->panel_image->photocam, file);
   _notify_trans_item_first_del(photo_data->panel_image);
   photo_data->panel_image->jobs_trans
            = eina_list_remove(photo_data->panel_image->jobs_trans, job);

   _update_undo_redo(photo_data->panel_image);
}

static void
_notify_trans_item_add(Panel_Image *panel_image, Evas_Object *item)
{
   if (!panel_image->notify_trans_items) evas_object_show(
                                                          panel_image->notify_trans);

   panel_image->notify_trans_items
            = eina_list_append(panel_image->notify_trans_items, item);

   elm_box_pack_start(panel_image->notify_trans_bx, item);
}

static void
_notify_trans_item_first_del(Panel_Image *panel_image)
{
   Evas_Object *item = eina_list_data_get(panel_image->notify_trans_items);
   panel_image->notify_trans_items
            = eina_list_remove(panel_image->notify_trans_items, item);

   elm_box_unpack(panel_image->notify_trans_bx, item);
   evas_object_del(item);

   if (!panel_image->notify_trans_items) evas_object_hide(
                                                          panel_image->notify_trans);
}

static void
_bt_notify_trans_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   Enlil_Trans_Job *job;

   if (!panel_image->jobs_trans) return;

   job = eina_list_data_get(panel_image->jobs_trans);
   panel_image->jobs_trans = eina_list_remove(panel_image->jobs_trans, job);
   enlil_trans_job_del(job);

   _notify_trans_item_first_del(panel_image);
}

static void
_update_undo_redo(Panel_Image *panel_image)
{
   const char *type = NULL;
   const Enlil_Trans_History_Item *current, *item;
   Elm_Menu_Item *mi_item;
   const Eina_List *h, *l;

   EINA_LIST_FREE(panel_image->undo.items_undo, mi_item)
   elm_menu_item_del(mi_item);

   EINA_LIST_FREE(panel_image->undo.items_redo, mi_item)
   elm_menu_item_del(mi_item);

   elm_menu_item_disabled_set(panel_image->undo.item_undo, 1);
   elm_menu_item_disabled_set(panel_image->undo.item_redo, 1);

   h = enlil_trans_history_get(panel_image->history);
   current = enlil_trans_history_current_get(panel_image->history);

   //jump the first item because this is the original file
   l = h;
   if (eina_list_data_get(h) == current) goto second_step;

   h = eina_list_next(h);

   EINA_LIST_FOREACH(h, l, item)
   {
      switch(enlil_trans_history_item_type_get(item))
      {
         case Enlil_TRANS_ROTATE_180:
         type = D_("Rotate 180°");
         break;

         case Enlil_TRANS_ROTATE_90:
         type = D_("Rotate 90°");
         break;

         case Enlil_TRANS_ROTATE_R90:
         type = D_("Rotate -90°");
         break;

         case Enlil_TRANS_FLIP_VERTICAL:
         type = D_("Flip Vertical");
         break;

         case Enlil_TRANS_FLIP_HORIZONTAL:
         type = D_("Flip Horizontal");
         break;

         case Enlil_TRANS_BLUR:
         type = D_("Blur");
         break;

         case Enlil_TRANS_SHARPEN:
         type = D_("Sharpen");
         break;

         case Enlil_TRANS_GRAYSCALE:
         type = D_("Grayscale");
         break;

         case Enlil_TRANS_SEPIA:
         type = D_("Sepia");
         break;
      }

      mi_item = elm_menu_item_add(panel_image->undo.undo, NULL, "icons/undo", type, _menu_history_cb, panel_image);
      panel_image->undo.items_undo = eina_list_append(panel_image->undo.items_undo, mi_item);

      elm_menu_item_disabled_set(panel_image->undo.item_undo, 0);
      if(item == current)
      break;
   }

   second_step: l = eina_list_next(l);
EINA_LIST_FOREACH(l, l, item)
{
   switch(enlil_trans_history_item_type_get(item))
   {
      case Enlil_TRANS_ROTATE_180:
      type = D_("Rotate 180°");
      break;

      case Enlil_TRANS_ROTATE_90:
      type = D_("Rotate 90°");
      break;

      case Enlil_TRANS_ROTATE_R90:
      type = D_("Rotate -90°");
      break;

      case Enlil_TRANS_FLIP_VERTICAL:
      type = D_("Flip Vertical");
      break;

      case Enlil_TRANS_FLIP_HORIZONTAL:
      type = D_("Flip Horizontal");
      break;

      case Enlil_TRANS_BLUR:
      type = D_("Blur");
      break;

      case Enlil_TRANS_SHARPEN:
      type = D_("Sharpen");
      break;

      case Enlil_TRANS_GRAYSCALE:
      type = D_("Grayscale");
      break;

      case Enlil_TRANS_SEPIA:
      type = D_("Sepia");
      break;
   }

   mi_item = elm_menu_item_add(panel_image->undo.redo, NULL, "icons/redo", type, _menu_history_cb, panel_image);
   panel_image->undo.items_redo = eina_list_append(panel_image->undo.items_redo, mi_item);

   elm_menu_item_disabled_set(panel_image->undo.item_redo, 0);
}
}

void
panel_image_undo(Enlil_Photo *photo)
{
   const char *file;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Panel_Image *panel_image = photo_data->panel_image;

   int count = eina_list_count(panel_image->undo.items_undo);
   if (count >= 0)
   {
      file = enlil_trans_history_goto(panel_image->history, count - 1);
      elm_photocam_file_set(panel_image->photocam, file);

      _update_undo_redo(panel_image);
   }

   panel_image->save.save = EINA_TRUE;
}

void
panel_image_redo(Enlil_Photo *photo)
{
   const char *file;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   Panel_Image *panel_image = photo_data->panel_image;

   int count = eina_list_count(panel_image->undo.items_undo);
   if (panel_image->undo.items_redo)
   {
      file = enlil_trans_history_goto(panel_image->history, count + 1);
      elm_photocam_file_set(panel_image->photocam, file);

      _update_undo_redo(panel_image);
   }

   panel_image->save.save = EINA_TRUE;
}

static void
_menu_history_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *file;
   const Eina_List *l;
   Panel_Image *panel_image = data;
   const Elm_Menu_Item *item = event_info, *_item;
   int pos = 0;

   if (obj == panel_image->undo.undo)
   {
EINA_LIST_FOREACH   (panel_image->undo.items_undo, l, _item)
   {
      if(item == _item)
      break;
      else
      pos++;
   }
}
else
{
   pos = eina_list_count(panel_image->undo.items_undo) + 1;
   EINA_LIST_FOREACH(panel_image->undo.items_redo, l, _item)
   {
      if(item == _item)
      break;
      else
      pos++;
   }
}

file = enlil_trans_history_goto(panel_image->history, pos);
elm_photocam_file_set(panel_image->photocam, file);

_update_undo_redo(panel_image);

panel_image->save.save = EINA_TRUE;
}

static void
_bt_save_as_done_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Panel_Image *panel_image = data;
   Enlil_Photo *photo = panel_image->photo;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
   const char *selected = event_info;

   if (panel_image->inwin)
   {
      evas_object_del(panel_image->inwin);
      panel_image->inwin = NULL;
   }

   if (selected)
   {
      const Enlil_Trans_History_Item *item =
               enlil_trans_history_current_get(panel_image->history);
      char *file = ecore_file_strip_ext(selected);
      snprintf(buf, PATH_MAX, "%s%s", file,
               strrchr(enlil_photo_file_name_get(photo), '.'));

      if (!panel_image->save.path && ecore_file_exists(buf))
      {
         panel_image->save.path = eina_stringshare_add(buf);
         inwin_save_as_file_exists_new(NULL, _inwin_save_as_apply_cb,
                                       panel_image, buf);
         return;
      }

      ecore_file_cp(enlil_trans_history_item_file_get(item), buf);

      //copy exifs data
      enlil_photo_copy_exif_in_file(photo, buf);
      enlil_photo_save_iptc_in_custom_file(photo, buf);

      photo_data->enlil_data->auto_open
               = eina_list_append(photo_data->enlil_data->auto_open,
                                  eina_stringshare_add(buf));
      FREE(file);
   }
   panel_image->save.save = EINA_FALSE;
}

static void
_inwin_save_as_apply_cb(void *data)
{
   Panel_Image *panel_image = data;
   _bt_save_as_done_cb(panel_image->photo, NULL,
                       (void *) panel_image->save.path);
   EINA_STRINGSHARE_DEL(panel_image->save.path);
}

static void
_bt_save_as_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_save_as(panel_image->photo);
}

static void
_bt_save_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   panel_image_save(panel_image->photo);
}

static void
_bt_close_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;

   if (panel_image->save_description_name) _save_description_name(panel_image);

   if (panel_image->save.save)
   {
      inwin_photo_save_new(NULL, _close_save_cb, _close_without_save_cb,
                           panel_image, panel_image->photo);
   }
   else
      panel_image_free(&panel_image);
}

static void
_close_without_save_cb(void *data)
{
   Panel_Image *panel_image = data;
   panel_image_free(&panel_image);
}

static void
_close_save_cb(void *data)
{
   Panel_Image *panel_image = data;
   panel_image_save(panel_image->photo);

   panel_image_free(&panel_image);
}

static void
_entry_name_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   Enlil_Photo *photo = panel_image->photo;
   //Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   const char *entry = elm_scrolled_entry_entry_get(panel_image->entry_name);
   if (entry != NULL && !strcmp(entry, "") && enlil_photo_name_get(photo)
            == NULL) return;

   if (entry == enlil_photo_name_get(photo)) return;

   panel_image->save_description_name = EINA_TRUE;

   if (panel_image->timer_description_name) ecore_timer_del(
                                                            panel_image->timer_description_name);
   panel_image->timer_description_name
            = ecore_timer_add(5, _save_description_name_timer, panel_image);
}

static Eina_Bool
_save_description_name_timer(void *data)
{
   Panel_Image *panel_image = data;
   _save_description_name(data);

   panel_image->timer_description_name = NULL;
   return EINA_FALSE;
}

static void
_save_description_name(Panel_Image *panel_image)
{
   Enlil_Photo *photo = panel_image->photo;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   if (!panel_image->save_description_name) return;
   panel_image->save_description_name = EINA_FALSE;

   enlil_photo_name_set(photo,
                        elm_scrolled_entry_entry_get(panel_image->entry_name));
   enlil_photo_eet_save(photo);

   tabpanel_item_label_set(panel_image->tabpanel_item,
                           enlil_photo_name_get(photo));

   Enlil_Photo *photo_prev =
            enlil_album_photo_prev_get(enlil_photo_album_get(photo), photo);
   if (!photo_prev)
      photos_list_object_child_move_after(photo_data->list_photo_item, NULL);
   else
   {
      Enlil_Photo_Data *photo_data_prev = enlil_photo_user_data_get(photo_prev);
      photos_list_object_child_move_after(photo_data->list_photo_item,
                                          photo_data_prev->list_photo_item);
   }

   enlil_photo_description_set(
                               photo,
                               elm_scrolled_entry_entry_get(
                                                            panel_image->entry_description));
   enlil_photo_author_set(
                          photo,
                          elm_scrolled_entry_entry_get(
                                                       panel_image->entry_author));

   enlil_photo_eet_save(photo);
   enlil_photo_save_iptc_in_file(photo);

   if (panel_image->timer_description_name) ecore_timer_del(
                                                            panel_image->timer_description_name);
   panel_image->timer_description_name = NULL;
}

static void
_entry_description_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   Enlil_Photo *photo = panel_image->photo;
   //Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   const char *entry =
            elm_scrolled_entry_entry_get(panel_image->entry_description);
   if (entry != NULL && !strcmp(entry, "")
            && enlil_photo_description_get(photo) == NULL) return;

   if (entry == enlil_photo_description_get(photo)) return;

   panel_image->save_description_name = EINA_TRUE;

   if (panel_image->timer_description_name) ecore_timer_del(
                                                            panel_image->timer_description_name);
   panel_image->timer_description_name
            = ecore_timer_add(5, _save_description_name_timer, panel_image);
}

static void
_entry_author_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   Enlil_Photo *photo = panel_image->photo;
   //Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);

   const char *entry = elm_scrolled_entry_entry_get(panel_image->entry_author);
   if (entry != NULL && !strcmp(entry, "") && enlil_photo_author_get(photo)
            == NULL) return;

   if (entry == enlil_photo_author_get(photo)) return;

   panel_image->save_description_name = EINA_TRUE;

   if (panel_image->timer_description_name) ecore_timer_del(
                                                            panel_image->timer_description_name);
   panel_image->timer_description_name
            = ecore_timer_add(5, _save_description_name_timer, panel_image);
}

static Evas_Object *
_slideshow_icon_get(const void *data, Evas_Object *obj)
{
   const char *s = NULL;
   Enlil_Photo *photo = (Enlil_Photo *) data;
   Enlil_Photo_Data *enlil_photo_data = enlil_photo_user_data_get(photo);

   Evas_Object *o = photo_object_add(obj);
   photo_object_theme_file_set(o, Theme, "photo_simple");
   photo_object_fill_set(o, EINA_TRUE);

   if (enlil_photo_data->cant_create_thumb == 1) return o;

   s = enlil_thumb_photo_get(photo, Enlil_THUMB_FDO_NORMAL, thumb_done_cb,
                             thumb_error_cb, NULL);

   evas_image_cache_flush(evas_object_evas_get(obj));

   if (s)
      photo_object_file_set(o, s, NULL);
   else
      photo_object_progressbar_set(o, EINA_TRUE);

   if (enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_VIDEO) photo_object_camera_set(
                                                                                      o,
                                                                                      EINA_TRUE);

   photo_object_text_set(o, enlil_photo_name_get(photo));

   evas_object_show(o);
   return o;
}

static void
_slideshow_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
   Slideshow_Item *item = event_info;
   Panel_Image *panel_image = data;

   Enlil_Photo *photo = slideshow_object_item_data_get(item);

   _panel_image_photo_set(panel_image, photo);
}

static void
_bt_menu_undo_open_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   int x, y;

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   elm_menu_move(panel_image->undo.undo, x, y);
   evas_object_show(panel_image->undo.undo);
}

static void
_bt_menu_redo_open_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   int x, y;

   evas_pointer_output_xy_get(evas_object_evas_get(obj), &x, &y);
   elm_menu_move(panel_image->undo.redo, x, y);
   evas_object_show(panel_image->undo.redo);
}

static void
_photo_thumb_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(panel_image->photo);
   enlil_photo_thumb_fdo_normal_set(panel_image->photo, NULL);
   enlil_photo_thumb_fdo_large_set(panel_image->photo, NULL);
   photos_list_object_item_update(photo_data->list_photo_item);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_photo_exif_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   enlil_exif_job_prepend(panel_image->photo, exif_load_done,
                          panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_photo_iptc_reload_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   enlil_iptc_job_prepend(panel_image->photo, iptc_load_done,
                          panel_image->photo);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_photo_wall_set_cb(void *data, Evas_Object *obj, void *event_info)
{
   char buf[PATH_MAX];
   Enlil_String s;
   Eet_Data_Descriptor *edd;

   Panel_Image *panel_image = data;

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(panel_image->photo),
            enlil_photo_file_name_get(panel_image->photo));
   enlil_win_bg_set(enlil_data->win, buf);

   s.string = eina_stringshare_add(buf);
   edd = enlil_string_edd_new();

   snprintf(buf, PATH_MAX, "%s %s", APP_NAME " background",
            enlil_library_path_get(enlil_data->library));
   enlil_eet_app_data_save(edd, buf, &s);
   eet_data_descriptor_free(edd);
   eina_stringshare_del(s.string);

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

static void
_photo_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
   Panel_Image *panel_image = data;
   inwin_photo_delete_new(enlil_data->win->win, NULL, NULL,
                          eina_list_append(NULL, panel_image->photo));

   //unselect items
   const Eina_List *l;
   Elm_List_Item *item;
EINA_LIST_FOREACH(elm_list_items_get(obj), l, item)
elm_list_item_selected_set(item, EINA_FALSE);
//
}

