
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

static char theme_path[PATH_LEN];

static E_DB_File *theme_db = NULL;

static Ewd_Hash *cached_theme_data = NULL;
static Ewd_Hash *def_theme_data = NULL;

/* Initialize the data structures involved with theme handling. This involves
 * finding the specified theme file. */
int
ewl_theme_init(void)
{
	struct stat st;
	char *theme_name;
	char theme_db_path[1024];
	char *home;

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
		  return -1;
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

	return 1;
}

/* Initialize the widget's theme */
void
ewl_theme_init_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	w->theme = def_theme_data;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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
	return strdup(theme_path);
}

/* Return the path of the current theme's fonts */
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

	return font_path;
}

/* Return a string with the path to the specified image */
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
	  }
	else			/* Absolute path given, so return it */
		path = strdup(data);

	if (((stat(path, &st)) == -1) || !S_ISREG(st.st_mode))
		printf("Couldn't stat %s\n", path);

	DRETURN_PTR(path, DLEVEL_STABLE);
}

/* Retrieve data from the theme */
void *
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

void
ewl_theme_data_get_int(Ewl_Widget * w, char *k, int *v)
{
	int ret = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("k", k);

	if (!v)
		DRETURN(DLEVEL_STABLE);

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

	*v = ret;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* Store data into the theme */
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

	if (REALIZED(w))
		ewl_widget_theme_update(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

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
