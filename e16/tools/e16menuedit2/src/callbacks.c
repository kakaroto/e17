#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "callbacks.h"
#include "e16menuedit2.h"
#include "file.h"

void on_file1_activate (GtkMenuItem *menuitem,
                        gpointer user_data)
{

}

void on_save1_activate (GtkMenuItem *menuitem,
                        gpointer user_data)
{
  g_print ("menu save not yet available!\n");
}

void on_new_button (GtkButton *button, gpointer user_data)
{
  GtkWidget *treeview_menu;
  GtkTreeModel *model;
  GtkTreeSelection *select;
  GtkTreeIter sibling;
  GtkTreeIter new_iter;
  GtkTreeIter root_iter;
  gchar *empty_desc, *empty_icon;
  gchar *tree_string;

  treeview_menu = (GtkWidget*) user_data;

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));

  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_menu));

  if (gtk_tree_selection_get_selected (select, &model, &sibling))
  {
    empty_desc = g_strdup (_("New Entry"));
    empty_icon = g_strdup ("");

    tree_string = gtk_tree_model_get_string_from_iter (
                    GTK_TREE_MODEL(model), &sibling);

    if (!strcmp (tree_string, "0"))
    {
      gtk_tree_model_get_iter_first (GTK_TREE_MODEL(model), &root_iter);
      gtk_tree_store_append (GTK_TREE_STORE (model), &new_iter, &root_iter);
    }
    else
    {
      gtk_tree_store_insert_after (GTK_TREE_STORE (model),
                                   &new_iter,
                                   NULL,
                                   &sibling);
    }

    gtk_tree_store_set (GTK_TREE_STORE (model), &new_iter,
                        COL_DESCRIPTION, empty_desc,
                        COL_ICONNAME, empty_icon,
                        -1);
    g_free (empty_desc);
    g_free (empty_icon);
  }
}

