#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

enum Ewl_Config_Types 
{
	EWL_CONFIG_DEBUG_ENABLE,
	EWL_CONFIG_DEBUG_LEVEL,
	EWL_CONFIG_ENGINE_NAME,
	EWL_CONFIG_EVAS_FONT_CACHE,
	EWL_CONFIG_EVAS_IMAGE_CACHE,
	EWL_CONFIG_THEME_ICON_THEME_NAME,
	EWL_CONFIG_THEME_NAME,
	EWL_CONFIG_THEME_CACHE,
	EWL_CONFIG_THEME_COLOR_CLASSES_OVERRIDE,
	EWL_CONFIG_THEME_PRINT_KEYS,
	EWL_CONFIG_THEME_PRINT_SIGNALS,
};

extern Ecore_List *ewl_embed_list;

static void ewl_config_defaults_set(void);
static void ewl_config_config_read(void);

static int ewl_config_listener(const char *key, const Ecore_Config_Type type, 
						    const int tag, void *data);

Ewl_Config ewl_config;

/**
 * @internal
 * @return Returns true on success, false on failure.
 * @brief Initialize the configuration system
 *
 * This sets up the necessary configuration variables.
 */
int
ewl_config_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_config_system_init();
	ecore_config_load();
	memset(&ewl_config, 0, sizeof(Ewl_Config));
	ewl_config_config_read();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value
 * @brief Shutdown the configuration system.
 */
void
ewl_config_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	IF_FREE(ewl_config.theme.name);
	ecore_config_system_shutdown();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param k: the key to set in the configuration database
 * @param v: the string value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief set the value of key to the specified string
 *
 * Sets the string value associated with the key @a k to @a v in the
 * configuration database.
 */
