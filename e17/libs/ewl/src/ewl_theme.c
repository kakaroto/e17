
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

static char theme_path[PATH_LEN];

static Ewd_Hash *def_theme_data = NULL;

static void *theme_keys[] = {
	"/appearance/box/horizontal/base",
	"/appearance/box/horizontal/base.bits.db",
	"/appearance/box/horizontal/base/visible", "yes",
	"/appearance/box/vertical/base",
	"/appearance/box/vertical/base.bits.db",
	"/appearance/box/vertical/base/visible", "yes",

	"/appearance/button/default/base",
	"/appearance/button/default/base.bits.db",
	"/appearance/button/default/base/visible", "yes",
	"/appearance/button/default/text/font", "borzoib",
	"/appearance/button/default/text/font_size", (void *) 8,
	"/appearance/button/default/text/style", "Default",

	"/appearance/button/check/base",
	"/appearance/button/check/base.bits.db",
	"/appearance/button/check/base/visible", "yes",
	"/appearance/button/check/text/font", "borzoib",
	"/appearance/button/check/text/font_size", (void *) 8,
	"/appearance/button/check/text/style", "Default",

	"/appearance/button/radio/base",
	"/appearance/button/radio/base.bits.db",
	"/appearance/button/radio/base/visible", "yes",
	"/appearance/button/radio/text/font", "borzoib",
	"/appearance/button/radio/text/font_size", (void *) 8,
	"/appearance/button/radio/text/style", "Default",

	"/appearance/cursor/default/base",
	"/appearance/cursor/default/base.bits.db",
	"/appearance/cursor/default/base/visible", "yes",

	"/appearance/entry/default/base",
	"/appearance/entry/default/base.bits.db",
	"/appearance/entry/default/base/visible", "yes",
	"/appearance/entry/default/cursor",
	"/appearance/entry/default/cursor.bits.db",
	"/appearance/entry/default/cursor/visible", "yes",
	"/appearance/entry/default/text/font", "borzoib",
	"/appearance/entry/default/text/font_size", (void *) 8,
	"/appearance/entry/default/text/style", "Default",

	"/appearance/list/default/base",
	"/appearance/list/default/base.bits.db",
	"/appearance/list/default/base/visible", "yes",
	"/appearance/list/marker/base",
	"/appearance/list/marker/base.bits.db",
	"/appearance/list/marker/base/visible", "yes",

	"/appearance/seeker/horizontal/base",
	"/appearance/seeker/horizontal/base.bits.db",
	"/appearance/seeker/horizontal/base/visible", "yes",
	"/appearance/seeker/horizontal/dragbar",
	"/appearance/seeker/horizontal/dragbar.bits.db",
	"/appearance/seeker/horizontal/dragbar/visible", "yes",

	"/appearance/seeker/vertical/base",
	"/appearance/seeker/vertical/base.bits.db",
	"/appearance/seeker/vertical/base/visible", "yes",
	"/appearance/seeker/vertical/dragbar",
	"/appearance/seeker/vertical/dragbar.bits.db",
	"/appearance/seeker/vertical/dragbar/visible", "yes",

	"/appearance/table/default/base",
	"/appearance/table/default/base.bits.db",
	"/appearance/table/default/base/visible", "yes",

	"/appearance/window/default/base",
	"/appearance/window/default/base.bits.db",
	"/appearance/window/default/base/visible", "yes",

	NULL, NULL
};


/* Initialize the data structures involved with theme handling. This involves
 * finding the specified theme file. */
