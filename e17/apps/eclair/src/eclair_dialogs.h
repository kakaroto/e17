#ifndef _ECLAIR_DIALOGS_H_
#define _ECLAIR_DIALOGS_H_

#include <gtk/gtk.h>
#include <glade/glade.h>
#include <pthread.h>
#include "eclair_types.h"

struct _Eclair_Dialogs_Manager
{
   //File chooser widget vars
   GladeXML *file_chooser_xml;
   GtkWidget *file_chooser_all_button;
   GtkWidget *file_chooser_none_button;
   GtkWidget *file_chooser_save_playlist_button;
   GtkWidget *file_chooser_load_playlist_button;
   GtkWidget *file_chooser_add_files_button;
   GtkWidget *file_chooser_cancel_button;
   GtkWidget *file_chooser_widget;
   GtkWidget *file_chooser_dialog;

   //Menu widget vars
   GladeXML *menu_xml;
   GtkWidget *menu_widget;

   //Search window vars
   GladeXML *search_window_xml;
   GtkWidget *search_window_window;
   GtkWidget *search_tree;
   GtkListStore *search_list_store;

   gboolean should_popup_menu;
   Eclair_Dialog_File_Chooser_Type should_open_file_chooser;
   gboolean should_open_search_window;
   gboolean should_quit;

   pthread_t dialogs_thread;

   Eclair *eclair;
};

void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair);
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_add_files_file_chooser(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_load_playlist_file_chooser(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_save_playlist_file_chooser(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_popup_menu(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_search_window(Eclair_Dialogs_Manager *dialogs_manager);

#endif