int
ewl_config_str_set(const char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	ecore_config_string_set(k, v);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param k: the key to set in the configuration database
 * @param v: the integer value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Set the value of key to the specified integer
 *
 * Sets the integer value associated with the key @a k to @a v in the
 * configuration database.
 */
int ewl_config_int_set(const char *k, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	ecore_config_int_set(k, v);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param k: the key to set in the configuration database
 * @param v: the float value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Set the value of key to the specified float
 *
 * Sets the float value associated with the key @a k to @a v in the
 * configuration database.
 */
int
ewl_config_float_set(const char *k, float v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	ecore_config_float_set(k, v);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param k: the key to search
 * @return Returns the found string value on success, NULL on failure.
 * @brief Retrieve string value associated with a key
 */
char *
ewl_config_str_get(const char *k)
{
	char *ret = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	ret = ecore_config_string_get(k);

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param k: the key to search
 * @return Returns the found integer value on success, 0 on failure.
 * @brief Retrieve integer value associated with a key
 */
int
ewl_config_int_get(const char *k)
{
	int v = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, 0);

	v = ecore_config_int_get(k);

	DRETURN_INT(v, DLEVEL_STABLE);
}

/**
 * @param k: the key to search
 * @return Returns the found float value on success, 0.0 on failure.
 * @brief Retrieve floating point value associated with a key
 */
float
ewl_config_float_get(const char *k)
{
	float v = 0.0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, 0.0);

	v = ecore_config_float_get(k);

	DRETURN_FLOAT(v, DLEVEL_STABLE);
}

static void
ewl_config_config_read(void)
{
	int cc;
	Ewl_Config nc;
	Ecore_Config_Prop *prop;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_config_defaults_set();

	nc.debug.enable = ewl_config_int_get("/ewl/debug/enable");
	nc.debug.level = ewl_config_int_get("/ewl/debug/level");
	nc.evas.font_cache = ewl_config_int_get("/ewl/evas/font_cache");
	nc.evas.image_cache = ewl_config_int_get("/ewl/evas/image_cache");
	nc.engine_name = ewl_config_str_get("/ewl/engine_name");
	nc.theme.name = ewl_config_str_get("/ewl/theme/name");
	nc.theme.icon_theme = ewl_config_str_get("/ewl/theme/icon/name");
	nc.theme.cache = ewl_config_int_get("/ewl/theme/cache");
	nc.theme.print_keys = ewl_config_int_get("/ewl/theme/print_keys");
	nc.theme.print_signals = ewl_config_int_get("/ewl/theme/print_signals");
	nc.theme.cclass_override = 
			ewl_config_int_get("/ewl/theme/color_classes/override");

	if (nc.theme.cclass_override) {
		int i;

		cc = ewl_config_int_get("/ewl/theme/color_classes/count");
		prop = ecore_config_get("/ewl/theme/color_classes/count");
		prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
		prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

		for (i = 0; i < cc; i++) {
			char *name;
			char key[PATH_MAX];

			snprintf(key, PATH_MAX,
					"/ewl/theme/color_classes/%d/name", i);
			name = ewl_config_str_get(key);
			prop = ecore_config_get(key);
			prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
			prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

			if (name) {
				int r, g, b, a;
				int r2, g2, b2, a2;
				int r3, g3, b3, a3;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/r", i);
				r = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/g", i);
				g = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/b", i);
				b = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/a", i);
				a = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/r2", i);
				r2 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/g2", i);
				g2 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/b2", i);
				b2 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/a2", i);
				a2 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/r3", i);
				r3 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/g3", i);
				g3 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/b3", i);
				b3 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				snprintf(key, PATH_MAX,
						"/ewl/theme/color_classes/%d/a3", i);
				a3 = ewl_config_int_get(key);
				prop = ecore_config_get(key);
				prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
				prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;

				edje_color_class_set(name, r, g, b, a,
						r2, g2, b2, a2,
						r3, g3, b3, a3);
				FREE(name);
			}
		}
	}

	if (ewl_embed_list && !ecore_list_is_empty(ewl_embed_list)) {
		Ewl_Embed *e;

		ecore_list_goto_first(ewl_embed_list);

		while ((e = ecore_list_next(ewl_embed_list)) != NULL) {
			if (!e->evas)
				continue;

			if (nc.evas.font_cache) {
				evas_font_cache_flush(e->evas);
				evas_font_cache_set(e->evas, 0);
			}

			if (nc.evas.image_cache) {
				evas_image_cache_flush(e->evas);
				evas_image_cache_set(e->evas,
						     nc.evas.image_cache);
			}
		}
	}

	ewl_config.debug.enable = nc.debug.enable;
	ewl_config.debug.level = nc.debug.level;
	ewl_config.evas.font_cache = nc.evas.font_cache;
	ewl_config.evas.image_cache = nc.evas.image_cache;
	IF_FREE(ewl_config.engine_name);
	ewl_config.engine_name = nc.engine_name;
	IF_FREE(ewl_config.theme.name);
	ewl_config.theme.name = nc.theme.name;
	IF_FREE(ewl_config.theme.icon_theme);
	ewl_config.theme.icon_theme = nc.theme.icon_theme;
	ewl_config.theme.cache = nc.theme.cache;
	ewl_config.theme.print_keys = nc.theme.print_keys;
	ewl_config.theme.print_signals = nc.theme.print_signals;
	ewl_config.theme.cclass_override = nc.theme.cclass_override;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_config_defaults_set(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ecore_config_int_default("/ewl/debug/enable", 0);
	ecore_config_int_default("/ewl/debug/level", 0);
	ecore_config_string_default("/ewl/engine_name", "evas_software_x11");
	ecore_config_int_default("/ewl/evas/font_cache", 2097152);
	ecore_config_int_default("/ewl/evas/image_cache", 8388608);
	ecore_config_theme_default("/ewl/theme/name", "default");
	ecore_config_string_default("/ewl/theme/icon/name", "Tango");
	ecore_config_int_default("/ewl/theme/cache", 0);
	ecore_config_int_default("/ewl/theme/color_classes/override", 0);
	ecore_config_int_default("/ewl/theme/print_keys", 0);
	ecore_config_int_default("/ewl/theme/print_signals", 0);

	/* need to set each of these keys into the system section */
	{
		Ecore_Config_Prop *prop = NULL;
		int i = 0;
		char *keys [] = {
		    "/ewl/debug/enable",
		    "/ewl/debug/level",
		    "/ewl/engine_name",
		    "/ewl/evas/font_cache",
		    "/ewl/evas/image_cache",
		    "/ewl/theme/name",
		    "/ewl/theme/icon/name",
		    "/ewl/theme/cache",
		    "/ewl/theme/color_classes/override",
		    "/ewl/theme/print_keys",
		    "/ewl/theme/print_signals",
		    NULL
		};

		for(i = 0; keys[i] != NULL; i++) {
			prop = ecore_config_get(keys[i]);
			prop->flags &= ~ECORE_CONFIG_FLAG_MODIFIED;
			prop->flags |= ECORE_CONFIG_FLAG_SYSTEM;
		}

		ecore_config_listen("ewl_debug_enable", "/ewl/debug/enable",
		    ewl_config_listener, EWL_CONFIG_DEBUG_ENABLE, NULL);
		ecore_config_listen("ewl_debug_level", "/ewl/debug/level",
		    ewl_config_listener, EWL_CONFIG_DEBUG_LEVEL, NULL);
		ecore_config_listen("ewl_engine_name", "/ewl/engine_name",
		    ewl_config_listener, EWL_CONFIG_ENGINE_NAME, NULL);
		ecore_config_listen("ewl_font_cache", "/ewl/evas/font_cache",
		    ewl_config_listener, EWL_CONFIG_EVAS_FONT_CACHE, NULL);
		ecore_config_listen("ewl_image_cache", "/ewl/evas/image_cache",
		    ewl_config_listener, EWL_CONFIG_EVAS_IMAGE_CACHE, NULL);
		ecore_config_listen("ewl_theme_name", "/ewl/theme/name",
		    ewl_config_listener, EWL_CONFIG_THEME_NAME, NULL);
		ecore_config_listen("ewl_theme_icon_name", "/ewl/theme/icon/name",
		    ewl_config_listener, EWL_CONFIG_THEME_ICON_THEME_NAME, NULL);
		ecore_config_listen("ewl_theme_cache", "/ewl/theme/cache",
		    ewl_config_listener, EWL_CONFIG_THEME_CACHE, NULL);
		ecore_config_listen("ewl_theme_print_keys", "/ewl/theme/print_keys",
		    ewl_config_listener, EWL_CONFIG_THEME_PRINT_KEYS, NULL);
		ecore_config_listen("ewl_theme_print_signals", "/ewl/theme/print_signals",
		    ewl_config_listener, EWL_CONFIG_THEME_PRINT_SIGNALS, NULL);
		ecore_config_listen("ewl_theme_cclases_override", 
					"/ewl/theme/color_classes/override",
		    ewl_config_listener, EWL_CONFIG_THEME_COLOR_CLASSES_OVERRIDE, NULL);
	}

	DRETURN(DLEVEL_STABLE);
}

static int
ewl_config_listener(const char *key, 
			const Ecore_Config_Type type __UNUSED__, 
			const int tag, void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("key", key, 0);

	switch(tag) {
		case EWL_CONFIG_DEBUG_ENABLE:
			ewl_config.debug.enable = ewl_config_int_get(key);
			break;

		case EWL_CONFIG_DEBUG_LEVEL:
			ewl_config.debug.level = ewl_config_int_get(key);
			break;

		case EWL_CONFIG_ENGINE_NAME:
			IF_FREE(ewl_config.engine_name);
			ewl_config.engine_name = ewl_config_str_get(key);
			break;

		case EWL_CONFIG_EVAS_FONT_CACHE:
			ewl_config.evas.font_cache = ewl_config_int_get(key);
			break;

		case EWL_CONFIG_EVAS_IMAGE_CACHE:
			ewl_config.evas.image_cache = ewl_config_int_get(key);
			break;

		case EWL_CONFIG_THEME_NAME:
			IF_FREE(ewl_config.theme.name);
			ewl_config.theme.name = ewl_config_str_get(key);
			break;
	
		case EWL_CONFIG_THEME_ICON_THEME_NAME:
			IF_FREE(ewl_config.theme.icon_theme);
			ewl_config.theme.icon_theme = ewl_config_str_get(key);
			ewl_icon_theme_theme_change();
			break;

		case EWL_CONFIG_THEME_CACHE:
			ewl_config.theme.cache = ewl_config_int_get(key);
			break;
			
		case EWL_CONFIG_THEME_COLOR_CLASSES_OVERRIDE:
			ewl_config.theme.cclass_override = ewl_config_int_get(key);
			break;

		case EWL_CONFIG_THEME_PRINT_KEYS:
			ewl_config.theme.print_keys = ewl_config_int_get(key);
			break;

		case EWL_CONFIG_THEME_PRINT_SIGNALS:
			ewl_config.theme.print_signals = ewl_config_int_get(key);
			break;
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}


