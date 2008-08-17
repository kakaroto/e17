#include <gtk/gtk.h>


void
init_type_menu                         (void);

void
new_db                                 (GtkWidget       *window,
                                        char            *file);

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
on_save_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit_activate                       (GtkMenuItem     *menuitem,
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
on_save_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_clicked                      (GtkButton       *button,
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

void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data);

void
on_new_key_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);
