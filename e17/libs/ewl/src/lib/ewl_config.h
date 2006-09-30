#ifndef EWL_CONFIG_H
#define EWL_CONFIG_H

/**
 * @addtogroup Ewl_Config Ewl_Config: Functions for Manipulating Configuration Data
 *
 * @{
 */

/**
 * The Ewl_Config structure
 */
typedef struct Ewl_Config Ewl_Config;

/**
 * The Ewl_Config stuct holds configuration data
 */
struct Ewl_Config
{
	char *app_name;	/**< The application name*/

	struct 
	{
		Ecore_Hash *user;	/**< User data */
		Ecore_Hash *system;	/**< System data */
		Ecore_Hash *instance;	/**< Data for this instance only */
	} data;
};

/**
 * The Ewl_Config_Cache structure
 */
typedef struct Ewl_Config_Cache Ewl_Config_Cache;

/**
 * The Ewl_Config_Cache struct holds cached configuration data
 */
struct Ewl_Config_Cache
{
	int level;			/**< Debug level */
	unsigned char enable:1;		/**< Enable debugging */
	unsigned char segv:1;		/**< Segv on D* messages */
	unsigned char backtrace:1;	/**< Print backtrace on D* messages */
	unsigned char evas_render:1;	/**< Debug evas render calls */
	unsigned char gc_reap:1;	/**< Debug garbage collection */

	unsigned char print_signals:1;	/**< Print theme signals */
	unsigned char print_keys:1;	/**< Print theem keys */
};

extern Ewl_Config *ewl_config;	/**< The global configuration data */
extern Ewl_Config_Cache ewl_config_cache; /**< global debug data */

int		 ewl_config_init(void);
void		 ewl_config_shutdown(void);
void 		 ewl_config_cache_init(void);

Ewl_Config 	*ewl_config_new(const char *app_name);
void		 ewl_config_destroy(Ewl_Config *cfg);

void		 ewl_config_string_set(Ewl_Config *cfg, const char *k, 
						const char *v, 
						Ewl_State_Type state);
const char	*ewl_config_string_get(Ewl_Config *cfg, const char *k);

void		 ewl_config_int_set(Ewl_Config *cfg, const char *k, int v,
						Ewl_State_Type state);
int		 ewl_config_int_get(Ewl_Config *cfg, const char *k);

void		 ewl_config_float_set(Ewl_Config *cfg, const char *k, 
						float v,
						Ewl_State_Type state);
float		 ewl_config_float_get(Ewl_Config *cfg, const char *k);

/**
 * @}
 */

#endif
