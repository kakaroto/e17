
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif


static E_DB_File *config_db = NULL;

static void     ewl_config_db_create(void);
static int      ewl_config_db_open(const char *name);
static int      ewl_config_db_stat(char *name);
static void     ewl_config_db_close(void);
static int      ewl_config_db_exists(char *name);

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

	if (ewl_config_db_exists("system") != -1)
		ewl_config_db_close();
	else
		ewl_config_db_create();

	if (ewl_config_db_exists("system") == -1)
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

	if (!ewl_config_db_open(config))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	e_db_str_set(config_db, k, v);

	ewl_config_db_close();

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

	if (!ewl_config_db_open(config))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	e_db_int_set(config_db, k, v);

	ewl_config_db_close();

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

	if (!ewl_config_db_open(config))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	e_db_float_set(config_db, k, v);

	ewl_config_db_close();

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

	if (ewl_config_db_open(config)) {
		ret = e_db_str_get(config_db, k);

		ewl_config_db_close();
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

	if (ewl_config_db_open(config)) {
		ret = e_db_int_get(config_db, k, &v);

		ewl_config_db_close();
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

	if (ewl_config_db_open(config)) {
		ret = e_db_float_get(config_db, k, &v);

		ewl_config_db_close();
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
	time_t          mt;
	Ewl_Config      nc;

	DENTER_FUNCTION(DLEVEL_STABLE);

	mt = ewl_config_db_stat("system");
	if (mt == ewl_config.mtime)
		DRETURN(DLEVEL_STABLE);

	ewl_config.mtime = mt;

	/*
	 * Clean out some memory first, this is likely to get re-used if the
	 * values have not changed.
	 */
	IF_FREE(ewl_config.evas.render_method);
	IF_FREE(ewl_config.theme.name);

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

	if (nc.theme.cclass_override) {
		int i;

		cc = ewl_config_get_int("system", "/theme/color_classes/count");
		for (i = 0; i < cc; i++) {
			char *name;
			char key[PATH_MAX];

			snprintf(key, PATH_MAX,
					"/theme/color_classes/%d/name", i);
			name = ewl_config_get_str("system", key);
			if (name) {
				int r, g, b, a;
				int r2, g2, b2, a2;
				int r3, g3, b3, a3;

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/r", i);
				r = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/g", i);
				g = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/b", i);
				b = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/a", i);
				a = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/r2",
						i);
				r2 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/g2",
						i);
				g2 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/b2",
						i);
				b2 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/a2",
						i);
				a2 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/r3",
						i);
				r3 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/g3",
						i);
				g3 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/b3",
						i);
				b3 = ewl_config_get_int("system", key);

				snprintf(key, PATH_MAX,
						"/theme/color_classes/%d/a3",
						i);
				a3 = ewl_config_get_int("system", key);

				edje_color_class_set(name, r, g, b, a,
						r2, g2, b2, a2,
						r3, g3, b3, a3);
				FREE(name);
			}
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void ewl_config_db_create(void)
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

static int ewl_config_db_open(const char *name)
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

static int ewl_config_db_stat(char *name)
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

	DRETURN_INT(st.st_mtime, DLEVEL_STABLE);
}

static void ewl_config_db_close()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (config_db)
		e_db_close(config_db);

	e_db_flush();

	config_db = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int ewl_config_db_exists(char *name)
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
