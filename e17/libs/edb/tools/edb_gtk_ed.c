#include "../src/Edb.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

GtkWidget *main_window;
char *db_file = NULL;
gint  ignore_changes = 0;
gint  row_selected = -1;

GtkWidget* create_window (void);
GtkWidget* create_keyname (void);
GtkWidget* create_fileselection (void);

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_destroy_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_open_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_int_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_str_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_float_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_data_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_list_select_row                     (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_list_unselect_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_integer_changed                     (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_string_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_float_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_add_clicked                     (GtkButton       *button,
				    gpointer         user_data);

void
on_delete_clicked                     (GtkButton       *button,
				       gpointer         user_data);

void
on_save_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_ok_clicked                          (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_fileselection_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_fileselection_destroy_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data);

int
sort_compare(const void *v1, const void *v2);

void
new_db(GtkWidget *window, char *file);

GtkWidget*
create_window (void)
{
   GtkWidget *window;
   GtkWidget *vbox1;
   GtkWidget *menubar1;
   GtkWidget *file;
   GtkWidget *file_menu;
   GtkAccelGroup *file_menu_accels;
   GtkWidget *open;
   GtkWidget *separator2;
   GtkWidget *save;
   GtkWidget *separator1;
   GtkWidget *exit;
   GtkWidget *hpaned1;
   GtkWidget *scrolledwindow1;
   GtkWidget *list;
   GtkWidget *label5;
   GtkWidget *label6;
   GtkWidget *label7;
   GtkWidget *vbox2;
   GtkWidget *hbox2;
   GtkWidget *label8;
   GtkWidget *type;
   GtkWidget *type_menu;
   GtkWidget *glade_menuitem;
   GtkWidget *frame1;
   GtkWidget *notebook1;
   GtkObject *integer_adj;
   GtkWidget *integer;
   GtkWidget *label1;
   GtkWidget *scrolledwindow2;
   GtkWidget *string;
   GtkWidget *label2;
   GtkObject *float_adj;
   GtkWidget *flot;
   GtkWidget *label3;
   GtkWidget *label9;
   GtkWidget *label4;
   GtkWidget *empty_notebook_page;
   GtkWidget *label10;
   GtkWidget *hbox3;
   GtkWidget *add;
   GtkWidget *button4;
   GtkWidget *key;
   GtkWidget *hseparator1;
   GtkWidget *hbuttonbox2;
   GtkWidget *sve;
   
   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_object_set_data (GTK_OBJECT (window), "window", window);
   gtk_window_set_title (GTK_WINDOW (window), "E DB Editor");
   gtk_window_set_policy (GTK_WINDOW (window), FALSE, TRUE, TRUE);
   gtk_window_set_wmclass (GTK_WINDOW (window), "edb_gtk_ed", "main");
   
   vbox1 = gtk_vbox_new (FALSE, 0);
   gtk_widget_ref (vbox1);
   gtk_object_set_data_full (GTK_OBJECT (window), "vbox1", vbox1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (vbox1);
   gtk_container_add (GTK_CONTAINER (window), vbox1);
   
   menubar1 = gtk_menu_bar_new ();
   gtk_widget_ref (menubar1);
   gtk_object_set_data_full (GTK_OBJECT (window), "menubar1", menubar1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (menubar1);
   gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);
   
   file = gtk_menu_item_new_with_label ("File");
   gtk_widget_ref (file);
   gtk_object_set_data_full (GTK_OBJECT (window), "file", file,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (file);
   gtk_container_add (GTK_CONTAINER (menubar1), file);
   
   file_menu = gtk_menu_new ();
   gtk_widget_ref (file_menu);
   gtk_object_set_data_full (GTK_OBJECT (window), "file_menu", file_menu,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_menu_item_set_submenu (GTK_MENU_ITEM (file), file_menu);
   file_menu_accels = gtk_menu_ensure_uline_accel_group (GTK_MENU (file_menu));
   
   open = gtk_menu_item_new_with_label ("Open ...");
   gtk_widget_ref (open);
   gtk_object_set_data_full (GTK_OBJECT (window), "open", open,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (open);
   gtk_container_add (GTK_CONTAINER (file_menu), open);

   separator1 = gtk_menu_item_new ();
   gtk_widget_ref (separator1);
   gtk_object_set_data_full (GTK_OBJECT (window), "separator1", separator1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (separator1);
   gtk_container_add (GTK_CONTAINER (file_menu), separator1);
   gtk_widget_set_sensitive (separator1, FALSE);
   
   exit = gtk_menu_item_new_with_label ("Exit");
   gtk_widget_ref (exit);
   gtk_object_set_data_full (GTK_OBJECT (window), "exit", exit,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (exit);
   gtk_container_add (GTK_CONTAINER (file_menu), exit);
   
   hpaned1 = gtk_hpaned_new ();
   gtk_widget_ref (hpaned1);
   gtk_object_set_data_full (GTK_OBJECT (window), "hpaned1", hpaned1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (hpaned1);
   gtk_box_pack_start (GTK_BOX (vbox1), hpaned1, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (hpaned1), 4);
   gtk_paned_set_gutter_size (GTK_PANED (hpaned1), 12);
   gtk_paned_set_position (GTK_PANED (hpaned1), 280);
   
   scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
   gtk_widget_ref (scrolledwindow1);
   gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow1", scrolledwindow1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (scrolledwindow1);
   gtk_paned_pack1 (GTK_PANED (hpaned1), scrolledwindow1, FALSE, TRUE);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   
   list = gtk_clist_new (3);
   gtk_widget_ref (list);
   gtk_object_set_data_full (GTK_OBJECT (window), "list", list,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (list);
   gtk_container_add (GTK_CONTAINER (scrolledwindow1), list);
   gtk_widget_set_usize (list, 240, 240);
   gtk_container_set_border_width (GTK_CONTAINER (list), 4);
   gtk_clist_set_column_width (GTK_CLIST (list), 0, 36);
   gtk_clist_set_column_width (GTK_CLIST (list), 1, 120);
   gtk_clist_set_column_width (GTK_CLIST (list), 2, 48);
   gtk_clist_set_selection_mode (GTK_CLIST (list), GTK_SELECTION_BROWSE);
   gtk_clist_column_titles_show (GTK_CLIST (list));
   
   label5 = gtk_label_new ("Type");
   gtk_widget_ref (label5);
   gtk_object_set_data_full (GTK_OBJECT (window), "label5", label5,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label5);
   gtk_clist_set_column_widget (GTK_CLIST (list), 0, label5);
   
   label6 = gtk_label_new ("Key");
   gtk_widget_ref (label6);
   gtk_object_set_data_full (GTK_OBJECT (window), "label6", label6,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label6);
   gtk_clist_set_column_widget (GTK_CLIST (list), 1, label6);
   
   label7 = gtk_label_new ("Value");
   gtk_widget_ref (label7);
   gtk_object_set_data_full (GTK_OBJECT (window), "label7", label7,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label7);
   gtk_clist_set_column_widget (GTK_CLIST (list), 2, label7);
   
   vbox2 = gtk_vbox_new (FALSE, 0);
   gtk_widget_ref (vbox2);
   gtk_object_set_data_full (GTK_OBJECT (window), "vbox2", vbox2,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (vbox2);
   gtk_paned_pack2 (GTK_PANED (hpaned1), vbox2, TRUE, TRUE);
   
   hbox2 = gtk_hbox_new (FALSE, 0);
   gtk_widget_ref (hbox2);
   gtk_object_set_data_full (GTK_OBJECT (window), "hbox2", hbox2,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (hbox2);
   gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, TRUE, 0);
   
   label8 = gtk_label_new ("Type:");
   gtk_widget_ref (label8);
   gtk_object_set_data_full (GTK_OBJECT (window), "label8", label8,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label8);
   gtk_box_pack_start (GTK_BOX (hbox2), label8, FALSE, FALSE, 0);
   gtk_misc_set_padding (GTK_MISC (label8), 4, 4);
   
   type = gtk_option_menu_new ();
   gtk_widget_ref (type);
   gtk_object_set_data_full (GTK_OBJECT (window), "type", type,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (type);
   gtk_box_pack_start (GTK_BOX (hbox2), type, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (type), 4);
   type_menu = gtk_menu_new ();
   glade_menuitem = gtk_menu_item_new_with_label ("Integer");
   gtk_widget_show (glade_menuitem);
   gtk_signal_connect (GTK_OBJECT (glade_menuitem), "activate",
		       GTK_SIGNAL_FUNC (on_int_activate),
		       NULL);
   gtk_menu_append (GTK_MENU (type_menu), glade_menuitem);
   glade_menuitem = gtk_menu_item_new_with_label ("String");
   gtk_widget_show (glade_menuitem);
   gtk_signal_connect (GTK_OBJECT (glade_menuitem), "activate",
		       GTK_SIGNAL_FUNC (on_str_activate),
		       NULL);
   gtk_menu_append (GTK_MENU (type_menu), glade_menuitem);
   glade_menuitem = gtk_menu_item_new_with_label ("Float");
   gtk_widget_show (glade_menuitem);
   gtk_signal_connect (GTK_OBJECT (glade_menuitem), "activate",
		       GTK_SIGNAL_FUNC (on_float_activate),
		       NULL);
   gtk_menu_append (GTK_MENU (type_menu), glade_menuitem);
   glade_menuitem = gtk_menu_item_new_with_label ("Binary Data");
   gtk_widget_show (glade_menuitem);
   gtk_signal_connect (GTK_OBJECT (glade_menuitem), "activate",
		       GTK_SIGNAL_FUNC (on_data_activate),
		       NULL);
   gtk_menu_append (GTK_MENU (type_menu), glade_menuitem);
   gtk_option_menu_set_menu (GTK_OPTION_MENU (type), type_menu);
   
   frame1 = gtk_frame_new (NULL);
   gtk_widget_ref (frame1);
   gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (frame1);
   gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);
   
   notebook1 = gtk_notebook_new ();
   gtk_widget_ref (notebook1);
   gtk_object_set_data_full (GTK_OBJECT (window), "notebook1", notebook1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (notebook1);
   gtk_container_add (GTK_CONTAINER (frame1), notebook1);
   gtk_container_set_border_width (GTK_CONTAINER (notebook1), 4);
   GTK_WIDGET_UNSET_FLAGS (notebook1, GTK_CAN_FOCUS);
   gtk_notebook_set_show_tabs (GTK_NOTEBOOK (notebook1), FALSE);
   gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook1), FALSE);
   gtk_notebook_set_tab_hborder (GTK_NOTEBOOK (notebook1), 0);
   gtk_notebook_set_tab_vborder (GTK_NOTEBOOK (notebook1), 0);
   
   integer_adj = gtk_adjustment_new (0, -2.14748e+09, 2.14748e+09, 1, 10, 10);
   integer = gtk_spin_button_new (GTK_ADJUSTMENT (integer_adj), 1, 0);
   gtk_widget_ref (integer);
   gtk_object_set_data_full (GTK_OBJECT (window), "integer", integer,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (integer);
   gtk_container_add (GTK_CONTAINER (notebook1), integer);
   gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (integer), TRUE);
   
   label1 = gtk_label_new ("label1");
   gtk_widget_ref (label1);
   gtk_object_set_data_full (GTK_OBJECT (window), "label1", label1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label1);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label1);
   
   scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
   gtk_widget_ref (scrolledwindow2);
   gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow2", scrolledwindow2,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (scrolledwindow2);
   gtk_container_add (GTK_CONTAINER (notebook1), scrolledwindow2);
   gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow2), 4);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
   
   string = gtk_text_new (NULL, NULL);
   gtk_widget_ref (string);
   gtk_object_set_data_full (GTK_OBJECT (window), "string", string,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (string);
   gtk_container_add (GTK_CONTAINER (scrolledwindow2), string);
   gtk_text_set_editable (GTK_TEXT (string), TRUE);
   
   label2 = gtk_label_new ("label2");
   gtk_widget_ref (label2);
   gtk_object_set_data_full (GTK_OBJECT (window), "label2", label2,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label2);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label2);
   
   float_adj = gtk_adjustment_new (0, -1e+16, 1e+16, 1, 10, 10);
   flot = gtk_spin_button_new (GTK_ADJUSTMENT (float_adj), 0.1, 3);
   gtk_widget_ref (flot);
   gtk_object_set_data_full (GTK_OBJECT (window), "float", flot,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (flot);
   gtk_container_add (GTK_CONTAINER (notebook1), flot);
   gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (flot), TRUE);
   
   label3 = gtk_label_new ("label3");
   gtk_widget_ref (label3);
   gtk_object_set_data_full (GTK_OBJECT (window), "label3", label3,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label3);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label3);
   
   label9 = gtk_label_new ("This value contains\nbinary data that is\nspecially encoded\nby the application\nusing this database\nand cannot be edited\nby this generic tool.");
   gtk_widget_ref (label9);
   gtk_object_set_data_full (GTK_OBJECT (window), "label9", label9,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label9);
   gtk_container_add (GTK_CONTAINER (notebook1), label9);
   
   label4 = gtk_label_new ("label4");
   gtk_widget_ref (label4);
   gtk_object_set_data_full (GTK_OBJECT (window), "label4", label4,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label4);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 3), label4);
   
   empty_notebook_page = gtk_vbox_new (FALSE, 0);
   gtk_widget_show (empty_notebook_page);
   gtk_container_add (GTK_CONTAINER (notebook1), empty_notebook_page);
   
   label10 = gtk_label_new ("label10");
   gtk_widget_ref (label10);
   gtk_object_set_data_full (GTK_OBJECT (window), "label10", label10,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label10);
   gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 4), label10);
   
   hbox3 = gtk_hbox_new (TRUE, 0);
   gtk_widget_ref (hbox3);
   gtk_object_set_data_full (GTK_OBJECT (window), "hbox3", hbox3,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (hbox3);
   gtk_box_pack_start (GTK_BOX (vbox2), hbox3, FALSE, FALSE, 0);
   
   add = gtk_button_new_with_label ("Add");
   gtk_widget_ref (add);
   gtk_object_set_data_full (GTK_OBJECT (window), "add", add,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (add);
   gtk_box_pack_start (GTK_BOX (hbox3), add, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (add), 4);
   
   button4 = gtk_button_new_with_label ("Delete");
   gtk_widget_ref (button4);
   gtk_object_set_data_full (GTK_OBJECT (window), "button4", button4,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (button4);
   gtk_box_pack_start (GTK_BOX (hbox3), button4, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (button4), 4);
   
   key = gtk_entry_new ();
   gtk_widget_ref (key);
   gtk_object_set_data_full (GTK_OBJECT (window), "key", key,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (key);
   gtk_box_pack_start (GTK_BOX (vbox1), key, FALSE, FALSE, 4);
   gtk_entry_set_editable (GTK_ENTRY (key), FALSE);
   
   hseparator1 = gtk_hseparator_new ();
   gtk_widget_ref (hseparator1);
   gtk_object_set_data_full (GTK_OBJECT (window), "hseparator1", hseparator1,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (hseparator1);
   gtk_box_pack_start (GTK_BOX (vbox1), hseparator1, FALSE, FALSE, 4);
   
   hbuttonbox2 = gtk_hbutton_box_new ();
   gtk_widget_ref (hbuttonbox2);
   gtk_object_set_data_full (GTK_OBJECT (window), "hbuttonbox2", hbuttonbox2,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (hbuttonbox2);
   gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox2, FALSE, FALSE, 0);
   
   save = gtk_button_new_with_label ("Done");
   gtk_widget_ref (save);
   gtk_object_set_data_full (GTK_OBJECT (window), "save", save,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (save);
   gtk_container_add (GTK_CONTAINER (hbuttonbox2), save);
   GTK_WIDGET_SET_FLAGS (save, GTK_CAN_DEFAULT);
      
   gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		       GTK_SIGNAL_FUNC (on_window_delete_event),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (window), "destroy_event",
		       GTK_SIGNAL_FUNC (on_window_destroy_event),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (open), "activate",
		       GTK_SIGNAL_FUNC (on_open_activate),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (exit), "activate",
		       GTK_SIGNAL_FUNC (on_exit_activate),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (list), "select_row",
		       GTK_SIGNAL_FUNC (on_list_select_row),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (list), "unselect_row",
		       GTK_SIGNAL_FUNC (on_list_unselect_row),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (integer), "changed",
		       GTK_SIGNAL_FUNC (on_integer_changed),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (string), "changed",
		       GTK_SIGNAL_FUNC (on_string_changed),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (flot), "changed",
		       GTK_SIGNAL_FUNC (on_float_changed),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (add), "clicked",
		       GTK_SIGNAL_FUNC (on_add_clicked),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (button4), "clicked",
		       GTK_SIGNAL_FUNC (on_delete_clicked),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (save), "clicked",
		       GTK_SIGNAL_FUNC (on_save_clicked),
		       NULL);
   
   return window;
}

GtkWidget*
create_keyname (void)
{
   GtkWidget *keyname;
   GtkWidget *dialog_vbox1;
   GtkWidget *hbox4;
   GtkWidget *label11;
   GtkWidget *key;
   GtkWidget *dialog_action_area1;
   GtkWidget *ok;
   
   keyname = gtk_dialog_new ();
   gtk_object_set_data (GTK_OBJECT (keyname), "keyname", keyname);
   gtk_window_set_title (GTK_WINDOW (keyname), "New Key Name");
   gtk_window_set_position (GTK_WINDOW (keyname), GTK_WIN_POS_MOUSE);
   gtk_window_set_modal (GTK_WINDOW (keyname), TRUE);
   gtk_window_set_policy (GTK_WINDOW (keyname), FALSE, FALSE, TRUE);
   gtk_window_set_wmclass (GTK_WINDOW (keyname), "edb_gtk_ed", "new_key");
   
   dialog_vbox1 = GTK_DIALOG (keyname)->vbox;
   gtk_object_set_data (GTK_OBJECT (keyname), "dialog_vbox1", dialog_vbox1);
   gtk_widget_show (dialog_vbox1);
   
   hbox4 = gtk_hbox_new (FALSE, 0);
   gtk_widget_ref (hbox4);
   gtk_object_set_data_full (GTK_OBJECT (keyname), "hbox4", hbox4,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (hbox4);
   gtk_box_pack_start (GTK_BOX (dialog_vbox1), hbox4, TRUE, TRUE, 0);
   gtk_container_set_border_width (GTK_CONTAINER (hbox4), 8);
   
   label11 = gtk_label_new ("New Key:");
   gtk_widget_ref (label11);
   gtk_object_set_data_full (GTK_OBJECT (keyname), "label11", label11,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (label11);
   gtk_box_pack_start (GTK_BOX (hbox4), label11, FALSE, FALSE, 0);
   gtk_misc_set_padding (GTK_MISC (label11), 8, 8);
   
   key = gtk_entry_new ();
   gtk_widget_ref (key);
   gtk_object_set_data_full (GTK_OBJECT (keyname), "key", key,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (key);
   gtk_box_pack_start (GTK_BOX (hbox4), key, TRUE, TRUE, 0);
   
   dialog_action_area1 = GTK_DIALOG (keyname)->action_area;
   gtk_object_set_data (GTK_OBJECT (keyname), "dialog_action_area1", dialog_action_area1);
   gtk_widget_show (dialog_action_area1);
   gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);
   
   ok = gtk_button_new_with_label ("OK");
   gtk_widget_ref (ok);
   gtk_object_set_data_full (GTK_OBJECT (keyname), "ok", ok,
			     (GtkDestroyNotify) gtk_widget_unref);
   gtk_widget_show (ok);
   gtk_box_pack_start (GTK_BOX (dialog_action_area1), ok, TRUE, TRUE, 0);
   
   gtk_signal_connect (GTK_OBJECT (ok), "clicked",
		       GTK_SIGNAL_FUNC (on_ok_clicked),
		       NULL);
   
   gtk_widget_grab_focus (key);
   return keyname;
}

GtkWidget*
create_fileselection (void)
{
   GtkWidget *fileselection;
   GtkWidget *ok_button1;
   GtkWidget *cancel_button1;
   
   fileselection = gtk_file_selection_new ("Select File");
   gtk_object_set_data (GTK_OBJECT (fileselection), "fileselection", fileselection);
   gtk_container_set_border_width (GTK_CONTAINER (fileselection), 2);
   GTK_WINDOW (fileselection)->type = GTK_WINDOW_DIALOG;
   gtk_window_set_modal (GTK_WINDOW (fileselection), TRUE);
   gtk_window_set_wmclass (GTK_WINDOW (fileselection), "edb_gtk_ed", "file_selector");
   
   ok_button1 = GTK_FILE_SELECTION (fileselection)->ok_button;
   gtk_object_set_data (GTK_OBJECT (fileselection), "ok_button1", ok_button1);
   gtk_widget_show (ok_button1);
   GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);
   
   cancel_button1 = GTK_FILE_SELECTION (fileselection)->cancel_button;
   gtk_object_set_data (GTK_OBJECT (fileselection), "cancel_button1", cancel_button1);
   gtk_widget_show (cancel_button1);
   GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);
   
   gtk_signal_connect (GTK_OBJECT (fileselection), "delete_event",
		       GTK_SIGNAL_FUNC (on_fileselection_delete_event),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (fileselection), "destroy_event",
		       GTK_SIGNAL_FUNC (on_fileselection_destroy_event),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
		       GTK_SIGNAL_FUNC (on_ok_button_clicked),
		       NULL);
   gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
		       GTK_SIGNAL_FUNC (on_cancel_button_clicked),
		       NULL);
   
   return fileselection;
}

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   e_db_flush();
   exit(0);
   return FALSE;
}


gboolean
on_window_destroy_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   e_db_flush();
   exit(0);
   return FALSE;
}


void
on_open_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   GtkWidget *filesel;
   
   filesel = create_fileselection();
   gtk_widget_show(filesel);
}


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   e_db_flush();
   exit(0);
}


void
on_int_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "int");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "0");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "integer");
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), 0);
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 0);
   E_DB_INT_SET(db_file, text, 0);
   e_db_flush();
}

