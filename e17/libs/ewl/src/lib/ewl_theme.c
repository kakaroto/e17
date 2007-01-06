/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

extern Ecore_List *ewl_embed_list;
static char *ewl_theme_path = NULL;

static Ecore_List *ewl_theme_font_paths = NULL;
static Ecore_Hash *ewl_theme_def_data = NULL;

static void ewl_theme_font_path_init(void);
static char *ewl_theme_path_find(const char *name);
static void ewl_theme_data_free(void *data);

/**
 * @internal
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the themeing  system
 *
 * Initializes the data structures involved with theme handling. Involves
 * finding the specified theme file. This is called by ewl_init, and is not
 * necessary for the end programmer to call.
 */
int
ewl_theme_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!ewl_theme_theme_set(ewl_config_string_get(ewl_config, 
						EWL_CONFIG_THEME_NAME)))
	{
		DWARNING("No usable theme found, exiting.");
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value.
 * @brief Shutdown the EWL themeing subsystem
 */
void
ewl_theme_shutdown(void)
{
	char *data;

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (ewl_theme_font_paths) {
		while ((data = ecore_list_remove_first(ewl_theme_font_paths)))
			FREE(data);
		
		ecore_list_destroy(ewl_theme_font_paths);
		ewl_theme_font_paths = NULL;
	}

	if (ewl_theme_def_data) {
		ecore_hash_destroy(ewl_theme_def_data);
		ewl_theme_def_data = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param theme_name: The name of the theme to set
 * @return Returns TRUE if successfully set, FALSE otherwise
 * @brief Sets the current theme to use.
 * Set the current theme
 */
int
ewl_theme_theme_set(const char *theme_name)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("theme_name", theme_name, FALSE);

	/* Allocate and clear the default theme */
	if (ewl_theme_def_data) ecore_hash_destroy(ewl_theme_def_data);

	ewl_theme_def_data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	if (!ewl_theme_def_data)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ecore_hash_set_free_key(ewl_theme_def_data, ewl_theme_data_free);
	ecore_hash_set_free_value(ewl_theme_def_data, ewl_theme_data_free);

	/* clean up the font path list */
	if (ewl_theme_font_paths)
		ecore_list_clear(ewl_theme_font_paths);

	/* get the new theme path and setup the font path */
	ewl_theme_path = ewl_theme_path_find(theme_name);
	if (!ewl_theme_path) DRETURN_INT(FALSE, DLEVEL_STABLE); 

	ewl_theme_font_path_init();

	/* Hide all embeds. If the embed was previously shown we re-show it
	 * again. This should cause everything to reset it's theme values to the
	 * new values */
	ecore_list_goto_first(ewl_embed_list);
	while ((w = ecore_list_next(ewl_embed_list)))
	{
		int vis;

		vis = REALIZED(w);

		ewl_widget_hide(w);
		ewl_widget_unrealize(w);
		if (vis) ewl_widget_realize(w);
	}

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to initialize theme information
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a widgets theme information to the default
 *
 * Sets the widget @a w's theme information to the default values.
 */
int
ewl_theme_widget_init(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, FALSE);

	w->theme = NULL;

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to remove theme information
 * @return Returns no value.
 * @brief remove the theme information from a widget
 *
 * Removes and frees the theme information from the widget @a w.
 */
void
ewl_theme_widget_shutdown(Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/*
	 * We only want to destroy the hash if its not ewl_theme_def_data
	 * We destroy ewl_theme_def_data from else where.. 
	 */
	if (w->theme && w->theme != ewl_theme_def_data)
		ecore_hash_destroy(w->theme);

	w->theme = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief Return the path of the current theme
 * @return Returns the current theme path on success, NULL on failure
 */
const char *
ewl_theme_path_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_theme_path, DLEVEL_STABLE);
}

/**
 * @return Returns the font path of widget @a w on success, NULL on failure.
 * @brief retrieve the path of a widgets theme's fonts
 */
Ecore_List *
ewl_theme_font_path_get(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DRETURN_PTR(ewl_theme_font_paths, DLEVEL_STABLE);
}

/**
 * @param path: the font to add to the search path
 * @return Returns no value.
 * @brief Add a specified path to the font search path
 *
 * Duplicates the string pointed to by @a path and adds it to
 * the list of paths that are searched for fonts.
 */
void
ewl_theme_font_path_add(char *path)
{
	char *temp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("path", path);

	temp = strdup(path);
	ecore_list_append(ewl_theme_font_paths, temp);
	ewl_embed_font_path_add(temp);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the image to search for
 * @return Returns the path of image key @a k on success, NULL on failure.
 * @brief retrieve the path to an image from a widgets theme
 */
char *
ewl_theme_image_get(Ewl_Widget *w, char *k)
{
	char *data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);
	DCHECK_TYPE_RET("w", w, EWL_WIDGET_TYPE, NULL);

	data = ewl_theme_data_str_get(w, k);
	if (!data)
	{
		if (!ewl_theme_path)
		{
			DRETURN_PTR(NULL, DLEVEL_STABLE);
		}
		else
			data = strdup(ewl_theme_path);
	}

	/*
	 * Convert a relative path to an absolute path
	 */
	if (*data != '/') {
		char path[PATH_MAX];

		snprintf(path, PATH_MAX, "%s/%s", ewl_theme_path, data);

		FREE(data);
		data = strdup(path);
	}

	DRETURN_PTR(data, DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the key to search for
 * @return Returns the string associated with @a k on success, NULL on failure.
 * @brief Retrieve an string value from a widgets theme
 */
char *
ewl_theme_data_str_get(Ewl_Widget *w, char *k)
{
	char *ret = NULL;
	char *temp = NULL;
	char key[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	/*
	 * Use the widget's appearance string to build a relative theme key.
	 */
	if (w) {
		char *tmp;

		tmp = ewl_widget_appearance_path_get(w);
		if (tmp) {
			snprintf(key, PATH_MAX, "%s/%s", tmp, k);
			FREE(tmp);
		} else
			snprintf(key, PATH_MAX, "%s", k);

	} else
		snprintf(key, PATH_MAX, "%s", k);

	if (ewl_config_cache.print_keys)
		printf("%s\n", key);

	/*
	 * Loop up the widget heirarchy looking for this key.
	 */
	temp = key;
	while (w && temp && !ret) {

		/*
		 * Find a widget with theme data.
		 */
		while (w && !w->theme)
			w = w->parent;

		if (w && w->theme)
			ret = ecore_hash_get(w->theme, temp);

		if (ret) {
			if (ret != EWL_THEME_KEY_NOMATCH)
				ret = strdup(ret);
			break;
		}

		temp++;
		temp = strchr(temp, '/');
		if (!temp && w && w->parent) {
			temp = key;
			w = w->parent;
		}
	}

	/*
	 * No key found in widgets, look in the default theme and edje.
	 */
	if (!ret) {
		temp = key;
		while (temp && !ret) {
			ret = ecore_hash_get(ewl_theme_def_data, temp);
			if (ret) {
				if (ret != EWL_THEME_KEY_NOMATCH)
					ret = strdup(ret);
				break;
			}

			/*
			 * Resort to looking in the edje.
			 */
			if (!ret) {
				ret = edje_file_data_get(ewl_theme_path, temp);
				if (ret) {
					ecore_hash_set(ewl_theme_def_data,
							strdup(temp),
							strdup(ret));
					break;
				}
			}
			temp++;
			temp = strchr(temp, '/');
		}
	}

	/*
	 * Mark unmatched keys in the cache.
	 */
	if (!ret) {
		ecore_hash_set(ewl_theme_def_data, strdup(key),
				EWL_THEME_KEY_NOMATCH);
	}

	/*
	 * Fixup unmatched keys in the cache.
	 */
	if (ret == EWL_THEME_KEY_NOMATCH)
		ret = NULL;

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the key to search for
 * @return Returns the integer associated with key @a k on success, 0 on failure.
 * @brief Retrieve an integer value from a widgets theme
 */
int
ewl_theme_data_int_get(Ewl_Widget *w, char *k)
{
	char *temp;
	int ret = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	temp = ewl_theme_data_str_get(w, k);
	if (temp) {
		ret = atoi(temp);
		FREE(temp);
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
void
ewl_theme_data_str_set(Ewl_Widget *w, char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	if (!w->theme || w->theme == ewl_theme_def_data) {
		w->theme = ecore_hash_new(ecore_str_hash, ecore_str_compare);

		ecore_hash_set_free_key(w->theme, ewl_theme_data_free);
		ecore_hash_set_free_value(w->theme, ewl_theme_data_free);
	}

	if (v && v != EWL_THEME_KEY_NOMATCH)
		ecore_hash_set(w->theme, strdup(k), strdup(v));
	else
		ecore_hash_set(w->theme, strdup(k), EWL_THEME_KEY_NOMATCH);

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
void
ewl_theme_data_int_set(Ewl_Widget *w, char *k, int v)
{
	char value[16];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("k", k);

	snprintf(value, 16, "%d", v);
	ewl_theme_data_str_set(w, k, value);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Private function for finding the theme path given a theme name, If no theme
 * of name is found we will return null.
 */
static char *
ewl_theme_path_find(const char *name) 
{		
	struct stat st;
	char *theme_found_path = NULL;
	char theme_tmp_path[PATH_MAX];
	char *home;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	/*
	 * Get the users home directory. This environment variable should
	 * always be set.
	 */
	home = getenv("HOME");
	if (!home) {
		DERROR("Environment variable HOME not defined\n"
		       "Try export HOME=/home/user in a bash like environemnt or\n"
		       "setenv HOME=/home/user in a csh like environment.\n");
	}

	/*
	 * Build a path to the theme if it is the users home dir and use it if
	 * available. 
	 */
	if (home) {
		snprintf(theme_tmp_path, PATH_MAX, "%s/.e/ewl/themes/%s.edj",
			home, name);
		if (((stat(theme_tmp_path, &st)) == 0) && S_ISREG(st.st_mode))
			theme_found_path = strdup(theme_tmp_path);
	}

	/*
	 * No user theme, so we try the system-wide theme.
	 */
	if (!theme_found_path) {
		snprintf(theme_tmp_path, PATH_MAX, PACKAGE_DATA_DIR
				"/ewl/themes/%s.edj", name);
		if (((stat(theme_tmp_path, &st)) == 0) && S_ISREG(st.st_mode))
			theme_found_path = strdup(theme_tmp_path);
	}

	/*
	 * see if they gave a full path to the theme
	 */
	if (!theme_found_path) {
		if (name[0] != '/') {
			char *cwd;

			cwd = getenv("PWD");
			if (cwd != NULL) 
				snprintf(theme_tmp_path, PATH_MAX, "%s/%s", cwd, name);
			else
				snprintf(theme_tmp_path, PATH_MAX, "%s", name);

		} else
			snprintf(theme_tmp_path, PATH_MAX, "%s", name);

		if (((stat(theme_tmp_path, &st)) == 0) && S_ISREG(st.st_mode))
			theme_found_path = strdup(theme_tmp_path);
	}

	DRETURN_PTR(theme_found_path, DLEVEL_STABLE);
}

/*
 * Initializes the font path based on the theme. Also called by ewl_init, and
 * is not recommended to be called separately.
 */
static void
ewl_theme_font_path_init(void)
{
	char *font_path;
	char key[PATH_MAX];

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Setup the default font paths
	 */
	ewl_theme_font_paths = ecore_list_new();
	if (!ewl_theme_font_paths)
		DRETURN(DLEVEL_STABLE);
	ecore_list_set_free_cb(ewl_theme_font_paths, free);

	font_path = ewl_theme_data_str_get(NULL, "/theme/font_path");
	if (!font_path)
		DRETURN(DLEVEL_STABLE);

	if (*font_path == '/')
		ecore_list_append(ewl_theme_font_paths, font_path);
	else {
		int len;
		char *tmp;

		len = strlen(ewl_theme_path);
		tmp = ewl_theme_path + len - 4;

		if (strcmp(tmp, ".edj"))
			snprintf(key, PATH_MAX, "%s/%s",
					ewl_theme_path, font_path);
		else
			snprintf(key, PATH_MAX, "%s", ewl_theme_path);
			
		ecore_list_append(ewl_theme_font_paths, strdup(key));
		FREE(font_path);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Private function for freeing theme data in the hash.
 */
static void
ewl_theme_data_free(void *data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!data || data == (void *)EWL_THEME_KEY_NOMATCH)
		DRETURN(DLEVEL_STABLE);

	FREE(data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

