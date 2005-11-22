#ifndef __EWL_CONFIG_H__
#define __EWL_CONFIG_H__

/**
 * @file ewl_config.h
 * @defgroup Ewl_Config Config: Functions for Manipulating Configuration Data
 *
 * @{
 */

typedef struct _ewl_config Ewl_Config;

struct _ewl_config
{
	time_t                  mtime;
	struct {
		int             enable;
		int             level;
		int 		indent_lvl;
	} debug;
	struct {
		int             font_cache;
		int             image_cache;
		char           *render_method;
	} evas;
	struct {
		char           *name;
		int             cache;
		int             cclass_override;
		int             print_keys;
	} theme;
};

extern Ewl_Config ewl_config;

int             ewl_config_init(void);
void            ewl_config_shutdown(void);
int             ewl_config_str_set(const char *k, char *v);
int             ewl_config_int_set(const char *k, int v);
int             ewl_config_float_set(const char *k, float v);
char           *ewl_config_str_get(const char *k);
int             ewl_config_int_get(const char *k);
float           ewl_config_float_get(const char *k);
char *          ewl_config_render_method_get(void);

/**
 * @}
 */

#endif				/* __EWL_CONFIG_H__ */
