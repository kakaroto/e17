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
      exit(1);
    }
  configgui->en = en;

  _emphasis_enhance_callbacks(configgui);
  enhance_file_load(en, "Config", PACKAGE_DATA_DIR EMPHASIS_CONFIG_GLADE);

  configgui->window   = enhance_var_get(en, "window");
  configgui->hostname = enhance_var_get(en, "hostname");
  //configgui->port     = enhance_var_get(en, "port");
  configgui->password = enhance_var_get(en, "password");

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

  etk_entry_text_set(ETK_ENTRY(configgui->hostname),
                     config->hostname);
  if(config->password)
    {
      etk_entry_text_set(ETK_ENTRY(configgui->password),
                         config->password);
    }

  config_free(config);
}

void
emphasis_configgui_save(Emphasis_Config_Gui *configgui)
{
  Emphasis_Config *config;

  config = config_load();

  config->hostname =
   (char*)etk_entry_text_get(ETK_ENTRY(configgui->hostname));
  config->password =
   (char*)etk_entry_text_get(ETK_ENTRY(configgui->password));

  config_save(config);
  config_free(config);
}
