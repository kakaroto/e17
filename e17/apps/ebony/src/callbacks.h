#include <gtk/gtk.h>


gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_load_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_draw_button_press_event             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_draw_button_release_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_draw_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_draw_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_draw_motion_notify_event            (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

void
on_layer__num_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_layer_add_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_layer_del_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_grad_col_2_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_angle_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_solid_color_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_inline_image_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_scroll_x_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_scroll_y_changed                    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_pos_x_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_pos_y_changed                       (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_size_w_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_size_h_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_size_orig_w_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_size_orig_h_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_fill_w_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_fill_h_changed                      (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_fill_orig_w_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_fill_orig_h_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_image_file_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_image_browse_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_type_image_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_type_gradient_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_type_color_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

gboolean
on_filesel_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_filesel_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_filesel_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_colorsel_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_colorsel_ok_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_colorsel_cancel_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_grad_col_1_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_color_class_changed                 (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_layer_num_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);