void
on_str_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "str");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "string");
     {
	guint pos;
	
	pos = gtk_text_get_length(GTK_TEXT(w));
	if (pos > 0)
	  {
	     gtk_editable_delete_text(GTK_EDITABLE(w), 0, pos);
	  }
     }
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 1);
   E_DB_STR_SET(db_file, text, "");
   e_db_flush();
}
void
on_float_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "float");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "0.0");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "float");
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), 0);
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 2);
   E_DB_FLOAT_SET(db_file, text, 0);
   e_db_flush();
}
void
on_data_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "?");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 3);
     {
	E_DB_File *db;
	
	db = e_db_open(db_file);
	if (db)
	  {
	     int data[1];
	     
	     data[0] = 0;
	     e_db_data_set(db, text, data, sizeof(int));
	     e_db_type_set(db, text, "?");
	     e_db_close(db);
	  }
     }
   e_db_flush();
}

void
on_list_select_row                     (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w, *top;
   
   ignore_changes ++;
   row_selected = row;   
   top = gtk_widget_get_toplevel(GTK_WIDGET(clist));
   gtk_clist_get_text(clist, row, 1, &text);
   gtk_clist_get_text(clist, row, 0, &type);
   w = gtk_object_get_data(GTK_OBJECT(top), "key");
   gtk_entry_set_text(GTK_ENTRY(w), text);
   if (!strcmp(type, "int"))
     {
	int val;
	int ok;
	
	w = gtk_object_get_data(GTK_OBJECT(top), "integer");
	E_DB_INT_GET(db_file, text, val, ok);
	if (ok)
	  {
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), (gfloat)val);
	  }
	else
	  {
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), 0);
	  }
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 0);   
     }
   else if (!strcmp(type, "str"))
     {
	char *val;
	int ok;
	
	w = gtk_object_get_data(GTK_OBJECT(top), "string");
	E_DB_STR_GET(db_file, text, val, ok);
	if (ok)
	  {
	     guint pos;

	     pos = gtk_text_get_length(GTK_TEXT(w));
	     if (pos > 0)
	       {
		  gtk_editable_delete_text(GTK_EDITABLE(w), 0, pos);
	       }
	     gtk_editable_insert_text(GTK_EDITABLE(w), val, strlen(val), &pos);
	     free(val);
	  }
	else
	  {
	     guint pos;

	     pos = gtk_text_get_length(GTK_TEXT(w));
	     if (pos > 0)
	       {
		  gtk_editable_delete_text(GTK_EDITABLE(w), 0, pos);
	       }
	  }
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 1);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 1);
     }
   else if (!strcmp(type, "float"))
     {
	float val;
	int ok;
	
	w = gtk_object_get_data(GTK_OBJECT(top), "float");
	E_DB_FLOAT_GET(db_file, text, val, ok);
	if (ok)
	  {
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), (gfloat)val);
	  }
	else
	  {
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), 0);
	  }
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 2);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 2);
     }
   else
     {
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 3);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 3);
     }
   ignore_changes --;
   e_db_flush();
}


