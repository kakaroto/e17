#include "gtk_util.h"
#include "util.h"
#include "callbacks.h"
#include "interface.h"          /* i shouldn't have included this here =( */
#include <gdk/gdkx.h>
#include <config.h>

static GtkWidget *recent_menu = NULL;

/**
 * get_spin_value - for a named GtkWidget, attempt to retrieve its data
 * @name - the name of the widget
 * Return the double value of the spinbutton
 */
double
get_spin_value(char *name)
{
   GtkWidget *w;
   double result = 0.0;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
   if (w)
      result = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
   else
      fprintf(stderr, "unable to get spin value for %s\n", name);

   return (result);
}

/**
 * set_spin_value - for a named GtkWidget, attempt to set its value 
 * @name - the name of the spinbutton
 */
void
set_spin_value(char *name, double val)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
   if (w)
      gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), val);
   else
      fprintf(stderr, "unable to set spin value for %s\n", name);
}

/**
 * set_toggled_state - for a named GtkWidget, attempt to set its value 
 * @name - the name of the togglebutton 
 * @val - 1 is toggled down, 0 is toggled up
 */
void
set_toggled_state(char *name, int val)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
   if (w)
   {
      if (val)
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
      else
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), FALSE);
   }
   else
      fprintf(stderr, "unable to set toggled state for %s\n", name);
}

/**
 * get_toggled_state - for a named GtkWidget, attempt to get its value 
 * @name - the name of the togglebutton 
 * Returns 1 is toggled down, 0 is toggled up
 */
int
get_toggled_state(char *name)
{
   int result = 0;
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
   if (w)
   {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
         result = 1;
   }
   else
      fprintf(stderr, "unable to get toggled state for %s\n", name);
   return (result);
}

/**
 * get_entry_text - for a named GtkWidget, attempt to get its value 
 * @name - the name of the spinbutton
 * Returns NULL on error, the string on success
 */
char *
get_entry_text(char *name)
{
   GtkWidget *w;
   char *result = NULL;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
   if (w)
      result = (char *) gtk_entry_get_text(GTK_ENTRY(w));

   return (result);
}

/**
 * set_entry_text - for a named GtkWidget, attempt to get its value 
 * @name - the name of the spinbutton
 * @txt The text to set in the entry
 */
void
set_entry_text(char *name, char *txt)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), name);
   if (w)
      gtk_entry_set_text(GTK_ENTRY(w), txt);
}

/**
 * get_range_value - get the value from the named GtkRange
 * @named - the name of the gtk range
 * Returns the double value of the range
 */
double
get_range_value(char *named)
{
   GtkWidget *w;
   double result = 0.0;

   w = gtk_object_get_data(GTK_OBJECT(bg_ref), named);
   if (w)
      result = (gtk_range_get_adjustment(GTK_RANGE(w))->value);
   else
      fprintf(stderr, "Unable to find range named %s\n", named);
   return (result);
}

/**
 * ebony_status_clear  - clear the current text in the statusbar
 * @data - ignored
 */
gint
ebony_status_clear(gpointer data)
{
   if (ebony_status)
      gtk_statusbar_pop(GTK_STATUSBAR(ebony_status), 1);
   return FALSE;
   UN(data);
}

/** 
 * ebony_status_message - display a message in the statusbar
 * @message - the text to display in the statusbar
 * @delay - the timeout before the text is deleted
 */
void
ebony_status_message(gchar * message, gint delay)
{
   if (ebony_status)
   {
      gtk_statusbar_push(GTK_STATUSBAR(ebony_status), 1, message);
      gtk_timeout_add(delay, (GtkFunction) ebony_status_clear, ebony_status);
   }
   return;
   UN(delay);
   UN(message);
}

/**
 * advanced_widgets_show_for_images - show widgets for an image layer
 * Only certain widgets in the advanced interface are applicable to layers
 * that are of type E_BACKGROUND_TYPE_IMAGE hide the rest so it's less
 * confusing.
 */
