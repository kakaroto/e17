#ifndef ELICIT_CONFIG_H
#define ELICIT_CONFIG_H

int elicit_config_load(Elicit *el);
int elicit_config_save(Elicit *el);

void elicit_config_zoom_level_set(Elicit *el, int zoom_level);
int elicit_config_zoom_level_get(Elicit *el);
void elicit_config_grid_visible_set(Elicit *el, int grid_visible);
int elicit_config_grid_visible_get(Elicit *el);
void elicit_config_show_band_set(Elicit *el, int show_band);
int elicit_config_show_band_get(Elicit *el);

#endif