void
on_list_unselect_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   
   ignore_changes ++;
   row_selected = -1;   
   top = gtk_widget_get_toplevel(GTK_WIDGET(clist));
   w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 3);
   w = gtk_object_get_data(GTK_OBJECT(top), "type");
   gtk_option_menu_set_history(GTK_OPTION_MENU(w), 3);
   ignore_changes --;
}


void
on_integer_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text = NULL;
   gchar t[256];
   int val;
   
   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(editable));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   w = gtk_object_get_data(GTK_OBJECT(top), "integer");
   val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
   g_snprintf(t, sizeof(t), "%i", val);
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, t);
   E_DB_INT_SET(db_file, text, val);
   e_db_flush();
}


void
on_string_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text = NULL;
   gchar t[256];
   char *val;
   guint pos;
   
   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(editable));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   w = gtk_object_get_data(GTK_OBJECT(top), "string");
   
   pos = gtk_text_get_length(GTK_TEXT(w));
   val = gtk_editable_get_chars(GTK_EDITABLE(w), 0, pos);

   if (val)
     {
	if (strlen(val) > 32)
	  {
	     strncpy(t, val, 32);
	     t[32] = 0;
	     strcat(t, "...");
	  }
	else
	   strcpy(t, val);
     }
   else
      strcpy(t, "");
   
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, t);
   E_DB_STR_SET(db_file, text, val);
   if (val)
      g_free(val);
   e_db_flush();
}


