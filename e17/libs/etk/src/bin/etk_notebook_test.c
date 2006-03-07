#include "etk_test.h"
#include "config.h"

static Etk_Widget *_etk_test_notebook_page1_widget_create();
static Etk_Widget *_etk_test_notebook_page2_widget_create();
static Etk_Widget *_etk_test_notebook_page3_widget_create();
static void _etk_test_notebook_next_page(Etk_Object *object, void *data);
static void _etk_test_notebook_prev_page(Etk_Object *object, void *data);

/* Creates the window for the notebook test */
void etk_test_notebook_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *notebook;
   Etk_Widget *page_widget;
   Etk_Widget *hbox;
   Etk_Widget *vbox;
   Etk_Widget *button;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), _("Etk Notebook Test"));
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_pack_start(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);

   notebook = etk_notebook_new();
   etk_box_pack_start(ETK_BOX(vbox), notebook, ETK_TRUE, ETK_TRUE, 0);
   
   button = etk_button_new_with_label(_("Prev"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_notebook_prev_page), notebook);
   etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);
   
   button = etk_button_new_with_label(_("Next"));
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_notebook_next_page), notebook);
   etk_box_pack_start(ETK_BOX(hbox), button, ETK_FALSE, ETK_FALSE, 0);   
   
   page_widget = _etk_test_notebook_page1_widget_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Tab 1 - Table test", page_widget);
   page_widget = _etk_test_notebook_page2_widget_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Tab 2 - Button test", page_widget);
   page_widget = _etk_test_notebook_page3_widget_create();
   etk_notebook_page_append(ETK_NOTEBOOK(notebook), "Tab 3 - Slider test", page_widget);
   
   etk_widget_show_all(win);
}

