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
#include "support.h"
#include "workspace.h"

void    
filedialog_show(FileDialogMode mode)
{
  GtkWidget *file;
  GtkWidget *entry;
  gchar     *name;
  char      *dir = NULL;
  int        ok = 0;
  
  file = create_filesel();

  switch (mode)
    {
    case EtchingMode:
      {
	entry = gtk_object_get_data(GTK_OBJECT(main_win), "file");
	name = gtk_entry_get_text(GTK_ENTRY(entry));
	
	gtk_object_set_data(GTK_OBJECT(file), "open", (gpointer)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/bit", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/bits/");
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), name);
	  }
      }
      break;
    case NewImageMode:
      {
	gtk_object_set_data(GTK_OBJECT(file), "new_image", (void *)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/image", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
      }
      break;
    case NormalImageMode:
      {
	gtk_object_set_data(GTK_OBJECT(file), "normal_image", (void *)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/image", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
      }
      break;
    case HilitedImageMode:
      {
	gtk_object_set_data(GTK_OBJECT(file), "hilited_image", (void *)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/image", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
      }
      break;
    case ClickedImageMode:
      {
	gtk_object_set_data(GTK_OBJECT(file), "clicked_image", (void *)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/image", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
      }
      break;
    case SelectedImageMode:
      {
        gtk_object_set_data(GTK_OBJECT(file), "selected_image", (void *)1);

        E_DB_STR_GET(pref_get_config(), "/paths/image", dir, ok);
        if (ok)
          {
            gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
            free(dir);
          }
        else
          {
            gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
          }
      }
      break;
    case DisabledImageMode:
      {
	gtk_object_set_data(GTK_OBJECT(file), "disabled_image", (void *)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/image", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/examples/images/");
	  }
      }
      break;
    case GridImageMode:
      {
	int r, g, b, ok_r=0, ok_g=0, ok_b=0;
	
	gtk_object_set_data(GTK_OBJECT(file), "grid_image", (void *)1);
	
	E_DB_INT_GET(pref_get_config(), "/grid/r", r, ok_r);
	E_DB_INT_GET(pref_get_config(), "/grid/g", g, ok_g);
	E_DB_INT_GET(pref_get_config(), "/grid/b", b, ok_b);

	if (ok_r && ok_g && ok_b)
	  workspace_set_grid_tint(r, g, b);

	E_DB_STR_GET(pref_get_config(), "/grid/image", dir, ok);
	if (ok)
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	    free(dir);
	  }
	else
	  {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), PACKAGE_DATA_DIR"/pixmaps/");
	  }

      }
      break;
    case SaveAsMode:
      {
	entry = gtk_object_get_data(GTK_OBJECT(main_win), "file");
	name = gtk_entry_get_text(GTK_ENTRY(entry));

	gtk_object_set_data(GTK_OBJECT(file), "save", (gpointer)1);
	
	E_DB_STR_GET(pref_get_config(), "/paths/bit", dir, ok);
	if (ok)
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), dir);
	     free(dir);
	  }
	else
	  {
	     gtk_file_selection_set_filename(GTK_FILE_SELECTION(file), getenv("HOME"));
	  }
      }
      break;
    default:
    }

  e_db_flush();
  gtk_widget_show(file);
}


void    
filedialog_hide(GtkButton *button)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  gtk_widget_destroy(top);
}

