#include <Eina.h>
#include <Elementary.h>

static void
lighting_control_win_del_cb(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
cb(void *data, Evas_Object *obj, void *event_info)
{
}

void
lighting_control_win_create(void)
{
   Evas_Object *win;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Lighting Controls");
   evas_object_smart_callback_add(win, "delete-request", lighting_control_win_del_cb, NULL);

   Evas_Object *bg;
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   Evas_Object *box;
   box = elm_box_add(win);
   elm_box_horizontal_set(box, EINA_TRUE);
   elm_box_homogenous_set(box, EINA_TRUE);
   evas_object_size_hint_weight_set(box, 1.0, 1.0);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   Evas_Object *devices_box;
   devices_box = elm_box_add(win);
   evas_object_size_hint_weight_set(devices_box, 1.0, 1.0);
   evas_object_size_hint_align_set(devices_box, -1.0, -1.0);
   elm_box_pack_start(box, devices_box);
   evas_object_show(devices_box);

   Evas_Object *label;
   label = elm_label_add(win);
   elm_label_label_set(label, "Devices");
   evas_object_size_hint_weight_set(label, 0.0, 0.0);
   evas_object_size_hint_align_set(label, 0.5, 0.5);
   elm_box_pack_start(devices_box, label);
   evas_object_show(label);

   Evas_Object *devices;
   devices = elm_list_add(win);
   elm_list_always_select_mode_set(devices, EINA_TRUE);
   evas_object_size_hint_align_set(devices, -1.0, -1.0);
   evas_object_size_hint_weight_set(devices, 1.0, 1.0);
   elm_box_pack_end(devices_box, devices);
   evas_object_show(devices);

   // FIXME REMOVE ME
   elm_list_item_append(devices, "Device 1", NULL, NULL, cb, NULL);
   elm_list_item_append(devices, "Device 2", NULL, NULL, cb, NULL);
   elm_list_go(devices);

   Evas_Object *controls;
   controls = elm_box_add(win);
   evas_object_size_hint_weight_set(controls, 1.0, 1.0);
   elm_box_pack_end(box, controls);
   evas_object_show(controls);

   /* Dimming control */
   Evas_Object *dimm;
   dimm = elm_slider_add(win);
   elm_slider_label_set(dimm, "Dimming");
   elm_slider_unit_format_set(dimm, "%%");
   elm_slider_min_max_set(dimm, 0, 100);
   elm_slider_indicator_format_set(dimm, "%3.0f%%");
   elm_slider_span_size_set(dimm, 200);
   evas_object_size_hint_weight_set(dimm, 1.0, 1.0);
   evas_object_size_hint_align_set(dimm, 0.5, 0.5);
   elm_box_pack_start(controls, dimm);
   evas_object_show(dimm);

   Evas_Object *set;
   set = elm_check_add(win);
   elm_check_label_set(set, "Light status");
   elm_check_state_set(set, EINA_FALSE);
   evas_object_size_hint_weight_set(set, 1.0, 1.0);
   evas_object_size_hint_weight_set(set, 0.5, 0.5);
   elm_box_pack_end(controls, set);
   evas_object_show(set);

   evas_object_show(win);
}
