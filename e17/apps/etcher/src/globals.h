#ifndef ETCHER_GLOBALS_H
#define ETCHER_GLOBALS_H

extern gint         render_method;
extern gint         zoom_method;
extern GtkWidget   *main_win;
extern Evas_Object  o_bg;
extern Evas         view_evas;
extern guint        current_idle;
extern gboolean     no_splash;
extern char        *load_file;
extern char         etcher_config[4096];
extern GtkWidget   *pref_dialog;
extern GtkWidget   *color_dialog;

#endif
