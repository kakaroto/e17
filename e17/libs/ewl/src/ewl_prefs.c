
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#define EWL_CONFIG_FILE			"config/system.db"
#define EWL_GLOBAL_CONFIG		PACKAGE_DATA_DIR "/config/system.db"

/* The paths to the config files */
static char user_config[PATH_LEN];
static char global_config[PATH_LEN];

static E_DB_File *user_prefs;
static E_DB_File *global_prefs;
static E_DB_File *prefs_db;

void
ewl_prefs_init(void)
{
	char *home = strdup(getenv("HOME"));

	snprintf(user_config, PATH_LEN, "%s/%s/%s", home,
		 EWL_USER_DIR, EWL_CONFIG_FILE);

	user_prefs = e_db_open(user_config);
	global_prefs = e_db_open_read(EWL_GLOBAL_CONFIG);

	if (!user_prefs && !global_prefs)
	  {
		  fprintf(stderr,
			  "Warning!\nNeither Global or user config db found\n");

		  ewl_main_quit();
	  }

	if (user_prefs)
		prefs_db = user_prefs;
	else
		prefs_db = global_prefs;
}

int
ewl_prefs_int_get(char *key)
{
	int value = 0;

	CHECK_PARAM_POINTER_RETURN("key", key, -1);

	e_db_int_get(prefs_db, key, &value);

	return value;
}

int
ewl_prefs_int_set(char *key, int value)
{
	CHECK_PARAM_POINTER_RETURN("key", key, -1);

	if (!user_prefs)
		return FALSE;

	e_db_int_set(user_prefs, key, value);

	return TRUE;
}

char *
ewl_prefs_str_get(char *key)
{
	CHECK_PARAM_POINTER_RETURN("key", key, NULL);

	if (!prefs_db)
		return NULL;

	return e_db_str_get(prefs_db, key);
}

int
ewl_prefs_str_set(char *key, char *value)
{
	CHECK_PARAM_POINTER_RETURN("key", key, -1);
	CHECK_PARAM_POINTER_RETURN("value", value, -1);

	if (!user_prefs || !key)
		return FALSE;

	e_db_str_set(user_prefs, key, value);

	return TRUE;
}

int
ewl_prefs_float_get(char *key, float *value)
{
	CHECK_PARAM_POINTER_RETURN("key", key, -1);

	return e_db_float_get(prefs_db, key, value);
}

int
ewl_prefs_float_set(char *key, float value)
{
	CHECK_PARAM_POINTER_RETURN("key", key, -1);

	if (!user_prefs)
		return FALSE;

	e_db_float_set(user_prefs, key, value);

	return TRUE;
}

char *
ewl_prefs_theme_name_get()
{
	char *str = NULL;

	str = ewl_prefs_str_get("/theme/name");

	if (!str || !strlen(str))
		return strdup("default");

	return str;
}

Evas_Render_Method
ewl_prefs_render_method_get()
{
	char *str = NULL;

	str = ewl_prefs_str_get("/evas/render_method");

	if (str)
	  {
		  if (!strcasecmp(str, "software"))
			  return RENDER_METHOD_ALPHA_SOFTWARE;
		  else if (!strcasecmp(str, "hardware"))
			  return RENDER_METHOD_3D_HARDWARE;
		  else if (!strcasecmp(str, "x11"))
			  return RENDER_METHOD_BASIC_HARDWARE;
	  }

	return RENDER_METHOD_ALPHA_SOFTWARE;
}

double
ewl_prefs_get_fx_max_fps()
{
	float val;


	if (!ewl_prefs_float_get("/fx/max_fps", &val))
		val = 20.0;

	return (double) (val);
}

double
ewl_prefs_get_fx_timeout()
{
	float val;

	if (!ewl_prefs_float_get("/fx/timeout", &val))
		val = 2.0;

	return (double) (val);
}
