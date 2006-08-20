#include "emphasis.h"

int
main(int argc, char **argv)
{
  Emphasis_Gui *gui;
 
  /* void */ enhance_init();
  if(!etk_init(&argc, &argv))
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
  etk_window_resize(ETK_WINDOW(player->full.window), config->geometry.w,
                    config->geometry.h);
  /* Set the last width of the playlist's columns */
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 0),
                         config->colwidth.title);
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 1),
                         config->colwidth.time);
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 2),
                         config->colwidth.artist);
  etk_tree_col_width_set(ETK_TREE_COL_GET(player->media.pls, 3),
                         config->colwidth.album);

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
  Evas_List *artist_list;
  Ecore_Timer *timer;

  gui = data;
  config = config_load();

  timer = mpc_init(config->hostname, config->port, config->password);
  config_free(config);

  if (!timer)
    {
      emphasis_clear(gui);
      return 1;
    }
  else
    {
      ecore_timer_del(gui->timer);
      gui->timer = timer;

      artist_list = mpc_mlib_artist_get();
      emphasis_tree_mlib_set(ETK_TREE(gui->player->media.artist), artist_list,
                             MPD_DATA_TYPE_TAG, NULL);
      etk_tree_row_select(etk_tree_first_row_get
                          (ETK_TREE(gui->player->media.artist)));

      mpc_signal_connect_status_changed(gui);
      return 0;
    }
}
