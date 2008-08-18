#include "eclair_dialogs.h"
#include "../config.h"
#include "eclair.h"
#include "eclair_config.h"
#include "eclair_playlist.h"
#include "eclair_playlist_container.h"
#include "eclair_utils.h"
#include "eclair_database.h"
#include "eclair_menu.h"

static void _eclair_dialogs_menu_on_add_files(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_load_playlist(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_save_playlist(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_select_all(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_select_none(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_invert_selection(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_remove_selected(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_remove_unselected(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_remove_all(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_shuffle_mode(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_repeat_mode(Eclair_Menu_Item *item, void *data);
static void _eclair_dialogs_menu_on_search_window(Eclair_Menu_Item *item, void *data);

//Initialize dialogs manager
void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair)
{
   if (!dialogs_manager || !eclair)
      return;

   dialogs_manager->eclair = eclair;

   //Create the menu
   dialogs_manager->menu = eclair_menu_new(eclair->gui_engine);
   dialogs_manager->menu_open_item = eclair_menu_add_item_with_icon(dialogs_manager->menu, "Open", PACKAGE_DATA_DIR "/icons/open.png");
   dialogs_manager->menu_playlist_item = eclair_menu_add_item_with_icon(dialogs_manager->menu, "Playlist", PACKAGE_DATA_DIR "/icons/playlist.png");
   dialogs_manager->menu_windows_item = eclair_menu_add_item_with_icon(dialogs_manager->menu, "Windows", PACKAGE_DATA_DIR "/icons/windows.png");

   dialogs_manager->menu_open_menu = eclair_menu_new(eclair->gui_engine);
   dialogs_manager->menu_open_add_files_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_open_menu, "Add Files", PACKAGE_DATA_DIR "/icons/add_files.png");
   eclair_menu_attach_to_item(dialogs_manager->menu_open_menu, dialogs_manager->menu_open_item);

   dialogs_manager->menu_playlist_menu = eclair_menu_new(eclair->gui_engine);
   dialogs_manager->menu_playlist_load_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Load Playlist", PACKAGE_DATA_DIR "/icons/load_playlist.png");
   dialogs_manager->menu_playlist_save_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Save Playlist", PACKAGE_DATA_DIR "/icons/save_playlist.png");
   eclair_menu_add_seperator(dialogs_manager->menu_playlist_menu);
   dialogs_manager->menu_playlist_select_all_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Select All", PACKAGE_DATA_DIR "/icons/select_all.png");
   dialogs_manager->menu_playlist_select_none_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Select None", PACKAGE_DATA_DIR "/icons/select_none.png");
   dialogs_manager->menu_playlist_select_invert_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Invert Selection", PACKAGE_DATA_DIR "/icons/invert_selection.png");
   eclair_menu_add_seperator(dialogs_manager->menu_playlist_menu);
   dialogs_manager->menu_playlist_remove_sel_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Remove Selected", PACKAGE_DATA_DIR "/icons/remove_selected.png");
   dialogs_manager->menu_playlist_remove_unsel_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Remove Unselected", PACKAGE_DATA_DIR "/icons/remove_unselected.png");
   dialogs_manager->menu_playlist_remove_all_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_playlist_menu, "Remove All", PACKAGE_DATA_DIR "/icons/remove_all.png");
   eclair_menu_add_seperator(dialogs_manager->menu_playlist_menu);
   dialogs_manager->menu_playlist_shuffle_mode_item = eclair_menu_add_item(dialogs_manager->menu_playlist_menu, "Shuffle Mode");
   dialogs_manager->menu_playlist_repeat_mode_item = eclair_menu_add_item(dialogs_manager->menu_playlist_menu, "Repeat Mode");
   eclair_menu_attach_to_item(dialogs_manager->menu_playlist_menu, dialogs_manager->menu_playlist_item);

   dialogs_manager->menu_windows_menu = eclair_menu_new(eclair->gui_engine);
   dialogs_manager->menu_windows_search_item = eclair_menu_add_item_with_icon(dialogs_manager->menu_windows_menu, "Search Window", PACKAGE_DATA_DIR "/icons/search_window.png");
   eclair_menu_attach_to_item(dialogs_manager->menu_windows_menu, dialogs_manager->menu_windows_item);

   eclair_menu_item_callback_set(dialogs_manager->menu_open_add_files_item, _eclair_dialogs_menu_on_add_files, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_load_item, _eclair_dialogs_menu_on_load_playlist, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_save_item, _eclair_dialogs_menu_on_save_playlist, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_select_all_item, _eclair_dialogs_menu_on_select_all, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_select_none_item, _eclair_dialogs_menu_on_select_none, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_select_invert_item, _eclair_dialogs_menu_on_invert_selection, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_remove_sel_item, _eclair_dialogs_menu_on_remove_selected, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_remove_unsel_item, _eclair_dialogs_menu_on_remove_unselected, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_remove_all_item, _eclair_dialogs_menu_on_remove_all, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_shuffle_mode_item, _eclair_dialogs_menu_on_shuffle_mode, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_playlist_repeat_mode_item, _eclair_dialogs_menu_on_repeat_mode, eclair);
   eclair_menu_item_callback_set(dialogs_manager->menu_windows_search_item, _eclair_dialogs_menu_on_search_window, eclair);
}

//Shutdown dialogs manager
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (!dialogs_manager)
      return;

   eclair_menu_free(dialogs_manager->menu);
}

//Open the file chooser dialog to add files
void eclair_dialogs_open_fc_add_files(Eclair_Dialogs_Manager *dialogs_manager)
{
}

//Open the file chooser dialog to load playlist
void eclair_dialogs_open_fc_load_playlist(Eclair_Dialogs_Manager *dialogs_manager)
{
}

//Open the file chooser dialog to save the playlist
void eclair_dialogs_open_fc_save_playlist(Eclair_Dialogs_Manager *dialogs_manager)
{
}

//Popup the eclair menu 
void eclair_dialogs_popup_menu(Eclair_Dialogs_Manager *dialogs_manager)
{
   if (dialogs_manager)
      eclair_menu_popup(dialogs_manager->menu);
}

//Open the playlist search window
void eclair_dialogs_search_window(Eclair_Dialogs_Manager *dialogs_manager)
{
}


//------------------------------
// Menu callbacks
//------------------------------

static void _eclair_dialogs_menu_on_add_files(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_open_fc_add_files(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_load_playlist(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_open_fc_load_playlist(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_save_playlist(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_open_fc_save_playlist(&eclair->dialogs_manager);
}

static void _eclair_dialogs_menu_on_select_all(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_select_all(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_select_none(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_select_none(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_invert_selection(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_container_invert_selection(eclair->playlist_container);
}

static void _eclair_dialogs_menu_on_remove_selected(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_remove_selected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_remove_unselected(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_remove_unselected_media_files(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_remove_all(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_empty(&eclair->playlist);
}

static void _eclair_dialogs_menu_on_shuffle_mode(Eclair_Menu_Item *item, void *data)
{
   /*Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_set_shuffle(&eclair->playlist, gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)));*/
}

static void _eclair_dialogs_menu_on_repeat_mode(Eclair_Menu_Item *item, void *data)
{
   /*Eclair *eclair;

   if ((eclair = data))
      eclair_playlist_set_repeat(&eclair->playlist, gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)));*/
}

static void _eclair_dialogs_menu_on_search_window(Eclair_Menu_Item *item, void *data)
{
   Eclair *eclair;

   if ((eclair = data))
      eclair_dialogs_search_window(&eclair->dialogs_manager);
}
