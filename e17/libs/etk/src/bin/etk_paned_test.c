#include "etk_test.h"

static void _etk_test_paned_child1_expand_toggled_cb(Etk_Object *object, void *data);
static void _etk_test_paned_child2_expand_toggled_cb(Etk_Object *object, void *data);

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
   etk_window_title_set(ETK_WINDOW(win), "Etk Paned Test");
   etk_widget_size_request_set(win, 300, 300);
   etk_signal_connect("delete-event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   
   /* Paned Area */
   vpaned = etk_vpaned_new();
   etk_box_append(ETK_BOX(vbox), vpaned, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   hpaned = etk_hpaned_new();
   etk_paned_child1_set(ETK_PANED(vpaned), hpaned, ETK_FALSE);

   label = etk_label_new("HPaned Child 1");
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_child1_set(ETK_PANED(hpaned), label, ETK_TRUE);
   
   label = etk_label_new("HPaned Child 2");
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_child2_set(ETK_PANED(hpaned), label, ETK_FALSE);
   
   label = etk_label_new("VPaned Child 2");
   etk_label_alignment_set(ETK_LABEL(label), 0.5, 0.5);
   etk_paned_child2_set(ETK_PANED(vpaned), label, ETK_TRUE);
   
   hseparator = etk_hseparator_new();
   etk_box_append(ETK_BOX(vbox), hseparator, ETK_BOX_START, ETK_BOX_NONE, 6);
   
   
   /* Hpaned's properties frame */
   hbox = etk_hbox_new(ETK_TRUE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   frame = etk_frame_new("HPaned Properties");
   etk_box_append(ETK_BOX(hbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   check_button = etk_check_button_new_with_label("Child 1 Expand");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check_button), ETK_TRUE);
   etk_signal_connect("toggled", ETK_OBJECT(check_button),
      ETK_CALLBACK(_etk_test_paned_child1_expand_toggled_cb), hpaned);
   
   etk_box_append(ETK_BOX(vbox), check_button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   check_button = etk_check_button_new_with_label("Child 2 Expand");
   etk_box_append(ETK_BOX(vbox), check_button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check_button),
      ETK_CALLBACK(_etk_test_paned_child2_expand_toggled_cb), hpaned);
   
   
   /* Vpaned's properties frame */
   frame = etk_frame_new("VPaned Properties");
   etk_box_append(ETK_BOX(hbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   vbox = etk_vbox_new(ETK_TRUE, 0);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   check_button = etk_check_button_new_with_label("Child 1 Expand");
   etk_box_append(ETK_BOX(vbox), check_button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check_button),
      ETK_CALLBACK(_etk_test_paned_child1_expand_toggled_cb), vpaned);
   
   check_button = etk_check_button_new_with_label("Child 2 Expand");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check_button), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check_button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check_button),
      ETK_CALLBACK(_etk_test_paned_child2_expand_toggled_cb), vpaned);
   
   etk_widget_show_all(win);
}

/* Called when one of the two "Child 1 expand" checkboxes is toggled */
static void _etk_test_paned_child1_expand_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *check;
   Etk_Paned *paned;
   
   if (!(check = ETK_TOGGLE_BUTTON(object)) || !(paned = ETK_PANED(data)))
      return;
   etk_paned_child1_expand_set(paned, etk_toggle_button_active_get(check));
}

/* Called when one of the two "Child 2 expand" checkboxes is toggled */
static void _etk_test_paned_child2_expand_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *check;
   Etk_Paned *paned;
   
   if (!(check = ETK_TOGGLE_BUTTON(object)) || !(paned = ETK_PANED(data)))
      return;
   etk_paned_child2_expand_set(paned, etk_toggle_button_active_get(check));
}
