#ifndef __EWL_CONFIG_H__
#define __EWL_CONFIG_H__

/**
 * @defgroup Ewl_Config Config: Functions for Manipulating Configuration Data
 *
 * @{
 */

typedef struct _ewl_config Ewl_Config;

struct _ewl_config
{
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
		char           *name;
		int             cache;
		int             cclass_override;
		Ewd_List       *cclasses;
	} theme;
};

extern Ewl_Config      ewl_config;

typedef struct _ewl_color_class Ewl_Color_Class;
struct _ewl_color_class
{
	char *name; /**< The name of the class, for matching to theme */
	int r; /**< Red color value */
	int g; /**< Green color value */
	int b; /**< Blue color value */
	int a; /**< Alpha value */
};

int             ewl_config_init(void);
int             ewl_config_set_str(char *config, char *k, char *v);
int             ewl_config_set_int(char *config, char *k, int v);
int             ewl_config_set_float(char *config, char *k, float v);
char           *ewl_config_get_str(char *config, char *k);
int             ewl_config_get_int(char *config, char *k);
float           ewl_config_get_float(char *config, char *k);
char *          ewl_config_get_render_method();
void            ewl_config_reread_and_apply(void);

/**
 * @}
 */

#endif				/* __EWL_CONFIG_H__ */
