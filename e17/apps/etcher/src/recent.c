#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <Evas.h>
#include <Edb.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "etching.h"
#include "globals.h"
#include "recent.h"
#include "preferences.h"
#include "workspace.h"

void
change_recent_menuitem(int item, char *label)
{
   GtkWidget          *w, *w2;
   char                s[1024];
   char               *s2;

   if (item < 0 || item >= NUM_RECENT)
      return;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "separator4");
   gtk_widget_show(w);

   sprintf(s, "recent%i", item);
   w = gtk_object_get_data(GTK_OBJECT(main_win), s);

   s2 = strrchr(label, '/');
   if (s2)
      sprintf(s, "%i. %s", item + 1, s2 + 1);
   else
      sprintf(s, "%i. %s", item + 1, label);

   w2 = gtk_object_get_data(GTK_OBJECT(main_win), "tooltips");
   if (w2)
      gtk_tooltips_set_tip(GTK_TOOLTIPS(w2), w, label, NULL);

   gtk_widget_show(w);
   gtk_label_set_text(GTK_LABEL(GTK_BIN(w)->child), s);
}

void
recent_init(void)
{
   GtkWidget          *w;

   w = gtk_object_get_data(GTK_OBJECT(main_win), "separator4");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "recent0");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "recent1");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "recent2");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "recent3");
   gtk_widget_hide(w);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "recent4");
   gtk_widget_hide(w);

   recent_reload_entries();
}

void
recent_reload_entries(void)
{
   char                s[1024];
   char               *file = NULL;
   int                 i;
   int                 ok;

   for (i = 0; i < NUM_RECENT; i++)
     {
	sprintf(s, "/recent/%i", i);
	E_DB_STR_GET(pref_get_config(), s, file, ok);

	if (file)
	   change_recent_menuitem(i, file);
     }
}

void
recent_add_file(char *filename)
{
   char                s[1024];
   char               *file = NULL;
   int                 i;
   int                 ok;

   for (i = NUM_RECENT - 2; i >= 0; i--)
     {
	sprintf(s, "/recent/%i", i);
	E_DB_STR_GET(pref_get_config(), s, file, ok);

	if (file)
	  {
	     change_recent_menuitem(i + 1, file);
	     sprintf(s, "/recent/%i", i + 1);
	     E_DB_STR_SET(pref_get_config(), s, file);
	  }
     }

   E_DB_STR_SET(pref_get_config(), "/recent/0", filename);
   e_db_flush();

   change_recent_menuitem(0, filename);
}

void
recent_load(int index)
{
   char                s[1024];
   char               *file = NULL;
   int                 ok;
   Etching            *e;

   sprintf(s, "/recent/%i", index);
   E_DB_STR_GET(pref_get_config(), s, file, ok);

   if (file)
     {
	e = etching_load(file);
	workspace_add_etching(e);
	workspace_set_current_etching(e);
	free(file);
     }
}
