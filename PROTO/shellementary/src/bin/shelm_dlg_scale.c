#include "shelm.h"

void 
shelm_scale_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, double slider_value, double slider_min_value, double slider_max_value, const char *slider_step, Eina_Bool slider_partial, Eina_Bool slider_hide_value, Eina_Bool slider_inverted, const char *slider_unit_format, const char *slider_label, const char *slider_icon, Eina_Bool slider_vertical)
{
  Evas_Object *window, *background, *frame, *box, *slider, *buttonbar, *button_cancel, *button_ok;
  char buf[PATH_MAX];

  if (window_title)
    window = create_window("shellementary-scaledialog", window_title, cancel_callback);
  else
    window = create_window("shellementary-scaledialog", _("Adjust the value"), cancel_callback);

  background = create_background(window, window_background, EINA_TRUE);
  elm_win_resize_object_add(window, background);
  evas_object_show(background);

  frame = create_frame(window, EINA_TRUE);
  elm_win_resize_object_add(window, frame);
  evas_object_show(frame);

  box = create_box(window, EINA_FALSE);
  elm_frame_content_set(frame, box);
  evas_object_show(box);

  if (window_text)
    {
      Evas_Object *label;
      label = create_label(window, window_text);
      elm_box_pack_end(box, label);
      evas_object_show(label);
    }

  slider = create_slider(window, slider_value, slider_min_value, slider_max_value, slider_step, slider_partial, slider_hide_value, slider_inverted, slider_unit_format, slider_label, slider_icon, slider_vertical);
  elm_box_pack_end(box, slider);
  evas_object_show(slider);

  buttonbar = create_box(window, EINA_TRUE);
  elm_box_pack_end(box, buttonbar);
  evas_object_show(buttonbar);

  snprintf(buf, sizeof(buf), "%s/icon-cancel.png", PACKAGE_DATA_DIR);
  button_cancel = create_button(window, buf, _("Cancel"));
  evas_object_smart_callback_add(button_cancel, "clicked", cancel_callback, NULL);
  elm_box_pack_start(buttonbar, button_cancel);
  evas_object_show(button_cancel);

  snprintf(buf, sizeof(buf), "%s/icon-ok.png", PACKAGE_DATA_DIR);
  button_ok = create_button(window, buf, _("OK"));
  evas_object_smart_callback_add(button_ok, "clicked", slider_callback, slider);
  elm_box_pack_end(buttonbar, button_ok);
  evas_object_show(button_ok);

  if (!window_width)
    evas_object_geometry_get(window, NULL, NULL, &window_width, NULL);
  if (!window_height)
    evas_object_geometry_get(window, NULL, NULL, NULL, &window_height);

  evas_object_resize(window, window_width, window_height);

  evas_object_show(window);
}
