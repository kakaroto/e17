#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include <gtk/gtk.h>

void on_file1_activate (GtkMenuItem *menuitem,
                        gpointer user_data);
void on_save1_activate (GtkMenuItem *menuitem,
                        gpointer user_data);		
void on_new_button (GtkButton *button, gpointer user_data);
void on_change_icon_button (GtkButton *button, gpointer user_data);
void on_save_button (GtkButton *button, gpointer user_data);
void on_delete_button (GtkButton *button, gpointer user_data);
void on_descrenderer_edited (GtkCellRendererText *celltext,
                             const gchar *string_path,
                             const gchar *new_text,
                             gpointer data);
void on_paramsrenderer_edited (GtkCellRendererText *celltext,
                               const gchar *string_path,
                               const gchar *new_text,
                               gpointer data);
void on_iconcolumn_clicked (GtkTreeViewColumn *treeviewcolumn,
                            gpointer user_data);
void update_preview_cb (GtkFileChooser *file_chooser, gpointer data);

#define ICON_SIZE_AUTO_STRING "auto (64x64 limit)"
#define ICON_SIZE1_STRING "64x64"
#define ICON_SIZE2_STRING "48x48"
#define ICON_SIZE3_STRING "32x32"
#define ICON_SIZE4_STRING "16x16"

#define ICON_SIZE_AUTO 64
#define ICON_SIZE1 64
#define ICON_SIZE2 48
#define ICON_SIZE3 32
#define ICON_SIZE4 16

#endif /* _CALLBACKS_H */
