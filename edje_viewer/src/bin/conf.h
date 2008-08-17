void edje_viewer_config_init(void);
void edje_viewer_config_load(void);
void edje_viewer_config_shutdown(void);

int edje_viewer_config_count_get(void);
void edje_viewer_config_count_set(int count);

char *edje_viewer_config_recent_get(int number);
void edje_viewer_config_recent_set(const char *path);

char *edje_viewer_config_last_get(void);
void edje_viewer_config_last_set(const char *path);

unsigned char edje_viewer_config_open_last_get(void);
void edje_viewer_config_open_last_set(unsigned char check);
unsigned char edje_viewer_config_sort_parts_get(void);
void edje_viewer_config_sort_parts_set(unsigned char check);
