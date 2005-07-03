#include "eclair_dialogs.h"
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include "../config.h"
#include "eclair.h"
#include "eclair_config.h"
#include "eclair_playlist.h"
#include "eclair_playlist_container.h"
#include "eclair_utils.h"

static void *_eclair_dialogs_thread(void *param);
static gint _eclair_dialogs_update(gpointer data);

static void _eclair_dialogs_file_chooser_open(Eclair_Dialogs_Manager *dialogs_manager, Eclair_Dialog_File_Chooser_Type file_chooser_type);
static gboolean _eclair_dialogs_file_chooser_on_add_files(GtkWidget *widget, gpointer data);
static gboolean _eclair_dialogs_file_chooser_on_save_playlist(GtkWidget *widget, gpointer data);
static gboolean _eclair_dialogs_file_chooser_on_delete(GtkWidget *widget, GdkEvent *event, gpointer data);

static void _eclair_dialogs_search_window_open(Eclair_Dialogs_Manager *dialogs_manager);

static void _eclair_dialogs_menu_on_add_files(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_load_playlist(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_save_playlist(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_select_all(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_select_none(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_invert_selection(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_remove_selected(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_remove_unselected(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_remove_all(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_shuffle_mode(GtkWidget *widget, gpointer data);
static void _eclair_dialogs_menu_on_search_window(GtkWidget *widget, gpointer data);

//Initialize dialogs manager
void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair)
{
   if (!dialogs_manager || !eclair)
      return;

   g_thread_init(NULL);
   gtk_init(eclair->argc, eclair->argv);

   dialogs_manager->eclair = eclair;

   dialogs_manager->file_chooser_xml = NULL;
   dialogs_manager->file_chooser_all_button = NULL;
   dialogs_manager->file_chooser_none_button = NULL;
   dialogs_manager->file_chooser_save_playlist_button = NULL;
   dialogs_manager->file_chooser_load_playlist_button = NULL;
   dialogs_manager->file_chooser_add_files_button = NULL;
   dialogs_manager->file_chooser_cancel_button = NULL;
   dialogs_manager->file_chooser_dialog = NULL;

   dialogs_manager->menu_xml = NULL;
   dialogs_manager->menu_widget = NULL;

   dialogs_manager->search_window_xml = NULL;

   dialogs_manager->should_quit = FALSE;
   dialogs_manager->should_popup_menu = FALSE;
   dialogs_manager->should_open_search_window = FALSE;
   dialogs_manager->should_open_file_chooser = ECLAIR_FC_NONE;

   pthread_create(&dialogs_manager->dialogs_thread, NULL, _eclair_dialogs_thread, eclair);
}

//Shutdown dialogs manager
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager)
{
   gchar *last_location_path;

   if (!dialogs_manager)
      return;

   if (dialogs_manager->eclair &&dialogs_manager->file_chooser_dialog && (last_location_path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget))))
   {
      eclair_config_set_prop_string(&dialogs_manager->eclair->config, "last_location", "path", last_location_path);
      g_free(last_location_path);
   }

   dialogs_manager->should_quit = TRUE;
   fprintf(stderr, "Dialogs: Debug: Destroying dialogs thread\n");
   pthread_join(dialogs_manager->dialogs_thread, NULL); 
   fprintf(stderr, "Dialogs: Debug: Dialogs thread destroyed\n");  
}

//Will open the file chooser dialog to add files
void eclair_dialogs_add_files_file_chooser(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (dialogs_manager)
      dialogs_manager->should_open_file_chooser = ECLAIR_FC_ADD_FILES;
}

//Will open the file chooser dialog to load playlist
void eclair_dialogs_load_playlist_file_chooser(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (dialogs_manager)
      dialogs_manager->should_open_file_chooser = ECLAIR_FC_LOAD_PLAYLIST;
}

//Will open the file chooser dialog to save the playlist
void eclair_dialogs_save_playlist_file_chooser(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (dialogs_manager)
      dialogs_manager->should_open_file_chooser = ECLAIR_FC_SAVE_PLAYLIST;
}

//Will popup the eclair menu 
void eclair_popup_menu(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (dialogs_manager)
      dialogs_manager->should_popup_menu = TRUE;
}

//Will open the playlist search window
void eclair_search_window(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (dialogs_manager)
      dialogs_manager->should_open_search_window = TRUE;
}

//Called each 50ms and check if we must do something (popup menu, open file selection dialog...)
static gint _eclair_dialogs_update(gpointer data)
{
   Eclair_Dialogs_Manager *dialogs_manager;

   if (!(dialogs_manager = data))
      return 1;

   if (dialogs_manager->should_quit)
   {
      gtk_main_quit();
      return 0;
   }

   if (dialogs_manager->should_popup_menu)
   {
      gtk_menu_popup(GTK_MENU(dialogs_manager->menu_widget), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
      dialogs_manager->should_popup_menu = FALSE;
   }
   if (dialogs_manager->should_open_file_chooser != ECLAIR_FC_NONE)
   {
      _eclair_dialogs_file_chooser_open(dialogs_manager, dialogs_manager->should_open_file_chooser);
      dialogs_manager->should_open_file_chooser = ECLAIR_FC_NONE;
   }
   if (dialogs_manager->should_open_search_window)
   {
      _eclair_dialogs_search_window_open(dialogs_manager);
      dialogs_manager->should_open_search_window = FALSE;
   }

   if (dialogs_manager->eclair)
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(glade_xml_get_widget(dialogs_manager->menu_xml, "shuffle_mode1")),
         dialogs_manager->eclair->playlist.shuffle);

   return 1;
}

//Init and start the eclair dialogs main loop
static void *_eclair_dialogs_thread(void *param)
{
   Eclair_Dialogs_Manager *dialogs_manager;
   Eclair *eclair;

   if (!(eclair = param))
      return NULL;

   dialogs_manager = &eclair->dialogs_manager;

   //Create the menu
   dialogs_manager->menu_xml = glade_xml_new(PACKAGE_DATA_DIR "/glade/eclair.glade", "popup_menu", NULL);
   dialogs_manager->menu_widget = glade_xml_get_widget(dialogs_manager->menu_xml, "popup_menu");
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "add_files_handler", G_CALLBACK(_eclair_dialogs_menu_on_add_files), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "load_playlist_handler", G_CALLBACK(_eclair_dialogs_menu_on_load_playlist), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "save_playlist_handler", G_CALLBACK(_eclair_dialogs_menu_on_save_playlist), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "select_all_handler", G_CALLBACK(_eclair_dialogs_menu_on_select_all), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "select_none_handler", G_CALLBACK(_eclair_dialogs_menu_on_select_none), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "invert_selection_handler", G_CALLBACK(_eclair_dialogs_menu_on_invert_selection), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "remove_selected_handler", G_CALLBACK(_eclair_dialogs_menu_on_remove_selected), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "remove_unselected_handler", G_CALLBACK(_eclair_dialogs_menu_on_remove_unselected), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "remove_all_handler", G_CALLBACK(_eclair_dialogs_menu_on_remove_all), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "shuffle_mode_handler", G_CALLBACK(_eclair_dialogs_menu_on_shuffle_mode), eclair);
   glade_xml_signal_connect_data(dialogs_manager->menu_xml, "search_window_handler", G_CALLBACK(_eclair_dialogs_menu_on_search_window), eclair);

   g_timeout_add(50, _eclair_dialogs_update, dialogs_manager);

   gtk_main();

   return NULL;
}

//------------------------------
// File chooser functions
//------------------------------

//Open the open file chooser
static void _eclair_dialogs_file_chooser_open(Eclair_Dialogs_Manager *dialogs_manager, Eclair_Dialog_File_Chooser_Type fc_type)
{
   char *last_location_path;
   Eclair *eclair;
   GtkWidget *file_chooser_alignment;

   if (!dialogs_manager || !(eclair = dialogs_manager->eclair) || fc_type == ECLAIR_FC_NONE)
      return;

   if (!dialogs_manager->file_chooser_xml || !dialogs_manager->file_chooser_dialog)
   {
      dialogs_manager->file_chooser_xml = glade_xml_new(PACKAGE_DATA_DIR "/glade/eclair.glade", "file_chooser_dialog", NULL);
      dialogs_manager->file_chooser_dialog = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "file_chooser_dialog");
      gtk_widget_hide(dialogs_manager->file_chooser_dialog);
      file_chooser_alignment = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "file_chooser_alignment");
      dialogs_manager->file_chooser_widget = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_container_add(GTK_CONTAINER(file_chooser_alignment), dialogs_manager->file_chooser_widget);
      gtk_widget_show(dialogs_manager->file_chooser_widget);
      dialogs_manager->file_chooser_all_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "select_all_button");
      dialogs_manager->file_chooser_none_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "select_none_button");
      dialogs_manager->file_chooser_save_playlist_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "save_playlist_button");
      dialogs_manager->file_chooser_load_playlist_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "load_playlist_button");
      dialogs_manager->file_chooser_add_files_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "add_files_button");
      dialogs_manager->file_chooser_cancel_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "cancel_button");

      //TODO: set_wmclass
      //gtk_window_set_wmclass(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "eclair", "eclair");
      gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), TRUE);
      if (eclair_config_get_prop_string(&eclair->config, "last_location", "path", &last_location_path))
      {
         if (strlen(last_location_path) > 0)
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), last_location_path);
         free(last_location_path);
      }

      g_signal_connect_swapped(dialogs_manager->file_chooser_all_button, "clicked", G_CALLBACK(gtk_file_chooser_select_all), dialogs_manager->file_chooser_widget);
      g_signal_connect_swapped(dialogs_manager->file_chooser_none_button, "clicked", G_CALLBACK(gtk_file_chooser_unselect_all), dialogs_manager->file_chooser_widget);
      g_signal_connect(dialogs_manager->file_chooser_save_playlist_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_save_playlist), eclair);
      g_signal_connect(dialogs_manager->file_chooser_load_playlist_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      g_signal_connect(dialogs_manager->file_chooser_add_files_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      g_signal_connect_swapped(dialogs_manager->file_chooser_cancel_button, "clicked", G_CALLBACK(gtk_widget_hide), dialogs_manager->file_chooser_dialog);
      g_signal_connect(dialogs_manager->file_chooser_dialog, "delete_event", G_CALLBACK(_eclair_dialogs_file_chooser_on_delete), eclair);
   }

   if (fc_type == ECLAIR_FC_ADD_FILES)
   {
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), TRUE);
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Add Files");
      gtk_widget_show(dialogs_manager->file_chooser_all_button);
      gtk_widget_show(dialogs_manager->file_chooser_none_button);
      gtk_widget_hide(dialogs_manager->file_chooser_save_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_load_playlist_button);
      gtk_widget_show(dialogs_manager->file_chooser_add_files_button);
      gtk_widget_show(dialogs_manager->file_chooser_cancel_button);
   }
   else if (fc_type == ECLAIR_FC_LOAD_PLAYLIST)
   {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Load Playlist");
      gtk_widget_hide(dialogs_manager->file_chooser_all_button);
      gtk_widget_hide(dialogs_manager->file_chooser_none_button);
      gtk_widget_hide(dialogs_manager->file_chooser_save_playlist_button);
      gtk_widget_show(dialogs_manager->file_chooser_load_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_add_files_button);
      gtk_widget_show(dialogs_manager->file_chooser_cancel_button);
   }
   else if (fc_type == ECLAIR_FC_SAVE_PLAYLIST)
   {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_SAVE);
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Save Playlist");
      gtk_widget_hide(dialogs_manager->file_chooser_all_button);
      gtk_widget_hide(dialogs_manager->file_chooser_none_button);
      gtk_widget_show(dialogs_manager->file_chooser_save_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_load_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_add_files_button);
      gtk_widget_show(dialogs_manager->file_chooser_cancel_button);
   }
   else
      return;

   gtk_window_resize(GTK_WINDOW(dialogs_manager->file_chooser_dialog), 600, 400);
   gtk_window_present(GTK_WINDOW(dialogs_manager->file_chooser_dialog));
}

