#include <gtk/gtk.h>


gboolean
on_main_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_recent0_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_recent1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_recent2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_recent3_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_recent4_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_undo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_redo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_view_button_press_event             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_view_button_release_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

gboolean
on_view_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_view_motion_notify_event            (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data);

gboolean
on_view_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_view_enter_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

gboolean
on_view_leave_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_new_image_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_new_icon_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_new_text_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_raise_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_lower_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_reset_clicked                       (GtkButton       *button,
                                        gpointer         user_data);

void
on_prop_apply_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_prop_reset_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_draft_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_images_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_states_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_browse_clicked_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_borwse_hilited_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_browse_normal_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_browse_disabled_clicked             (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_zoom_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

gboolean
on_zoom_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

void
on_zoomin_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_zoomout_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_filesel_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_file_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_file_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_software_clicked                    (GtkButton       *button,
                                        gpointer         user_data);

void
on_3d_hardware_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_preferences_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_pref_da_expose_event                (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_gridimage_changed                   (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_selectimage_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_tintcolor_button_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_ok_clicked                          (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_etchings1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_browse_selected_clicked             (GtkButton       *button,
                                        gpointer         user_data);
