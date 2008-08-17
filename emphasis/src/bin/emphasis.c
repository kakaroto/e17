#include "emphasis.h"

int
main(int argc, char **argv)
{
  Emphasis_Gui *gui;
 
  /* void */ enhance_init();
  if(!etk_init(argc, argv))
    {
      fprintf(stderr, "Unable to init etk\n");
      return 1;
    }
  if (ecore_config_init("emphasis"))
    {
      fprintf(stderr, "Unable to init ecore_config\n");
      return 1;
    }

  gui = malloc(sizeof(Emphasis_Gui));
  if (!gui)
    {
      fprintf(stderr, "Memory full\n");
      exit(1);
    }
  
  emphasis_init_gui(gui);
  emphasis_pref_init(gui);
  emphasis_init_connect(gui);
  emphasis_cover_init(gui);
  emphasis_try_connect(gui);

  etk_main();

  enhance_free(gui->player->en);
  enhance_shutdown();
  etk_shutdown();
  evas_shutdown();
  ecore_shutdown();

  return 0;
}


void
emphasis_pref_init(void *data)
{
  Emphasis_Gui        *gui;
  Emphasis_Player_Gui *player;
  Emphasis_Config     *config;

  gui = data;
  player = gui->player;
  config = config_load();

  /* Set the last size of the window */
  etk_window_resize(ETK_WINDOW(player->full.window),
                    config->geometry.w,
                    config->geometry.h);
  /* Set the last size of the media window */
  etk_window_resize(ETK_WINDOW(player->media.window),
                    config->geometry.media_w,
                    config->geometry.media_h);
  /* Set the last width of the playlist's columns */
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 0),
                         config->colwidth.title);
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 1),
                         config->colwidth.time);
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 2),
                         config->colwidth.artist);
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 3),
                         config->colwidth.album);

  /* Set the medialib paned size */
  etk_paned_position_set(ETK_PANED(player->media.paned), config->media_paned);

  /* Set the media visibility */
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(player->small.media),
                               config->pls_show);
  if(config->pls_show==ETK_FALSE)
    {
      etk_widget_hide(player->media.window);
    }

  /* Set the cover aspect */
  etk_image_keep_aspect_set(ETK_IMAGE(player->small.cover),config->keep_aspect);
  etk_image_keep_aspect_set(ETK_IMAGE(player->full.cover) ,config->keep_aspect);
  
  emphasis_player_force_mode_set(player, config->mode);
  emphasis_player_info_set(player, NULL, "Not connected to MPD");

  config_free(config);
}

void
emphasis_init_connect(Emphasis_Gui *gui)
{
  gui->timer = ecore_timer_add(0.2, emphasis_try_connect, gui);
}

int
emphasis_try_connect(void *data)
{
  Emphasis_Gui *gui;
  Emphasis_Config *config;
  Ecore_Timer *timer;

  gui = data;
  config = config_load();

  timer = mpc_init(config->hostname, config->port, config->password, data);
  if(mpc_get_crossfade()==0) { mpc_set_crossfade(config->crossfade); }
  config_free(config);

  if (!timer)
    {
      mpc_check_error(gui->player);
      emphasis_clear(gui);
      return 1;
    }
  else
    {
      ecore_timer_del(gui->timer);
      gui->timer = timer;

      emphasis_tree_mlib_init(gui->player, EMPHASIS_ARTIST);
      etk_tree_row_select(etk_tree_first_row_get
                          (ETK_TREE(gui->player->media.artist)));

      return 0;
    }
}
