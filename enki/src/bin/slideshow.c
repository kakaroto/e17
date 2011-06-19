#include "slideshow.h"
#include "../../config.h"

#define TRANSFORMATION_DURATION 1.0

static Evas_Object *win = NULL;
static Evas_Object *layout;
static Evas_Object *slideshow, *bt_start, *bt_stop, *edje, *bt_layout, *spin;
static Elm_Slideshow_Item_Class itc;
static Evas_Object *
_get(void *data, Evas_Object *obj);
static Enlil_Album *galbum = NULL;
static Enlil_Library *glibrary = NULL;

static void
_key_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Up *ev = (Evas_Event_Key_Up *) event_info;

   //printf("%s\n", ev->key);
   if (!strcmp(ev->key, "Right") || !strcmp(ev->key, "Up"))
      elm_slideshow_next(slideshow);
   else if (!strcmp(ev->key, "Left") || !strcmp(ev->key, "Down"))
      elm_slideshow_previous(slideshow);
   else if (!strcmp(ev->key, "Escape"))
      slideshow_hide();
   else if (!strcmp(ev->key, "Home"))
   {
      const Eina_List *l = elm_slideshow_items_get(slideshow);
      elm_slideshow_show(eina_list_data_get(l));
   }
   else if (!strcmp(ev->key, "End"))
   {
      const Eina_List *l = elm_slideshow_items_get(slideshow);
      elm_slideshow_show(eina_list_data_get(eina_list_last(l)));
   }
}

static void
_bt_layout_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (!strcmp(elm_slideshow_layout_get(data), "fullscreen"))
   {
      elm_slideshow_layout_set(data, "not_fullscreen");
      edje_object_signal_emit(elm_layout_edje_get(layout), "windowed", "");
   }
   else
   {
      elm_slideshow_layout_set(data, "fullscreen");
      edje_object_signal_emit(elm_layout_edje_get(layout), "fullscreen", "");
   }
}

static void
_next(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_next(data);
}

static void
_previous(void *data, Evas_Object *obj, void *event)
{
   elm_slideshow_previous(slideshow);
}

static void
_hv_select(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_transition_set(slideshow, data);
   elm_hoversel_label_set(obj, data);
}

static void
_start(void *data, Evas_Object *obj, void *event_info)
{
   slideshow_start();
}

static void
_stop(void *data, Evas_Object *obj, void *event_info)
{
   elm_slideshow_timeout_set(slideshow, 0);
   elm_object_disabled_set(bt_start, 0);
   elm_object_disabled_set(bt_stop, 1);
}

static void
_spin(void *data, Evas_Object *obj, void *event_info)
{
   if (elm_slideshow_timeout_get(slideshow) > 0) elm_slideshow_timeout_set(
                                                                           slideshow,
                                                                           (int) elm_spinner_value_get(
                                                                                                       data));
}

static void
_enki_close(void *data, Evas_Object *obj, void *event_info)
{
   slideshow_hide();
}

static Evas_Object *
_get(void *data, Evas_Object *obj)
{
   char buf[PATH_MAX];

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(data),
            enlil_photo_file_name_get(data));
   //Evas_Object *o = photo_object_add(slideshow);
   //photo_object_theme_file_set(o, THEME, "photo/slideshow");
   //photo_object_file_set(o, buf, NULL);

   //Evas_Object *o = elm_photocam_add(obj);
   //elm_photocam_zoom_mode_set(o,ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
   //elm_photocam_file_set(o, buf);

   Evas_Object *o = elm_photo_add(obj);
   elm_photo_file_set(o, buf);
   elm_photo_fill_inside_set(o, EINA_TRUE);
   elm_object_style_set(o, "shadow");

   return o;
}