void
advanced_widgets_show_for_image(void)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
   gtk_widget_show(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_fill_frame");
   gtk_widget_show(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_scroll_follow");
   gtk_widget_show(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
}

/**
 * advanced_widgets_show_for_gradient - show widgets for a gradient layer
 * Only certain widgets in the advanced interface are applicable to layers
 * that are of type E_BACKGROUND_TYPE_GRADIENT hide the rest so it's less
 * confusing.
 */
void
advanced_widgets_show_for_gradient(void)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_scroll_follow");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_fill_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
   gtk_widget_show(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
   if (w)
      gtk_widget_set_sensitive(w, FALSE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
   if (w)
      gtk_widget_set_sensitive(w, FALSE);
}

/**
 * advanced_widgets_show_for_color - show widgets for a color layer
 * Only certain widgets in the advanced interface are applicable to layers
 * that are of type E_BACKGROUND_TYPE_SOLID hide the rest so it's less
 * confusing.
 */
void
advanced_widgets_show_for_color(void)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
   gtk_widget_show(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_fill_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_scroll_follow");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_w");
   if (w)
      gtk_widget_set_sensitive(w, FALSE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_orig_h");
   if (w)
      gtk_widget_set_sensitive(w, FALSE);
}

/**
 * cs_ok_button_clicked - set the Background color from the color selection
 * @w - the ok button
 * @data - the color selection
 * This is only for the drawing area and section when the layer is a SOLID,
 * gradients have different cbs
 */
void
cs_ok_button_clicked(GtkWidget * w, gpointer data)
{
   GtkWidget *da;
   gdouble color[4];

   if (!bl)
      return;
   if (data)
   {
      gtk_color_selection_get_color(GTK_COLOR_SELECTION
                                    (GTK_COLOR_SELECTION_DIALOG(data)->
                                     colorsel), color);
      bl->fg.r = 255 * color[0];
      bl->fg.g = 255 * color[1];
      bl->fg.b = 255 * color[2];
      bl->fg.a = 255 * color[3];

      evas_object_color_set(bl->obj, bl->fg.r, bl->fg.g, bl->fg.b, bl->fg.a);
      update_background(bg);
   }

   da = gtk_object_get_data(GTK_OBJECT(win_ref), "color_box");
   gtk_widget_set_sensitive(da, TRUE);
   on_color_box_expose_event(da, NULL, NULL);

   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

/**
 * cs_cancel_button_clicked - close the color selection on cancel
 * @w - cancel button
 * @data - the color selection
 */
void
cs_cancel_button_clicked(GtkWidget * w, gpointer data)
{
   GtkWidget *da;

   gtk_widget_destroy(GTK_WIDGET(data));
   da = gtk_object_get_data(GTK_OBJECT(win_ref), "color_box");
   gtk_widget_set_sensitive(da, TRUE);
   return;
   UN(w);
   UN(data);
}

/**
 * filemenu_load_cancel_clicked - select a file and click cancel
 * @w - the cancel button
 * @data - pointer to the fileselection the user chose from
 */
void
filemenu_load_cancel_clicked(GtkWidget * w, gpointer data)
{
   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

/**
 * filemenu_load_ok_clicked - select a file and click ok
 * @w - the ok button
 * @data - pointer to the fileselection the user chose from
 */
void
filemenu_load_ok_clicked(GtkWidget * w, gpointer data)
{
   char *dirpath;

   gchar *file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));

   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(bg_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   if (file)
      open_bg_named((char *) file);
   gtk_widget_destroy(GTK_WIDGET(data));

   return;
   UN(w);
   UN(data);
}

/**
 * display_bg - display the bg in the global evas
 * @_bg - the bg to display
 * Sets the image cache to 0, add th ebg to the evas, then select the first
 * layer in the bg
 */
void
display_bg(E_Background _bg)
{
   E_Background_Layer _bl;
   int size;

   /* clear the evas cache */
   size = evas_image_cache_get(evas);
   evas_image_cache_set(evas, 0);
   evas_image_cache_set(evas, size);

   e_bg_add_to_evas(_bg, evas);
   e_bg_set_layer(_bg, 0);
   e_bg_show(_bg);
   bg = _bg;
   _bl = e_bg_get_layer_number(_bg, 0);
   display_layer_values(_bl);
   set_spin_value("layer_num", 0);
   update_background(_bg);
}

/**
 * save_as_ok_clicked - ok clicked on the save as file selection
 * @w - the ok button
 * @data - the file selection
 */
void
save_as_ok_clicked(GtkWidget * w, gpointer data)
{
   gchar *file;
   gchar errstr[1024];
   char *dirpath, *filesize = NULL;

   file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));

   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(save_as_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   if (!file)
      return;

   e_bg_save(bg, (char *) file);
   open_bg_named((char *) file);
   if ((filesize = filesize_as_string(bg->file)))
   {
      g_snprintf(errstr, 1024, "Saved background: %s(%s)", (char *) bg->file,
                 filesize);
      free(filesize);
   }
   else
      g_snprintf(errstr, 1024, "Saved background: %s", (char *) bg->file);
   ebony_status_message(errstr, EBONY_STATUS_TO);

   gtk_widget_destroy(GTK_WIDGET(data));

   return;
   UN(w);
   UN(data);
}

/**
 * save_as_cancel_clicked - cancel clicked on save as file selection
 * @w - ignored
 * @data - the file selection to destroy
 */
void
save_as_cancel_clicked(GtkWidget * w, gpointer data)
{
   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

/**
 * redraw - render the evas
 * @data - ignored
 */
int
redraw(void *data)
{
   evas_render(evas);
   return (FALSE);
}

/**
 * display_layer_values - setup the widgets to have the current bl's values
 * @_bl - the background layer to select
 * Assign global bl to the Background_Layer passed to it
 */
void
display_layer_values(E_Background_Layer _bl)
{
   GtkWidget *w, *da;

   if (!_bl)
      return;

   bl = NULL;
   switch (_bl->type)
   {
     case E_BACKGROUND_TYPE_IMAGE:
        w = gtk_object_get_data(GTK_OBJECT(win_ref), "type_image");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
        display_layer_values_for_image(_bl);
        break;
     case E_BACKGROUND_TYPE_GRADIENT:
        w = gtk_object_get_data(GTK_OBJECT(win_ref), "type_gradient");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
        display_layer_values_for_gradient(_bl);
        break;
     case E_BACKGROUND_TYPE_SOLID:
        w = gtk_object_get_data(GTK_OBJECT(win_ref), "type_color");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(w), TRUE);
        display_layer_values_for_solid(_bl);
        break;
     default:
        fprintf(stderr, "Unknown Layer Type Yo: %d\n", _bl->type);
        return;
   }
   set_spin_value("pos_y", (double) _bl->pos.y);
   set_spin_value("pos_x", (double) _bl->pos.x);
   set_spin_value("abs_y", (double) _bl->abs.y);
   set_spin_value("abs_x", (double) _bl->abs.x);
   set_spin_value("size_w", (double) _bl->size.w);
   set_spin_value("size_h", (double) _bl->size.h);

   /* outline the object */
   if (_bl->obj)
   {
      outline_evas_object(_bl->obj);
      DRAW();
   }
   bl = _bl;

   switch (bl->type)
   {
     case E_BACKGROUND_TYPE_SOLID:
        da = gtk_object_get_data(GTK_OBJECT(win_ref), "color_box");
        if (da)
           on_color_box_expose_event(da, NULL, NULL);
        break;
     case E_BACKGROUND_TYPE_GRADIENT:
        da =
           gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_one_color_box");
        if (da)
           gradient_one_color_drawing_area_expose(da, NULL, NULL);
        da =
           gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_two_color_box");
        if (da)
           gradient_two_color_drawing_area_expose(da, NULL, NULL);
        break;
     default:
        break;
   }

   return;
   UN(_bl);
}

/**
 * gradient_one_cs_cancel_button_clicked - close the color selection on cancel
 * @w - cancel button
 * @data - the color selection
 */
void
gradient_one_cs_cancel_button_clicked(GtkWidget * w, gpointer data)
{
   GtkWidget *da;

   gtk_widget_destroy(GTK_WIDGET(data));
   da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_one_color_box");
   gtk_widget_set_sensitive(da, TRUE);
   return;
   UN(w);
   UN(data);
}

/**
 * gradient_two_cs_cancel_button_clicked - close the color selection on cancel
 * @w - cancel button
 * @data - the color selection
 */
void
gradient_two_cs_cancel_button_clicked(GtkWidget * w, gpointer data)
{
   GtkWidget *da;

   gtk_widget_destroy(GTK_WIDGET(data));
   da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_two_color_box");
   gtk_widget_set_sensitive(da, TRUE);
   return;
   UN(w);
   UN(data);
}

void
gradient_one_cs_ok_button_clicked(GtkWidget * w, gpointer data)
{
   GtkWidget *da;
   E_Background_Gradient g;
   gdouble color[4];

   if (!bl)
      return;
   if (bl->gradient.colors)
   {
      g = (E_Background_Gradient) bl->gradient.colors->data;
   }
   else
   {
      g = (E_Background_Gradient)
         malloc(sizeof(struct _E_Background_Gradient));
      memset(g, 0, sizeof(struct _E_Background_Gradient));

      bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
   }

   if (data)
   {
      gtk_color_selection_get_color(GTK_COLOR_SELECTION
                                    (GTK_COLOR_SELECTION_DIALOG(data)->
                                     colorsel), color);
      g->r = 255 * color[0];
      g->g = 255 * color[1];
      g->b = 255 * color[2];
      g->a = 255 * color[3];
      g->dist = 1;
   }
   da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_one_color_box");
   gtk_widget_set_sensitive(da, TRUE);
   gradient_one_color_drawing_area_expose(da, NULL, NULL);
   redraw_gradient_object();
   update_background(bg);

   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

void
gradient_two_cs_ok_button_clicked(GtkWidget * w, gpointer data)
{
   GtkWidget *da;
   E_Background_Gradient g;
   gdouble color[4];

   if (!bl)
      return;
   if ((bl->gradient.colors) && (bl->gradient.colors->next))
   {
      g = (E_Background_Gradient) bl->gradient.colors->next->data;
   }
   else
   {
      g = (E_Background_Gradient)
         malloc(sizeof(struct _E_Background_Gradient));
      memset(g, 0, sizeof(struct _E_Background_Gradient));

      bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
   }

   if (data)
   {
      gtk_color_selection_get_color(GTK_COLOR_SELECTION
                                    (GTK_COLOR_SELECTION_DIALOG(data)->
                                     colorsel), color);
      g->r = 255 * color[0];
      g->g = 255 * color[1];
      g->b = 255 * color[2];
      g->a = 255 * color[3];
      g->dist = 1;
   }
   da = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_two_color_box");
   gtk_widget_set_sensitive(da, TRUE);
   gradient_one_color_drawing_area_expose(da, NULL, NULL);
   redraw_gradient_object();
   update_background(bg);

   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

/**
 * display_layer_values_for_gradient - display widgets for a gradient layer
 * @_bl - the background layer of type E_BACKGROUND_TYPE_GRADIENT
 * Some controls aren't applicable to gradients, hide them to avoid
 * confusion.
 */
void
display_layer_values_for_gradient(E_Background_Layer _bl)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   if (_bl->file)
      set_entry_text("image_file", (gchar *) _bl->file);
   else
      set_entry_text("image_file", "");

   set_toggled_state("size_abs_h", _bl->size.absolute.h);
   set_toggled_state("size_abs_w", _bl->size.absolute.w);
   set_toggled_state("size_orig_h", 0);
   set_toggled_state("size_orig_w", 0);

   set_spin_value("gradient_angle", _bl->gradient.angle);
   return;
   UN(_bl);
}

/**
 * display_layer_values_for_solid - display widgets for a solid color layer
 * @_bl - the background layer of type E_BACKGROUND_TYPE_SOLID
 * Some controls aren't applicable to colors, hide them to avoid
 * confusion.
 */
void
display_layer_values_for_solid(E_Background_Layer _bl)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
   if (w)
      gtk_widget_set_sensitive(w, TRUE);
   if (_bl->file)
      set_entry_text("image_file", (gchar *) _bl->file);
   else
      set_entry_text("image_file", "");

   set_toggled_state("size_abs_h", _bl->size.absolute.h);
   set_toggled_state("size_abs_w", _bl->size.absolute.w);
   set_toggled_state("size_orig_h", 0);
   set_toggled_state("size_orig_w", 0);

   return;
   UN(_bl);
}

/**
 * display_layer_values_for_image - display widgets for a image layer
 * @_bl - the background layer of type E_BACKGROUND_TYPE_IMAGE
 * Some controls aren't applicable to image, hide them to avoid
 * confusion.
 */
void
display_layer_values_for_image(E_Background_Layer _bl)
{
   GtkWidget *w;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "image_file");
   if (w)
   {
      if (_bl->file && _bl->inlined)
      {
         char buf[PATH_MAX];

         snprintf(buf, PATH_MAX, "%s:%s", bg->file, (gchar *) _bl->file);
         gtk_entry_set_text(GTK_ENTRY(w), buf);
      }

      else if (_bl->file)
         gtk_entry_set_text(GTK_ENTRY(w), (gchar *) _bl->file);
      else
         gtk_entry_set_text(GTK_ENTRY(w), "");
   }
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_h");
   if (w)
      gtk_widget_set_sensitive(w, FALSE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "size_abs_w");
   if (w)
      gtk_widget_set_sensitive(w, FALSE);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "gradient_frame");
   if (w)
      gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(win_ref), "layer_color_frame");
   if (w)
      gtk_widget_hide(w);
   set_toggled_state("size_abs_h", 0);
   set_toggled_state("size_abs_w", 0);

   /* setup some stuff */
   set_toggled_state("inline_image", _bl->inlined);
   set_toggled_state("fill_orig_w", _bl->fill.orig.w);
   set_toggled_state("fill_orig_h", _bl->fill.orig.h);
   set_toggled_state("size_orig_h", _bl->size.orig.h);
   set_toggled_state("size_orig_w", _bl->size.orig.w);

   set_spin_value("fill_w", (double) _bl->fill.w);
   set_spin_value("fill_h", (double) _bl->fill.h);
   set_spin_value("scroll_x", (double) _bl->scroll.x);
   set_spin_value("scroll_y", (double) _bl->scroll.y);

   return;
   UN(_bl);
}

