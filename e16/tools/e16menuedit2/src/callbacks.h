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
#define ICON_SIZE3_STRING "36x36"
#define ICON_SIZE4_STRING "32x32"
#define ICON_SIZE5_STRING "24x24"
#define ICON_SIZE6_STRING "22x22"
#define ICON_SIZE7_STRING "16x16"

#define ICON_SIZE_AUTO 64
#define ICON_SIZE1 64
#define ICON_SIZE2 48
#define ICON_SIZE3 36
#define ICON_SIZE4 32
#define ICON_SIZE5 24
#define ICON_SIZE6 22
#define ICON_SIZE7 16

#endif /* _CALLBACKS_H */
