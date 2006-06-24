#include "etk_test.h"

static void _etk_test_entry_print_clicked(Etk_Object *object, void *data);
static void _etk_test_entry_password_toggled(Etk_Object *object, void *data);

static Etk_Widget *label = NULL;
static Etk_Widget *entry = NULL;

/* Creates the window for the entry test */
void etk_test_entry_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *table;
   Etk_Widget *button;
   
   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }	
  
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Entry Test");
   etk_signal_connect("delete_event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);	
   
   table = etk_table_new(3, 2, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(win), table);

   entry = etk_entry_new();
   etk_table_attach(ETK_TABLE(table), entry, 0, 0, 0, 0, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);

   button = etk_button_new_with_label("Print text");
   etk_table_attach(ETK_TABLE(table), button, 1, 1, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_entry_print_clicked), NULL);

   button = etk_toggle_button_new_with_label("Toggle password");
   etk_table_attach(ETK_TABLE(table), button, 2, 2, 0, 0, 0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("toggled", ETK_OBJECT(button), ETK_CALLBACK(_etk_test_entry_password_toggled), NULL);
   
   label = etk_label_new(" ");
   etk_table_attach(ETK_TABLE(table), label, 0, 1, 1, 1, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   
   etk_widget_show_all(win);
}

/* Prints the text of the entry in the label */
static void _etk_test_entry_print_clicked(Etk_Object *object, void *data)
{
   etk_label_set(ETK_LABEL(label), etk_entry_text_get(ETK_ENTRY(entry)));
}


/* Turns the entry into a password */
static void _etk_test_entry_password_toggled(Etk_Object *object, void *data)
{
   etk_entry_password_set(ETK_ENTRY(entry), !etk_entry_password_get(ETK_ENTRY(entry)));
}
