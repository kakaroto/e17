#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <Evas.h>
#include <Edb.h>
#include <gdk/gdkx.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bits.h"
#include "callbacks.h"
#include "file_dialog.h"
#include "globals.h"
#include "interface.h"
#include "macros.h"
#include "preferences.h"
#include "recent.h"
#include "splash.h"
#include "support.h"
#include "workspace.h"

gint view_configure_handles(gpointer data);

gboolean
on_pref_da_expose_event2                (GtkWidget       *widget,
					 GdkEventExpose  *event,
					 gpointer         user_data);


void
on_file_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  if (gtk_object_get_data(GTK_OBJECT(top), "open"))
    {
      Etching *e;

      e = etching_load(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
      if (e)
	{
	  recent_add_file(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
	  workspace_add_etching(e);
	  workspace_set_current_etching(e);
	}
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "new_image"))
    {
      workspace_add_item(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "normal_image"))
    {
      workspace_set_normal_image(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "hilited_image"))
    {
      workspace_set_hilited_image(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "clicked_image"))
    {
      workspace_set_clicked_image(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "selected_image"))
    {
      workspace_set_selected_image(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "disabled_image"))
    {
      workspace_set_disabled_image(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "grid_image"))
    {
      pref_set_grid_image(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  else if (gtk_object_get_data(GTK_OBJECT(top), "save"))
    {
      etching_save_as(workspace_get_current_etching(), gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
    }
  
  gtk_widget_destroy(top);
}


void
on_file_cancel_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_hide(button);
}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  filedialog_show(EtchingMode);
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  etching_save(workspace_get_current_etching());
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  filedialog_show(SaveAsMode);
}


void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  pref_preferences1_activate(menuitem, user_data);
}


void
on_recent0_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  recent_load(0);
}


void
on_recent1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  recent_load(1);
}


void
on_recent2_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  recent_load(2);
}


void
on_recent3_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  recent_load(3);
}


void
on_recent4_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  recent_load(4);

}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  exit(0);
}


void
on_delete1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  etching_delete_current_item(workspace_get_current_etching());
}


void
on_undo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_redo1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  workspace_show_splash();
}


gboolean
on_view_motion_notify_event            (GtkWidget       *widget,
                                        GdkEventMotion  *event,
                                        gpointer         user_data)
{
  workspace_move_to(event->x, event->y);
  return FALSE;
}


gboolean
on_view_button_press_event             (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  workspace_button_press_event(event->x, event->y, event->button);
  return FALSE;
}


gboolean
on_view_button_release_event           (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data)
{
  workspace_button_release_event(event->x, event->y, event->button);
  return FALSE;
}

gboolean
on_view_enter_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
  workspace_enter_notify_event(event);
  return FALSE;
}


gboolean
on_view_leave_notify_event             (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data)
{
  workspace_leave_notify_event(event);
  return FALSE;
}


gboolean
on_view_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  workspace_expose_event(widget, event);
  return FALSE;
}


gboolean
on_view_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  if (!widget->window)
    return FALSE;
  
  workspace_configure_event(widget);
  return FALSE;
}


gboolean
on_zoom_configure_event                (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data)
{
  workspace_queue_draw();
  return FALSE;
}


gboolean
on_zoom_expose_event                   (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
  workspace_queue_draw();
  return FALSE;
}


void
on_images_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  workspace_select_image(row);
}


void
on_states_select_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  workspace_set_state(row);
}


void
on_new_image_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(NewImageMode);
}


void
on_new_icon_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
}


void
on_new_text_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
}


void
on_raise_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_raise_selection();
}


void
on_lower_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_lower_selection();
}


void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_delete_selection();
}


void
on_reset_clicked                       (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_reset_selection();
}


void
on_prop_apply_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_apply_settings();
}


void
on_prop_reset_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_update_widget_from_selection();
}


void
on_draft_toggled                       (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
  workspace_set_draft_mode(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(togglebutton)));
}


gboolean
on_main_delete_event                   (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  exit(0);
  return FALSE;
}


gboolean
on_filesel_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  return FALSE;
}


void
on_software_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  pref_set_render_method(Software);
  gtk_widget_destroy(top);
  gtk_main_quit();
}

void
on_3d_hardware_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  pref_set_render_method(Hardware);
  gtk_widget_destroy(top);
  gtk_main_quit();
}


void
on_browse_clicked_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(ClickedImageMode);
}


void
on_borwse_hilited_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(HilitedImageMode);
}


void
on_browse_normal_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(NormalImageMode);
}


void
on_browse_selected_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(SelectedImageMode);
}


void
on_browse_disabled_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(DisabledImageMode);
}


gboolean
on_preferences_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gtk_widget_destroy(widget);
   return FALSE;
}


void
on_ok_clicked                          (GtkButton       *button,
                                        gpointer         user_data)
{
   pref_ok_clicked(button, user_data);
}


void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
   pref_cancel_clicked(button, user_data);
}


void
on_zoomin_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_zoom_in();
}


void
on_zoomout_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
  workspace_zoom_out();
}


void
on_tintcolor_button_clicked            (GtkButton       *button,
                                        gpointer         user_data)
{
  pref_tintcolor_button_clicked(button, user_data);
}

void
on_selectimage_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  filedialog_show(GridImageMode);
}


void
on_gridimage_changed                   (GtkEditable     *editable,
                                        gpointer         user_data)
{
  pref_gridimage_changed(editable, user_data);
}


gboolean
on_pref_da_expose_event                (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data)
{
   on_pref_da_expose_event2(widget, event, user_data);
   return FALSE;
}


void
on_etchings1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

