#include "shelm.h"

void 
shelm_about_dialog()
{
  Evas_Object *window, *background, *frame, *box, *icon, *label, *sublabel, *scroller, *textinfo, *buttonbar, *button_close;
  char buf[PATH_MAX];

  window = create_window("shellementary-about", _("About Shellementary"), destroy);

  background = create_background(window, NULL, EINA_TRUE);
  elm_win_resize_object_add(window, background);
  evas_object_show(background);

  frame = create_frame(window, EINA_FALSE);
  elm_win_resize_object_add(window, frame);
  evas_object_show(frame);

  box = create_box(window, EINA_FALSE);
  elm_frame_content_set(frame, box);
  evas_object_show(box);

  snprintf(buf, sizeof(buf), "%s/logo.png", PACKAGE_DATA_DIR);
  icon = create_icon(window, buf);
  elm_box_pack_start(box, icon);
  evas_object_show(icon);

  snprintf(buf, sizeof(buf), "<b>Shellementary %s</>", PACKAGE_VERSION);
  label = create_label(window, buf);
  elm_object_scale_set(label, 1.6);
  elm_box_pack_end(box, label);
  evas_object_show(label);

  sublabel = create_label(window, _("<b>Display dialogs from shell scripts</><br>Written as a zenity replacement, supports the same arguments<br><b>http://svn.enlightenment.org/svn/e/trunk/PROTO/shellementary</><br>Based on great Elementary toolkit by <b>raster</> and C programming language.<br><b>License:</> MIT"));
  elm_box_pack_end(box, sublabel);
  evas_object_show(sublabel);

  scroller = create_scroller(window, EINA_TRUE);
  elm_box_pack_end(box, scroller);
  evas_object_show(scroller);

  textinfo = create_entry(window, EINA_TRUE, NULL, EINA_FALSE, EINA_FALSE, EINA_FALSE);
  elm_entry_entry_set(textinfo, _("<b>Author:</> quaker (quaker66@gmail.com)<br>"
			"<br>"
			"<b>Credits:</><br>"
			"<b>Carsten Haitzler (raster)</> "
			"for Enlightenment DR17, great Elementary toolkit and help with developenment<br>"
			"<b>Christopher Michael (devilhorns)</> "
			"for help with developenment<br>"));
  elm_scroller_content_set(scroller, textinfo);
  evas_object_show(textinfo);

  buttonbar = create_box(window, EINA_TRUE);
  elm_box_pack_end(box, buttonbar);
  evas_object_show(buttonbar);

  snprintf(buf, sizeof(buf), "%s/icon-cancel.png", PACKAGE_DATA_DIR);
  button_close = create_button(window, buf, _("Close"));
  evas_object_smart_callback_add(button_close, "clicked", destroy, NULL);
  elm_box_pack_end(buttonbar, button_close);
  evas_object_show(button_close);



  evas_object_show(window);
}