void
on_float_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text = NULL;
   gchar t[256];
   float val;
   
   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(editable));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   w = gtk_object_get_data(GTK_OBJECT(top), "float");
   val = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
   g_snprintf(t, sizeof(t), "%1.6f", val);
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, t);
   E_DB_FLOAT_SET(db_file, text, val);
   e_db_flush();
}


void
on_delete_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text = NULL;
   gchar t[256];
   float val;
   
   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   E_DB_DEL(db_file, text);
   gtk_clist_remove(GTK_CLIST(w), row_selected);
   e_db_flush();
}


void
on_add_clicked                     (GtkButton       *button,
				    gpointer         user_data)
{
   GtkWidget *newkey;
   
   newkey = create_keyname();
   gtk_widget_show(newkey);  
}


void
on_save_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
   e_db_flush();
   exit(0);
}


void
on_ok_clicked                          (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text[3];
   gint entry;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   w = gtk_object_get_data(GTK_OBJECT(top), "key");
   text[0] = "int";   
   text[1] = gtk_entry_get_text(GTK_ENTRY(w));
   text[2] = "0";
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   entry = gtk_clist_append(GTK_CLIST(w), text);
   gtk_clist_set_sort_column(GTK_CLIST(w), 1);
   gtk_clist_set_sort_type(GTK_CLIST(w), GTK_SORT_ASCENDING);
   gtk_clist_sort(GTK_CLIST(w));
   gtk_clist_select_row(GTK_CLIST(w), entry, 0);
   gtk_clist_moveto(GTK_CLIST(w), entry, 0, 0.5, 0.5);
   printf("%s %s\n", db_file, text[1]);
   E_DB_INT_SET(db_file, text[1], 0);
   gtk_widget_destroy(top);
   e_db_flush();
}


