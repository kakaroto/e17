#ifndef ETCHER_WORKSPACE_H
#define ETCHER_WORKSPACE_H

#include <etching.h>

typedef enum lamp_color
{
  Green, Red
}
LampColor;

void      workspace_init(void);
void      workspace_set_current_etching(Etching *e);
Etching  *workspace_get_current_etching(void);
void      workspace_add_etching(Etching *e);
void      workspace_add_item(char *filename);
void      workspace_remove_etching(Etching *e);
Evas      workspace_get_evas(void);

void      workspace_show_splash(void);
void      workspace_set_light(LampColor color);

void      workspace_update_selection_from_widget(void);
void      workspace_update_widget_from_selection(void);
void      workspace_update_relative_combos(void);
void      workspace_update_image_list(void);
void      workspace_update_sync_list(void);
void      workspace_update_visible_selection(void);

void      workspace_set_normal_image(char *filename);
void      workspace_set_hilited_image(char *filename);
void      workspace_set_clicked_image(char *filename);
void      workspace_set_selected_image(char *filename);
void      workspace_set_disabled_image(char *filename);

void      workspace_set_grid_image(char *filename);
void      workspace_set_grid_tint(int r, int g, int b);

void      workspace_zoom_redraw(int x, int y);
void      workspace_zoom_in(void);
void      workspace_zoom_out(void);

gint      workspace_redraw(gpointer data);
void      workspace_queue_draw(void);

void      workspace_move_to(int x, int y);
void      workspace_select_image(int index);
void      workspace_select_state(int state);

void      workspace_raise_selection(void);
void      workspace_lower_selection(void);
void      workspace_delete_selection(void);
void      workspace_reset_selection(void);

void      workspace_set_draft_mode(int active);
void      workspace_apply_settings(void);

void      workspace_button_press_event(int x, int y, int button);
void      workspace_button_release_event(int x, int y, int button);
void      workspace_enter_notify_event(GdkEventCrossing *event);
void      workspace_leave_notify_event(GdkEventCrossing *event);
void      workspace_expose_event(GtkWidget *widget, GdkEventExpose  *event);
void      workspace_configure_event(GtkWidget *widget);
void      workspace_set_state(int state);

#endif