void on_change_icon_button (GtkButton *button, gpointer user_data)
{
  GtkWidget *main_window;
  GtkWidget *dialog;
  GtkWidget *filechooser;
  GtkWidget *preview;
  GtkWidget *treeview_menu;
  GtkTreeModel *model;
  GtkTreeSelection *select;
  GdkPixbuf *icon_pixbuf;
  GtkTreeIter iter;
  GtkWidget *resize_combo;
  GtkWidget *resize_hbox;
  GtkWidget *dialog_vbox;
  GtkWidget *resize_label;
  GList *glist = NULL;
  gint dialog_response;

  main_window = gtk_widget_get_toplevel (GTK_WIDGET(button));
  treeview_menu = (GtkWidget*) user_data;
  preview = gtk_image_new ();
  filechooser = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_widget_set_size_request (filechooser, 600, 400);

  dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (dialog), _("Select Icon..."));
  dialog_vbox = GTK_DIALOG(dialog)->vbox;

  gtk_dialog_add_button (GTK_DIALOG(dialog),
                         GTK_STOCK_CLEAR, GTK_RESPONSE_REJECT);
  gtk_dialog_add_button (GTK_DIALOG(dialog),
                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
  gtk_dialog_add_button (GTK_DIALOG(dialog),
                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT);

  gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
  gtk_window_resize (GTK_WINDOW (dialog), 700, 500);
  gtk_dialog_set_has_separator (GTK_DIALOG(dialog), TRUE);

  gtk_box_set_homogeneous (GTK_BOX (dialog_vbox), FALSE);
  gtk_container_add (GTK_CONTAINER (dialog_vbox),
                     filechooser);

  gtk_file_chooser_set_preview_widget (GTK_FILE_CHOOSER (filechooser),
                                       preview);

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (filechooser),
                                       "/usr/share/pixmaps");

  gtk_file_chooser_set_preview_widget_active (GTK_FILE_CHOOSER (filechooser),
      TRUE);

  g_signal_connect (filechooser, "update-preview",
                    G_CALLBACK (update_preview_cb), preview);

  /* combo box */
  resize_hbox = gtk_hbox_new (FALSE, 0);
  dialog_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                     resize_hbox);
  resize_combo = gtk_combo_new ();
  resize_label = gtk_label_new (_("Icon size:"));
  glist = g_list_append (glist, ICON_SIZE_AUTO_STRING);
  glist = g_list_append (glist, ICON_SIZE1_STRING);
  glist = g_list_append (glist, ICON_SIZE2_STRING);
  glist = g_list_append (glist, ICON_SIZE3_STRING);
  glist = g_list_append (glist, ICON_SIZE4_STRING);
  glist = g_list_append (glist, ICON_SIZE5_STRING);
  glist = g_list_append (glist, ICON_SIZE6_STRING);
  glist = g_list_append (glist, ICON_SIZE7_STRING);
  gtk_combo_set_popdown_strings (GTK_COMBO (resize_combo), glist);
  gtk_entry_set_text (GTK_ENTRY (GTK_COMBO (resize_combo)->entry),
                      "auto (64x64 limit)");
  gtk_box_pack_start (GTK_BOX (resize_hbox), resize_label, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (resize_hbox), resize_combo, FALSE, FALSE, 0);

  gtk_widget_show_all (dialog);

  dialog_response = gtk_dialog_run (GTK_DIALOG (dialog));

  switch (dialog_response)
  {
    /* select new icon */
  case GTK_RESPONSE_ACCEPT:
    {
      gchar *filename;


      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));

      model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));
      select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_menu));

      if (gtk_tree_selection_get_selected (select, &model, &iter))
      {
        const gchar *entry_select;
        gint width, height;
        gint width_new, height_new;
        gchar filename_resized[PATH_MAX];
        gchar *right;

        entry_select = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (
                                             resize_combo)->entry));

        /* temporary fix for probability broken librsvg function.
         * it should autodetect librsvg version...
         */
        right = g_malloc (3);
        strsplit (filename, &right, g_utf8_strlen (filename, -1) - 3);
        if (strcmp (right, "svg"))
        {
          gdk_pixbuf_get_file_info (filename,
                                    &width,
                                    &height);
        }
        else
        {
          width = ICON_SIZE_AUTO;
          height = ICON_SIZE_AUTO;
        }
        g_free (right);

        if (width <= 0)
          width = 1;
        if (height <= 0)
          height = 1;

        if (!strcmp (entry_select, ICON_SIZE_AUTO_STRING))
        {
          if ((width > ICON_SIZE_AUTO) || (height > ICON_SIZE_AUTO))
          {
            width_new = ICON_SIZE_AUTO;
            height_new = ICON_SIZE_AUTO;
          }
          else
          {
            width_new = width;
            height_new = height;
          }
        }
        else if (!strcmp (entry_select, ICON_SIZE1_STRING))
        {
          width_new = ICON_SIZE1;
          height_new = ICON_SIZE1;
        }
        else if (!strcmp (entry_select, ICON_SIZE2_STRING))
        {
          width_new = ICON_SIZE2;
          height_new = ICON_SIZE2;
        }
        else if (!strcmp (entry_select, ICON_SIZE3_STRING))
        {
          width_new = ICON_SIZE3;
          height_new = ICON_SIZE3;
        }
        else if (!strcmp (entry_select, ICON_SIZE4_STRING))
        {
          width_new = ICON_SIZE4;
          height_new = ICON_SIZE4;
        }
        else if (!strcmp (entry_select, ICON_SIZE5_STRING))
        {
          width_new = ICON_SIZE5;
          height_new = ICON_SIZE5;
        }
        else if (!strcmp (entry_select, ICON_SIZE6_STRING))
        {
          width_new = ICON_SIZE6;
          height_new = ICON_SIZE6;
        }
        else if (!strcmp (entry_select, ICON_SIZE7_STRING))
        {
          width_new = ICON_SIZE7;
          height_new = ICON_SIZE7;
        }

        if ((width == width_new) && (height == height_new))
        {
          icon_pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
        }
        else
        {
          icon_pixbuf = gdk_pixbuf_new_from_file_at_size (filename,
                        width_new,
                        height_new,
                        NULL);

          sprintf (filename_resized, "%s/%s/%s/%s_%dx%d.png", homedir (getuid ()),
                   APP_HOME, ICON_DIR, g_path_get_basename (filename),
                   width_new, height_new);

          g_free (filename);
          filename = strdup (filename_resized);

          /*printf ("%s\n", filename);*/

          gdk_pixbuf_save (icon_pixbuf, filename, "png", NULL, NULL);
        }

        if (icon_pixbuf != NULL)
        {
          gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                              COL_ICON, icon_pixbuf,
                              COL_ICONNAME, filename,
                              -1);
        }
      }

      g_free (filename);
      break;
    }

    /* delete current icon */
  case GTK_RESPONSE_REJECT:
    {
      gchar *filename;

      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));

      model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));
      select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_menu));

      if (gtk_tree_selection_get_selected (select, &model, &iter))
      {
        gtk_tree_model_get (model, &iter,
                            COL_ICON, &icon_pixbuf,
                            -1);

        if (icon_pixbuf != NULL)
        {
          g_object_unref (icon_pixbuf);
        }

        gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                            COL_ICON, NULL,
                            COL_ICONNAME, "",
                            -1);

      }

      g_free (filename);
      break;
    }
  }

  gtk_widget_destroy (dialog);

}
void on_save_button (GtkButton *button, gpointer user_data)
{
  GtkWidget *treeview_menu;
  GtkTreeModel *model;
  int i = 0;

  treeview_menu = (GtkWidget*) user_data;
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));
  gtk_tree_model_foreach (GTK_TREE_MODEL(model), table_save_func, NULL);

  printf ("Menu saved!\n");

  /* free allocated menu files */
  while (menu_file[i] != NULL)
  {
    g_free (menu_file[i]);
    i++;
  }
}

