
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif


static E_DB_File *config_db = NULL;

void            __ewl_color_class_free(void *data);
void            __create_user_config(void);

static int      __open_config_db(const char *name);
static void     __close_config_db(void);
static int      __config_exists(char *name);

extern Ewd_List *ewl_embed_list;

Ewl_Config ewl_config;

/**
 * @return Returns true on success, false on failure.
 * @brief Initialize the configuration system
 *
 * This sets up the necessary configuration variables.
 */
int ewl_config_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	memset(&ewl_config, 0, sizeof(Ewl_Config));

	if (__config_exists("system") != -1)
		__close_config_db();
	else
		__create_user_config();

	if (__config_exists("system") == -1)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param config: in what config to search for the key
 * @param k: the key to set in the configuration database
 * @param v: the string value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief set the value of key to the specified string
 *
 * Sets the string value associated with the key @a k to @a v in the
 * configuration database.
 */
int ewl_config_set_str(char *config, char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!__open_config_db(config))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	e_db_str_set(config_db, k, v);

	__close_config_db();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}



/**
 * @param config: in what config to search for the key
 * @param k: the key to set in the configuration database
 * @param v: the integer value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Set the value of key to the specified integer
 *
 * Sets the integer value associated with the key @a k to @a v in the
 * configuration database.
 */
int ewl_config_set_int(char *config, char *k, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!__open_config_db(config))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	e_db_int_set(config_db, k, v);

	__close_config_db();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * @param config: in what config to search for the key
 * @param k: the key to set in the configuration database
 * @param v: the float value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Set the value of key to the specified float
 *
 * Sets the float value associated with the key @a k to @a v in the
 * configuration database.
 */
int ewl_config_set_float(char *config, char *k, float v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!__open_config_db(config))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	e_db_float_set(config_db, k, v);

	__close_config_db();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}


/**
 * @param config: in what config to search for the key
 * @param k: the key to search
 * @return Returns the found string value on success, NULL on failure.
 * @brief Retrieve string value associated with a key
 */
char           *ewl_config_get_str(char *config, char *k)
{
	char           *ret = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (__open_config_db(config)) {
		ret = e_db_str_get(config_db, k);

		__close_config_db();
	}

	DRETURN_PTR(ret, DLEVEL_STABLE);
}


/**
 * @param config: in what config to search for the key
 * @param k: the key to search
 * @return Returns the found integer value on success, 0 on failure.
 * @brief Retrieve integer value associated with a key
 */
int ewl_config_get_int(char *config, char *k)
{
	int             ret = -1;
	int             v;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (__open_config_db(config)) {
		ret = e_db_int_get(config_db, k, &v);

		__close_config_db();
	}

	if (!ret)
		DRETURN_INT(ret, DLEVEL_STABLE);

	DRETURN_INT(v, DLEVEL_STABLE);
}

/**
 * @param config: in what config to search for the key
 * @param k: the key to search
 * @return Returns the found float value on success, 0.0 on failure.
 * @brief Retrieve floating point value associated with a key
 */
float ewl_config_get_float(char *config, char *k)
{
	int             ret = -1;
	float           v = 0.0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (__open_config_db(config)) {
		ret = e_db_float_get(config_db, k, &v);

		__close_config_db();
	}

	if (!ret)
		DRETURN_FLOAT(ret, DLEVEL_STABLE);

	DRETURN_FLOAT(v, DLEVEL_STABLE);
}

/**
 * @return Returns the found render method, default software render.
 * @brief Retrieve the render method of the evas
 */
char *ewl_config_get_render_method()
{

	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_INT((ewl_config.evas.render_method ?
				strdup(ewl_config.evas.render_method) : NULL),
			DLEVEL_STABLE);
}

/**
 * @return Returns no value.
 * @brief Reread the values of the configuration database
 *
 * Reads in the values of the configuration database and applies them to the
 * running ewl program.
 */
