#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <Edb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

extern GtkWidget *main_window;
extern char *db_file;
gint  ignore_changes = 0;
gint  row_selected = -1;

int
sort_compare(const void *v1, const void *v2);

void
on_int_activate(GtkMenuItem *menuitem, gpointer user_data);

void
on_str_activate(GtkMenuItem *menuitem, gpointer user_data);

void
on_float_activate(GtkMenuItem *menuitem, gpointer user_data);

void
on_data_activate(GtkMenuItem *menuitem, gpointer user_data);

gboolean
on_window_delete_event                 (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  e_db_flush();
  exit(0);
  return FALSE;
}


gboolean
on_window_destroy_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  e_db_flush();
  exit(0);
  return FALSE;
}


void
on_open_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  GtkWidget *filesel;
  
  filesel = create_fileselection();
  gtk_widget_show(filesel);
}


void
on_delete_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text = NULL;
   gchar t[256];
   float val;

   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);

   /* make sure there is something to delete */
   if (text == NULL)
       return;

   E_DB_DEL(db_file, text);
   gtk_clist_remove(GTK_CLIST(w), row_selected);
   e_db_flush();
}

void
on_save_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  e_db_flush();
}


void
on_exit_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
  e_db_flush();
  exit(0);
}


void
on_list_select_row                     (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   gchar *key = NULL, *type = NULL;
   GtkWidget *w, *top;

   top = gtk_widget_get_toplevel(GTK_WIDGET(clist));

   /* make sure we are within the bounds of the list */
   if (row >= clist->rows)
     {
       row = clist->rows - 1;

       /* there are no rows left so make sure all the boxes
          and things are reset to blank or default */
       if (row < 0) 
         {
           w = gtk_object_get_data(GTK_OBJECT(top), "key");
           gtk_entry_set_text(GTK_ENTRY(w), "");

           w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
           gtk_notebook_set_page(GTK_NOTEBOOK(w), 0);

           w = gtk_object_get_data(GTK_OBJECT(top), "type");
           gtk_option_menu_set_history(GTK_OPTION_MENU(w), 0);   

           w = gtk_object_get_data(GTK_OBJECT(top), "integer");
           gtk_entry_set_text(GTK_ENTRY(w), "");
           return;
         }

       gtk_clist_select_row(clist, row, 0);
     }

   ignore_changes ++;
   row_selected = row;   
   gtk_clist_get_text(clist, row, 1, &key);
   gtk_clist_get_text(clist, row, 0, &type);
   w = gtk_object_get_data(GTK_OBJECT(top), "key");
   gtk_entry_set_text(GTK_ENTRY(w), key);
   if (!strcmp(type, "int"))
     {
       char s[64];
       int  val;
       int  ok;
	
	w = gtk_object_get_data(GTK_OBJECT(top), "integer");
	E_DB_INT_GET(db_file, key, val, ok);
	if (ok)
	  {
	    snprintf(s, sizeof(s), "%i", val);
	    gtk_entry_set_text(GTK_ENTRY(w), s);
	  }
	else
	  {
	    gtk_entry_set_text(GTK_ENTRY(w), "");
	  }
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 0);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 0);   
     }
   else if (!strcmp(type, "str"))
     {
	char *val;
	int ok;
	
	w = gtk_object_get_data(GTK_OBJECT(top), "string");
	E_DB_STR_GET(db_file, key, val, ok);
	if (ok)
	  {
	     guint pos;

	     pos = gtk_text_get_length(GTK_TEXT(w));
	     gtk_editable_delete_text(GTK_EDITABLE(w), 0, pos);
	     gtk_text_insert(GTK_TEXT(w), NULL, NULL, NULL,
			    val, strlen(val));
	     free(val);
	  }
	else
	  {
	     guint pos;

	     pos = gtk_text_get_length(GTK_TEXT(w));
	     if (pos > 0)
	       {
		  gtk_editable_delete_text(GTK_EDITABLE(w), 0, pos);
	       }
	  }
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 1);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 1);
     }
   else if (!strcmp(type, "float"))
     {
	float val;
	int ok;
	
	w = gtk_object_get_data(GTK_OBJECT(top), "float");
	E_DB_FLOAT_GET(db_file, key, val, ok);
	if (ok)
	  {
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), (gfloat)val);
	  }
	else
	  {
	     gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), 0);
	  }
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 2);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 2);
     }
   else
     {
	w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
	gtk_notebook_set_page(GTK_NOTEBOOK(w), 3);
	w = gtk_object_get_data(GTK_OBJECT(top), "type");
	gtk_option_menu_set_history(GTK_OPTION_MENU(w), 3);
     }
   ignore_changes --;
   e_db_flush();
}


void
on_list_unselect_row                   (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   
   ignore_changes ++;
   row_selected = -1;   
   top = gtk_widget_get_toplevel(GTK_WIDGET(clist));
   w = gtk_object_get_data(GTK_OBJECT(top), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 3);
   w = gtk_object_get_data(GTK_OBJECT(top), "type");
   gtk_option_menu_set_history(GTK_OPTION_MENU(w), 3);
   ignore_changes --;
}


