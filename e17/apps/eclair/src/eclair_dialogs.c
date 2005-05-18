#include "eclair_dialogs.h"
#include <gtk/gtk.h>
#include "eclair.h"
#include "eclair_playlist.h"
#include "eclair_playlist_container.h"

static void _eclair_dialogs_on_open_files(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_on_select_all(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_on_select_none(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_on_invert_selection(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_on_remove_selected(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_on_remove_unselected(gpointer data, guint action, GtkWidget  *widget);
static void _eclair_dialogs_on_remove_all(gpointer data, guint action, GtkWidget  *widget);

static GtkItemFactoryEntry menu_items[] = {
   { "/tear",                       NULL,       NULL,                                  0, "<Tearoff>" },
   { "/Open",                       NULL,       NULL,                                  0, "<Branch>" },
   { "/Open/tear",                  NULL,       NULL,                                  0, "<Tearoff>" },
   { "/Open/Open Files...",         NULL,       _eclair_dialogs_on_open_files,         1, "<Item>" },
   { "/Playlist",                   NULL,       NULL,                                  0, "<Branch>" },
   { "/Playlist/tear",              NULL,       NULL,                                  0, "<Tearoff>" },
   { "/Playlist/Select All",        NULL,       _eclair_dialogs_on_select_all,         1, "<Item>" },
   { "/Playlist/Select None",       NULL,       _eclair_dialogs_on_select_none,        1, "<Item>" },
   { "/Playlist/Invert Selection",  NULL,       _eclair_dialogs_on_invert_selection,   1, "<Item>" },
   { "/Playlist/separator1",        NULL,       NULL,                                  0, "<Separator>" },
   { "/Playlist/Remove Selected",   NULL,       _eclair_dialogs_on_remove_selected,    1, "<Item>" },
   { "/Playlist/Remove Unselected", NULL,       _eclair_dialogs_on_remove_unselected,  1, "<Item>" },
   { "/Playlist/Remove All",        NULL,       _eclair_dialogs_on_remove_all,         1, "<Item>" }
};

static gint num_menu_items = sizeof(menu_items) / sizeof(menu_items[0]);


static void *_eclair_dialogs_thread(void *param);
static gint _eclair_dialogs_update(gpointer data);
static void _eclair_dialogs_file_chooser(Eclair_Dialogs_Manager *dialogs_manager);

//Initialize dialogs manager
void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair)
{
   if (!dialogs_manager || !eclair)
      return;

   dialogs_manager->eclair = eclair;
   dialogs_manager->file_chooser_dialog = NULL;
   dialogs_manager->menu_widget = NULL;
   dialogs_manager->should_popup_menu = FALSE;
   dialogs_manager->should_open_file_chooser = FALSE;

   pthread_create(&dialogs_manager->dialogs_thread, NULL, _eclair_dialogs_thread, eclair);
}

//Shutdown dialogs manager
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;
   
   fprintf(stderr, "Dialogs: Debug: Destroying dialogs thread\n");
   pthread_join(dialogs_manager->dialogs_thread, NULL); 
   fprintf(stderr, "Meta tag: Debug: Meta dialogs destroyed\n");  
}

//Will open the file selection dialog
void eclair_dialogs_open_file_dialog(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;

   dialogs_manager->should_open_file_chooser = TRUE;
}

//Will popup the eclair menu 
void eclair_popup_menu(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;

   dialogs_manager->should_popup_menu = TRUE;
}

//Open the file chooser
static void _eclair_dialogs_file_chooser(Eclair_Dialogs_Manager *dialogs_manager)
{
   Eclair *eclair;
   GSList *filenames, *l;

   if (!dialogs_manager || !(eclair = dialogs_manager->eclair))
      return;

   dialogs_manager->file_chooser_dialog = gtk_file_chooser_dialog_new("Open files...", NULL, GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialogs_manager->file_chooser_dialog), GTK_RESPONSE_ACCEPT);
	gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_dialog), 1);
	gtk_file_chooser_set_local_only(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_dialog), 0);

   gtk_widget_show(dialogs_manager->file_chooser_dialog);

   if (gtk_dialog_run(GTK_DIALOG(dialogs_manager->file_chooser_dialog)) == GTK_RESPONSE_ACCEPT)
   {
      filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialogs_manager->file_chooser_dialog));
      for (l = filenames; l; l = l->next)
         eclair_playlist_add_uri(&eclair->playlist, (char *)l->data, 0);
      eclair_playlist_container_update(eclair->playlist_container);

      g_slist_foreach(filenames, (GFunc)g_free, NULL);
      g_slist_free(filenames);
   }

   gtk_widget_destroy(dialogs_manager->file_chooser_dialog);
   dialogs_manager->file_chooser_dialog = NULL;
   //gtk_window_present(GTK_WINDOW(dialogs_manager->file_chooser_dialog));
}
/*
static void _eclair_dialogs_file_chooser_add(GtkFileChooser *file_chooser)
{
   GSList *filenames, *l;

   if (!(filenames = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser))))
      return;

   for (l = filenames; l; l = l->next)
         eclair_playlist_add_uri(&eclair->playlist, (char *)l->data);
   g_slist_foreach(filenames, (GFunc)g_free, NULL);
   g_slist_free(filenames);
}*/

//Called each 50ms and check if we must do something (popup menu, open file selection dialog...)
static gint _eclair_dialogs_update(gpointer data)
{
   Eclair_Dialogs_Manager *dialogs_manager = (Eclair_Dialogs_Manager *)data;

   if (!dialogs_manager)
      return 1;

   if (dialogs_manager->should_popup_menu)
   {
      gtk_menu_popup(GTK_MENU(dialogs_manager->menu_widget), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
      dialogs_manager->should_popup_menu = FALSE;
   }
   if (dialogs_manager->should_open_file_chooser)
   {
      _eclair_dialogs_file_chooser(dialogs_manager);
      dialogs_manager->should_open_file_chooser = FALSE;
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

   item_factory = gtk_item_factory_new(GTK_TYPE_MENU, "<main>", NULL);
   gtk_item_factory_create_items(item_factory, num_menu_items, menu_items, eclair);
   dialogs_manager->menu_widget = gtk_item_factory_get_widget(item_factory, "<main>");

   g_timeout_add(50, G_CALLBACK(_eclair_dialogs_update), dialogs_manager);

   gtk_main();

   return NULL;
}

//------------------------------
// Menu functions
//------------------------------

static void _eclair_dialogs_on_open_files(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_dialogs_open_file_dialog(&eclair->dialogs_manager);
}

static void _eclair_dialogs_on_select_all(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_playlist_container_select_all(eclair->playlist_container);
}

static void _eclair_dialogs_on_select_none(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_playlist_container_select_none(eclair->playlist_container);
}

static void _eclair_dialogs_on_invert_selection(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_playlist_container_invert_selection(eclair->playlist_container);
}

static void _eclair_dialogs_on_remove_selected(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_playlist_remove_selected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_on_remove_unselected(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_playlist_remove_unselected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_on_remove_all(gpointer data, guint action, GtkWidget *widget)
{
   Eclair *eclair = (Eclair *)data;
   if (!eclair)
      return;
   eclair_playlist_empty(&eclair->playlist);
}
