/**************************************************************************
 * focus_interface.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 16, 2001
 * Notebook tab, and callback function definitions for the focus interface
 * in ebindings
 *************************************************************************/
#include "focus_interface.h"

static void         focus_mouse_focus_type_cb(GtkWidget *, gpointer);
static void         focus_mouse_autoraise_window(GtkWidget *, gpointer);

static void
focus_mouse_focus_type_cb(GtkWidget * w, gpointer data)
{
   char                buf[4096];
   gchar              *button;

   if(!w)
      return;
   snprintf(buf, 4096, "%s/.e/behavior/settings.db", getenv("HOME"));

   button = gtk_widget_get_name(GTK_WIDGET(w));

   if(!strcmp(button, "follow_pointer")) {
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
      E_DB_INT_SET(buf, "/focus/mode", 0)}
   } else if(!strcmp(button, "follow_click")) {
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) {
	 /* FIXME cvs this breaks soemthing */
	 /* E_DB_INT_SET(buf, "/focus/mode", 2) */
	 fprintf(stderr, "Mouse Focus Follwing clicks is broken in e\n");
      }
   } else {
      fprintf(stderr, "ERROR: Unknown Focus button clicked\n");
   }

   return;
   UN(data);
   UN(w);
}
static void
focus_mouse_autoraise_window(GtkWidget * w, gpointer data)
{
   char                buf[4096];

   if(!w)
      return;
   snprintf(buf, 4096, "%s/.e/behavior/settings.db", getenv("HOME"));

   if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
      E_DB_INT_SET(buf, "/window/raise/auto", 1)
	 else
      E_DB_INT_SET(buf, "/window/raise/auto", 0) return;
   UN(data);
   UN(w);
}
static void
focus_autoraise_timeout_cb(GtkWidget * w, gpointer data)
{
   char                buf[4096];
   float               f = 0.000;

   if(!w)
      return;
   f = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));

   snprintf(buf, 4096, "%s/.e/behavior/settings.db", getenv("HOME"));
   E_DB_FLOAT_SET(buf, "/window/raise/delay", f) return;
   UN(data);

}

static void
focus_guides_location_change_cb(GtkWidget * w, gpointer data)
{
   char                buf[4096];
   float               _f = 0.000, f = 0.000;
   float               scale = 100.00;
   char               *wname;

   if(!w)
      return;

   wname = gtk_widget_get_name(GTK_WIDGET(w));

   _f = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
   f = (_f / scale);
   snprintf(buf, 4096, "%s/.e/behavior/settings.db", getenv("HOME"));
   if(!strcmp(wname, "horiz_guide")) {
   E_DB_FLOAT_SET(buf, "/guides/display/x", f)}
      else if(!strcmp(wname, "vert_guide")) {
   E_DB_FLOAT_SET(buf, "/guides/display/y", f)} else {
      fprintf(stderr, "ERROR: Unknown Guide Spinbox\n%s\n", wname);
   }

}

/* the world can only ask for this to be added to an existing notebook, all
 * the callbacks and variable names are static for cleanliness 
 * @param w: widget pointing to the window
 * @param note: widget pointing to the notebook to attach to
 * @param sheet: what tab to insert as
 */
