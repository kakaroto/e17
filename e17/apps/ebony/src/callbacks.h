#include <gtk/gtk.h>


void on_new_bg_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_open_bg_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_recent_bg_mi_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_save_bg_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_save_bg_as_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_quit_ebony_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_controls_visible_activate(GtkMenuItem * menuitem, gpointer user_data);

gboolean on_bg_evas_configure_event(GtkWidget * widget,
                                    GdkEventConfigure * event,
                                    gpointer user_data);

void on_bg_evas_drag_data_received(GtkWidget * widget,
                                   GdkDragContext * drag_context, gint x,
                                   gint y, GtkSelectionData * data,
                                   guint info, guint time,
                                   gpointer user_data);

void on_layer_num_spin_button_changed(GtkEditable * editable,
                                      gpointer user_data);

void on_layer_add_clicked(GtkButton * button, gpointer user_data);

void on_layer_delete_clicked(GtkButton * button, gpointer user_data);

void on_move_layer_up_clicked(GtkButton * button, gpointer user_data);

void on_move_layer_downclicked(GtkButton * button, gpointer user_data);

void on_image_file_entry_changed(GtkEditable * editable, gpointer user_data);

void on_file_select_button_clicked(GtkButton * button, gpointer user_data);

void on_inline_image_toggled(GtkToggleButton * togglebutton,
                             gpointer user_data);

void on_color_class_changed(GtkEditable * editable, gpointer user_data);

void on_scroll_x_changed(GtkEditable * editable, gpointer user_data);

void on_scroll_y_changed(GtkEditable * editable, gpointer user_data);

void on_pos_x_changed(GtkEditable * editable, gpointer user_data);

void on_pos_y_changed(GtkEditable * editable, gpointer user_data);

void on_pos_abs_x_changed(GtkEditable * editable, gpointer user_data);

void on_pos_abs_y_changed(GtkEditable * editable, gpointer user_data);

void on_size_w_changed(GtkEditable * editable, gpointer user_data);

void on_size_h_changed(GtkEditable * editable, gpointer user_data);

void on_size_abs_w_toggled(GtkToggleButton * togglebutton,
                           gpointer user_data);

void on_size_abs_h_toggled(GtkToggleButton * togglebutton,
                           gpointer user_data);

void on_size_orig_w_toggled(GtkToggleButton * togglebutton,
                            gpointer user_data);

void on_size_orig_h_toggled(GtkToggleButton * togglebutton,
                            gpointer user_data);

void on_fill_w_changed(GtkEditable * editable, gpointer user_data);

void on_fill_h_changed(GtkEditable * editable, gpointer user_data);

void on_fill_orig_w_toggled(GtkToggleButton * togglebutton,
                            gpointer user_data);

void on_fill_orig_h_toggled(GtkToggleButton * togglebutton,
                            gpointer user_data);

gboolean on_color_box_expose_event(GtkWidget * widget, GdkEventExpose * event,
                                   gpointer user_data);

gboolean on_color_box_button_release_event(GtkWidget * widget,
                                           GdkEventButton * event,
                                           gpointer user_data);

gboolean on_gradient_one_expose_event(GtkWidget * widget,
                                      GdkEventExpose * event,
                                      gpointer user_data);

gboolean on_gradient_one_button_release_event(GtkWidget * widget,
                                              GdkEventButton * event,
                                              gpointer user_data);

gboolean on_gradient_two_button_release_event(GtkWidget * widget,
                                              GdkEventButton * event,
                                              gpointer user_data);

gboolean on_gradient_two_expose_event(GtkWidget * widget,
                                      GdkEventExpose * event,
                                      gpointer user_data);

gboolean on_main_win_delete_event(GtkWidget * widget, GdkEvent * event,
                                  gpointer user_data);

void on_layer_outline_invert_activate(GtkMenuItem * menuitem,
                                      gpointer user_data);

gboolean on_evas_expose_event(GtkWidget * widget, GdkEventExpose * event,
                              gpointer user_data);

void on_layer_type_toggled(GtkToggleButton * togglebutton,
                           gpointer user_data);

gboolean on_color_box_button_release_event(GtkWidget * widget,
                                           GdkEventButton * event,
                                           gpointer user_data);

gboolean on_gradient_one_button_release_event(GtkWidget * widget,
                                              GdkEventButton * event,
                                              gpointer user_data);

gboolean on_gradient_two_button_release_event(GtkWidget * widget,
                                              GdkEventButton * event,
                                              gpointer user_data);

void on_layer_type_toggled(GtkToggleButton * togglebutton,
                           gpointer user_data);

void on_gradient_angle_changed(GtkEditable * editable, gpointer user_data);

gboolean on_win_bg_delete_event(GtkWidget * widget, GdkEvent * event,
                                gpointer user_data);

gboolean on_win_bg_button_press_event(GtkWidget * widget, GdkEventKey * event,
                                      gpointer user_data);

void on_scale_scroll_request(GtkWidget * widget, gpointer user_data);

void on_export_bg_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_scale_preview_toggled(GtkToggleButton * togglebutton,
                              gpointer user_data);

void on_export_size_toggled(GtkToggleButton * togglebutton,
                            gpointer user_data);

void on_export_cancel_button_clicked(GtkButton * button, gpointer user_data);

void on_export_screen_h_changed(GtkEditable * editable, gpointer user_data);

void on_export_screen_w_changed(GtkEditable * editable, gpointer user_data);

void on_export_xinerama_v_changed(GtkEditable * editable, gpointer user_data);

void on_export_xinerama_h_changed(GtkEditable * editable, gpointer user_data);

void on_export_size_toggled(GtkToggleButton * togglebutton,
                            gpointer user_data);

void on_export_button_clicked(GtkButton * button, gpointer user_data);