void on_delete_button (GtkButton *button, gpointer user_data)
{
  GtkTreeIter iter;
  GtkWidget *treeview_menu;
  GtkTreeModel *model;
  GtkTreeSelection *select;

  treeview_menu = (GtkWidget*) user_data;
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview_menu));
  select = gtk_tree_view_get_selection (GTK_TREE_VIEW (treeview_menu));

  if (gtk_tree_selection_get_selected (select, &model, &iter))
  {
    gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);
  }
}

void on_descrenderer_edited (GtkCellRendererText *celltext,
                             const gchar *string_path,
                             const gchar *new_text,
                             gpointer data)
{
  GtkTreeModel *model = GTK_TREE_MODEL(data);
  GtkTreeIter iter;
  gchar *desc = NULL;

  gtk_tree_model_get_iter_from_string (model, &iter, string_path);
  gtk_tree_model_get (model, &iter, COL_DESCRIPTION, &desc, -1);
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                      COL_DESCRIPTION, new_text,
                      -1);

  g_free (desc);

  g_print ("edited\n");
}

void on_paramsrenderer_edited (GtkCellRendererText *celltext,
                               const gchar *string_path,
                               const gchar *new_text,
                               gpointer data)
{
  GtkTreeModel *model = GTK_TREE_MODEL(data);
  GtkTreeIter iter;
  gchar *params = NULL;

  gtk_tree_model_get_iter_from_string (model, &iter, string_path);
  gtk_tree_model_get (model, &iter, COL_PARAMS, &params, -1);
  gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
                      COL_PARAMS, new_text,
                      -1);

  g_free (params);

  g_print ("edited\n");
}

void on_iconcolumn_clicked (GtkTreeViewColumn *treeviewcolumn,
                            gpointer user_data)

{
  printf ("change icon\n");
}

void update_preview_cb (GtkFileChooser *file_chooser, gpointer data)
{
  GtkWidget *preview;
  char *filename;
  GdkPixbuf *pixbuf;
  gboolean have_preview;
  gint width, height;

  preview = GTK_WIDGET (data);
  filename = gtk_file_chooser_get_preview_filename (file_chooser);

  if (filename != NULL)
  {
    gchar *right;

    /* temporary fix for probability broken librsvg function
     * it should autodetect librsvg version...
     */
    right = g_malloc (3);
    strsplit (filename, &right, g_utf8_strlen (filename, -1) - 3);

    if (strcmp (right, "svg"))
    {
      gdk_pixbuf_get_file_info (filename, &width, &height);
    }
    else
    {
      width = ICON_SIZE_AUTO;
      height = ICON_SIZE_AUTO;
    }
    g_free (right);

    if ((width > ICON_SIZE_AUTO) || (height > ICON_SIZE_AUTO))
    {
      width = ICON_SIZE_AUTO;
      height = ICON_SIZE_AUTO;
    }

    if (width <= 0)
      width = 1;
    if (height <= 0)
      height = 1;

    pixbuf = gdk_pixbuf_new_from_file_at_size (filename, width, height, NULL);
    have_preview = (pixbuf != NULL);
    g_free (filename);

    gtk_image_set_from_pixbuf (GTK_IMAGE (preview), pixbuf);
    if (pixbuf)
      gdk_pixbuf_unref (pixbuf);

    gtk_file_chooser_set_preview_widget_active (file_chooser, have_preview);
  }
}
