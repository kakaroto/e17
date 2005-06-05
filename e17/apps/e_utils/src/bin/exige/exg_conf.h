#ifndef EXG_CONF_H
#define EXG_CONF_H

#include "Exige.h"
#include "exg_gui.h"

int exg_conf_init(Exige *exg);

int exg_conf_default_set();

char *exg_conf_theme_get();
void elicit_config_theme_set(char *theme);

#endif
