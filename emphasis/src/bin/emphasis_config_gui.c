#include "emphasis.h"
#include "emphasis_config_gui.h"

/* TODO : all docs */
static void
_emphasis_enhance_callbacks(Emphasis_Config_Gui *configgui);

void
emphasis_init_configgui(Emphasis_Config_Gui *configgui)
{
  Enhance *en;
  en = enhance_new();

  if(en==NULL)
    {
      fprintf(stderr, "enhance new failed\n");
      return;
    }
  configgui->en = en;

  _emphasis_enhance_callbacks(configgui);
  enhance_file_load(en, "Config", PACKAGE_DATA_DIR EMPHASIS_CONFIG_GLADE);

  configgui->window   = enhance_var_get(en, "window");
  configgui->hostname = enhance_var_get(en, "hostname");
  configgui->port     = enhance_var_get(en, "port");
  configgui->password = enhance_var_get(en, "password");

  configgui->xfade    = enhance_var_get(en, "crossfade");
  configgui->stop     = enhance_var_get(en, "stop");
  configgui->aspect   = enhance_var_get(en, "aspect");

  etk_window_wmclass_set(ETK_WINDOW(configgui->window), "emphasis", "Emphasis");
}

static void
_emphasis_enhance_callbacks(Emphasis_Config_Gui *configgui)
{
  Enhance *en;

  en = configgui->en;

  enhance_callback_data_set(en, "cb_config_hide", configgui);
  enhance_callback_data_set(en, "cb_config_ok"  , configgui);
  enhance_callback_data_set(en, "cb_config_hide", configgui);
}

void
emphasis_configgui_autoset(Emphasis_Config_Gui *configgui)
{
  Emphasis_Config *config;

  config = config_load();

  if(config->port)
    {
      etk_range_value_set(ETK_RANGE(configgui->port), config->port);
    }
  if(config->hostname)
    {
      etk_entry_text_set(ETK_ENTRY(configgui->hostname),
                         config->hostname);
    }
  if(config->password)
    {
      etk_entry_text_set(ETK_ENTRY(configgui->password),
                         config->password);
    }

  if(!mpc_assert_status(MPD_PLAYER_STOP) || mpc_get_crossfade())
    {
      etk_range_value_set(ETK_RANGE(configgui->xfade),
                          mpc_get_crossfade());
    }
  else
    {
      etk_range_value_set(ETK_RANGE(configgui->xfade),
                          config->crossfade);
    }
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(configgui->stop),
                               config->stop_on_exit);
  etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(configgui->aspect),
                               config->keep_aspect);

  config_free(config);
}

void
emphasis_configgui_save(Emphasis_Config_Gui *configgui)
{
  Emphasis_Config *config;
  char *key = NULL, *value = NULL;
  int reconnect = 0;

  config = config_load();

  if((key = config->hostname))
    {
      value = strdup(etk_entry_text_get(ETK_ENTRY(configgui->hostname)));
      if(strcmp(key,value)) { reconnect = 1; }
      free(config->hostname);
    }
  config->hostname = value;

  if((key = config->password))
    {
      value = strdup(etk_entry_text_get(ETK_ENTRY(configgui->password)));
      if(!reconnect && strcmp(key,value)) { reconnect = 1; }
      free(config->password);
    }
  config->password = value;
  
  if(!reconnect && config->port != etk_range_value_get(ETK_RANGE(configgui->port)))
    {
      reconnect = 1;
    }
  config->port = etk_range_value_get(ETK_RANGE(configgui->port));


  config->crossfade    =
   etk_range_value_get(ETK_RANGE(configgui->xfade));
  config->stop_on_exit =
   etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(configgui->stop));
  config->keep_aspect  =
   etk_toggle_button_active_get(ETK_TOGGLE_BUTTON(configgui->aspect));

  if(configgui->data)
    {
      etk_image_keep_aspect_set
        (ETK_IMAGE(((Emphasis_Gui*)configgui->data)->player->small.cover),
         config->keep_aspect);
      etk_image_keep_aspect_set
        (ETK_IMAGE(((Emphasis_Gui*)configgui->data)->player->full.cover),
         config->keep_aspect);
    }
  mpc_set_crossfade(config->crossfade);

  config_save(config);
  config_free(config);

  if(reconnect)
    {
      Emphasis_Gui *gui = configgui->data;
      emphasis_player_cover_set(gui->player, NULL);
      emphasis_player_progress_set(gui->player, 0.0, 0.001);
      mpc_disconnect();
    }
}

