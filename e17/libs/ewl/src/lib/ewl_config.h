#ifndef EWL_CONFIG_H
#define EWL_CONFIG_H

/**
 * @addtogroup Ewl_Config Ewl_Config: Functions for Manipulating Configuration Data
 *
 * @{
 */

typedef struct Ewl_Config Ewl_Config;

/**
 * The Ewl_Config stuct holds configuration data for the EWL library 
 */
struct Ewl_Config
{
	struct {
		int enable;		/**< Is debugging enabled */
		int level;		/**< Current debug level */
		int indent_lvl;		/**< How far to indent */

		int segv;		/**< Segv on warning */
		int backtrace;		/**< Backtrace on warning */
		int gc_reap;		/**< Log on garbage collection */
	} debug;			/**< Debuggin configuration */

	struct {
		int font_cache;		/**< Font cache size */
		int image_cache;	/**< Image cache size */
		int engine;		/**< Engines available */
		char *render_method;	/**< Current render method */
		int render_debug;	/**< Enable debugging mode in Evas */
	} evas;				/**< Evas configuration */

	struct {
		char *name;		/**< Theme name */
		int cache;		/**< Cache size */
		int cclass_override;	/**< Colour class override */
		int print_keys;		/**< Print theme keys as accessed */
		int print_signals;	/**< Print theme signals as accessed */
	} theme;			/**< Theme configuration */
};

extern Ewl_Config ewl_config;	/**< The global configuration data */

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

#endif