//Add the files to the playlist
static gboolean _eclair_dialogs_file_chooser_on_add_files(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;
   GtkWidget *file_chooser;
   GSList *filenames, *l;

   if (!(eclair = data) || !(file_chooser = eclair->dialogs_manager.file_chooser_widget))
      return FALSE;

   if (!(filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser))))
      return FALSE;

   for (l = filenames; l; l = l->next)
      eclair_playlist_add_uri(&eclair->playlist, l->data, 0, 1);
   eclair_playlist_container_update(eclair->playlist_container);
   g_slist_foreach(filenames, (GFunc)g_free, NULL);
   g_slist_free(filenames);

   gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(file_chooser));
   gtk_widget_hide(eclair->dialogs_manager.file_chooser_dialog);
   return TRUE;
}

//Save the playlist
static gboolean _eclair_dialogs_file_chooser_on_save_playlist(GtkWidget *widget, gpointer data)
{
   Eclair *eclair = data;
   GtkWidget *file_chooser;
   gchar *filename;
   char *ext, *new_filename;

   if (!(eclair = data) || !(file_chooser = eclair->dialogs_manager.file_chooser_widget) ||
      !(filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser))) || strlen(filename) <= 0)
      return FALSE;

   if ((ext = eclair_utils_file_get_extension(filename)) && strcmp(ext, "m3u") == 0)
      eclair_playlist_save(&eclair->playlist, filename);
   else
   {
      new_filename = malloc(strlen(filename) + 5);
      sprintf(new_filename, "%s.m3u", filename);
      eclair_playlist_save(&eclair->playlist, new_filename);
      free(new_filename);      
   }
   g_free(filename);

   gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(file_chooser));
   gtk_widget_hide(eclair->dialogs_manager.file_chooser_dialog);
   return TRUE;
}