void
open_bg(GtkWidget * w, gpointer data)
{
   if (!data)
      return;
   open_bg_named((char *) data);
   return;
   UN(w);
   UN(data);
}

/**
 * open_bg_named - have ebony load the bg named name
 * @name - the filename on disk
 * Handle the freeing of the old bg, and loading of the new bg, plus keep up
 * with the recently used bg stuff
 */
void
open_bg_named(char *name)
{
   E_Background _bg = NULL;

   if (!name)
      return;
   if (!bg_ref)
      rebuild_bg_ref();

   _bg = e_bg_load(name);
   if (_bg)
   {
      char buf[256];

      snprintf(buf, 256, "Ebony - %s", name);
      gtk_window_set_title(GTK_WINDOW(bg_ref), buf);
      handle_recent_bgs_append(name);
      e_bg_free(bg);
      display_bg(_bg);

      bg = _bg;
   }
   else
   {
      gchar errstr[1024];

      g_snprintf(errstr, 1024, "Unable to load %s", name);
      ebony_status_message(errstr, EBONY_STATUS_TO);
   }
}

gboolean
gradient_one_color_drawing_area_expose(GtkWidget * w, GdkEventExpose * e,
                                       gpointer data)
{
   GdkColor fg;
   static GdkGC *gc1 = NULL;
   E_Background_Gradient g = NULL;

   if (!w)
      return (FALSE);
   if (!w->window)
      return (FALSE);
   if (!bl)
      return (FALSE);
   if (bl->gradient.colors)
   {
      if (bl->gradient.colors->data)
         g = bl->gradient.colors->data;
   }
   else
   {
      g = (E_Background_Gradient)
         malloc(sizeof(struct _E_Background_Gradient));
      memset(g, 0, sizeof(struct _E_Background_Gradient));
      bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
   }
   if (gc1)
      gdk_gc_destroy(gc1);
   gc1 = gdk_gc_new(w->window);
   fg.pixel = gdk_rgb_xpixel_from_rgb((g->r << 16) | (g->g << 8) | g->b);
   gdk_gc_set_foreground(gc1, &fg);
   gdk_draw_rectangle(w->window, gc1, 1, 0, 0, w->allocation.width,
                      w->allocation.height);

   return (TRUE);
   UN(w);
   UN(e);
   UN(data);
}

