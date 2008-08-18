#ifndef _ECLAIR_DIALOGS_H_
#define _ECLAIR_DIALOGS_H_

#include "eclair_types.h"

struct _Eclair_Dialogs_Manager
{
   //Menu widget vars
   Eclair_Menu *menu;

   Eclair_Menu_Item *menu_open_item;
   Eclair_Menu *menu_open_menu;
   Eclair_Menu_Item *menu_open_add_files_item;

   Eclair_Menu_Item *menu_playlist_item;
   Eclair_Menu *menu_playlist_menu;
   Eclair_Menu_Item *menu_playlist_load_item;
   Eclair_Menu_Item *menu_playlist_save_item;
   Eclair_Menu_Item *menu_playlist_select_all_item;
   Eclair_Menu_Item *menu_playlist_select_none_item;
   Eclair_Menu_Item *menu_playlist_select_invert_item;
   Eclair_Menu_Item *menu_playlist_remove_sel_item;
   Eclair_Menu_Item *menu_playlist_remove_unsel_item;
   Eclair_Menu_Item *menu_playlist_remove_all_item;
   Eclair_Menu_Item *menu_playlist_shuffle_mode_item;
   Eclair_Menu_Item *menu_playlist_repeat_mode_item;

   Eclair_Menu_Item *menu_windows_item;
   Eclair_Menu *menu_windows_menu;
   Eclair_Menu_Item *menu_windows_search_item;

   Eclair *eclair;
};

void eclair_dialogs_init(Eclair_Dialogs_Manager *dialogs_manager, Eclair *eclair);
void eclair_dialogs_shutdown(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_open_fc_add_files(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_open_fc_load_playlist(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_open_fc_save_playlist(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_popup_menu(Eclair_Dialogs_Manager *dialogs_manager);
void eclair_dialogs_search_window(Eclair_Dialogs_Manager *dialogs_manager);

#endif
