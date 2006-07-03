void edje_viewer_config_init(void);
void edje_viewer_config_load(void);
void edje_viewer_config_shutdown(void);

int edje_viewer_config_count_get(void);
void edje_viewer_config_count_set(int count);

char *edje_viewer_config_recent_get(int number);
void edje_viewer_config_recent_set(char *path);
