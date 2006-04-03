#ifdef E_MOD_DEVIAN_TYPEDEFS

#else

#ifndef E_MOD_CONTAINER_H_INCLUDED
#define E_MOD_CONTAINER_H_INCLUDED

#define CONTAINER_NO 0
#define CONTAINER_BOX 1
#define CONTAINER_BAR 2


/* Calls specific container */

int DEVIANF(container_change)(DEVIANN *devian, int container);

int DEVIANF(container_edje_part_get)(DEVIANN *devian);
int DEVIANF(container_edje_part_change)(DEVIANN *devian);

int DEVIANF(container_infos_text_change)(DEVIANN *devian, char *text);
void DEVIANF(container_infos_text_scroll)(DEVIANN *devian);
void DEVIANF(container_infos_text_change_set)(DEVIANN *deviant, int action);
int DEVIANF(container_infos_edje_part_is_visible)(DEVIANN *devian);

void DEVIANF(container_loading_state_change)(DEVIANN *devian, int state);

int DEVIANF(container_update_id_devian)(DEVIANN *devian);
int DEVIANF(container_devian_dying)(DEVIANN *devian);

void DEVIANF(container_idle_resize_auto)(DEVIANN *devian);
void DEVIANF(container_idle_update_actions)(DEVIANN *devian);
int DEVIANF(container_idle_is_in_transition)(DEVIANN *devian);
void DEVIANF(container_idle_alpha_set)(void *container, int alpha);

/* Common functions */

const char *DEVIANF(container_edje_load)(Evas_Object *edje_obj, char *part, DEVIANN *devian, int container_type, void *container);
int DEVIANF(container_theme_change)(DEVIANN *devian, int source_type, const char **theme);
void DEVIANF(container_theme_check)(DEVIANN *devian, const char **source_theme);

#endif
#endif
