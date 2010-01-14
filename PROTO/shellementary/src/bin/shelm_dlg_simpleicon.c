#include "shelm.h"

void 
shelm_simple_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, const char *window_icccm_name, const char *window_default_title, const char *window_default_icon)
{
  Evas_Object *window, *background, *frame, *box, *hbox, *icon, *button_ok;
  char buf[PATH_MAX];

  snprintf(buf, sizeof(buf), "shellementary-%s", window_icccm_name);
  if (window_title)
    window = create_window(buf, window_title, destroy);
  else
    window = create_window(buf, window_default_title, destroy);

  background = create_background(window, window_background, EINA_FALSE);
  elm_win_resize_object_add(window, background);
  evas_object_show(background);

  frame = create_frame(window, EINA_FALSE);
  elm_win_resize_object_add(window, frame);
  evas_object_show(frame);

  box = create_box(window, EINA_FALSE);
  elm_frame_content_set(frame, box);
  evas_object_show(box);

  hbox = create_box(window, EINA_TRUE);
  elm_box_pack_end(box, hbox);
  evas_object_show(hbox);

  icon = create_icon(window, window_default_icon);
  elm_box_pack_start(hbox, icon);
  evas_object_show(icon);

  if (window_text)
    {
      Evas_Object *label;
      label = create_label(window, window_text);
      elm_box_pack_end(hbox, label);
      evas_object_show(label);
    }

  snprintf(buf, sizeof(buf), "%s/icon-ok.png", PACKAGE_DATA_DIR);
  button_ok = create_button(window, buf, _("OK"));
  evas_object_smart_callback_add(button_ok, "clicked", destroy, NULL);
  elm_box_pack_end(box, button_ok);
  evas_object_show(button_ok);

  if (!window_width)
    evas_object_geometry_get(window, NULL, NULL, &window_width, NULL);
  if (!window_height)
    evas_object_geometry_get(window, NULL, NULL, NULL, &window_height);

  evas_object_resize(window, window_width, window_height);

  evas_object_show(window);
}
