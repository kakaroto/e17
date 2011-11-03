#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "excessive_private.h"

struct _Excessive_File_Object
{
   Elm_Slideshow_Item *item;
};

static int _timeout_value = 3;

static char *
_excessive_folder_item_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Excessive_File_Info *info = data;

   return strdup(info->info.path + info->info.name_start);
}

static Evas_Object *
_excessive_folder_item_object_get(void *data, Evas_Object *obj, const char *part __UNUSED__)
{
   Excessive_File_Info *info = data;
   Evas_Object *ic;

   ic = elm_icon_add(obj);
   elm_icon_order_lookup_set(ic, ELM_ICON_LOOKUP_FDO_THEME);
   elm_icon_standard_set(ic, info->type->type);
   elm_icon_thumb_set(ic, info->info.path, NULL);

   return ic;
}

static void
_excessive_folder_item_object_del(void *data, Evas_Object *obj __UNUSED__)
{
   Excessive_File_Info *info = data;
   /* FIXME: implement a cache of object */
   if (info->link)
     free(info->link);
   free(info);
}

static Eina_Bool
_excessive_is_image(Eio_File *handler __UNUSED__, const Eina_File_Direct_Info *info)
{
   return evas_object_image_extension_can_load_get(info->path + info->name_start);
}

static Evas_Object *
_excessive_image_display_object(Evas_Object *parent)
{
   Evas_Object *result;

   result = elm_slideshow_add(parent);
   evas_object_data_set(result, "excessive/parent", parent);
   elm_slideshow_cache_before_set(result, 3);
   elm_slideshow_cache_after_set(result, 4);
   elm_slideshow_timeout_set(result, 3);
   return result;
}

static void
_excessive_image_display_clear(Evas_Object *display)
{
   elm_slideshow_clear(display);
}

static Evas_Object *
_excessive_image_slideshow_get(void *data, Evas_Object *obj)
{
   /* could use photocam for jpeg */
   Evas_Object *photo = elm_photo_add(obj);
   Excessive_File_Info *file = data;

   elm_photo_file_set(photo, file->info.path);
   elm_photo_fill_inside_set(photo, EINA_TRUE);
   elm_object_style_set(photo, "shadow");

   return photo;
}

static const Elm_Slideshow_Item_Class _excessive_image_display_call = {
  { _excessive_image_slideshow_get, NULL }
};

static int
_excessive_file_cmp(const void *a, const void *b)
{
   const Excessive_File_Info *fa = elm_slideshow_item_data_get(a);
   const Excessive_File_Info *fb = elm_slideshow_item_data_get(b);

   return strcoll(fa->info.path + fa->info.name_start, fb->info.path + fb->info.name_start);
}

static Excessive_File_Object *
_excessive_image_file_insert(Evas_Object *display, const Excessive_File_Info *info)
{
   Elm_Slideshow_Item *item;
   Excessive_File_Object *object;

   item = elm_slideshow_item_sorted_insert(display, &_excessive_image_display_call, info, _excessive_file_cmp);

   object = malloc(sizeof (Excessive_File_Object));
   if (!object)
     {
        elm_slideshow_item_del(item);
        return NULL;
     }
   object->item = item;

   return object;
}

static void
_excessive_image_file_del(Evas_Object *display __UNUSED__, Excessive_File_Object *object)
{
   elm_slideshow_item_del(object->item);
   free(object);
}

static void
_notify_show(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   evas_object_show(data);
}

static void
_next(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_slideshow_next(data);
}

static void
_previous(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_slideshow_previous(data);
}

static void
_mouse_in(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_notify_timeout_set(data, 0.0);
   evas_object_show(data);
}

static void
_mouse_out(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_notify_timeout_set(data, 3.0);
}

static void
_hv_select(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   elm_slideshow_transition_set(evas_object_data_get(obj, "slideshow"), data);
   elm_object_text_set(obj, data);
}

static void
_layout_select(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   elm_slideshow_layout_set(evas_object_data_get(obj, "slideshow"), data);
   elm_object_text_set(obj, data);
}

static void
_start(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *spin = data;
   Evas_Object *slideshow = evas_object_data_get(spin, "excessive/slideshow");

   elm_slideshow_timeout_set(slideshow, elm_spinner_value_get(spin));
}

static void
_stop(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *spin = data;
   Evas_Object *slideshow = evas_object_data_get(spin, "excessive/slideshow");

   elm_slideshow_timeout_set(slideshow, 0.0);
}

static void
_disable(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *btn = data;
   elm_object_disabled_set(btn, EINA_TRUE);
}

static void
_enable(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Evas_Object *btn = data;
   elm_object_disabled_set(btn, EINA_FALSE);
}

static void
_spin(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *slideshow = data;

   if (elm_slideshow_timeout_get(slideshow) > 0)
     {
       elm_slideshow_timeout_set(slideshow, elm_spinner_value_get(obj));
       _timeout_value = elm_spinner_value_get(obj);
     }
}

static void
_quit(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   evas_object_del(evas_object_data_get(data, "excessive/notify"));
   evas_object_data_set(data, "excessive/notify", NULL);
   edje_object_signal_emit(elm_layout_edje_get(data), "hide,content", "code");
   elm_slideshow_timeout_set(evas_object_data_get(data, "excessive/slideshow"), 0.0);
}