/* Create the widget for the page 1 */
static Etk_Widget *_etk_test_notebook_page1_widget_create()
{
   Etk_Widget *widget[21];
   Etk_Widget *vbox, *hbox, *table;
   
   widget[0] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_OPEN);
   etk_button_label_set(ETK_BUTTON(widget[0]), _("Set Icon"));

   widget[20] = etk_alignment_new(0.5, 0.5, 0.0, 0.0);
   etk_container_add(ETK_CONTAINER(widget[20]), widget[0]);

   widget[1] = etk_label_new(_("App name"));
   widget[2] = etk_entry_new();

   widget[3] = etk_label_new(_("Generic Info"));
   widget[4] = etk_entry_new();

   widget[5] = etk_label_new(_("Comments"));
   widget[6] = etk_entry_new();

   widget[7] = etk_label_new(_("Executable"));
   widget[8] = etk_entry_new();

   widget[9] = etk_label_new(_("Window name"));
   widget[10] = etk_entry_new();

   widget[11] = etk_label_new(_("Window class"));
   widget[12] = etk_entry_new();

   widget[13] = etk_label_new(_("Startup notify"));
   widget[14] = etk_check_button_new();

   widget[15] = etk_label_new(_("Wait exit"));
   widget[16] = etk_check_button_new();

   widget[17] = etk_button_new_from_stock(ETK_STOCK_DIALOG_CANCEL);
   widget[18] = etk_button_new_from_stock(ETK_STOCK_DOCUMENT_SAVE);
   
   widget[19] = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/test.png");

   vbox = etk_vbox_new(ETK_FALSE, 0);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   table = etk_table_new(2, 10, ETK_FALSE);

   etk_box_pack_start(ETK_BOX(vbox), table, ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_end(ETK_BOX(vbox), hbox, ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_end(ETK_BOX(hbox), widget[18], ETK_FALSE, ETK_FALSE, 0);
   etk_box_pack_end(ETK_BOX(hbox), widget[17], ETK_FALSE, ETK_FALSE, 0);

   etk_table_attach(ETK_TABLE(table), widget[19], 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_table_attach(ETK_TABLE(table), widget[20], 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   etk_table_attach(ETK_TABLE(table), widget[1], 0, 0, 2, 2, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[2], 1, 1, 2, 2);
   etk_table_attach(ETK_TABLE(table), widget[3], 0, 0, 3, 3, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[4], 1, 1, 3, 3);
   etk_table_attach(ETK_TABLE(table), widget[5], 0, 0, 4, 4, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VEXPAND);
   etk_table_attach_defaults(ETK_TABLE(table), widget[6], 1, 1, 4, 4);
   etk_table_attach(ETK_TABLE(table), widget[7], 0, 0, 5, 5, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[8], 1, 1, 5, 5);
   etk_table_attach(ETK_TABLE(table), widget[9], 0, 0, 6, 6, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[10], 1, 1, 6, 6);
   etk_table_attach(ETK_TABLE(table), widget[11], 0, 0, 7, 7, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[12], 1, 1, 7, 7);
   etk_table_attach(ETK_TABLE(table), widget[13], 0, 0, 8, 8, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[14], 1, 1, 8, 8);
   etk_table_attach(ETK_TABLE(table), widget[15], 0, 0, 9, 9, 0, 0, ETK_FILL_POLICY_HFILL);
   etk_table_attach_defaults(ETK_TABLE(table), widget[16], 1, 1, 9, 9);
   
   return vbox;
}

/* Create the widget for the page 2 */
static Etk_Widget *_etk_test_notebook_page2_widget_create()
{
   Etk_Widget *alignment;
   Etk_Widget *vbox;
   Etk_Widget *button_normal;
   Etk_Widget *button_toggle;
   Etk_Widget *button_check;
   Etk_Widget *button_radio;
   Etk_Widget *image;
   
   alignment = etk_alignment_new(0.5, 0.5, 0.5, 0.0);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(alignment), vbox);

   button_normal = etk_button_new_with_label(_("Normal button"));
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);

   image = etk_image_new_from_file(PACKAGE_DATA_DIR "/images/e_icon.png");
   button_normal = etk_button_new_with_label(_("Button with an image"));
   etk_button_image_set(ETK_BUTTON(button_normal), ETK_IMAGE(image));
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);
   
   button_normal = etk_button_new();
   etk_box_pack_start(ETK_BOX(vbox), button_normal, ETK_FALSE, ETK_FALSE, 0);
   
   button_check = etk_check_button_new_with_label(_("Check button"));
   etk_box_pack_start(ETK_BOX(vbox), button_check, ETK_FALSE, ETK_FALSE, 0);
   
   button_check = etk_check_button_new();
   etk_box_pack_start(ETK_BOX(vbox), button_check, ETK_FALSE, ETK_FALSE, 0);

   button_radio = etk_radio_button_new_with_label(_("Radio button"), NULL);
   etk_box_pack_start(ETK_BOX(vbox), button_radio, ETK_FALSE, ETK_FALSE, 0);
   
   button_radio = etk_radio_button_new_from_widget(ETK_RADIO_BUTTON(button_radio));
   etk_box_pack_start(ETK_BOX(vbox), button_radio, ETK_FALSE, ETK_FALSE, 0);
   
   button_toggle = etk_toggle_button_new_with_label(_("Toggle button"));
   etk_box_pack_start(ETK_BOX(vbox), button_toggle, ETK_FALSE, ETK_FALSE, 0);
   
   button_toggle = etk_toggle_button_new();
   etk_box_pack_start(ETK_BOX(vbox), button_toggle, ETK_FALSE, ETK_FALSE, 0);
   
   return alignment;
}

/* Create the widget for the page 3 */
static Etk_Widget *_etk_test_notebook_page3_widget_create()
{
   Etk_Widget *hbox;
   Etk_Widget *slider;
   
   hbox = etk_hbox_new(ETK_TRUE, 0);
   
   slider = etk_hslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_widget_size_request_set(slider, 130, 130);
   etk_box_pack_start(ETK_BOX(hbox), slider, ETK_TRUE, ETK_TRUE, 0);
   
   slider = etk_vslider_new(0.0, 255.0, 128.0, 1.0, 10.0);
   etk_widget_size_request_set(slider, 130, 130);
   etk_box_pack_end(ETK_BOX(hbox), slider, ETK_TRUE, ETK_TRUE, 0);

   return hbox;
}

static void _etk_test_notebook_next_page(Etk_Object *object, void *data)
{
   etk_notebook_next_page(ETK_NOTEBOOK(data));
}

static void _etk_test_notebook_prev_page(Etk_Object *object, void *data)
{
   etk_notebook_prev_page(ETK_NOTEBOOK(data));
}