int
ewl_theme_init (void)
{
	struct stat st;
	char *str;
	char *home;

	/*
	 * Alloacte and clear the default theme 
	 */
	def_theme_data = ewd_hash_new (ewd_str_hash, ewd_str_compare);

	/*
	 * Setup a string with the path to the users theme dir 
	 */
	str = ewl_config_get_str ("/theme/name");
	if (!str)
		str = strdup ("default");

	home = getenv ("HOME");
	if (!home)
	  {
		  DERROR ("Environment variable HOME not defined\n"
			  "Try export HOME=/home/user in a bash like environemnt or\n"
			  "setenv HOME=/home/user in a sh like environment.\n");
		  return -1;
	  }

	snprintf (theme_path, PATH_LEN, "%s/.e/ewl/themes/%s", home, str);

	/*
	 * Check the users theme dir to make sure it exists and is a dir 
	 */
	stat (theme_path, &st);
	if (!S_ISDIR (st.st_mode))
	  {

		  /*
		   * Theme dir is ok, now get the specified theme's path 
		   */
		  snprintf (theme_path, PATH_LEN, PACKAGE_DATA_DIR
			    "/themes/%s", str);
		  stat (theme_path, &st);

		  if (!S_ISDIR (st.st_mode))
			  DERROR ("No theme dir =( exiting....");
	  }

	IF_FREE (str);

	ewl_theme_data_set_defaults ();

	return 1;
}

/* Initialize the widget's theme */
void
ewl_theme_init_widget (Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	w->theme = def_theme_data;

	DLEAVE_FUNCTION;
}

void
ewl_theme_deinit_widget (Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	/*
	 * We only want to destroy the hash if its not def_theme_data
	 * * We destroy def_theme_data from else where.. 
	 */
	if (w->theme && w->theme != def_theme_data)
		ewd_hash_destroy (w->theme);

	else
		w->theme = NULL;

	DLEAVE_FUNCTION;
}

/* Return the path of the current theme */
char *
ewl_theme_path ()
{
	return strdup (theme_path);
}

/* Return the path of the current theme's fonts */
char *
ewl_theme_font_path ()
{
	static char *font_path = NULL;

	DENTER_FUNCTION;

	/*
	 * No font path specified yet, so build it up 
	 */
	if (!font_path)
	  {
		  font_path = NEW (char, PATH_LEN);

		  snprintf (font_path, PATH_LEN, "%s/appearance/fonts",
			    theme_path);
	  }

	return font_path;
}

/* Return a string with the path to the specified image */
char *
ewl_theme_image_get (Ewl_Widget * w, char *k)
{
	char *path;
	char *data;
	struct stat st;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET ("w", w, NULL);
	DCHECK_PARAM_PTR_RET ("k", k, NULL);

	data = ewl_theme_data_get (w, k);

	if (!data)
		DRETURN_PTR (NULL);

	if (!strncmp (data, "/appearance", 11))
	  {
		  path = NEW (char, PATH_LEN);

		  snprintf (path, PATH_LEN, "%s%s", theme_path, data);
	  }
	else			/* Absolute path given, so return it */
		path = strdup (data);

	stat (path, &st);

	if (!S_ISREG (st.st_mode))
		printf ("Couldn't stat %s\n", path);

	DRETURN_PTR (path);
}

/* Retrieve data from the theme */
void *
ewl_theme_data_get (Ewl_Widget * w, char *k)
{
	void *ret = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET ("k", k, NULL);

	if (w->theme)
		ret = ewd_hash_get (w->theme, k);

	if (!ret)
		ret = ewd_hash_get (def_theme_data, k);

	DRETURN_PTR (ret);
}

/* Store data into the theme */
void
ewl_theme_data_set (Ewl_Widget * w, char *k, char *v)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);
	DCHECK_PARAM_PTR ("k", k);

	if (w->theme == def_theme_data)
		w->theme = ewd_hash_new (ewd_str_hash, ewd_str_compare);

	ewd_hash_set (w->theme, k, v);

	if (REALIZED (w))
		ewl_widget_theme_update (w);

	DLEAVE_FUNCTION;
}

void
ewl_theme_data_set_default (char *k, char *v)
{
	DENTER_FUNCTION;

	ewd_hash_set (def_theme_data, k, v);

	DLEAVE_FUNCTION;
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

void
ewl_theme_data_set_defaults (void)
{
	char *str, *str2;
	int i;

	for (i = 0; theme_keys[i]; i++)
	  {
		  str = theme_keys[i];
		  str2 = theme_keys[++i];

		  ewd_hash_set (def_theme_data, str, str2);
	  }
}
