#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <string.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "utils.h"

extern GtkWidget *colorsel;
extern GtkWidget *filesel;
extern GtkWidget *window;

int idle = 0;
int busy = 0;

int
redraw(void *data)
{
   evas_render(evas);
   return FALSE;
}

char *
get_entry_val(char *name)
{
   GtkWidget *w;
   
   w = gtk_object_get_data(GTK_OBJECT(window), name);
   return gtk_entry_get_text(GTK_ENTRY(w));
}

void
set_entry_val(char *name, char *str)
{
   GtkWidget *w;
   
   w = gtk_object_get_data(GTK_OBJECT(window), name);
   gtk_entry_set_text(GTK_ENTRY(w), str);
}

double
get_spin_value(char *name)
{
   GtkWidget *w;
   
   w = gtk_object_get_data(GTK_OBJECT(window), name);
   return gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
}

void
set_spin_value(char *name, double val)
{
   GtkWidget *w;
   
   w = gtk_object_get_data(GTK_OBJECT(window), name);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), val);
}

int
get_toggle_value(char *name)
{
   GtkWidget *w;
   
   w = gtk_object_get_data(GTK_OBJECT(window), name);
   return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));
}

void
set_toggle_value(char *name, int on)
{    
   GtkWidget *w;
   
   w = gtk_object_get_data(GTK_OBJECT(window), name);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), on);
}

void
display_layer(E_Background_Layer *bl)
{
   busy = 1;
   
   if (bl->type == 0) set_toggle_value("type_image", 1);
   else if (bl->type == 1) set_toggle_value("type_gradient", 1);
   else if (bl->type == 2) set_toggle_value("type_color", 1);
   if (bl->file) set_entry_val("image_file", bl->file);
   else set_entry_val("image_file", "");
   if (bl->color_class) set_entry_val("color_class", bl->color_class);
   else set_entry_val("color_class", "");
   set_spin_value("scroll_x", bl->scroll.x);
   set_spin_value("scroll_y", bl->scroll.y);
   set_spin_value("pos_x", bl->pos.x);
   set_spin_value("pos_y", bl->pos.y);
   set_spin_value("angle", bl->angle);
   set_toggle_value("inline_image", bl->inlined);
   set_spin_value("size_w", bl->size.w);
   set_spin_value("size_h", bl->size.h);
   set_toggle_value("size_orig_w", bl->size.orig.w);
   set_toggle_value("size_orig_h", bl->size.orig.h);
   set_spin_value("fill_w", bl->fill.w);
   set_spin_value("fill_h", bl->fill.h);
   set_toggle_value("fill_orig_w", bl->fill.orig.w);
   set_toggle_value("fill_orig_h", bl->fill.orig.h);
   busy = 0;
}

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   exit(0);
   return FALSE;
}


void
on_load_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   if (busy) return;
   gtk_object_set_data(GTK_OBJECT(filesel), "mode", (gpointer)1);
   gtk_widget_show (filesel);
}


void
on_save_as_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   if (busy) return;
   if (!background->file) background->file = strdup("unnamed.bg.db");
   if (background->file)
     {
	int ok;
	char *s;
	
	ok = 1;
	s = background->file + strlen(background->file) - 6;
	if (s >= background->file)
	  {
	     if (strcmp(s, ".bg.db")) ok = 0;
	  }
	if (!ok)
	  {
	     char buf[4096];
	     
	     sprintf(buf, "%s.bg.db", background->file);
	     free(background->file);
	     background->file = strdup(buf);
	  }
     }
   gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel), background->file);
   
   gtk_object_set_data(GTK_OBJECT(filesel), "mode", (gpointer)2);
   gtk_widget_show (filesel);
}


void
on_save_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   if (busy) return;
   if (!background->file)
     {
	gtk_object_set_data(GTK_OBJECT(filesel), "mode", (gpointer)2);
	gtk_widget_show (filesel);	
     }
   else
     e_background_save(background->file, background);
}


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
   if (busy) return;
   exit(0);
}


gboolean
on_draw_button_press_event             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   evas_event_button_down(evas, event->x, event->y, event->button);   
   DRAW();
   return FALSE;
}


