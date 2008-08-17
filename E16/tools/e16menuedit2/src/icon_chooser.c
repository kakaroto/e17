/* Copyright (C) 2004 Andreas Volz and various contributors
 * 
 * Permission  is  hereby granted, free of charge, to any person ob-
 * taining a copy of  this  software  and  associated  documentation
 * files  (the "Software"), to deal in the Software without restric-
 * tion, including without limitation the rights to use, copy, modi-
 * fy, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is  fur-
 * nished to do so, subject to the following conditions:
 * 
 * The  above  copyright  notice and this permission notice shall be
 * included in all copies of the  Software,  its  documentation  and
 * marketing & publicity materials, and acknowledgment shall be giv-
 * en in the documentation, materials  and  software  packages  that
 * this Software was used.
 * 
 * THE  SOFTWARE  IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO  THE  WARRANTIES
 * OF  MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONIN-
 * FRINGEMENT. IN NO EVENT SHALL  THE  AUTHORS  BE  LIABLE  FOR  ANY
 * CLAIM,  DAMAGES  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CON-
 * TRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR  IN  CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  File: icon_chooser.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "icon_chooser.h"
#include "e16menuedit2.h"
#include "callbacks.h"
#include "file.h"
#include "treeview.h"

extern struct global_variables gv;

void open_icon_chooser (GtkWidget *treeview_menu)
{
  GtkWidget *main_window;
  GtkWidget *dialog;
  GtkWidget *filechooser;
  GtkWidget *preview;
  /*GtkWidget *treeview_menu;*/
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

  main_window = gtk_widget_get_toplevel (GTK_WIDGET(treeview_menu));
  /*treeview_menu = (GtkWidget*) user_data;*/
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

      if (gtk_tree_selection_get_selected (select, &model, &iter) 
	&& filename != NULL)
      {
        const gchar *entry_select;
        gint width, height;
        gint width_new, height_new;
        gchar filename_resized[PATH_MAX];
        gchar *right;

        entry_select = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO (
                                             resize_combo)->entry));

        /* check for broken librsvg function
         * perhaps better handling next stable librsvg release
         */
        right = g_malloc (3);
        strsplit (filename, &right, g_utf8_strlen (filename, -1) - 3);
        if ((strcmp (right, "svg")) || (gv.librsvg_cmp >= 0))
        {
          gdk_pixbuf_get_file_info (filename,
                                    &width,
                                    &height);
        }
        else
        {
          width = ICON_SIZE_AUTO+1;
          height = ICON_SIZE_AUTO+1;
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
      break;
    }
  }

  gtk_widget_destroy (dialog);
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

    /* check for broken librsvg function
     * perhaps better handling next stable librsvg release
     */
    right = g_malloc (3);
    strsplit (filename, &right, g_utf8_strlen (filename, -1) - 3);

    if ((strcmp (right, "svg")) || (gv.librsvg_cmp >= 0))
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
