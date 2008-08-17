#ifndef ELICIT_CONF_H
#define ELICIT_CONF_H

#include "Elicit.h"

enum
{
  ELICIT_CONF_TAG_THEME,
  ELICIT_CONF_TAG_NUM
};

int elicit_config_init(Elicit *el);
void elicit_config_load();
void elicit_config_save();
void elicit_config_shutdown();

char *elicit_config_theme_get();
void elicit_config_theme_set(char *name);

void elicit_config_color_get(int *r, int *g, int *b);
void elicit_config_color_set(int r, int g, int b);

void elicit_config_zoom_set(double zoom);
double elicit_config_zoom_get();

void elicit_config_zoom_max_set(double zoom_max);
double elicit_config_zoom_max_get();

void elicit_config_colorclass_get(int *r, int *g, int *b);
void elicit_config_colorclass_set(int r, int g, int b);

char *elicit_config_editor_get(void);
void elicit_config_editor_set(char *ed);

int elicit_config_grid_visible_get(void);
void elicit_config_grid_visible_set(int visible);

#endif
