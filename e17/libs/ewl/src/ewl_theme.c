#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

static char     theme_path[PATH_LEN];

static E_DB_File *theme_db = NULL;

static Ewd_List *font_paths = NULL;
static Ewd_Hash *cached_theme_data = NULL;
static Ewd_Hash *def_theme_data = NULL;

/**
 * ewl_theme_init - initialize the themeing  system
 *
 * Returns TRUE on success, FALSE on failure. Initializes the data structures
 * involved with theme handling. This involves finding the specified theme file. */
int ewl_theme_init(void)
{
	struct stat     st;
	char           *theme_name;
	char            theme_db_path[PATH_LEN];
	char           *home;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Alloacte and clear the default theme 
	 */
	def_theme_data = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	/*
	 * Setup a string with the path to the users theme dir 
	 */
	theme_name = ewl_config_get_str("system", "/theme/name");

	if (!theme_name)
		theme_name = strdup("default");

	home = getenv("HOME");
	if (!home) {
		DERROR("Environment variable HOME not defined\n"
		       "Try export HOME=/home/user in a bash like environemnt or\n"
		       "setenv HOME=/home/user in a csh like environment.\n");
		exit(-1);
	}

	snprintf(theme_path, PATH_LEN, "%s/.e/ewl/themes/%s", home, theme_name);

	if (((stat(theme_path, &st)) == 0) || S_ISDIR(st.st_mode)) {
		snprintf(theme_db_path, PATH_LEN, "%s/theme.db", theme_path);

		theme_db = e_db_open_read(theme_db_path);
	}

	if (!theme_db) {

		/*
		 * Theme dir is ok, now get the specified theme's path 
		 */
		snprintf(theme_path, PATH_LEN, PACKAGE_DATA_DIR
			 "/themes/%s", theme_name);
		stat(theme_path, &st);

		if (S_ISDIR(st.st_mode)) {
			snprintf(theme_db_path, PATH_LEN, "%s/theme.db",
				 theme_path);

			theme_db = e_db_open_read(theme_db_path);
		}

		if (!theme_db) {
			DERROR("No theme db %s =( exiting....", theme_db_path);
			exit(-1);
		}
	}

	ewl_theme_init_font_path();

	IF_FREE(theme_name);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_theme_init_font_path - initialize the font path from the theme
 *
 * Returns no value. Initializes the font path based on the theme.
 */
void ewl_theme_init_font_path()
{
	char            font_path[PATH_LEN];

	/*
	 * Setup the default font paths
	 */
	font_paths = ewd_list_new();
	if (font_paths) {
		snprintf(font_path, PATH_LEN, "%s/appearance/fonts",
			 theme_path);

		ewd_list_append(font_paths, font_path);
	}
}

/**
 * ewl_theme_init_widget - initialize a widgets theme information to the default
 * @w: the widget to initialize theme information
 *
 * Returns no value. Sets the widget @w's theme information to the default
 * values.
 */
void ewl_theme_init_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	w->theme = def_theme_data;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_theme_deinit_widget - remove the theme information from a widget
 * @w: the widget to remove theme information
 *
 * Returns no value. Removes and frees (if not default) the theme information
 * from the widget @w.
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

/* Return the path of the current theme */
char           *ewl_theme_path()
{
	DRETURN_PTR(strdup(theme_path), DLEVEL_STABLE);
}

/**
 * ewl_theme_font_path - retrieve the path of a widgets theme's fonts
 * @w: the widget to search
 *
 * Returns the font path associated with widget @w on success, NULL on failure.
 */
Ewd_List       *ewl_theme_font_path_get()
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(font_paths, DLEVEL_STABLE);
}

/**
 * ewl_theme_font_path_add - add a specified path to the font search path
 * @path: the font to add to the search path
 *
 * Returns no value. Duplicates the string pointed to by @path and adds it to
 * the list of paths that are searched for fonts.
 */