gboolean
gradient_two_color_drawing_area_expose(GtkWidget * w, GdkEventExpose * e,
                                       gpointer data)
{
   E_Background_Gradient g = NULL;
   GdkColor fg;
   static GdkGC *gc2 = NULL;

   if (!w)
      return (FALSE);
   if (!w->window)
      return (FALSE);
   if (!bl)
      return (FALSE);
   if ((bl->gradient.colors) && (bl->gradient.colors->next))
   {
      if (bl->gradient.colors->next->data)
         g = bl->gradient.colors->next->data;
   }
   else
   {
      g = (E_Background_Gradient)
         malloc(sizeof(struct _E_Background_Gradient));
      memset(g, 0, sizeof(struct _E_Background_Gradient));
      bl->gradient.colors = evas_list_append(bl->gradient.colors, g);
   }
   if (gc2)
      gdk_gc_destroy(gc2);
   gc2 = gdk_gc_new(w->window);
   fg.pixel = gdk_rgb_xpixel_from_rgb((g->r << 16) | (g->g << 8) | g->b);
   gdk_gc_set_foreground(gc2, &fg);
   gdk_draw_rectangle(w->window, gc2, 1, 0, 0, w->allocation.width,
                      w->allocation.height);

   return (TRUE);
   UN(w);
   UN(e);
   UN(data);
}

