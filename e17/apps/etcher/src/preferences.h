#ifndef ETCHER_PREF_H
#define ETCHER_PREF_H

#include <gtk/gtk.h>


typedef enum etcher_rendermethod {
	Software,
	Hardware
} EtcherRenderMethod;


typedef enum etcher_zoommethod {
	Smooth,
	Framebuffer
} EtcherZoomMethod;


int             pref_init(void);
int             pref_exists(void);
void            pref_set_defaults(void);
char           *pref_get_config(void);

void            pref_set_grid_image(char *filename);

void            pref_set_render_method(EtcherRenderMethod method);
EtcherRenderMethod pref_get_render_method(void);

void            pref_set_zoom_method(EtcherZoomMethod method);
EtcherZoomMethod pref_get_zoom_method(void);

void            pref_enable_splashscreen(gboolean yesno);
gboolean        pref_splashscreen_enabled(void);

void            pref_add_file(char *filename);
Evas_List       pref_get_files(void);

void            pref_preferences1_activate(GtkMenuItem * menuitem,
					   gpointer user_data);
void            pref_ok_clicked(GtkButton * button, gpointer user_data);
void            pref_cancel_clicked(GtkButton * button, gpointer user_data);
void            pref_tintcolor_button_clicked(GtkButton * button,
					      gpointer user_data);
void            pref_color_changed(GtkWidget * widget,
				   GtkColorSelection * colorsel);
void            pref_gridimage_changed(GtkEditable * editable,
				       gpointer user_data);

#endif