void ewl_config_reread_and_apply(void)
{
	int             cc;
	Ewl_Config      nc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Clean out some memory first, this is likely to get re-used if the
	 * values have not changed.
	 */
	IF_FREE(ewl_config.evas.render_method);
	IF_FREE(ewl_config.theme.name);
	if (ewl_config.theme.cclasses) {
		ewd_list_destroy(ewl_config.theme.cclasses);
		ewl_config.theme.cclasses = NULL;
	}

	nc.debug.enable = ewl_config_get_int("system", "/debug/enable");
	nc.debug.level = ewl_config_get_int("system", "/debug/level");
	nc.evas.font_cache = ewl_config_get_int("system", "/evas/font_cache");
	nc.evas.image_cache = ewl_config_get_int("system", "/evas/image_cache");
	nc.evas.render_method =
	    ewl_config_get_str("system", "/evas/render_method");
	nc.theme.name = ewl_config_get_str("system", "/theme/name");
	nc.theme.cache = ewl_config_get_int("system", "/theme/cache");
	nc.theme.cclass_override = ewl_config_get_int("system",
			"/theme/color_classes/override");

	nc.theme.cclasses = NULL;
	if (nc.theme.cclass_override) {
		int i;

		nc.theme.cclasses = ewd_list_new();
		ewd_list_set_free_cb(nc.theme.cclasses, __ewl_color_class_free);
		cc = ewl_config_get_int("system", "/theme/color_classes/count");
		for (i = 0; i < cc; i++) {
			char key[PATH_MAX];
			Ewl_Color_Class *cclass;

			cclass = NEW(Ewl_Color_Class, 1);

			snprintf(key, PATH_MAX,
					"/theme/color_classes/%d/name", i);
			cclass->name = ewl_config_get_str("system", key);
			if (cclass->name) {
				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/r", i);
				cclass->r = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/g", i);
				cclass->g = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/b", i);
				cclass->b = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/a", i);
				cclass->a = ewl_config_get_int("system", key);

				ewd_list_append(nc.theme.cclasses, cclass);
			}
			else
				FREE(cclass);
		}
	}

	if (ewl_embed_list && !ewd_list_is_empty(ewl_embed_list)) {
		Ewl_Embed      *e;

		ewd_list_goto_first(ewl_embed_list);

		while ((e = ewd_list_next(ewl_embed_list)) != NULL) {
			if (!e->evas)
				continue;

			if (nc.evas.font_cache) {
				evas_font_cache_flush(e->evas);
				evas_font_cache_set(e->evas,
						    nc.evas.font_cache);
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
	ewl_config.evas.render_method = nc.evas.render_method;
	ewl_config.theme.name = nc.theme.name;
	ewl_config.theme.cache = nc.theme.cache;
	ewl_config.theme.cclass_override = nc.theme.cclass_override;
	ewl_config.theme.cclasses = nc.theme.cclasses;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_color_class_free(void *data)
{
	Ewl_Color_Class *class = data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	FREE(class->name);
	FREE(class);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __create_user_config(void)
{
	char           *home;
	char            pe[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);

	home = getenv("HOME");

	if (!home) {
		DWARNING("Failed to fetch environment variable HOME\n");
		DRETURN(DLEVEL_STABLE);
	}

	snprintf(pe, PATH_MAX, "%s/.e", home);
	mkdir(pe, 0700);
	snprintf(pe, PATH_MAX, "%s/.e/ewl", home);
	mkdir(pe, 0700);
	snprintf(pe, PATH_MAX, "%s/.e/ewl/config", home);
	mkdir(pe, 0700);

	ewl_config_set_int("system", "/debug/enable", 0);
	ewl_config_set_int("system", "/debug/level", 0);
	ewl_config_set_str("system", "/evas/render_method", "software_x11");
	ewl_config_set_int("system", "/evas/font_cache", 2097152);
	ewl_config_set_int("system", "/evas/image_cache", 8388608);
	ewl_config_set_str("system", "/theme/name", "default");
	ewl_config_set_int("system", "/theme/cache", 0);

	DRETURN(DLEVEL_STABLE);
}

static int __open_config_db(const char *name)
{
	char           *home;
	char            path[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);

	home = getenv("HOME");

	if (!home) {
		DWARNING("Failed to fetch environment variable HOME\n");
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	snprintf(path, PATH_MAX, "%s/.e/ewl/config/%s.db", home, name);

	config_db = e_db_open(path);

	if (config_db)
		DRETURN_INT(TRUE, DLEVEL_STABLE);

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

static void __close_config_db()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	e_db_flush();

	if (config_db)
		e_db_close(config_db);

	config_db = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int __config_exists(char *name)
{
	char           *home;
	char            path[PATH_MAX];
	struct stat     st;

	DENTER_FUNCTION(DLEVEL_STABLE);

	home = getenv("HOME");

	if (!home) {
		DWARNING("Failed to fetch environment variable HOME\n");
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	snprintf(path, PATH_MAX, "%s/.e/ewl/config/%s.db", home, name);

	stat(path, &st);

	if (!S_ISREG(st.st_mode))
		DRETURN_INT(-1, DLEVEL_STABLE);

	DRETURN_INT(1, DLEVEL_STABLE);
}
