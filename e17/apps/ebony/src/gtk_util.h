#ifndef GTK_UTILS_H
#define GTK_UTILS_H

#include"ebony.h"

void new_bg(GtkWidget * widget, gpointer data);

double get_spin_value(char *name);
void set_spin_value(char *name, double val);
void set_toggled_state(char *name, int val);
int get_toggled_state(char *name);
double get_range_value(char *name);
char *get_entry_text(char *name);
void set_entry_text(char *name, char *txt);
gint ebony_status_clear(gpointer data);
void ebony_status_message(gchar * message, gint delay);

void advanced_widgets_show_for_image(void);
void advanced_widgets_show_for_gradient(void);
void advanced_widgets_show_for_color(void);

void save_as_ok_clicked(GtkWidget * w, gpointer data);
void save_as_cancel_clicked(GtkWidget * w, gpointer data);

void cs_ok_button_clicked(GtkWidget * w, gpointer data);
void cs_cancel_button_clicked(GtkWidget * w, gpointer data);
void filemenu_load_ok_clicked(GtkWidget * w, gpointer data);
void filemenu_load_cancel_clicked(GtkWidget * w, gpointer data);

int redraw(void *data);
void open_bg_named(char *name);
void display_bg(E_Background _bg);
void display_layer_values(E_Background_Layer _bl);
void display_layer_values_for_solid(E_Background_Layer _bl);
void display_layer_values_for_image(E_Background_Layer _bl);
void display_layer_values_for_gradient(E_Background_Layer _bl);

void gradient_one_cs_ok_button_clicked(GtkWidget * w, gpointer data);
void gradient_two_cs_ok_button_clicked(GtkWidget * w, gpointer data);
void gradient_one_cs_cancel_button_clicked(GtkWidget * w, gpointer data);
void gradient_two_cs_cancel_button_clicked(GtkWidget * w, gpointer data);
void browse_file_ok_clicked(GtkWidget * w, gpointer data);
void browse_file_cancel_clicked(GtkWidget * w, gpointer data);

gboolean gradient_one_color_drawing_area_expose(GtkWidget * w,
                                                GdkEventExpose * e,
                                                gpointer data);
gboolean gradient_two_color_drawing_area_expose(GtkWidget * w,
                                                GdkEventExpose * e,
                                                gpointer data);
#endif