void
new_bg(GtkWidget * widget, void *data)
{
   E_Background_Layer _bl;
   E_Background _bg = NULL;
   GtkWidget *w;

   if (bg)
      e_bg_free(bg);
   if (!bg_ref)
      rebuild_bg_ref();

   /* FIXME Need to free the outlining objects in the evas */
   _bg = e_bg_new();
   if (_bg)
   {
      _bl = e_bg_layer_new();
      _bl->type = E_BACKGROUND_TYPE_SOLID;
      _bl->fg.a = _bl->fg.r = _bl->fg.g = _bl->fg.b = 255;
      _bl->size.w = _bl->size.h = 1.0;
      _bg->layers = evas_list_append(_bg->layers, _bl);
      display_bg(_bg);
   }
   return;
   UN(w);
   UN(data);
}

/**
 * browse_file_ok_clicked - loading an image layer from a file selection
 * @w - the ok button
 * @data - pointer to the file selection
 */
void
browse_file_ok_clicked(GtkWidget * w, gpointer data)
{
   gchar *file;
   char *dirpath;

   file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
   dirpath = get_dirpath_from_filename((char *) file);

   snprintf(image_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(image_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(image_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(image_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(image_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   if (file)
      set_entry_text("image_file", file);

   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

/**
 * browse_file_cancel_clicked - canceling image layer file selection
 * @w - the cancel button
 * @data - pointer to the file selection
 */
void
browse_file_cancel_clicked(GtkWidget * w, gpointer data)
{
   gtk_widget_destroy(GTK_WIDGET(data));
   return;
   UN(w);
   UN(data);
}

/**
 * regen_recent_menu - regenerate from recent_bgs list
 * Destroy the current widget submenu for recent bgs, recreate the submenu
 * and show it.
 */
void
regen_recent_menu(void)
{
   GList *l;
   GtkWidget *w;
   GtkWidget *menu;
   GtkWidget *mi = NULL;
   char *short_name, *filename;

   w = gtk_object_get_data(GTK_OBJECT(win_ref), "recent_bg_mi");
   if (w)
   {
      gtk_menu_item_remove_submenu(GTK_MENU_ITEM(w));
      menu = gtk_menu_new();
      gtk_widget_show(menu);
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(w), menu);

      for (l = recent_bgs; l; l = l->next)
      {
         filename = (char *) l->data;

         short_name = get_shortname_for(filename);
         if (short_name)
         {
            mi = gtk_menu_item_new_with_label(short_name);

            free(short_name);
         }

         gtk_menu_append(GTK_MENU(menu), mi);
         gtk_signal_connect(GTK_OBJECT(mi), "activate",
                            GTK_SIGNAL_FUNC(open_bg), (gpointer) filename);
         gtk_widget_show(mi);
      }
      recent_menu = menu;
   }
   else
   {
      fprintf(stderr, "Unable to locate the recent_bg_mi widget\n");
   }
}

/**
 * handle_recent_bgs_append - append the file to recent bg list
 * @name - the filename on disk
 * If the file name already exists in the list, remove it and append it to
 * the list, if the list is full pop the first element and append the
 * filename to the recent_bgs list
 */
void
handle_recent_bgs_append(char *name)
{
   GList *l;
   int ok, length;
   char *str;

   ok = 1;
   if (!strcmp((char *) name, PACKAGE_DATA_DIR "/pixmaps/ebony.bg.db"))
      return;
   str = strdup((char *) name);
   for (l = recent_bgs; l; l = l->next)
   {
      if (!l->data)
      {
         recent_bgs = g_list_remove_link(recent_bgs, l);
      }
      else if (!strcmp((char *) l->data, name))
      {
         recent_bgs = g_list_remove(recent_bgs, l->data);
         free((char *) l->data);
         ok = 0;
      }
   }
   if (!ok)
   {
      char *bg_to_pop = NULL;

      length = 0;
      for (l = recent_bgs; (l) && (length < 5); l = l->next)
         length++;
      if (length == 4)
      {
         bg_to_pop = g_list_nth_data(recent_bgs, 0);
         recent_bgs = g_list_remove(recent_bgs, (gpointer) bg_to_pop);
         free((char *) bg_to_pop);
      }
      if (recent_menu)
         gtk_widget_destroy(recent_menu);
      recent_menu = NULL;
   }
   recent_bgs = g_list_append(recent_bgs, str);
   regen_recent_menu();
}

/**
 * export_ok_clicked - ok clicked on the export image file selection
 * @w - the ok button
 * @data - the file selection
 */
void
export_ok_clicked(GtkWidget * w, gpointer data)
{
#if 0
   E_Background export_bg;
   Evas *export_evas;
   Imlib_Image image;
   gchar *file;
   gchar errstr[1024];
   char *dirpath;

   /* Set default options */
   int width = (export_info.screen.w * export_info.xinerama.h);
   int height = (export_info.screen.h * export_info.xinerama.v);

   file = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
   dirpath = get_dirpath_from_filename((char *) file);
   snprintf(export_fileselection_dir, PATH_MAX, "%s/", dirpath);
   free(dirpath);

   if (!file)
      return;

   export_bg = e_bg_copy(bg);

   /* Setup Imlib2 */
   image = imlib_create_image(width, height);
   imlib_context_set_image(image);
   imlib_image_clear();

   /* Setup Evas and render */
   export_evas = evas_new();
   evas_output_method_set(export_evas, RENDER_METHOD_IMAGE);
   evas_set_output_image(export_evas, image);
   evas_set_output_viewport(export_evas, 0, 0, width, height);
   evas_set_output_size(export_evas, width, height);
   e_bg_add_to_evas(export_bg, export_evas);
   e_bg_show(export_bg);
   evas_render(export_evas);

   imlib_context_set_image(image);

   /* Finally, save image */
   imlib_save_image(file);

   /* then free stuffs */
   imlib_free_image_and_decache();
   e_bg_free(export_bg);
   evas_free(export_evas);

   g_snprintf(errstr, 1024, "Exported background to %s", (char *) file);
   ebony_status_message(errstr, EBONY_STATUS_TO);

   gtk_widget_destroy(GTK_WIDGET(data));

   return;
   UN(w);
   UN(data);
#endif
}
