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
#include "etching.h"
#include "globals.h"
#include "interface.h"
#include "macros.h"
#include "preferences.h"
#include "recent.h"
#include "support.h"
#include "workspace.h"

struct etching
{
   char               *filename;
   gboolean            dirty;
   Ebits_Object        bits;
   gchar              *current_state;
   Ebits_Object_Bit_State selected_item;
};

Etching            *
etching_new(char *filename)
{
   Etching            *e;

   e = (Etching *) malloc(sizeof(Etching));
   bzero(e, sizeof(Etching));

   if (!e)
      return NULL;

   if (filename)
      e->filename = strdup(filename);

   e->bits = ebits_new();
   e->bits->description = ebits_new_description();
   e->bits->description->file = strdup(filename);

   return e;
}

Etching            *
etching_load(char *filename)
{
   Etching            *e;

   if (!filename)
      return NULL;

   e = etching_new(filename);
   e->bits = ebits_load(filename);

   if (!e->bits)
     {
	etching_free(e);
	return NULL;
     }

   return e;
}

void
etching_save_as(Etching * e, char *filename)
{
   GtkWidget          *w;

   if (!e || !filename)
      return;

   if (e->bits->description->file)
      free(e->bits->description->file);

   e->bits->description->file = strdup(filename);
   w = gtk_object_get_data(GTK_OBJECT(main_win), "file");
   gtk_entry_set_text(GTK_ENTRY(w), filename);

   if (e->bits)
      ebits_save(e->bits, filename);

   E_DB_STR_SET(pref_get_config(), "/paths/bit", filename);
   e_db_flush();
}

void
etching_save(Etching * e)
{
   if (!e)
      return;

   /*
    * GtkWidget *entry;
    * gchar *name;
    * 
    * entry = gtk_object_get_data(GTK_OBJECT(main_win), "file");
    * name = gtk_entry_get_text(GTK_ENTRY(entry));
    */

   if (e->bits)
      ebits_save(e->bits, e->filename);
}

void
etching_free(Etching * e)
{
   if (!e)
      return;

   if (e->bits)
      ebits_free(e->bits);
   if (e->filename)
      free(e->filename);
   free(e);
}

void
etching_set_dirty(Etching * e)
{
   if (!e)
      return;

   e->dirty = 1;
   workspace_set_light(Red);
}

int
etching_is_dirty(Etching * e)
{
   if (!e)
      return 0;

   if (e->dirty)
      return 1;

   return 0;
}

void
etching_set_filename(Etching * e, char *filename)
{
   if (!e)
      return;

   if (e->filename)
      free(e->filename);

   e->filename = filename;
}

char               *
etching_get_filename(Etching * e)
{
   if (e)
     {
	return e->filename;
     }

   return NULL;
}

Ebits_Object
etching_get_bits(Etching * e)
{
   if (!e)
      return NULL;

   return e->bits;
}

char               *
etching_get_current_state(Etching * e)
{
   if (!e)
      return NULL;

   if (!e->current_state)
      return "normal";

   return e->current_state;
}

void
etching_set_current_state(Etching * e, char *state)
{
   if (!e)
      return;

   if (e->current_state)
      free(e->current_state);

   e->current_state = strdup(state);
}

Ebits_Object_Bit_State
etching_get_selected_item(Etching * e)
{
   if (!e)
      return NULL;

   return e->selected_item;
}

void
etching_set_selected_item(Etching * e, Ebits_Object_Bit_State item)
{
   if (!e)
      return;

   e->selected_item = item;
}

void
etching_delete_current_item(Etching * e)
{
   if (!e)
      return;

   if (e->selected_item)
     {
	ebits_del_bit(e->bits, e->selected_item);
	e->selected_item = NULL;

	workspace_update_visible_selection();
	workspace_update_widget_from_selection();
	workspace_queue_draw();
	workspace_update_relative_combos();
	workspace_update_image_list();
	workspace_update_sync_list();
     }
}
