
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif


static E_DB_File *config_db = NULL;

void            __create_user_config(void);
void            __create_fx_config(void);

static int      __open_config_db(const char *name);
static void     __close_config_db(void);

static int      __config_exists(char *name);

Ewl_Config ewl_config;

extern Ewd_List *ewl_window_list;

/**
 * ewl_config_init - initialize the configuration system
 *
 * Returns true on success, false on failure. This sets up the necessary
 * configuration variables.
 */
int ewl_config_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	memset(&ewl_config, 0, sizeof(Ewl_Config));

	if (__config_exists("system") != -1)
		__close_config_db();
	else
		__create_user_config();

	if (__config_exists("fx") != -1)
		__close_config_db();
	else
		__create_fx_config();

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_config_set_str - set the value of key to the specified string
 * @config: in what config to search for the key
 * @k: the key to set in the configuration database
 * @v: the string value that will be associated with the key
 *
 * Returns TRUE on success, FALSE on failure. Sets the string value associated
 * with the key @k to @v in the configuration database.
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
 * ewl_config_set_int - set the value of key to the specified integer
 * @config: in what config to search for the key
 * @k: the key to set in the configuration database
 * @v: the integer value that will be associated with the key
 *
 * Returns TRUE on success, FALSE on failure. Sets the integer value associated
 * with the key @k to @v in the configuration database.
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
 * ewl_config_set_float - set the value of key to the specified float
 * @config: in what config to search for the key
 * @k: the key to set in the configuration database
 * @v: the float value that will be associated with the key
 *
 * Returns TRUE on success, FALSE on failure. Sets the float value associated
 * with the key @k to @v in the configuration database.
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
 * ewl_config_get_str - retrieve string value associated with a key
 * @config: in what config to search for the key
 * @k: the key to search
 *
 * Returns the string value associated with key @k in the configuration
 * database on success, NULL on failure.
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
 * ewl_config_get_int - retrieve integer value associated with a key
 * @config: in what config to search for the key
 * @k: the key to search
 *
 * Returns the integer value associated with key @k in the configuration
 * database on success, 0 on failure.
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
 * ewl_config_get_float - retrieve floating point value associated with a key
 * @config: in what config to search for the key
 * @k: the key to search
 *
 * Returns the float value associated with key @k in the configuration
 * database on success, 0.0 on failure.
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
 * ewl_config_get_render_method - retrieve the render method of the evas
 *
 * Returns the found render method on success, software rendering on failure.
 */
int ewl_config_get_render_method()
{
	char           *str = NULL;
	int             method = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	str = ewl_config_get_str("system", "/evas/render_method");
	if (str)
		method = evas_render_method_lookup(str);

	DRETURN_INT(method, DLEVEL_STABLE);
}

/**
 * ewl_config_reread_and_apply - reread the values of the configuration database
 *
 * Returns no value. Reads in the values of the configuration database and
 * applies them to the running ewl program.
 */
void ewl_config_reread_and_apply(void)
{
	Ewl_Config      nc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	nc.debug.enable = ewl_config_get_int("system", "/debug/enable");
	nc.debug.level = ewl_config_get_int("system", "/debug/level");
	nc.evas.font_cache = ewl_config_get_int("system", "/evas/font_cache");
	nc.evas.image_cache = ewl_config_get_int("system", "/evas/image_cache");
	nc.evas.render_method =
	    ewl_config_get_str("system", "/evas/render_method");
	nc.theme.name = ewl_config_get_str("system", "/theme/name");
	nc.theme.cache = ewl_config_get_int("system", "/theme/cache");

	if (ewl_window_list && !ewd_list_is_empty(ewl_window_list)) {
		Ewl_Window     *w;

		ewd_list_goto_first(ewl_window_list);

		while ((w = ewd_list_next(ewl_window_list)) != NULL) {
			if (!w->evas)
				continue;

			if (nc.evas.font_cache) {
				evas_font_cache_flush(w->evas);
				evas_font_cache_set(w->evas,
						    nc.evas.font_cache);
			}

			if (nc.evas.image_cache) {
				evas_image_cache_flush(w->evas);
				evas_image_cache_set(w->evas,
						     nc.evas.image_cache);
			}
		}
	}

	IF_FREE(ewl_config.evas.render_method);
	IF_FREE(ewl_config.theme.name);

	ewl_config.debug.enable = nc.debug.enable;
	ewl_config.debug.level = nc.debug.level;
	ewl_config.evas.font_cache = nc.evas.font_cache;
	ewl_config.evas.image_cache = nc.evas.image_cache;
	ewl_config.evas.render_method = nc.evas.render_method;
	ewl_config.theme.name = nc.theme.name;
	ewl_config.theme.cache = nc.theme.cache;

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
	mkdir(pe, 0755);
	snprintf(pe, PATH_MAX, "%s/.e/ewl", home);
	mkdir(pe, 0755);
	snprintf(pe, PATH_MAX, "%s/.e/ewl/config", home);
	mkdir(pe, 0755);

	ewl_config_set_int("system", "/debug/enable", 0);
	ewl_config_set_int("system", "/debug/level", 0);
	ewl_config_set_str("system", "/evas/render_method", "software_x11");
	ewl_config_set_int("system", "/evas/font_cache", 2097152);
	ewl_config_set_int("system", "/evas/image_cache", 8388608);
	ewl_config_set_str("system", "/fx/paths/0",
			   PACKAGE_DATA_DIR "/plugins/fx");
	ewl_config_set_int("system", "/fx/paths/count", 1);
	ewl_config_set_int("system", "/fx/fps", 50);
	ewl_config_set_str("system", "/theme/name", "default");
	ewl_config_set_int("system", "/theme/cache", 0);

	DRETURN(DLEVEL_STABLE);
}

void __create_fx_config(void)
{
	/* Give buttons fade_in & glow & fade_out effects by default */
	ewl_config_set_int("fx", "/user/button/count", 2);
	ewl_config_set_str("fx", "/user/button/0/name", "glow");
	ewl_config_set_int("fx", "/user/button/0/callbacks/count", 1);
	ewl_config_set_int("fx", "/user/button/0/callbacks/0/cb_start",
			   EWL_CALLBACK_FOCUS_IN);
	ewl_config_set_int("fx", "/user/button/0/callbacks/0/cb_stop",
			   EWL_CALLBACK_FOCUS_OUT);

	ewl_config_set_str("fx", "/user/button/1/name", "fade_in");
	ewl_config_set_int("fx", "/user/button/1/callbacks/count", 1);
	ewl_config_set_int("fx", "/user/button/1/callbacks/0/cb_start",
			   EWL_CALLBACK_SHOW);
	ewl_config_set_int("fx", "/user/button/1/callbacks/0/cb_stop",
			   EWL_CALLBACK_HIDE);
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
