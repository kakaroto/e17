
#ifndef __EWL_CONFIG_H__
#define __EWL_CONFIG_H__

typedef struct _ewl_config Ewl_Config;

struct _ewl_config {
	struct {
		int             enable;
		int             level;
	} debug;
	struct {
		int             font_cache;
		int             image_cache;
		char           *render_method;
	} evas;
	struct {
		Ewd_List       *paths;
		int             fps;
	} fx;
	struct {
		char           *name;
		int             cache;
	} theme;
};

extern Ewl_Config      ewl_config;

int             ewl_config_init(void);
int             ewl_config_set_str(char *config, char *k, char *v);
int             ewl_config_set_int(char *config, char *k, int v);
int             ewl_config_set_float(char *config, char *k, float v);
char           *ewl_config_get_str(char *config, char *k);
int             ewl_config_get_int(char *config, char *k);
float           ewl_config_get_float(char *config, char *k);
int             ewl_config_get_render_method();
void            ewl_config_reread_and_apply(void);


#endif				/* __EWL_CONFIG_H__ */