gboolean
on_draw_button_release_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   evas_event_button_up(evas, event->x, event->y, event->button);
   DRAW();
   return FALSE;
}


gboolean
on_draw_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   evas_set_output_size(evas, event->width, event->height);
   evas_set_output_viewport(evas, 0, 0, event->width, event->height);
   DRAW();
   return FALSE;
}


gboolean
on_draw_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   evas_update_rect(evas, 
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);
   DRAW();
   return FALSE;
}


gboolean
on_draw_motion_notify_event            (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   evas_event_move(evas, event->x, event->y);
   DRAW();
   return FALSE;
}


void
on_layer_add_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;
   
   if (busy) return;
   bl = malloc(sizeof(E_Background_Layer));
   memset(bl, 0, sizeof(E_Background_Layer));
   bl->size.w = 1.0;
   bl->size.h = 1.0;
   bl->fill.w = 1.0;
   bl->fill.h = 1.0;
   bl->inlined = 1;
   background->layers = evas_list_append(background->layers, bl);
   if (bl->file) bl->obj = evas_add_image_from_file(evas, bl->file);
   else  bl->obj = evas_add_image_from_file(evas, "");
   evas_show(evas, bl->obj);
   set_spin_value("layer_num", (int)get_spin_value("layer_num") + 1);
   e_update_background(background);
   DRAW();
}


void
on_layer_del_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;
   
   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	background->layers = evas_list_remove(background->layers, bl);
	if (bl->color_class) free(bl->color_class);
	if (bl->file) free(bl->file);
	if (bl->obj) evas_del_object(evas, bl->obj);
	if (bl->image)
	  {
	     imlib_context_set_image(bl->image);
	     imlib_free_image_and_decache();
	  }
	set_spin_value("layer_num", get_spin_value("layer_num") - 1);
	free(bl);
	DRAW();
     }
}


gboolean
on_grad_col_2_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   gtk_object_set_data(GTK_OBJECT(colorsel), "mode", (gpointer)3);
   gtk_widget_show (colorsel);

   return FALSE;
}


void
on_angle_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->angle = get_spin_value("angle");
	if (bl->type == 1)
	  evas_set_angle(evas, bl->obj, bl->angle);
	e_update_background(background);
     }
}


gboolean
on_solid_color_button_press_event      (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   if (busy) return FALSE;
   gtk_object_set_data(GTK_OBJECT(colorsel), "mode", (gpointer)1);
   gtk_widget_show (colorsel);   
   
   return FALSE;
}


void
on_inline_image_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	if (bl->inlined && (!get_toggle_value("inline_image")))
	  {
	     bl->inlined = 0;
	  }
	else if (!bl->inlined && (get_toggle_value("inline_image")))
	  {
	     bl->inlined = 1;
	  }
     }
}


void
on_scroll_x_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->scroll.x = get_spin_value("scroll_x");
	e_update_background(background);
     }
}


void
on_scroll_y_changed                    (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->scroll.x = get_spin_value("scroll_y");
	e_update_background(background);
     }
}


void
on_pos_x_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->pos.x = get_spin_value("pos_x");
	e_update_background(background);
     }
}


void
on_pos_y_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->pos.y = get_spin_value("pos_y");
	e_update_background(background);
     }
}


void
on_size_w_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->size.w = get_spin_value("size_w");
	e_update_background(background);
     }
}


void
on_size_h_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->size.h = get_spin_value("size_h");
	e_update_background(background);
     }
}


void
on_size_orig_w_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->size.orig.w = get_toggle_value("size_orig_w");
	e_update_background(background);
     }
}


void
on_size_orig_h_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->size.orig.h = get_toggle_value("size_orig_h");
	e_update_background(background);
     }
}


void
on_fill_w_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->fill.w = get_spin_value("fill_w");
	e_update_background(background);
     }
}


void
on_fill_h_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->fill.h = get_spin_value("fill_h");
	e_update_background(background);
     }
}


void
on_fill_orig_w_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->fill.orig.w = get_toggle_value("fill_orig_w");
	e_update_background(background);
     }
}