//Do not delete the open file chooser, just hide it
static gboolean _eclair_dialogs_file_chooser_on_delete(GtkWidget *widget, GdkEvent *event, gpointer data)
{
   Eclair *eclair;

   if (!(eclair = data) || !eclair->dialogs_manager.file_chooser_widget)
      return FALSE;

   gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(eclair->dialogs_manager.file_chooser_widget));
   gtk_widget_hide(eclair->dialogs_manager.file_chooser_dialog);
   return TRUE;
}

//------------------------------
// Search window functions
//------------------------------

//Open the search window
static void _eclair_dialogs_search_window_open(Eclair_Dialogs_Manager *dialogs_manager)
{
   Eclair *eclair;

   if (!dialogs_manager || !(eclair = dialogs_manager->eclair))
      return;

   if (!dialogs_manager->search_window_xml || !dialogs_manager->search_window_window)
   {
      dialogs_manager->search_window_xml = glade_xml_new(PACKAGE_DATA_DIR "/glade/eclair.glade", "search_window", NULL);
      dialogs_manager->search_window_window = glade_xml_get_widget(dialogs_manager->search_window_xml, "search_window");
      
      /*dialogs_manager->file_chooser_all_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "select_all_button");
      dialogs_manager->file_chooser_none_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "select_none_button");
      dialogs_manager->file_chooser_save_playlist_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "save_playlist_button");
      dialogs_manager->file_chooser_load_playlist_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "load_playlist_button");
      dialogs_manager->file_chooser_add_files_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "add_files_button");
      dialogs_manager->file_chooser_cancel_button = glade_xml_get_widget(dialogs_manager->file_chooser_xml, "cancel_button");
*/
      /*g_signal_connect_swapped(dialogs_manager->file_chooser_all_button, "clicked", G_CALLBACK(gtk_file_chooser_select_all), dialogs_manager->file_chooser_widget);
      g_signal_connect_swapped(dialogs_manager->file_chooser_none_button, "clicked", G_CALLBACK(gtk_file_chooser_unselect_all), dialogs_manager->file_chooser_widget);
      g_signal_connect(dialogs_manager->file_chooser_save_playlist_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_save_playlist), eclair);
      g_signal_connect(dialogs_manager->file_chooser_load_playlist_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      g_signal_connect(dialogs_manager->file_chooser_add_files_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      g_signal_connect_swapped(dialogs_manager->file_chooser_cancel_button, "clicked", G_CALLBACK(gtk_widget_hide), dialogs_manager->file_chooser_dialog);
      g_signal_connect(dialogs_manager->file_chooser_dialog, "delete_event", G_CALLBACK(_eclair_dialogs_file_chooser_on_delete), eclair);
   */}

   /*if (fc_type == ECLAIR_FC_ADD_FILES)
   {
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), TRUE);
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Add Files");
      gtk_widget_show(dialogs_manager->file_chooser_all_button);
      gtk_widget_show(dialogs_manager->file_chooser_none_button);
      gtk_widget_hide(dialogs_manager->file_chooser_save_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_load_playlist_button);
      gtk_widget_show(dialogs_manager->file_chooser_add_files_button);
      gtk_widget_show(dialogs_manager->file_chooser_cancel_button);
   }
   else if (fc_type == ECLAIR_FC_LOAD_PLAYLIST)
   {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Load Playlist");
      gtk_widget_hide(dialogs_manager->file_chooser_all_button);
      gtk_widget_hide(dialogs_manager->file_chooser_none_button);
      gtk_widget_hide(dialogs_manager->file_chooser_save_playlist_button);
      gtk_widget_show(dialogs_manager->file_chooser_load_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_add_files_button);
      gtk_widget_show(dialogs_manager->file_chooser_cancel_button);
   }
   else if (fc_type == ECLAIR_FC_SAVE_PLAYLIST)
   {
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_SAVE);
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Save Playlist");
      gtk_widget_hide(dialogs_manager->file_chooser_all_button);
      gtk_widget_hide(dialogs_manager->file_chooser_none_button);
      gtk_widget_show(dialogs_manager->file_chooser_save_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_load_playlist_button);
      gtk_widget_hide(dialogs_manager->file_chooser_add_files_button);
      gtk_widget_show(dialogs_manager->file_chooser_cancel_button);
   }
   else
      return;*/

   //gtk_window_resize(GTK_WINDOW(dialogs_manager->file_chooser_dialog), 600, 400);
   gtk_window_present(GTK_WINDOW(dialogs_manager->search_window_window));
}

//------------------------------
// Menu functions
//------------------------------

static void _eclair_dialogs_menu_on_add_files(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_add_files_file_chooser(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_load_playlist(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_load_playlist_file_chooser(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_save_playlist(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_save_playlist_file_chooser(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_select_all(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_select_all(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_select_none(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_select_none(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_invert_selection(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_invert_selection(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_remove_selected(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_remove_selected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_remove_unselected(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_remove_unselected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_remove_all(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_empty(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_shuffle_mode(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_set_shuffle(&eclair->playlist, gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)));
}

static void _eclair_dialogs_menu_on_search_window(GtkWidget *widget, gpointer data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_search_window(&eclair->dialogs_manager);
}