void
on_integer_changed                     (GtkEditable     *editable,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *key = NULL;
   gchar *text = NULL;
   gchar t[256];
   int val;
   
   if (ignore_changes) return;

   top = gtk_widget_get_toplevel(GTK_WIDGET(editable));

   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &key);

   w = gtk_object_get_data(GTK_OBJECT(top), "integer");
   text = gtk_entry_get_text(GTK_ENTRY(w));
   val = strtoul(text, NULL, 0);
   if ((val != LONG_MAX) && (val != LONG_MIN))
     {
       w = gtk_object_get_data(GTK_OBJECT(top), "list");
       snprintf(t, 256, "%i", val);
       gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, t);

       if (key)
	 {
	   E_DB_INT_SET(db_file, key, val);
	   e_db_flush();
	 }
     }
   else
     {
       printf("err\n");
     }
}


void
on_string_changed                      (GtkEditable     *editable,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *key = NULL;
   gchar t[256];
   char *val;
   guint pos;
   
   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(editable));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &key);
   w = gtk_object_get_data(GTK_OBJECT(top), "string");
   
   pos = gtk_text_get_length(GTK_TEXT(w));
   val = gtk_editable_get_chars(GTK_EDITABLE(w), 0, pos);

   if (val)
     {
	if (strlen(val) > 32)
	  {
	     strncpy(t, val, 32);
	     t[32] = 0;
	     strcat(t, "...");
	  }
	else
	   strcpy(t, val);
     }
   else
      strcpy(t, "");
   
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, t);

   if (key && val)
     {
       E_DB_STR_SET(db_file, key, val);
       e_db_flush();
     }
   if (val)
      g_free(val);
}


void
on_float_changed                       (GtkEditable     *editable,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *key = NULL;
   gchar t[256];
   float val;
   
   if (ignore_changes) return;
   top = gtk_widget_get_toplevel(GTK_WIDGET(editable));
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &key);
   w = gtk_object_get_data(GTK_OBJECT(top), "float");
   val = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
   g_snprintf(t, sizeof(t), "%1.6f", val);
   w = gtk_object_get_data(GTK_OBJECT(top), "list");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, t);

   if (key)
     {
       E_DB_FLOAT_SET(db_file, key, val);
       e_db_flush();
     }
}


void
on_save_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
   e_db_flush();
}


void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  gtk_widget_destroy(top);
  exit(0);
}


void
on_ok_clicked                          (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  if (db_file) g_free(db_file);
  db_file = NULL;
  db_file = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
  new_db(main_window, db_file);
  gtk_widget_destroy(top);
}


gboolean
on_fileselection_delete_event          (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_widget_destroy(widget);
  return FALSE;
}


gboolean
on_fileselection_destroy_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}


void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  if (db_file) g_free(db_file);
  db_file = NULL;
  db_file = g_strdup(gtk_file_selection_get_filename(GTK_FILE_SELECTION(top)));
  new_db(main_window, db_file);
  gtk_widget_destroy(top); 
}


void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *top;
  
  top = gtk_widget_get_toplevel(GTK_WIDGET(button));
  gtk_widget_destroy(top);
}


void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *newkey;
   
   newkey = create_keyname();
   gtk_widget_show(newkey);  
}


void
on_new_key_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
   GtkWidget *w, *top;
   gchar *text[3];
   gint entry;
   
   top = gtk_widget_get_toplevel(GTK_WIDGET(button));
   w = gtk_object_get_data(GTK_OBJECT(top), "key");

   text[0] = "int";   
   text[1] = gtk_entry_get_text(GTK_ENTRY(w));
   text[2] = "0";
   
   if ((text[1] == NULL) ||
       (text[1][0] == '\0'))
     return;

   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   entry = gtk_clist_append(GTK_CLIST(w), text);
   gtk_clist_set_sort_column(GTK_CLIST(w), 1);
   gtk_clist_set_sort_type(GTK_CLIST(w), GTK_SORT_ASCENDING);
   gtk_clist_sort(GTK_CLIST(w));
   gtk_clist_select_row(GTK_CLIST(w), entry, 0);
   gtk_clist_moveto(GTK_CLIST(w), entry, 0, 0.5, 0.5);
   E_DB_INT_SET(db_file, text[1], 0);
   gtk_widget_destroy(top);
   e_db_flush();
}


void
on_int_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "int");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "0");

   w = gtk_object_get_data(GTK_OBJECT(main_window), "integer");
   gtk_entry_set_text(GTK_ENTRY(w), "0");

   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 0);

   E_DB_INT_SET(db_file, text, 0);
   e_db_flush();
}


void
on_str_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "str");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "string");
     {
	guint pos;
	
	pos = gtk_text_get_length(GTK_TEXT(w));
	if (pos > 0)
	  {
	     gtk_editable_delete_text(GTK_EDITABLE(w), 0, pos);
	  }
     }
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 1);
   E_DB_STR_SET(db_file, text, "");
   e_db_flush();
}

void
on_float_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "float");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "0.0");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "float");
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), 0);
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 2);
   E_DB_FLOAT_SET(db_file, text, 0);
   e_db_flush();
}