void
on_fill_orig_h_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	bl->fill.orig.h = get_toggle_value("fill_orig_h");
	e_update_background(background);
     }
}


void
on_image_file_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{

   E_Background_Layer *bl;

   if (busy) return;
   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	if (bl->file) free(bl->file);
	bl->file = strdup(get_entry_val("image_file"));
	if (bl->file) evas_set_image_file(evas, bl->obj, bl->file);
	else  evas_set_image_file(evas, bl->obj, "");
	e_update_background(background);
	if (bl->image)
	  {
	     imlib_context_set_image(bl->image);
	     imlib_free_image_and_decache();
	     bl->image = NULL;
	  }
	if (bl->file)
	  bl->image = imlib_load_image(bl->file);
     }
}


void
on_image_browse_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
   if (busy) return;
   gtk_object_set_data(GTK_OBJECT(filesel), "mode", (gpointer)3);
   gtk_widget_show (filesel);
}


void
on_type_image_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	if (bl->type != 0)
	  {
	     bl->type = 0;
	     if (bl->obj) evas_del_object(evas, bl->obj);
	     if (bl->file) bl->obj = evas_add_image_from_file(evas, bl->file);
	     else  bl->obj = evas_add_image_from_file(evas, "");
	     evas_show(evas, bl->obj);
	     e_update_background(background);
	     if (bl->image)
	       {
		  imlib_context_set_image(bl->image);
		  imlib_free_image_and_decache();
		  bl->image = NULL;
	       }
	     if (bl->file)
	       bl->image = imlib_load_image(bl->file);
	  }
     }
}


void
on_type_gradient_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	if (bl->type != 1)
	  {
	     bl->type = 1;
	     if (bl->obj) evas_del_object(evas, bl->obj);
	     bl->obj = evas_add_gradient_box(evas);
	     e_update_background(background);
	  }
     }
}


void
on_type_color_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	if (bl->type != 2)
	  {
	     bl->type = 2;
	     if (bl->obj) evas_del_object(evas, bl->obj);
	     bl->obj = evas_add_rectangle(evas);
	     e_update_background(background);
	  }
     }
}


gboolean
on_filesel_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gtk_widget_hide(widget);
   return TRUE;
}


void
on_filesel_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
   int mode;
   
   mode = gtk_object_get_data(GTK_OBJECT(filesel), "mode");
   if (mode == 1)
     {
	E_Background *bg;
	
	bg = e_background_load(gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)));
	if (bg)
	  {
	     if (background) e_background_free(background);
	     background = bg;
	     e_background_realize(background, evas);
	     e_background_set_size(background, 320, 240);
	     
	     set_spin_value("layer_num", 0);
	     if (bg->layers)
	       display_layer(bg->layers->data);
	  }
     }
   else if (mode == 2)
     {
	if (background->file) free(background->file);
	background->file = strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)));
	e_background_save(background->file, background);
     }
   else if (mode == 3)
     {
	set_entry_val("image_file", gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)));
     }
   gtk_widget_hide(filesel);
}


void
on_filesel_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
   gtk_widget_hide(filesel);
}


gboolean
on_colorsel_delete_event               (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gtk_widget_hide(widget);
   return TRUE;
}


void
on_colorsel_ok_clicked                 (GtkButton       *button,
					gpointer         user_data)
{
   gtk_widget_hide(colorsel);   
}


void
on_colorsel_cancel_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
   gtk_widget_hide(colorsel);
}


gboolean
on_grad_col_1_button_press_event       (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
   gtk_object_set_data(GTK_OBJECT(filesel), "mode", (gpointer)2);
   gtk_widget_show (colorsel);
   return FALSE;
}


void
on_color_class_changed                 (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
     }
}


void
on_layer_num_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
   E_Background_Layer *bl;

   bl = e_get_layer_number(background, (int)get_spin_value("layer_num"));
   if (bl)
     {
	display_layer(bl);
     }
   else
     {
	set_spin_value("layer_num", get_spin_value("layer_num") - 1);
     }
}