gboolean
on_fileselection_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gtk_widget_destroy(widget);
   return FALSE;
}


gboolean
on_fileselection_destroy_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   return FALSE;
}


void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   if (db_file) g_free(db_file);
   db_file = NULL;
   db_file = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
   new_db(main_window, db_file);
   gtk_widget_destroy(top);
}


void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *top;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   gtk_widget_destroy(top);
}

int
sort_compare(const void *v1, const void *v2)
{
   return strcmp(*(char **)v1, *(char **)v2);
}

void
new_db(GtkWidget *window, char *file)
{
   GtkWidget *w;

   ignore_changes ++;
   w = gtk_object_get_data(GTK_OBJECT(window), "list");
   gtk_clist_freeze(GTK_CLIST(w));
   gtk_clist_clear(GTK_CLIST(w));
   
   if (file)
     {
	E_DB_File *db;
	char **keys;
	int keys_num;
	
	db = e_db_open_read(file);
	w = gtk_object_get_data(GTK_OBJECT(window), "list");
	
	keys = e_db_dump_key_list(file, &keys_num);
	if (keys)
	  {
	     int i;
	     
	     qsort(keys, keys_num, sizeof(char *), sort_compare);
	     for (i = 0; i < keys_num; i++)
	       {
		  char *t;
		  char *type;
		  
		  type = e_db_type_get(db, keys[i]);
		  if (type) t = type;
		  else t = "?";
		  
		    {
		       gchar *text[3];
		       char val[256];
		       int i_val;
		       float f_val;
		       char *s_val;
		       
		       val[0] = 0;
		       if (!strcmp(t, "int"))
			 {
			    if (e_db_int_get(db, keys[i], &i_val))
			       g_snprintf(val, sizeof(val), "%i", i_val);
			 }
		       else if (!strcmp(t, "str"))
			 {
			    char s_val2[64];
			    
			    s_val = NULL;
			    s_val = e_db_str_get(db, keys[i]);
			    if (s_val)
			      {
				 strncpy(s_val2, s_val, 32);
				 if (strlen(s_val) > 32)
				   {
				      s_val2[32] = 0;
				      strcat(s_val2, "...");
				   }
				 free(s_val);
				 g_snprintf(val, sizeof(val), "%s", s_val2);
			      }
			 }
		       else if (!strcmp(t, "float"))
			 {
			    if (e_db_float_get(db, keys[i], &f_val))
			       g_snprintf(val, sizeof(val), "%1.6f", f_val);
			 }
		       text[0] = t;
		       text[1] = keys[i];
		       text[2] = val;
		       gtk_clist_append(GTK_CLIST(w), text);
		    }
		  if (type) free(type);
		  free(keys[i]);
	       }
	     free(keys);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
	if (db) e_db_close(db);
	e_db_flush();
     }
   ignore_changes --;
}

int
main(int argc, char **argv)
{
   if (argc > 1) db_file = g_strdup(argv[1]);
   
   gtk_init(&argc, &argv);
   
   main_window = create_window();
   new_db(main_window, db_file);
   gtk_widget_show(main_window);
   
   gtk_main();
   
   return 0;
}
