#include "eclair_dialogs.h"
#include <gtk/gtk.h>
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

static void _eclair_dialogs_menu_on_add_files(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_load_list(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_save_list(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_select_all(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_select_none(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_invert_selection(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_remove_selected(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_remove_unselected(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_menu_on_remove_all(gpointer data, guint action, GtkWidget  *widget);

static GtkItemFactoryEntry menu_items[] = {
   { "/tear",                       NULL,       NULL,                                        0, "<Tearoff>" },
   { "/Open",                       NULL,       NULL,                                        0, "<Branch>" },
   { "/Open/tear",                  NULL,       NULL,                                        0, "<Tearoff>" },
   { "/Open/Add Files",             NULL,       _eclair_dialogs_menu_on_add_files,           1, "<Item>" },
   { "/Playlist",                   NULL,       NULL,                                        0, "<Branch>" },
   { "/Playlist/tear",              NULL,       NULL,                                        0, "<Tearoff>" },
   { "/Playlist/Load List",         NULL,       _eclair_dialogs_menu_on_load_list,           1, "<Item>" },
   { "/Playlist/Save List",         NULL,       _eclair_dialogs_menu_on_save_list,           1, "<Item>" },
   { "/Playlist/separator1",        NULL,       NULL,                                        0, "<Separator>" },
   { "/Playlist/Select All",        NULL,       _eclair_dialogs_menu_on_select_all,          1, "<Item>" },
   { "/Playlist/Select None",       NULL,       _eclair_dialogs_menu_on_select_none,         1, "<Item>" },
   { "/Playlist/Invert Selection",  NULL,       _eclair_dialogs_menu_on_invert_selection,    1, "<Item>" },
   { "/Playlist/separator2",        NULL,       NULL,                                        0, "<Separator>" },
   { "/Playlist/Remove Selected",   NULL,       _eclair_dialogs_menu_on_remove_selected,     1, "<Item>" },
   { "/Playlist/Remove Unselected", NULL,       _eclair_dialogs_menu_on_remove_unselected,   1, "<Item>" },
   { "/Playlist/Remove All",        NULL,       _eclair_dialogs_menu_on_remove_all,          1, "<Item>" }
};

static gint num_menu_items = sizeof(menu_items) / sizeof(menu_items[0]);

//Initialize dialogs manager
void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair)
{
   if (!dialogs_manager || !eclair)
      return;

   dialogs_manager->eclair = eclair;

   dialogs_manager->file_chooser_all_button = NULL;
   dialogs_manager->file_chooser_none_button = NULL;
   dialogs_manager->file_chooser_ok_button = NULL;
   dialogs_manager->file_chooser_close_button = NULL;
   dialogs_manager->file_chooser_widget = NULL;
   dialogs_manager->file_chooser_dialog = NULL;
   dialogs_manager->menu_widget = NULL;

   dialogs_manager->should_quit = FALSE;
   dialogs_manager->should_popup_menu = FALSE;
   dialogs_manager->should_open_file_chooser = ECLAIR_FC_NONE;

   pthread_create(&dialogs_manager->dialogs_thread, NULL, _eclair_dialogs_thread, eclair);
}

//Shutdown dialogs manager
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager)
{
   gchar *last_location_path;

   if (!dialogs_manager)
      return;

   if (dialogs_manager->file_chooser_widget && dialogs_manager->eclair && (last_location_path = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget))))
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
   if (!dialogs_manager)
      return;

   dialogs_manager->should_open_file_chooser = ECLAIR_FC_ADD_FILES;
}

//Will open the file chooser dialog to load playlist
void eclair_dialogs_load_playlist_file_chooser(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;

   dialogs_manager->should_open_file_chooser = ECLAIR_FC_LOAD_PLAYLIST;
}

//Will open the file chooser dialog to save the playlist
void eclair_dialogs_save_playlist_file_chooser(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;

   dialogs_manager->should_open_file_chooser = ECLAIR_FC_SAVE_PLAYLIST;
}

//Will popup the eclair menu 
void eclair_popup_menu(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;

   dialogs_manager->should_popup_menu = TRUE;
}

//Called each 50ms and check if we must do something (popup menu, open file selection dialog...)
static gint _eclair_dialogs_update(gpointer data)
{
   Eclair_Dialogs_Manager *dialogs_manager = (Eclair_Dialogs_Manager *)data;

   if (!dialogs_manager)
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

   return 1;
}

//Init and start the eclair dialogs main loop
static void *_eclair_dialogs_thread(void *param)
{
   GtkItemFactory *item_factory;
   Eclair_Dialogs_Manager *dialogs_manager;
   Eclair *eclair = (Eclair *)param;

   if (!eclair)
      return NULL;

   gtk_init(eclair->argc, eclair->argv);

   dialogs_manager = &eclair->dialogs_manager;

   //Create the menu
   item_factory = gtk_item_factory_new(GTK_TYPE_MENU, "<main>", NULL);
   gtk_item_factory_create_items(item_factory, num_menu_items, menu_items, eclair);
   dialogs_manager->menu_widget = gtk_item_factory_get_widget(item_factory, "<main>");

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

   if (!dialogs_manager || !(eclair = dialogs_manager->eclair) || fc_type == ECLAIR_FC_NONE)
      return;

   if (!dialogs_manager->file_chooser_dialog)
   {
      //Create the file chooser
      dialogs_manager->file_chooser_dialog = gtk_dialog_new_with_buttons("Add Files", NULL, GTK_DIALOG_NO_SEPARATOR, NULL);
      gtk_window_set_wmclass(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "eclair", "eclair");
      gtk_window_set_default_size(GTK_WINDOW(dialogs_manager->file_chooser_dialog), 600, 420);
      dialogs_manager->file_chooser_widget = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      if (eclair_config_get_prop_string(&eclair->config, "last_location", "path", &last_location_path))
      {
         if (strlen(last_location_path) > 0)
            gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), last_location_path);
         free(last_location_path);
      }
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialogs_manager->file_chooser_dialog)->vbox), dialogs_manager->file_chooser_widget, TRUE, TRUE, 0);
   
      dialogs_manager->file_chooser_all_button = gtk_button_new_with_label("All");
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialogs_manager->file_chooser_dialog)->action_area), dialogs_manager->file_chooser_all_button, TRUE, TRUE, 0);
      dialogs_manager->file_chooser_none_button = gtk_button_new_with_label("None");
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialogs_manager->file_chooser_dialog)->action_area), dialogs_manager->file_chooser_none_button, TRUE, TRUE, 0);
      dialogs_manager->file_chooser_ok_button = gtk_button_new_with_label("Add Files");
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialogs_manager->file_chooser_dialog)->action_area), dialogs_manager->file_chooser_ok_button, TRUE, TRUE, 0);
      dialogs_manager->file_chooser_close_button = gtk_button_new_with_label("Close");
      gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialogs_manager->file_chooser_dialog)->action_area), dialogs_manager->file_chooser_close_button, TRUE, TRUE, 0);

      g_signal_connect_swapped(dialogs_manager->file_chooser_all_button, "clicked", G_CALLBACK(gtk_file_chooser_select_all), dialogs_manager->file_chooser_widget);
      g_signal_connect_swapped(dialogs_manager->file_chooser_none_button, "clicked", G_CALLBACK(gtk_file_chooser_unselect_all), dialogs_manager->file_chooser_widget);
      dialogs_manager->file_chooser_ok_handler = g_signal_connect(dialogs_manager->file_chooser_ok_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      g_signal_connect_swapped(dialogs_manager->file_chooser_close_button, "clicked", G_CALLBACK(gtk_widget_hide), dialogs_manager->file_chooser_dialog);
      g_signal_connect(dialogs_manager->file_chooser_dialog, "delete_event", G_CALLBACK(_eclair_dialogs_file_chooser_on_delete), eclair);
      
      gtk_widget_show_all(dialogs_manager->file_chooser_dialog);
   }

   if (fc_type == ECLAIR_FC_ADD_FILES)
   {
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), TRUE);
      gtk_button_set_label(GTK_BUTTON(dialogs_manager->file_chooser_ok_button), "Add Files");
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Add Files");
      g_signal_handler_disconnect(dialogs_manager->file_chooser_ok_button, dialogs_manager->file_chooser_ok_handler);
      dialogs_manager->file_chooser_ok_handler = g_signal_connect(dialogs_manager->file_chooser_ok_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      gtk_widget_show(dialogs_manager->file_chooser_all_button);
      gtk_widget_show(dialogs_manager->file_chooser_none_button);
   }
   else if (fc_type == ECLAIR_FC_LOAD_PLAYLIST)
   {
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_OPEN);
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      gtk_button_set_label(GTK_BUTTON(dialogs_manager->file_chooser_ok_button), "Load Playlist");
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Load Playlist");
      g_signal_handler_disconnect(dialogs_manager->file_chooser_ok_button, dialogs_manager->file_chooser_ok_handler);
      dialogs_manager->file_chooser_ok_handler = g_signal_connect(dialogs_manager->file_chooser_ok_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_add_files), eclair);
      gtk_widget_hide(dialogs_manager->file_chooser_all_button);
      gtk_widget_hide(dialogs_manager->file_chooser_none_button);
   }
   else if (fc_type == ECLAIR_FC_SAVE_PLAYLIST)
   {
      gtk_file_chooser_set_action(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), GTK_FILE_CHOOSER_ACTION_SAVE);
      gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_widget), FALSE);
      gtk_button_set_label(GTK_BUTTON(dialogs_manager->file_chooser_ok_button), "Save Playlist");
      gtk_window_set_title(GTK_WINDOW(dialogs_manager->file_chooser_dialog), "Save Playlist");
      g_signal_handler_disconnect(dialogs_manager->file_chooser_ok_button, dialogs_manager->file_chooser_ok_handler);
      dialogs_manager->file_chooser_ok_handler = g_signal_connect(dialogs_manager->file_chooser_ok_button, "clicked", G_CALLBACK(_eclair_dialogs_file_chooser_on_save_playlist), eclair);
      gtk_widget_hide(dialogs_manager->file_chooser_all_button);
      gtk_widget_hide(dialogs_manager->file_chooser_none_button);
   }
   else
      return;

   gtk_window_present(GTK_WINDOW(dialogs_manager->file_chooser_dialog));
}