void ewl_theme_font_path_add(char *path)
{
	char *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("path", path);

	temp = strdup(path);
	ewd_list_append(font_paths, temp);
	ewl_window_font_path_add(temp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_theme_image_get - retrieve the path to an image from a widgets theme
 * @w: the widget to search
 * @k: the image to search for
 *
 * Returns the path associated with image key @k on success, NULL on failure.
 */
char           *ewl_theme_image_get(Ewl_Widget * w, char *k)
{
	char           *path;
	char           *data;
	struct stat     st;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	data = ewl_theme_data_get_str(w, k);

	if (!data)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (*data != '/') {
		path = NEW(char, PATH_LEN);

		snprintf(path, PATH_LEN, "%s/%s", theme_path, data);

		FREE(data);
	} else			/* Absolute path given, so return it */
		path = strdup(data);

	if (((stat(path, &st)) == -1) || !S_ISREG(st.st_mode))
		DWARNING("Couldn't stat %s\n", path);

	DRETURN_PTR(path, DLEVEL_STABLE);
}


/**
 * ewl_theme_data_get_str - retrieve an string value from a widgets theme
 * @w: the widget to search
 * @k: the key to search for
 *
 * Returns the string associated with key @k on success, NULL on failure.
 */
char           *ewl_theme_data_get_str(Ewl_Widget * w, char *k)
{
	char           *ret = NULL;
	char           *temp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	printf("%s\n", k);

	for (temp = k; temp && !ret; temp = strchr(temp, '/')) {
		if (w->theme)
			ret = ewd_hash_get(w->theme, temp);

		if (!ret && def_theme_data)
			ret = ewd_hash_get(def_theme_data, temp);

		if (!ret && ewl_config.theme.cache && cached_theme_data)
			ret = ewd_hash_get(cached_theme_data, temp);

		if (!ret && theme_db) {
			ret = e_db_str_get(theme_db, temp);

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

	if (ret)
		ret = strdup(ret);

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * ewl_theme_data_get_int - retrieve an integer value from a widgets theme
 * @w: the widget to search
 * @k: the key to search for
 *
 * Returns the integer associated with key @k on success, 0 on failure.
 */
int ewl_theme_data_get_int(Ewl_Widget * w, char *k)
{
	int             ret = 0;
	char           *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	printf("%s\n", k);

	for (temp = k; temp && !ret; temp = strchr(temp, '/')) {
		if (w->theme)
			ret = (int) (ewd_hash_get(w->theme, temp));
		else
			ret = (int) (ewd_hash_get(def_theme_data, temp));

		if (!ret)
			e_db_int_get(theme_db, temp, &ret);
		temp++;
	}


	DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * ewl_theme_data_set_str -  store data into a widgets theme
 * @w: the widget to change theme data
 * @k: the key to change
 * @v: the data to assign to the key
 *
 * Returns no value. Changes the theme data in widget @w so that key @k now is
 * associated with value @v.
 */
void ewl_theme_data_set_str(Ewl_Widget * w, char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (w->theme == def_theme_data)
		w->theme = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	if (v)
		ewd_hash_set(w->theme, k, strdup(v));
	else
		ewd_hash_set(w->theme, k, v);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_theme_data_set_int -  store data into a widgets theme
 * @w: the widget to change theme data
 * @k: the key to change
 * @v: the data to assign to the key
 *
 * Returns no value. Changes the theme data in widget @w so that key @k now is
 * associated with value @v.
 */
void ewl_theme_data_set_int(Ewl_Widget * w, char *k, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (w->theme == def_theme_data)
		w->theme = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	ewd_hash_set(w->theme, k, (void *) v);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_theme_data_set_default_str - set a theme key to a default value
 * @k: the key to be set
 * @v: the value to set for the key
 *
 * Returns no value. Sets the data associated with key @k to value @v in the
 * default theme data.
 */
void ewl_theme_data_set_default_str(char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_hash_set(def_theme_data, k, strdup(v));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_theme_data_set_default_int - set a theme key to a default value
 * @k: the key to be set
 * @v: the value to set for the key
 *
 * Returns no value. Sets the data associated with key @k to value @v in the
 * default theme data.
 */
void ewl_theme_data_set_default_int(char *k, int v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_hash_set(def_theme_data, k, (void *) v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
