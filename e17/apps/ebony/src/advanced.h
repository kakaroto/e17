#ifndef EBONY_ADVANCED_H
#define EBONY_ADVANCED_H

#include "interface.h"
#include "callbacks.h"

E_Background_Layer bl;

void display_bg(E_Background _bg);
void display_layer_values(E_Background_Layer _bl);

void on_bg_color_changed(GtkSpinButton*, gpointer);

/* spin button callbacks */
void on_layer_num_changed(GtkEditable *e, gpointer data);
void on_scroll_x_changed(GtkEditable *e, gpointer data);
void on_scroll_y_changed(GtkEditable *e, gpointer data);
void on_pos_x_changed(GtkEditable *e, gpointer data);
void on_pos_y_changed(GtkEditable *e, gpointer data);
void on_size_width_changed(GtkEditable *e, gpointer data);
void on_size_height_changed(GtkEditable *e, gpointer data);
void on_fill_width_changed(GtkEditable *e, gpointer data);
void on_fill_height_changed(GtkEditable *e, gpointer data);
void on_inline_image_toggled(GtkWidget *w, gpointer data);
void on_abs_x_changed(GtkEditable *e, gpointer data);
void on_abs_y_changed(GtkEditable *e, gpointer data);

void on_layer_type_changed(GtkWidget *w, gpointer data);
void on_size_orig_height_toggled(GtkWidget *w, gpointer data);
void on_size_orig_width_toggled(GtkWidget *w, gpointer data);
void on_fill_orig_height_toggled(GtkWidget *w, gpointer data);
void on_fill_orig_width_toggled(GtkWidget *w, gpointer data);
void on_size_abs_w_toggled(GtkWidget *w, gpointer data);
void on_size_abs_h_toggled(GtkWidget *w, gpointer data);

void on_layer_add_clicked(GtkWidget *w, gpointer data);
void on_image_file_browse_clicked(GtkWidget *w, gpointer data);
void on_image_file_entry_changed(GtkWidget *w, gpointer data);
void on_delete_layer_clicked(GtkWidget *w, gpointer data);
void on_layer_up_clicked(GtkWidget *w, gpointer data);
void on_layer_down_clicked(GtkWidget *w, gpointer data);

void cs_ok_button_clicked(GtkWidget *w, gpointer data);
void cs_cancel_button_clicked(GtkWidget *w, gpointer data);
void gradient_one_cs_ok_button_clicked(GtkWidget *w, gpointer data);
void on_gradient_angle_changed(GtkEditable *e, gpointer data);
/* drawing area in the color frome */
gboolean
color_drawing_area_expose(GtkWidget *w, GdkEventExpose *e, gpointer data);
gboolean
color_drawing_area_mouse_down(GtkWidget *w, GdkEventButton *e, gpointer data);
/* gradient one drawing stuffs */
gboolean
gradient_one_color_drawing_area_expose(GtkWidget *w, 
				    GdkEventExpose *e, gpointer data);
gboolean
gradient_one_color_drawing_area_mouse_down(GtkWidget *w, GdkEventButton *e,
	gpointer data);
/* gradient two drawing area cbs */
gboolean
gradient_two_color_drawing_area_expose(GtkWidget *w, 
				    GdkEventExpose *e, gpointer data);
gboolean
gradient_two_color_drawing_area_mouse_down(GtkWidget *w, GdkEventButton *e,
	gpointer data);

void redraw_gradient_object();

#endif