static void
_excessive_image_action(Evas_Object *display, Excessive_File_Object *object)
{
   Evas_Object *layout;
   Evas_Object *notify;
   Evas_Object *bx;
   Evas_Object *bt;
   Evas_Object *hv;
   Evas_Object *spin;
   Evas_Object *bt_start;
   Evas_Object *bt_stop;
   Evas_Object *bt_quit;
   Evas_Object *slideshow;
   const Eina_List *l;
   const char *transition;
   char *layout_name;

   layout = evas_object_data_get(display, "excessive/parent");
   elm_slideshow_show(object->item);

   if (evas_object_data_get(layout, "excessive/notify")) return ;

   notify = elm_notify_add(layout);
   elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_BOTTOM);
   elm_notify_timeout_set(notify, 3.0);
   evas_object_data_set(layout, "excessive/notify", notify);

   slideshow = display;

   bx = elm_box_add(layout);
   elm_box_horizontal_set(bx, EINA_TRUE);
   elm_object_content_set(notify, bx);
   evas_object_show(bx);

   evas_object_data_set(layout, "excessive/box", bx);

   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_IN, _mouse_in,
                                  notify);
   evas_object_event_callback_add(bx, EVAS_CALLBACK_MOUSE_OUT, _mouse_out,
                                  notify);

   bt = elm_button_add(layout);
   elm_object_text_set(bt, "Previous");
   evas_object_smart_callback_add(bt, "clicked", _previous, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(layout);
   elm_object_text_set(bt, "Next");
   evas_object_smart_callback_add(bt, "clicked", _next, slideshow);
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   hv = elm_hoversel_add(layout);
   evas_object_data_set(hv, "slideshow", slideshow);
   elm_box_pack_end(bx, hv);
   elm_hoversel_hover_parent_set(hv, layout);
   EINA_LIST_FOREACH(elm_slideshow_transitions_get(slideshow), l, transition)
     elm_hoversel_item_add(hv, transition, NULL, 0, _hv_select, transition);
   elm_object_text_set(hv, eina_list_data_get(elm_slideshow_transitions_get(slideshow)));
   evas_object_show(hv);

   hv = elm_hoversel_add(layout);
   evas_object_data_set(hv, "slideshow", slideshow);
   elm_box_pack_end(bx, hv);
   elm_hoversel_hover_parent_set(hv, layout);
   EINA_LIST_FOREACH(elm_slideshow_layouts_get(slideshow), l, layout_name)
     elm_hoversel_item_add(hv, layout_name, NULL, 0, _layout_select, layout_name);
   elm_object_text_set(hv, elm_slideshow_layout_get(slideshow));
   evas_object_show(hv);

   spin = elm_spinner_add(layout);
   elm_spinner_label_format_set(spin, "%2.0f secs.");
   evas_object_smart_callback_add(spin, "changed", _spin, slideshow);
   elm_spinner_step_set(spin, 1);
   elm_spinner_min_max_set(spin, 1, 30);
   elm_spinner_value_set(spin, _timeout_value);
   elm_box_pack_end(bx, spin);
   evas_object_show(spin);

   bt = elm_button_add(layout);
   bt_start = bt;
   elm_object_text_set(bt, "Start");
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   bt = elm_button_add(layout);
   bt_stop = bt;
   elm_object_text_set(bt, "Stop");
   elm_box_pack_end(bx, bt);
   elm_object_disabled_set(bt, EINA_TRUE);
   evas_object_show(bt);

   bt = elm_button_add(layout);
   bt_quit = bt;
   elm_object_text_set(bt, "Exit");
   elm_box_pack_end(bx, bt);
   evas_object_show(bt);

   evas_object_data_set(spin, "excessive/slideshow", slideshow);
   evas_object_data_set(layout, "excessive/slideshow", slideshow);

   evas_object_smart_callback_add(bt_start, "clicked", _start, spin);
   evas_object_smart_callback_add(bt_start, "clicked", _disable, bt_start);
   evas_object_smart_callback_add(bt_start, "clicked", _enable, bt_stop);

   evas_object_smart_callback_add(bt_stop, "clicked", _stop, spin);
   evas_object_smart_callback_add(bt_stop, "clicked", _disable, bt_stop);
   evas_object_smart_callback_add(bt_stop, "clicked", _enable, bt_start);

   evas_object_smart_callback_add(bt_quit, "clicked", _quit, layout);

   evas_object_event_callback_add(slideshow, EVAS_CALLBACK_MOUSE_UP,
                                  _notify_show, notify);
   evas_object_event_callback_add(slideshow, EVAS_CALLBACK_MOUSE_MOVE,
                                  _notify_show, notify);

   evas_object_show(notify);
}

static const Elm_Gengrid_Item_Class _excessive_image_class = {
  "excessive-image",
  {
    _excessive_folder_item_label_get,
    _excessive_folder_item_object_get,
    NULL,
    _excessive_folder_item_object_del,
  }
};

static const Excessive_Mapping _excessive_image_mapping = {
  "image", &_excessive_image_class,
  {
    _excessive_is_image,
    _excessive_image_display_object,
    _excessive_image_display_clear,
    _excessive_image_file_insert,
    _excessive_image_file_del,
    _excessive_image_action
  }
};

Eina_Bool
excessive_show_image_init(void)
{
   return excessive_browse_register(&_excessive_image_mapping);
}

Eina_Bool
excessive_show_image_shutdown(void)
{
   return EINA_TRUE;
}