void
_init_slideshow()
{
   Evas_Object *bg, *bt, *hv;
   const Eina_List *l;
   const char *transition;

   if (win) return;

   itc.func.get = _get;
   itc.func.del = NULL;

   win = elm_win_add(NULL, "Slideshow", ELM_WIN_BASIC);
   elm_win_title_set(win, "Slideshow");

   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   evas_object_color_set(bg, 0, 0, 0, 255);
   evas_object_show(bg);

   //
   layout = elm_layout_add(win);
   elm_layout_file_set(layout, Theme, "slideshow/fullscreen");
   evas_object_size_hint_weight_set(layout, -1.0, -1.0);
   evas_object_size_hint_align_set(layout, -1.0, -1.0);
   elm_win_resize_object_add(win, layout);
   evas_object_show(layout);

   edje = elm_layout_edje_get(layout);

   //
   slideshow = edje_object_part_external_object_get(edje, "object.slideshow");

   bt = edje_object_part_external_object_get(edje, "object.previous");
   evas_object_smart_callback_add(bt, "clicked", _previous, slideshow);

   bt = edje_object_part_external_object_get(edje, "object.next");
   evas_object_smart_callback_add(bt, "clicked", _next, slideshow);

   bt_layout = edje_object_part_external_object_get(edje, "object.layout");
   evas_object_smart_callback_add(bt_layout, "clicked", _bt_layout_cb,
                                  slideshow);

   bt = edje_object_part_external_object_get(edje, "object.close");
   evas_object_smart_callback_add(bt, "clicked", _enki_close, slideshow);

   spin = edje_object_part_external_object_get(edje, "object.timing");
   evas_object_smart_callback_add(spin, "changed", _spin, spin);

   bt_start = edje_object_part_external_object_get(edje, "object.start");
   evas_object_smart_callback_add(bt_start, "clicked", _start, spin);

   bt_stop = edje_object_part_external_object_get(edje, "object.stop");
   evas_object_smart_callback_add(bt_stop, "clicked", _stop, spin);
   elm_object_disabled_set(bt_stop, EINA_TRUE);

   hv = edje_object_part_external_object_get(edje, "object.styles");
   EINA_LIST_FOREACH(elm_slideshow_transitions_get(slideshow), l, transition)
   elm_hoversel_item_add(hv, transition, NULL, 0, _hv_select, transition);
   elm_hoversel_label_set(
                          hv,
                          eina_list_data_get(
                                             elm_slideshow_transitions_get(
                                                                           slideshow)));
   //

   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_UP, _key_up_cb, NULL);
   evas_object_focus_set(win, 1);
}

void
slideshow_start()
{
   elm_slideshow_timeout_set(slideshow, (int) elm_spinner_value_get(spin));

   elm_object_disabled_set(bt_start, 1);
   elm_object_disabled_set(bt_stop, 0);
}

void
slideshow_show()
{
   _init_slideshow();

   elm_win_fullscreen_set(win, 1);
   //evas_object_resize(win, 400,400);
   evas_object_show(win);
}

Evas_Object *
slideshow_win_get()
{
   return win;
}

void
slideshow_album_add(Enlil_Album *album, Enlil_Photo *_photo)
{
   Eina_List *l;
   Enlil_Photo *photo;
   Elm_Slideshow_Item *item = NULL;

   _init_slideshow();

   galbum = album;
   EINA_LIST_FOREACH(enlil_album_photos_get(album), l, photo)
   {
      if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
      {
         Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
         photo_data->slideshow_item = elm_slideshow_item_add(slideshow, &itc, photo);

         if(photo == _photo)
         item = photo_data->slideshow_item;
      }
   }
   if (item) elm_slideshow_show(item);
}

void
slideshow_library_add(Enlil_Library *library, Enlil_Photo *_photo)
{
   Eina_List *l, *l2;
   Enlil_Photo *photo;
   Enlil_Album *album;
   Elm_Slideshow_Item *item = NULL;

   _init_slideshow();

   glibrary = library;
   EINA_LIST_FOREACH(enlil_library_albums_get(library), l, album)
   {
      EINA_LIST_FOREACH(enlil_album_photos_get(album), l2, photo)
      {
         if(enlil_photo_type_get(photo) == ENLIL_PHOTO_TYPE_PHOTO)
         {
            Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
            photo_data->slideshow_item = elm_slideshow_item_add(slideshow, &itc, photo);
            if(photo == _photo)
            item = photo_data->slideshow_item;
         }
      }
   }

   if (item) elm_slideshow_show(item);
}

void
slideshow_clear()
{
   Eina_List *l, *l2;
   Enlil_Photo *photo;
   Enlil_Album *album;

   _init_slideshow();

   elm_slideshow_clear(slideshow);

   if (galbum)
   {
      EINA_LIST_FOREACH(enlil_album_photos_get(galbum), l, photo)
      {
         Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
         photo_data->slideshow_item = NULL;
      }
      galbum = NULL;
   }

   if (glibrary)
   {
      EINA_LIST_FOREACH(enlil_library_albums_get(glibrary), l, album)
      {
         EINA_LIST_FOREACH(enlil_album_photos_get(album), l2, photo)
         {
            Enlil_Photo_Data *photo_data = enlil_photo_user_data_get(photo);
            photo_data->slideshow_item = NULL;
         }
      }
      glibrary = NULL;
   }
}

void
slideshow_hide()
{
   _init_slideshow();
   evas_object_hide(win);
}