//Add the files to the playlist
static gboolean _eclair_dialogs_file_chooser_on_add_files(GtkWidget *widget, gpointer data)
{
   Eclair *eclair = (Eclair *)data;
   GtkWidget *file_chooser;
   GSList *filenames, *l;

   if (!eclair || !(file_chooser = eclair->dialogs_manager.file_chooser_widget))
      return FALSE;

   if (!(filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser))))
      return FALSE;


   for (l = filenames; l; l = l->next)
      eclair_playlist_add_uri(&eclair->playlist, (char *)l->data, 0);
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
   Eclair *eclair = (Eclair *)data;
   GtkWidget *file_chooser;
   gchar *filename;
   char *ext, *new_filename;

   if (!eclair || !(file_chooser = eclair->dialogs_manager.file_chooser_widget))
      return FALSE;

   if (!(filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser))) || strlen(filename) <= 0)
      return FALSE;

   if ((ext = eclair_utils_file_get_extension(filename)) && strcmp(ext, "m3u") == 0)
      eclair_playlist_save(&eclair->playlist, filename);
   else
   {
      new_filename = (char *)malloc(strlen(filename) + 5);
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
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return FALSE;

   gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(eclair->dialogs_manager.file_chooser_widget));
   gtk_widget_hide(eclair->dialogs_manager.file_chooser_dialog);
   return TRUE;
}

//------------------------------
// Menu functions
//------------------------------

static void _eclair_dialogs_menu_on_add_files(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_dialogs_add_files_file_chooser(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_load_list(gpointer data, guint action, GtkWidget  *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_dialogs_load_playlist_file_chooser(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_save_list(gpointer data, guint action, GtkWidget  *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_dialogs_save_playlist_file_chooser(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_select_all(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_playlist_container_select_all(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_select_none(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_playlist_container_select_none(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_invert_selection(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_playlist_container_invert_selection(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_remove_selected(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_playlist_remove_selected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_remove_unselected(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_playlist_remove_unselected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_remove_all(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;

   if (!eclair)
      return;

   eclair_playlist_empty(&eclair->playlist);
}
