#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

static char     *theme_path = NULL;

static E_DB_File *theme_db = NULL;

static Ewd_List *font_paths = NULL;
static Ewd_Hash *cached_theme_data = NULL;
static Ewd_Hash *def_theme_data = NULL;

static void ewl_theme_init_font_path(void);

/**
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the themeing  system
 *
 * Initializes the data structures involved with theme handling. Involves
 * finding the specified theme file. This is called by ewl_init, and is not
 * necessary for the end programmer to call.
 */
int ewl_theme_init(void)
{
	struct stat     st;
	char           *theme_name;
	char            theme_db_path[PATH_MAX];
	char           *home;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Alloacte and clear the default theme 
	 */
	def_theme_data = ewd_hash_new(ewd_str_hash, ewd_str_compare);
	if (!def_theme_data)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/*
	 * Retrieve the current theme from the users config.
	 */
	theme_name = ewl_config_get_str("system", "/theme/name");
	if (!theme_name)
		theme_name = strdup("default");

	if (!theme_name)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	/*
	 * Get the users home directory. This environment variable should
	 * always be set.
	 */
	home = getenv("HOME");
	if (!home) {
		DERROR("Environment variable HOME not defined\n"
		       "Try export HOME=/home/user in a bash like environemnt or\n"
		       "setenv HOME=/home/user in a csh like environment.\n");
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	/*
	 * Build a path to the theme if it is the users home dir and use it if
	 * available. First attempts to use the theme.db method, but fails
	 * over to an eet.
	 */
	snprintf(theme_db_path, PATH_MAX, "%s/.e/ewl/themes/%s", home,
			theme_name);

	if (((stat(theme_db_path, &st)) == 0) && S_ISDIR(st.st_mode)) {
		snprintf(theme_db_path, PATH_MAX,
				"%s/.e/ewl/themes/%s/theme.db", home,
				theme_name);
		theme_db = e_db_open_read(theme_db_path);
		if (theme_db) {
			snprintf(theme_db_path, PATH_MAX,
					"%s/.e/ewl/themes/%s/", home,
					theme_name);
			theme_path = strdup(theme_db_path);
		}
	}

	if (!theme_path) {
		snprintf(theme_db_path, PATH_MAX, "%s/.e/ewl/themes/%s.eet",
				home, theme_name);
		if (((stat(theme_db_path, &st)) == 0) && S_ISREG(st.st_mode)) {
			theme_path = strdup(theme_db_path);
		}
	}

	/*
	 * No user theme, so we try the system-wide theme. Same failover
	 * scheme, theme.db first fails over to the eet.
	 */
	if (!theme_path) {

		/*
		 * Theme dir is ok, now get the specified theme's path 
		 */
		snprintf(theme_db_path, PATH_MAX, PACKAGE_DATA_DIR
			 "/themes/%s", theme_name);

		if (((stat(theme_db_path, &st)) == 0) && S_ISDIR(st.st_mode)) {
			snprintf(theme_db_path, PATH_MAX, PACKAGE_DATA_DIR
					"/themes/%s/theme.db", theme_name);

			theme_db = e_db_open_read(theme_db_path);
			if (theme_db) {
				snprintf(theme_db_path, PATH_MAX,
						PACKAGE_DATA_DIR "/themes/%s",
						theme_name);
				theme_path = strdup(theme_db_path);
			}
		}

	}

	if (!theme_path) {
		snprintf(theme_db_path, PATH_MAX, PACKAGE_DATA_DIR
				"/themes/%s.eet", theme_name);
		if (((stat(theme_db_path, &st)) == 0) &&
				S_ISREG(st.st_mode)) {
			theme_path = strdup(theme_db_path);
		}
	}

	/*
	 * If we can't find a theme, no point in continuing further.
	 */
	if (!theme_path) {
		DERROR("No usable theme found, exiting EWL");
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_theme_init_font_path();

	IF_FREE(theme_name);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/*
 * Initializes the font path based on the theme. Also called by ewl_init, and
 * is not recommended to be called separately.
 */
static void ewl_theme_init_font_path()
{
	char           *font_path;
	char            key[PATH_MAX];

	/*
	 * Setup the default font paths
	 */
	font_paths = ewd_list_new();
	if (font_paths) {
		snprintf(key, PATH_MAX, "/theme/font_path");
		font_path = ewl_theme_data_get_str(NULL, key);

		if (font_path) {
			if (*font_path == '/')
				ewd_list_append(font_paths, font_path);
			else {
				snprintf(key, PATH_MAX, "%s/%s", theme_path,
						font_path);
				ewd_list_append(font_paths, strdup(key));
			}

			FREE(font_path);
		}
	}
}

/**
 * @param w: the widget to initialize theme information
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a widgets theme information to the default
 *
 * Sets the widget @a w's theme information to the default values.
 */
int ewl_theme_init_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	w->theme = def_theme_data;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param w: the widget to remove theme information
 * @return Returns no value.
 * @brief remove the theme information from a widget
 *
 * Removes and frees the theme information from the widget @a w.
 */
void ewl_theme_deinit_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * We only want to destroy the hash if its not def_theme_data
	 * We destroy def_theme_data from else where.. 
	 */
	if (w->theme && w->theme != def_theme_data)
		ewd_hash_destroy(w->theme);

	else
		w->theme = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief Return the path of the current theme
 * @return Returns a copy of the current theme path on success, NULL on failure
 */
char           *ewl_theme_path()
{
	DRETURN_PTR(strdup(theme_path), DLEVEL_STABLE);
}

/**
 * @brief Returns the open theme db file handle.
 * @return Returns the open theme database on success, NULL for an eet theme.
 */
E_DB_File *ewl_theme_get_db()
{
	DRETURN_PTR(theme_db, DLEVEL_STABLE);
}

/**
 * @return Returns the font path of widget @a w on success, NULL on failure.
 * @brief retrieve the path of a widgets theme's fonts
 */
Ewd_List       *ewl_theme_font_path_get()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(font_paths, DLEVEL_STABLE);
}

/**
 * @param path: the font to add to the search path
 * @return Returns no value.
 * @brief Add a specified path to the font search path
 *
 * Duplicates the string pointed to by @a path and adds it to
 * the list of paths that are searched for fonts.
 */
void ewl_theme_font_path_add(char *path)
{
	char *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("path", path);

	temp = strdup(path);
	ewd_list_append(font_paths, temp);
	ewl_embed_font_path_add(temp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the image to search for
 * @return Returns the path of image key @a k on success, NULL on failure.
 * @brief retrieve the path to an image from a widgets theme
 */
char           *ewl_theme_image_get(Ewl_Widget * w, char *k)
{
	char           *data;
	struct stat     st;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (theme_db)
		data = ewl_theme_data_get_str(w, k);
	else
		data = strdup(theme_path);

	if (!data)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	/*
	 * Convert a relative path to an absolute path
	 */
	if (*data != '/') {
		char path[PATH_MAX];

		snprintf(path, PATH_MAX, "%s/%s", theme_path, data);

		FREE(data);
		data = strdup(path);
	}

	if (((stat(data, &st)) == -1) || !S_ISREG(st.st_mode))
		DWARNING("Couldn't stat image\n");

	DRETURN_PTR(data, DLEVEL_STABLE);
}


/**
 * @param w: the widget to search
 * @param k: the key to search for
 * @return Returns the string associated with @a k on success, NULL on failure.
 * @brief Retrieve an string value from a widgets theme
 */
char           *ewl_theme_data_get_str(Ewl_Widget * w, char *k)
{
	char           *ret = NULL;
	char           *temp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	for (temp = k; temp && !ret; temp = strchr(temp, '/')) {
		if (w && w->theme)
			ret = ewd_hash_get(w->theme, temp);

		if (!ret && def_theme_data)
			ret = ewd_hash_get(def_theme_data, temp);

		if (!ret && ewl_config.theme.cache && cached_theme_data)
			ret = ewd_hash_get(cached_theme_data, temp);

		if (!ret) {
			if (theme_db)
				ret = e_db_str_get(theme_db, temp);
			else
				ret = edje_file_data_get(theme_path, temp);

			if (ret && ewl_config.theme.cache) {
				if (!cached_theme_data)
					cached_theme_data =
						ewd_hash_new(ewd_str_hash,
								ewd_str_compare);
				ewd_hash_set(cached_theme_data, temp,
						strdup(ret));
			}
		}
		temp++;
	}

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the key to search for
 * @return Returns the integer associated with key @a k on success, 0 on failure.
 * @brief Retrieve an integer value from a widgets theme
 */
int ewl_theme_data_get_int(Ewl_Widget * w, char *k)
{
	int             ret = 0;
	char           *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	for (temp = k; temp && !ret; temp = strchr(temp, '/')) {
		if (w->theme)
			ret = (int) (ewd_hash_get(w->theme, temp));
		else
			ret = (int) (ewd_hash_get(def_theme_data, temp));

		if (!ret) {
			if (theme_db)
				e_db_int_get(theme_db, temp, &ret);
			else {
				char *val;

				val = edje_file_data_get(theme_path, temp);
				if (val) {
					ret = atoi(val);
					FREE(val);
				}
			}
		}
		temp++;
	}


	DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to change theme data
 * @param k: the key to change
 * @param v: the data to assign to the key
 * @return Returns no value.
 * @brief Store data into a widgets theme
 *
 * Changes the theme data in widget @a w so that key @a k now is
 * associated with value @a v.
 */
void ewl_theme_data_set_str(Ewl_Widget * w, char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->theme || w->theme == def_theme_data)
		w->theme = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	if (v)
		ewd_hash_set(w->theme, k, strdup(v));
	else
		ewd_hash_set(w->theme, k, v);

	if (REALIZED(w)) {
		ewl_widget_unrealize(w);
		ewl_widget_realize(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to change theme data
 * @param k: the key to change
 * @param v: the data to assign to the key
 * @return Returns no value.
 * @brief Store data into a widgets theme
 *
 * Changes the theme data in widget @a w so that key @a k now is
 * associated with value @a v.
 */
void ewl_theme_data_set_int(Ewl_Widget * w, char *k, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->theme || w->theme == def_theme_data)
		w->theme = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	ewd_hash_set(w->theme, k, (void *) v);

	if (REALIZED(w)) {
		ewl_widget_unrealize(w);
		ewl_widget_realize(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param k: the key to be set
 * @param v: the value to set for the key
 * @return Returns no value.
 * @brief Set a theme key to a default value
 *
 * Sets the data associated with key @a k to value @a v in the default theme
 * data.
 */
void ewl_theme_data_set_default_str(char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_hash_set(def_theme_data, k, strdup(v));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param k: the key to be set
 * @param v: the value to set for the key
 * @return Returns no value.
 * @brief Set a theme key to a default value
 *
 * Sets the data associated with key @a k to value @a v in the
 * default theme data.
 */
void ewl_theme_data_set_default_int(char *k, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_hash_set(def_theme_data, k, (void *) v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
