/**************************************************************************
 * desktops.c
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * November 5, 2002
 *************************************************************************/
#include "desktops.h"

static void
toggle_desk_slide_cb(GtkWidget * w, gpointer data)
{
   char buf[PATH_MAX];

   if (!w)
      return;
   snprintf(buf, PATH_MAX, "%s/.e/behavior/behavior.db", getenv("HOME"));

   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
   {
      E_DB_INT_SET(buf, "/desktops/scroll", 1) e_db_flush();
   }
   else
   {
      E_DB_INT_SET(buf, "/desktops/scroll", 0) e_db_flush();
   }
   return;
   UN(w);
   UN(data);
}

static void
desktop_count_changed(GtkWidget * w, gpointer data)
{
   char buf[PATH_MAX];
   gint val;

   if (!w)
      return;
   snprintf(buf, PATH_MAX, "%s/.e/behavior/behavior.db", getenv("HOME"));

   val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
   E_DB_INT_SET(buf, "/desktops/count", (int) val) e_db_flush();
}
static void
virtual_desktop_count_changed(GtkWidget * w, gpointer data)
{
   char buf[PATH_MAX], key[PATH_MAX];
   gint val;

   if (!w)
      return;
   snprintf(buf, PATH_MAX, "%s/.e/behavior/behavior.db", getenv("HOME"));
   snprintf(key, PATH_MAX, "/desktops/%s", (char *) data);

   val = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
   E_DB_INT_SET(buf, key, (int) val) e_db_flush();
}

/* the world can only ask for this to be added to an existing notebook, all
 * the callbacks and variable names are static for cleanliness 
 * @param w: widget pointing to the window
 * @param note: widget pointing to the notebook to attach to
 * @param sheet: what tab to insert as
 */
void
add_desks_notebook(GtkWidget * w, GtkWidget * note, int sheet)
{

   GtkWidget *tab_label;
   GtkWidget *vbox, *vbox2;
   GtkWidget *frame;
   GtkObject *adjustment;
   GtkWidget *hbox, *label;
   GtkWidget *spinbutton;
   GtkWidget *hbox2;
   GtkWidget *toggle;

   int vdesk_height = 1;
   int vdesk_width = 1;
   int desktop_count = 1;
   int desktop_scroll = 0;
   int ok = 0;
   GtkWidget *vbox3;

   char buf[PATH_MAX];

   snprintf(buf, PATH_MAX, "%s/.e/behavior/behavior.db", getenv("HOME"));

   {
   E_DB_INT_GET(buf, "/desktops/count", desktop_count, ok)}
   {
   E_DB_INT_GET(buf, "/desktops/width", vdesk_width, ok)}
   {
   E_DB_INT_GET(buf, "/desktops/height", vdesk_height, ok)}
   {
   E_DB_INT_GET(buf, "/desktops/scroll", desktop_scroll, ok)}


   tab_label = gtk_label_new("Desks");

   vbox = gtk_vbox_new(FALSE, 0);

   frame = gtk_frame_new("Desktop Settings");
   gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 2);

   gtk_notebook_insert_page(GTK_NOTEBOOK(note), vbox, tab_label, sheet);

   vbox2 = gtk_vbox_new(FALSE, 0);
   gtk_container_add(GTK_CONTAINER(frame), vbox2);

   /* number of desks */
   hbox = gtk_hbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, TRUE, 2);
   gtk_widget_show(hbox);
   gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);

   label = gtk_label_new("Number of Desktops");
   gtk_widget_show(label);
   gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
   gtk_misc_set_alignment(GTK_MISC(label), 0.02, 0.5);

   adjustment = gtk_adjustment_new(desktop_count, 1, 16, 1, 10, 10);
   spinbutton = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
   gtk_widget_show(spinbutton);
   gtk_box_pack_start(GTK_BOX(hbox), spinbutton, FALSE, TRUE, 7);
   gtk_signal_connect(GTK_OBJECT(spinbutton), "changed",
                      GTK_SIGNAL_FUNC(desktop_count_changed), NULL);

   /* virtual desks */
   hbox = gtk_hbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, TRUE, 2);
   gtk_widget_show(hbox);
   gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);

   label = gtk_label_new("Virtual Deskspaces");
   gtk_widget_show(label);
   gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);
   gtk_misc_set_alignment(GTK_MISC(label), 0.02, 0.5);

   vbox3 = gtk_vbox_new(FALSE, 0);
   gtk_widget_show(vbox3);
   gtk_box_pack_start(GTK_BOX(hbox), vbox3, FALSE, TRUE, 7);

   hbox2 = gtk_hbox_new(FALSE, 0);
   gtk_widget_show(hbox2);
   gtk_box_pack_start(GTK_BOX(vbox3), hbox2, TRUE, TRUE, 0);
   gtk_container_set_border_width(GTK_CONTAINER(hbox2), 2);

   label = gtk_label_new("High");
   gtk_widget_show(label);
   gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 0);

   adjustment = gtk_adjustment_new(vdesk_height, 1, 8, 1, 8, 8);
   spinbutton = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
   gtk_widget_show(spinbutton);
   gtk_box_pack_start(GTK_BOX(hbox2), spinbutton, TRUE, TRUE, 0);
   gtk_signal_connect(GTK_OBJECT(spinbutton), "changed",
                      GTK_SIGNAL_FUNC(virtual_desktop_count_changed),
                      (gpointer) "height");

   hbox2 = gtk_hbox_new(FALSE, 0);
   gtk_widget_show(hbox2);
   gtk_box_pack_start(GTK_BOX(vbox3), hbox2, TRUE, TRUE, 0);
   gtk_container_set_border_width(GTK_CONTAINER(hbox2), 2);

   label = gtk_label_new("Wide");
   gtk_widget_show(label);
   gtk_box_pack_start(GTK_BOX(hbox2), label, FALSE, FALSE, 0);

   adjustment = gtk_adjustment_new(vdesk_width, 1, 8, 1, 8, 8);
   spinbutton = gtk_spin_button_new(GTK_ADJUSTMENT(adjustment), 1, 0);
   gtk_widget_show(spinbutton);
   gtk_box_pack_start(GTK_BOX(hbox2), spinbutton, TRUE, TRUE, 0);
   gtk_signal_connect(GTK_OBJECT(spinbutton), "changed",
                      GTK_SIGNAL_FUNC(virtual_desktop_count_changed),
                      (gpointer) "width");

   /* desktop slide */
   hbox = gtk_hbox_new(FALSE, 0);
   gtk_box_pack_start(GTK_BOX(vbox2), hbox, FALSE, TRUE, 2);
   gtk_widget_show(hbox);
   gtk_container_set_border_width(GTK_CONTAINER(hbox), 2);

   toggle = gtk_check_button_new_with_label("Slide Enabled");
   gtk_widget_show(toggle);
   gtk_box_pack_start(GTK_BOX(hbox), toggle, FALSE, TRUE, 0);
   if (desktop_scroll)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), TRUE);
   else
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle), FALSE);

   gtk_signal_connect(GTK_OBJECT(toggle), "toggled",
                      GTK_SIGNAL_FUNC(toggle_desk_slide_cb), NULL);


   /* desktop slide enabled */
   gtk_widget_show(vbox2);
   gtk_widget_show(frame);
   gtk_widget_show(vbox);
   gtk_widget_show(tab_label);
   return;
   UN(w);
}
