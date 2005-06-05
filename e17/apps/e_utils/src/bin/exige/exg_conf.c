#include "exg_conf.h"

int exg_conf_init(Exige *exg)
{
    ecore_config_init("exige");
    
    exg_conf_default_set();
    ecore_config_save();

    return 1;
}


int exg_conf_default_set()
{
    ecore_config_theme_default("/settings/theme", "default");
}


char *exg_conf_theme_get()
{
    char *theme = ecore_config_theme_get("/settings/theme");
    return theme;
}


void elicit_config_theme_set(char *theme)
{
    ecore_config_theme_set("/settings/theme", theme);
}
