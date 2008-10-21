#ifndef _EV_CONF_H
#define _EV_CONF_H

int edje_viewer_config_init(void);
int edje_viewer_config_shutdown(void);

int edje_viewer_config_save(Gui *gui);
int edje_viewer_config_load(Gui *gui);
void edje_viewer_config_defaults(Gui *gui);

void edje_viewer_config_recent_set(Gui *gui, const char *file);

#endif
