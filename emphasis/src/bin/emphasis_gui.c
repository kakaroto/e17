#include "emphasis.h"
#include "emphasis_gui.h"


/**
 * @brief Build all widgets for Emphasis GUI
 * @param gui A gui to initialize
 */
void
emphasis_init_gui(Emphasis_Gui *gui)
{
  gui->player = malloc(sizeof(Emphasis_Player_Gui));
  if(!gui->player)
    {
      fprintf(stderr, "Memory full\n");
      exit(1);
    }
  /* TODO : check player */
  /* TODO ; check config, state, etc */
  
  emphasis_init_player(gui->player);
  emphasis_init_menu(gui);

  if(!etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(gui->player->small.media)))
    {
      etk_widget_hide(gui->player->media.window);
    }

  gui->cover_queue = NULL;
  gui->config_gui  = NULL;
}


/* TODO : documentation */
void
emphasis_clear(Emphasis_Gui *gui)
{
  Emphasis_Player_Gui *player;
  
  player = gui->player;

  etk_tree_clear(ETK_TREE(player->media.artist));
  etk_tree_clear(ETK_TREE(player->media.album));
  etk_tree_clear(ETK_TREE(player->media.track));
  etk_tree_clear(ETK_TREE(player->media.pls));
}

/* TODO : documentation */
void
emphasis_init_menu(Emphasis_Gui *gui)
{
  Emphasis_Player_Gui *player;
	
  /* playlist menu setup*/
  gui->menu = etk_menu_new();
  player = gui->player;
	
  emphasis_menu_append(gui->menu,
                       "clear",
                       ETK_STOCK_EDIT_CLEAR        , cb_playlist_clear , NULL,
                       "shuffle",
                       ETK_STOCK_APPLICATIONS_GAMES, cb_playlist_shuffle, NULL,
                       "delete",
                       ETK_STOCK_EDIT_DELETE       , cb_playlist_delete, player,
                       "update",
                       ETK_STOCK_VIEW_REFRESH      , cb_database_update, player,
                       "config",
                       ETK_STOCK_PREFERENCES_SYSTEM, cb_config_show    , gui,
                       NULL);
	
  /* Do we need all this connect ? */
	etk_signal_connect("mouse-down", ETK_OBJECT(player->full.window), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
  etk_signal_connect("mouse-down", ETK_OBJECT(player->small.window), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
	etk_signal_connect("mouse-down", ETK_OBJECT(player->media.window),
                     ETK_CALLBACK(cb_pls_contextual_menu), gui);
	                   
	etk_signal_connect("mouse-down", ETK_OBJECT(player->media.pls), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
	etk_signal_connect("mouse-down", ETK_OBJECT(player->media.artist), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
	etk_signal_connect("mouse-down", ETK_OBJECT(player->media.album), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);
	etk_signal_connect("mouse-down", ETK_OBJECT(player->media.track), 
	                   ETK_CALLBACK(cb_pls_contextual_menu), gui);

}

/**
 * @brief Make a menu with small stock image and sets a callback on "activated" on each elements
 * @param menu The Etk_Menu to setup
 * @param ... An (char*)menu_item name, an (Etk_Stock_Id)image id, a Etk_Callback function and 
 * it data ... terminated by NULL
 */
void
emphasis_menu_append(Etk_Widget *menu, ...)
{
  Etk_Widget *menu_item, *item_image = NULL;
  char *item_name;
  Etk_Stock_Id item_image_id;
  void *callback, *data;
  va_list arglist;

  va_start(arglist, menu);

  while ((item_name = va_arg(arglist, char *)) != NULL)
    {
      menu_item = etk_menu_item_image_new_with_label(item_name);
      item_image_id = va_arg(arglist, Etk_Stock_Id);
      if (item_image_id)
        {
          item_image =
            etk_image_new_from_stock(item_image_id, ETK_STOCK_SMALL);
          etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item),
                                  ETK_IMAGE(item_image));
        }
      callback = va_arg(arglist, void *);
      data = va_arg(arglist, void *);
      if (callback)
        {
          etk_signal_connect("activated", ETK_OBJECT(menu_item),
                             ETK_CALLBACK(callback), data);
        }

      etk_menu_shell_append(ETK_MENU_SHELL(menu), ETK_MENU_ITEM(menu_item));
    }
  va_end(arglist);
}

/**
 * @brief Replace a null string by "Unkown"
 * @param table A table of char** terminated by NULL
 */
void
emphasis_unknow_if_null(char **table[])
{
  int i = 0;

  while (table[i])
    {
      if (!*table[i])
        {
          *table[i] = strdup("Unknown");
        }
      i++;
    }
}

void
emphasis_cover_clear_queue(Cover_Info *q)
{
#undef EMP_CLEAR_QUEUE
#define EMP_CLEAR_QUEUE(data) if(data) { free(data), data=NULL; }

  if(q)
    {
      EMP_CLEAR_QUEUE(q->artist);
      EMP_CLEAR_QUEUE(q->album);
      free(q);
    }
}

/* TODO : documenation */
void
emphasis_cover_change(Emphasis_Gui *gui, char *artist, char *album)
{
  Cover_Info *ci;
  Cover_Info *old;

  ci = malloc(sizeof(Cover_Info));
  if(ci == NULL) { return; }

  ci->artist = strdupnull(artist);
  ci->album  = strdupnull(album);

  old = gui->cover_queue;
  gui->cover_queue = ci;

  emphasis_cover_clear_queue(old);
}

