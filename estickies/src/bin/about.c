#include "stickies.h"

static void _about_destroy_cb(void *data, Evas_Object *obj, void *event_info);

void
_e_about_show()
{
   Evas_Object *win, *background, *frame, *vbox, *logo, *scroller, *desctext, *abouttext, *helptext, *close_button;
   char buf[PATH_MAX];

   if (win)
     {
	evas_object_show(win);
	evas_object_show(background);
	evas_object_show(frame);
	evas_object_show(vbox);
	evas_object_show(logo);
	evas_object_show(scroller);
	evas_object_show(desctext);
	evas_object_show(abouttext);
	evas_object_show(helptext);
	evas_object_show(close_button);
     }

   win = elm_win_add(NULL, "estickies-about", ELM_WIN_BASIC);
   elm_win_title_set(win, "E Stickies "VERSION);
   evas_object_smart_callback_add(win, "delete,request", _about_destroy_cb, NULL);
   evas_object_show(win);

   background = elm_bg_add(win);
   evas_object_size_hint_weight_set(background, 1.0, 1.0);
   elm_win_resize_object_add(win, background);
   evas_object_show(background);

   vbox = elm_box_add(win);
   evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
   elm_box_horizontal_set(vbox, 0);
   elm_win_resize_object_add(win, vbox);
   evas_object_show(vbox);

   /* Logo */
   logo = elm_image_add(win);
   snprintf(buf, sizeof(buf), "%s/images/estickies.png", PACKAGE_DATA_DIR);
   elm_image_file_set(logo, buf, NULL);
   elm_image_scale_set(logo, 0, 0);
   elm_box_pack_end(vbox, logo);
   evas_object_show(logo);

   /* Description */
   frame = elm_frame_add(win);
   elm_frame_label_set(frame, _("What is E Stickies?"));
   evas_object_size_hint_weight_set(frame, 1.0, 1.0);
   evas_object_size_hint_align_set(frame, -1.0, -1.0);
   elm_box_pack_end(vbox, frame);
   evas_object_show(frame);

   scroller = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scroller, 1.0, 1.0);
   evas_object_size_hint_align_set(scroller, -1.0, -1.0);
   elm_frame_content_set(frame, scroller);
   evas_object_show(scroller);

   desctext = elm_entry_add(win);
   elm_entry_editable_set(desctext, 0);
   evas_object_size_hint_weight_set(desctext, 1.0, 1.0);
   evas_object_size_hint_align_set(desctext, -1.0, -1.0);
   elm_entry_entry_set(desctext,
			  _("E Stickies is a sticky notes application that uses Elementary. It uses Elementary's runtime theming support to change the look and feel of the windows and buttons.<br>"));
   elm_scroller_content_set(scroller, desctext);
   evas_object_show(desctext);

   /* Authors */
   frame = elm_frame_add(win);
   elm_frame_label_set(frame, _("Authors"));
   evas_object_size_hint_weight_set(frame, 1.0, 1.0);
   evas_object_size_hint_align_set(frame, -1.0, -1.0);
   elm_box_pack_end(vbox, frame);
   evas_object_show(frame);

   scroller = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scroller, 1.0, 1.0);
   evas_object_size_hint_align_set(scroller, -1.0, -1.0);
   elm_frame_content_set(frame, scroller);
   evas_object_show(scroller);

   abouttext = elm_entry_add(win);
   elm_entry_editable_set(abouttext, 0);
   evas_object_size_hint_weight_set(abouttext, 1.0, 1.0);
   evas_object_size_hint_align_set(abouttext, -1.0, -1.0);
   elm_entry_entry_set(abouttext,
			  _("<b>Code:</b><br>Daniel '<b>quaker</b>' Kolesa<br><b>Old Etk version:</b><br>Hisham '<b>CodeWarrior</b>' Mardam Bey<br><br><b>Themes:</b><br>Brian 'morlenxus' Miculcy"));
   elm_scroller_content_set(scroller, abouttext);
   evas_object_show(abouttext);

   /* Help */
   frame = elm_frame_add(win);
   elm_frame_label_set(frame, _("Common shortcuts"));
   evas_object_size_hint_weight_set(frame, 1.0, 1.0);
   evas_object_size_hint_align_set(frame, -1.0, -1.0);
   elm_box_pack_end(vbox, frame);
   evas_object_show(frame);

   scroller = elm_scroller_add(win);
   evas_object_size_hint_weight_set(scroller, 1.0, 1.0);
   evas_object_size_hint_align_set(scroller, -1.0, -1.0);
   elm_frame_content_set(frame, scroller);
   evas_object_show(scroller);

   helptext = elm_entry_add(win);
   elm_entry_editable_set(helptext, 0);
   evas_object_size_hint_weight_set(helptext, 1.0, 1.0);
   evas_object_size_hint_align_set(helptext, -1.0, -1.0);
   elm_entry_entry_set(helptext,
			  _("<b>control-n:</b> create a new note<br><b>control-q:</b> quit application<br><b>control-c:</b> copy selected text<br><b>control-x:</b> cut selected text<br><b>control-v:</b> paste text from clipboard<br>"));
   elm_scroller_content_set(scroller, helptext);
   evas_object_show(helptext);

   close_button = elm_button_add(win);
   elm_button_label_set(close_button, _("Close"));
   evas_object_smart_callback_add(close_button, "clicked", _about_destroy_cb, win);
   elm_box_pack_end(vbox, close_button);
   evas_object_show(close_button);
}

static void _about_destroy_cb(void *data, Evas_Object *obj, void *event_info)
{
   evas_object_del(data);
}
