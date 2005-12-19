#include "etk_test.h"

/* Creates the window for the paned test */
void etk_test_paned_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vbox, *hbox;
   Etk_Widget *vpaned, *hpaned;
   Etk_Widget *label;
   Etk_Widget *frame;
   Etk_Widget *check_button;
   Etk_Widget *hseparator;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Paned Test"));
   etk_widget_size_request_set(win, 300, 300);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   /* Paned Area */
   vpaned = etk_vpaned_new();
   etk_box_pack_start(ETK_BOX(vbox), vpaned, TRUE, TRUE, 0);

   hpaned = etk_hpaned_new();
   etk_paned_add1(ETK_PANED(vpaned), hpaned, FALSE);

   label = etk_label_new(_("HPaned Child 1"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_add1(ETK_PANED(hpaned), label, TRUE);
   
   label = etk_label_new(_("HPaned Child 2"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_add2(ETK_PANED(hpaned), label, FALSE);
   
   label = etk_label_new(_("VPaned Child 2"));
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_add2(ETK_PANED(vpaned), label, TRUE);
   
   hseparator = etk_hseparator_new();
   etk_box_pack_start(ETK_BOX(vbox), hseparator, FALSE, FALSE, 6);
   
   /* Properties Area */
   hbox = etk_hbox_new(TRUE, 0);
   etk_box_pack_start(ETK_BOX(vbox), hbox, FALSE, TRUE, 0);
   
   frame = etk_frame_new(_("HPaned Properties"));
   etk_box_pack_start(ETK_BOX(hbox), frame, TRUE, TRUE, 0);
   vbox = etk_vbox_new(TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   check_button = etk_check_button_new_with_label(_("Child 1 Expand"));
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check_button), TRUE);
   etk_box_pack_start(ETK_BOX(vbox), check_button, TRUE, TRUE, 0);
   check_button = etk_check_button_new_with_label(_("Child 2 Expand"));
   etk_box_pack_start(ETK_BOX(vbox), check_button, TRUE, TRUE, 0);
   
   frame = etk_frame_new(_("VPaned Properties"));
   etk_box_pack_start(ETK_BOX(hbox), frame, TRUE, TRUE, 0);
   vbox = etk_vbox_new(TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   check_button = etk_check_button_new_with_label(_("Child 1 Expand"));
   etk_box_pack_start(ETK_BOX(vbox), check_button, TRUE, TRUE, 0);
   check_button = etk_check_button_new_with_label(_("Child 2 Expand"));
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check_button), TRUE);
   etk_box_pack_start(ETK_BOX(vbox), check_button, TRUE, TRUE, 0);
   
   etk_widget_show_all(win);
}