void
add_focus_notebook(GtkWidget * w, GtkWidget * note, int sheet)
{

   GtkWidget          *tab_label, *scale_label;
   GtkWidget          *vbox;
   GtkWidget          *frame, *frame2;
   GtkWidget          *table, *table2;
   GtkWidget          *follow_pointer;
   GtkWidget          *follow_clicks;
   GtkWidget          *autoraise;
   GtkObject          *raise_adj;
   GtkWidget          *raise_gauge;
   GtkWidget          *hsep, *hsep2;
   GtkObject          *guide_x_adj, *guide_y_adj;
   GtkWidget          *guide_x, *guide_y, *guide_x_label, *guide_y_label;

   char                buf[4096];
   int                 current_val = 0, ok = 0;
   float               flot = 0.00, _flot = 0.00;

   snprintf(buf, 4096, "%s/.e/behavior/settings.db", getenv("HOME"));

   tab_label = gtk_label_new("Focus");

   vbox = gtk_vbox_new(FALSE, 0);

   gtk_notebook_insert_page(GTK_NOTEBOOK(note), vbox, tab_label, sheet);

   frame = gtk_frame_new("Mouse Focus Settings");

   gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, TRUE, 0);

   table = gtk_table_new(8, 2, FALSE);

   gtk_container_add(GTK_CONTAINER(frame), table);

   follow_pointer =
      gtk_radio_button_new_with_label(NULL, "Mouse Focus Follows Pointer");

   gtk_widget_set_name(GTK_WIDGET(follow_pointer), "follow_pointer");
   gtk_signal_connect(GTK_OBJECT(follow_pointer), "toggled",
		      GTK_SIGNAL_FUNC(focus_mouse_focus_type_cb), NULL);
   gtk_table_attach(GTK_TABLE(table), follow_pointer, 0, 1, 0, 1, GTK_FILL,
		    0, 2, 2);

   follow_clicks =
      gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON
						  (follow_pointer),
						  "Mouse Focus Follows Mouse Clicks");
   gtk_widget_set_name(GTK_WIDGET(follow_clicks), "follow_click");
   gtk_signal_connect(GTK_OBJECT(follow_clicks), "toggled",
		      GTK_SIGNAL_FUNC(focus_mouse_focus_type_cb), NULL);
   gtk_table_attach(GTK_TABLE(table), follow_clicks, 0, 1, 1, 2, GTK_FILL,
		    0, 2, 2);

   E_DB_INT_GET(buf, "/focus/mode", current_val, ok) if(ok) {
      switch (current_val) {
      case 0:
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(follow_pointer),
				      TRUE);
	 break;
      case 2:
	 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(follow_clicks),
				      TRUE);
	 break;
      default:
	 fprintf(stderr, "I don't know focus mode %d yet\n", current_val);
      }
   }

   hsep = gtk_hseparator_new();
   gtk_table_attach(GTK_TABLE(table), hsep, 0, 2, 2, 3, GTK_FILL |
		    GTK_EXPAND, 0, 2, 2);

   /*  */
   autoraise =
      gtk_check_button_new_with_label
      ("Autoraise window on mouse focus switch");
   gtk_table_attach(GTK_TABLE(table), autoraise, 0, 1, 3, 4, GTK_FILL, 0, 2,
		    2);
   E_DB_INT_GET(buf, "/window/raise/auto", current_val, ok)
      gtk_signal_connect(GTK_OBJECT(autoraise), "toggled",
			 GTK_SIGNAL_FUNC(focus_mouse_autoraise_window),
			 NULL);
   if(current_val)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(autoraise), TRUE);

   /*  */

   scale_label = gtk_label_new("Delay in Seconds");
   gtk_label_set_justify(GTK_LABEL(scale_label), GTK_JUSTIFY_LEFT);
   gtk_table_attach(GTK_TABLE(table), scale_label, 0, 1, 4, 5, 0, 0, 2, 2);

   /* */
   E_DB_FLOAT_GET(buf, "/window/raise/delay", flot, ok) if(ok)
      raise_adj = gtk_adjustment_new(flot, 0.00, 5.00, 0.02, 1.0, 0);
   else
      raise_adj = gtk_adjustment_new(0.00, 0.00, 5.00, 0.02, 1.0, 0);
   raise_gauge = gtk_spin_button_new(GTK_ADJUSTMENT(raise_adj), 0.05, 3);
   gtk_table_attach(GTK_TABLE(table), raise_gauge, 0, 1, 5, 6, GTK_FILL, 0,
		    2, 2);
   gtk_signal_connect(GTK_OBJECT(raise_gauge), "changed",
		      GTK_SIGNAL_FUNC(focus_autoraise_timeout_cb), NULL);
   /* */

   hsep2 = gtk_hseparator_new();
   gtk_table_attach(GTK_TABLE(table), hsep2, 0, 2, 6, 7, GTK_FILL |
		    GTK_EXPAND, 0, 2, 2);
   /* End Mouse Focus */

   /* Start Guide frame */
   frame2 = gtk_frame_new("Guide Settings");
   gtk_box_pack_start(GTK_BOX(vbox), frame2, FALSE, TRUE, 0);

   table2 = gtk_table_new(2, 2, FALSE);
   gtk_table_set_col_spacings(GTK_TABLE(table2), 3);
   gtk_container_add(GTK_CONTAINER(frame2), table2);

   /* Guides_x */
   guide_x_label = gtk_label_new("Horizontal Positioning (0 left most, "
				 "100 right most) ");
   gtk_table_attach(GTK_TABLE(table2), guide_x_label, 0, 1, 0, 1, GTK_FILL,
		    0, 2, 2);
   E_DB_FLOAT_GET(buf, "/guides/display/x", _flot, ok) flot =
      (_flot * 100.00);
   if(ok)
      guide_x_adj = gtk_adjustment_new(flot, 0.00, 100.00, 0.5, 3.0, 0);
   else
      guide_x_adj = gtk_adjustment_new(50.0, 0.00, 100.00, 0.5, 3.0, 0);

   guide_x = gtk_spin_button_new(GTK_ADJUSTMENT(guide_x_adj), 0.50, 2);
   gtk_widget_set_name(GTK_WIDGET(guide_x), "horiz_guide");
   gtk_table_attach(GTK_TABLE(table2), guide_x, 0, 1, 1, 2, GTK_FILL, 0, 2,
		    2);
   gtk_signal_connect(GTK_OBJECT(guide_x), "changed",
		      GTK_SIGNAL_FUNC(focus_guides_location_change_cb),
		      NULL);

   /* Guides_y */
   guide_y_label = gtk_label_new("Vertical Positioning (0 top most, "
				 "100 bottom most) ");
   gtk_table_attach(GTK_TABLE(table2), guide_y_label, 1, 2, 0, 1, GTK_FILL,
		    0, 2, 2);

   _flot = 0.00;
   E_DB_FLOAT_GET(buf, "/guides/display/y", _flot, ok) flot =
      (_flot * 100.00);
   if(ok)
      guide_y_adj = gtk_adjustment_new(flot, 0.00, 100.00, 0.5, 3.0, 0);
   else
      guide_y_adj = gtk_adjustment_new(50.0, 0.00, 100.00, 0.5, 3.0, 0);
   guide_y = gtk_spin_button_new(GTK_ADJUSTMENT(guide_y_adj), 0.05, 2);
   gtk_widget_set_name(GTK_WIDGET(guide_y), "vert_guide");
   gtk_table_attach(GTK_TABLE(table2), guide_y, 1, 2, 1, 2, GTK_FILL, 0, 2,
		    2);
   gtk_signal_connect(GTK_OBJECT(guide_y), "changed",
		      GTK_SIGNAL_FUNC(focus_guides_location_change_cb),
		      NULL);
   /* End Guide Frame */

   /* Show those widgets */
   gtk_widget_show(guide_x);
   gtk_widget_show(guide_y);
   gtk_widget_show(guide_x_label);
   gtk_widget_show(guide_y_label);
   gtk_widget_show(table2);
   gtk_widget_show(frame2);

   gtk_widget_show(scale_label);
   gtk_widget_show(hsep2);
   gtk_widget_show(raise_gauge);
   gtk_widget_show(autoraise);
   gtk_widget_show(hsep);
   gtk_widget_show(follow_clicks);
   gtk_widget_show(follow_pointer);
   gtk_widget_show(table);
   gtk_widget_show(frame);

   gtk_widget_show(vbox);
   gtk_widget_show(tab_label);
   return;
   UN(w);
}