void
on_data_activate                       (GtkMenuItem     *menuitem,
				       gpointer         user_data)
{
   gchar *text = NULL, *type = NULL;
   GtkWidget *w;
   
   if (ignore_changes) return;
   w = gtk_object_get_data(GTK_OBJECT(main_window), "list");
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 1, &text);
   gtk_clist_get_text(GTK_CLIST(w), row_selected, 0, &type);
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 0, "?");
   gtk_clist_set_text(GTK_CLIST(w), row_selected, 2, "");
   w = gtk_object_get_data(GTK_OBJECT(main_window), "notebook1");
   gtk_notebook_set_page(GTK_NOTEBOOK(w), 3);
     {
	E_DB_File *db;
	
	db = e_db_open(db_file);
	if (db)
	  {
	     int data[1];
	     
	     data[0] = 0;
	     e_db_data_set(db, text, data, sizeof(int));
	     e_db_type_set(db, text, "?");
	     e_db_close(db);
	  }
     }
   e_db_flush();
}


int
sort_compare(const void *v1, const void *v2)
{
   return strcmp(*(char **)v1, *(char **)v2);
}

void
new_db(GtkWidget *window, char *file)
{
   GtkWidget *w;

   ignore_changes ++;
   w = gtk_object_get_data(GTK_OBJECT(window), "list");
   gtk_clist_freeze(GTK_CLIST(w));
   gtk_clist_clear(GTK_CLIST(w));
   
   if (file)
     {
	E_DB_File *db;
	char **keys;
	int keys_num;
	
	db = e_db_open_read(file);
	w = gtk_object_get_data(GTK_OBJECT(window), "list");
	
	keys = e_db_dump_key_list(file, &keys_num);
	if (keys)
	  {
	     int i;
	     
	     qsort(keys, keys_num, sizeof(char *), sort_compare);
	     for (i = 0; i < keys_num; i++)
	       {
		  char *t;
		  char *type;
		  
		  type = e_db_type_get(db, keys[i]);
		  if (type) t = type;
		  else t = "?";
		  
		    {
		       gchar *text[3];
		       char val[256];
		       int i_val;
		       float f_val;
		       char *s_val;
		       
		       val[0] = 0;
		       if (!strcmp(t, "int"))
			 {
			    if (e_db_int_get(db, keys[i], &i_val))
			       g_snprintf(val, sizeof(val), "%i", i_val);
			 }
		       else if (!strcmp(t, "str"))
			 {
			    char s_val2[64];
			    
			    s_val = NULL;
			    s_val = e_db_str_get(db, keys[i]);
			    if (s_val)
			      {
				 strncpy(s_val2, s_val, 32);
				 if (strlen(s_val) > 32)
				   {
				      s_val2[32] = 0;
				      strcat(s_val2, "...");
				   }
				 free(s_val);
				 g_snprintf(val, sizeof(val), "%s", s_val2);
			      }
			 }
		       else if (!strcmp(t, "float"))
			 {
			    if (e_db_float_get(db, keys[i], &f_val))
			       g_snprintf(val, sizeof(val), "%1.6f", f_val);
			 }
		       text[0] = t;
		       text[1] = keys[i];
		       text[2] = val;
		       gtk_clist_append(GTK_CLIST(w), text);
		    }
		  if (type) free(type);
		  free(keys[i]);
	       }
	     free(keys);
	  }
	gtk_clist_thaw(GTK_CLIST(w));
	if (db) e_db_close(db);
	e_db_flush();
     }
   ignore_changes --;
}


void
init_type_menu(void)
{
  GtkWidget *w;
  GtkWidget *type_menu;
  GtkWidget *menuitem;

  type_menu = gtk_menu_new();
  w = gtk_object_get_data(GTK_OBJECT(main_window), "type");
  
  menuitem = gtk_menu_item_new_with_label ("Integer");
  gtk_widget_show (menuitem);
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
		      GTK_SIGNAL_FUNC (on_int_activate),
		      NULL);
  gtk_menu_append (GTK_MENU (type_menu), menuitem);
  menuitem = gtk_menu_item_new_with_label ("String");
  gtk_widget_show (menuitem);
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
		      GTK_SIGNAL_FUNC (on_str_activate),
		      NULL);
  gtk_menu_append (GTK_MENU (type_menu), menuitem);
  menuitem = gtk_menu_item_new_with_label ("Float");
  gtk_widget_show (menuitem);
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
		      GTK_SIGNAL_FUNC (on_float_activate),
		      NULL);
  gtk_menu_append (GTK_MENU (type_menu), menuitem);
  menuitem = gtk_menu_item_new_with_label ("Binary Data");
  gtk_widget_show (menuitem);
  gtk_signal_connect (GTK_OBJECT (menuitem), "activate",
		      GTK_SIGNAL_FUNC (on_data_activate),
		      NULL);
  gtk_menu_append (GTK_MENU (type_menu), menuitem);
  gtk_option_menu_set_menu (GTK_OPTION_MENU(w), type_menu);
}


