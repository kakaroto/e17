
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

static char theme_path[PATH_LEN];

static E_DB_File *theme_db = NULL;

static Ewd_Hash *cached_theme_data = NULL;
static Ewd_Hash *def_theme_data = NULL;

/**
 * ewl_theme_init - initialize the themeing  system
 *
 * Returns TRUE on success, FALSE on failure. Initializes the data structures
 * involved with theme handling. This involves finding the specified theme file. */
int
ewl_theme_init(void)
{
	struct stat st;
	char *theme_name;
	char theme_db_path[1024];
	char *home;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Alloacte and clear the default theme 
	 */
	def_theme_data = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	/*
	 * Setup a string with the path to the users theme dir 
	 */
	theme_name = ewl_config_get_str("/theme/name");
	if (!theme_name)
		theme_name = strdup("default");

	home = getenv("HOME");
	if (!home)
	  {
		  DERROR("Environment variable HOME not defined\n"
			 "Try export HOME=/home/user in a bash like environemnt or\n"
			 "setenv HOME=/home/user in a sh like environment.\n");
		  DRETURN_INT(FALSE, DLEVEL_STABLE);
	  }

	snprintf(theme_path, PATH_LEN, "%s/.e/ewl/themes/%s", home,
		 theme_name);

	if (((stat(theme_path, &st)) == 0) || S_ISDIR(st.st_mode))
	  {
		  snprintf(theme_db_path, 1024, "%s/theme.db", theme_path);

		  theme_db = e_db_open_read(theme_db_path);
	  }

	if (!theme_db)
	  {

		  /*
		   * Theme dir is ok, now get the specified theme's path 
		   */
		  snprintf(theme_path, PATH_LEN, PACKAGE_DATA_DIR
			   "/themes/%s", theme_name);
		  stat(theme_path, &st);

		  if (S_ISDIR(st.st_mode))
		    {
			    snprintf(theme_db_path, 1024, "%s/theme.db",
				     theme_path);

			    theme_db = e_db_open_read(theme_db_path);
		    }

		  if (!theme_db)
			  DERROR("No theme dir =( exiting....");
	  }

	IF_FREE(theme_name);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * ewl_theme_init_widget - initialize a widgets theme information to the default
 * @w: the widget to initialize theme information
 *
 * Returns no value. Sets the widget @w's theme information to the default
 * values.
 */
void
ewl_theme_init_widget(Ewl_Widget * w)
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
void
ewl_theme_deinit_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * We only want to destroy the hash if its not def_theme_data
	 * * We destroy def_theme_data from else where.. 
	 */
	if (w->theme && w->theme != def_theme_data)
		ewd_hash_destroy(w->theme);

	else
		w->theme = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* Return the path of the current theme */
char *
ewl_theme_path()
{
	DRETURN_PTR(strdup(theme_path), DLEVEL_STABLE);
}

/**
 * ewl_theme_font_path - retrieve the path of a widgets theme's fonts
 * @w: the widget to search
 *
 * Returns the font path associated with widget @w on success, NULL on failure.
 */
char *
ewl_theme_font_path()
{
	static char *font_path = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * No font path specified yet, so build it up 
	 */
	if (!font_path)
	  {
		  font_path = NEW(char, PATH_LEN);

		  snprintf(font_path, PATH_LEN, "%s/appearance/fonts",
			   theme_path);
	  }

	DRETURN_PTR(font_path, DLEVEL_STABLE);
}

/**
 * ewl_theme_image_get - retrieve the path to an image from a widgets theme
 * @w: the widget to search
 * @k: the image to search for
 *
 * Returns the path associated with image key @k on success, NULL on failure.
 */
char *
ewl_theme_image_get(Ewl_Widget * w, char *k)
{
	char *path;
	char *data;
	struct stat st;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	data = ewl_theme_data_get_str(w, k);

	if (!data)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!strncmp(data, "/appearance", 11))
	  {
		  path = NEW(char, PATH_LEN);

		  snprintf(path, PATH_LEN, "%s%s", theme_path, data);
	} else			/* Absolute path given, so return it */
		path = strdup(data);

	if (((stat(path, &st)) == -1) || !S_ISREG(st.st_mode))
		printf("Couldn't stat %s\n", path);

	DRETURN_PTR(path, DLEVEL_STABLE);
}


/**
 * ewl_theme_data_get_str - retrieve an string value from a widgets theme
 * @w: the widget to search
 * @k: the key to search for
 *
 * Returns the string associated with key @k on success, NULL on failure.
 */
char *
ewl_theme_data_get_str(Ewl_Widget * w, char *k)
{
	void *ret = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	if (w->theme)
		ret = ewd_hash_get(w->theme, k);

	if (!ret && def_theme_data)
		ret = ewd_hash_get(def_theme_data, k);

	if (!ret && cached_theme_data)
		ret = ewd_hash_get(cached_theme_data, k);

	if (!ret && theme_db)
	  {
		  ret = e_db_str_get(theme_db, k);

		  if (ewl_config.theme.cache)
		    {
			    if (!cached_theme_data)
				    cached_theme_data =
					    ewd_hash_new(ewd_str_hash,
							 ewd_str_compare);
			    ewd_hash_set(cached_theme_data, k, ret);
		    }
	  }

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * ewl_theme_data_get_int - retrieve an integer value from a widgets theme
 * @w: the widget to search
 * @k: the key to search for
 *
 * Returns the integer associated with key @k on success, 0 on failure.
 */
int
ewl_theme_data_get_int(Ewl_Widget * w, char *k)
{
	int ret = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, FALSE);

	if (w->theme)
		ret = (int) (ewd_hash_get(w->theme, k));
	else
		ret = (int) (ewd_hash_get(def_theme_data, k));

	if (!ret && cached_theme_data)
		ret = (int) (ewd_hash_get(cached_theme_data, k));

	if (!ret)
	  {
		  e_db_int_get(theme_db, k, &ret);

		  if (!cached_theme_data)
			  cached_theme_data =
				  ewd_hash_new(ewd_str_hash, ewd_str_compare);

		  if (ewl_config.theme.cache)
			  ewd_hash_set(cached_theme_data, k, (void *) ret);
	  }

	DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * ewl_theme_data_set -  store data into a widgets theme
 * @w: the widget to change theme data
 * @k: the key to change
 * @v: the data to assign to the key
 *
 * Returns no value. Changes the theme data in widget @w so that key @k now is
 * associated with value @v.
 */
void
ewl_theme_data_set(Ewl_Widget * w, char *k, char *v)
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
 * ewl_theme_data_set_default - set a theme key to a default value
 * @k: the key to be set
 * @v: the value to set for the key
 *
 * Returns no value. Sets the data associated with key @k to value @v in the
 * default theme data.
 */
void
ewl_theme_data_set_default(char *k, char *v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_hash_set(def_theme_data, k, v);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This isn't needed yet...
void
ewl_theme_data_gen_default_theme_db(char *f)
{
	E_DB_File *db;
	char str[7], key[512], val[512];
	int i = -1, j, jj, l;

	db = e_db_open(f);

	while (theme_keys[++i]) {
		snprintf(key, 512, "%s", (char *) theme_keys[i]);

		l = strlen(key);

		jj = 0;

		for (j = l - 7; j < l; j++)
			str[jj++] = key[j];

		if (!strncasecmp(str, "visible", 7))
			snprintf(val, 512, "yes");
		else
			snprintf(val, 512, "%s.bits.db", theme_keys[i]);

		e_db_str_set(db, key, val);

		++i;
	}

	e_db_flush();

	e_db_close(db);
}
*/
