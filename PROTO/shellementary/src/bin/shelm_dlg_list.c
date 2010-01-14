#include "shelm.h"

void 
shelm_list_dialog(const char *window_title, const char *window_text, int window_width, int window_height, const char *window_background, const char **listitems, int args_num, int argc)
{
  Evas_Object *window, *background, *frame, *box, *list, *buttonbar, *button_cancel, *button_ok;
  char buf[PATH_MAX];

  if (window_title)
    window = create_window("shellementary-listdialog", window_title, cancel_callback);
  else
    window = create_window("shellementary-listdialog", _("Select an item"), cancel_callback);

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

  list = create_list(window);

  if (args_num >= argc)
    ecore_main_fd_handler_add (0, ECORE_FD_READ | ECORE_FD_ERROR, _read_stdin_list, list, NULL, NULL);
  else
    {
	  int i;
      for (i = args_num; i < argc; i++)
        {
          elm_list_item_append(list, listitems[i], NULL, NULL, NULL, NULL);
		}
	  elm_list_go(list);
    }

  elm_box_pack_end(box, list);
  evas_object_show(list);

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
  evas_object_smart_callback_add(button_ok, "clicked", list_dialog_callback, list);
  elm_box_pack_end(buttonbar, button_ok);
  evas_object_show(button_ok);

  if (!window_width)
    evas_object_geometry_get(window, NULL, NULL, &window_width, NULL);
  if (!window_height)
    window_height = 400;

  evas_object_resize(window, window_width, window_height);

  evas_object_show(window);
}
