#include "shelm.h"

void 
shelm_question_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background)
{
  Evas_Object *window, *background, *frame, *box, *hbox, *icon, *buttonbar, *button_yes, *button_no;
  char buf[PATH_MAX];

  if (window_title)
    window = create_window("shellementary-questiondialog", window_title, cancel_callback);
  else
    window = create_window("shellementary-questiondialog", _("Question"), cancel_callback);

  background = create_background(window, window_background, EINA_TRUE);
  elm_win_resize_object_add(window, background);
  evas_object_show(background);

  frame = create_frame(window, EINA_TRUE);
  elm_win_resize_object_add(window, frame);
  evas_object_show(frame);

  box = create_box(window, EINA_FALSE);
  elm_frame_content_set(frame, box);
  evas_object_show(box);
  
  hbox = create_box(window, EINA_TRUE);
  elm_box_pack_end(box, hbox);
  evas_object_show(hbox);

  snprintf(buf, sizeof(buf), "%s/icon-question.png", PACKAGE_DATA_DIR);
  icon = create_icon(window, buf);
  elm_box_pack_start(hbox, icon);
  evas_object_show(icon);

  if (window_text)
    {
      Evas_Object *label;
      label = create_label(window, window_text);
      elm_box_pack_end(hbox, label);
      evas_object_show(label);
    }

  buttonbar = create_box(window, EINA_TRUE);
  elm_box_pack_end(box, buttonbar);
  evas_object_show(buttonbar);

  snprintf(buf, sizeof(buf), "%s/icon-cancel.png", PACKAGE_DATA_DIR);
  button_no = create_button(window, buf, _("No"));
  evas_object_smart_callback_add(button_no, "clicked", cancel_callback, NULL);
  elm_box_pack_start(buttonbar, button_no);
  evas_object_show(button_no);

  snprintf(buf, sizeof(buf), "%s/icon-ok.png", PACKAGE_DATA_DIR);
  button_yes = create_button(window, buf, _("Yes"));
  evas_object_smart_callback_add(button_yes, "clicked", destroy, NULL);
  elm_box_pack_end(buttonbar, button_yes);
  evas_object_show(button_yes);

  if (!window_width)
    evas_object_geometry_get(window, NULL, NULL, &window_width, NULL);
  if (!window_height)
    evas_object_geometry_get(window, NULL, NULL, NULL, &window_height);

  evas_object_resize(window, window_width, window_height);

  evas_object_show(window);
}
