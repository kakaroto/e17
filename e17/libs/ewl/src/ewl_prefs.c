
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#define EWL_CONFIG_FILE			"config.db"
#define EWL_GLOBAL_CONFIG		PACKAGE_DATA_DIR "/config/system.db"

/* The paths to the config files */
char user_config[PATH_LEN];
char global_config[PATH_LEN];

E_DB_File *user_prefs;
E_DB_File *global_prefs;

void
ewl_prefs_init(void)
{
	char * home = strdup(getenv("HOME"));

	snprintf(user_config, PATH_LEN, "%s/%s/%s", home,
					EWL_USER_DIR, EWL_CONFIG_FILE);

	user_prefs = e_db_open(user_config);
	global_prefs = e_db_open_read(EWL_GLOBAL_CONFIG);

	if (!user_prefs && !global_prefs)
		fprintf(stderr, "Warning!\nNeither Global or user config db found\n");
}

int
ewl_prefs_int_get(char * key)
{
	int value = 0;

	if (!key)
		return value;

	if (user_prefs) {
		if (e_db_int_get(user_prefs, key, &value))
			return value;
	}

	e_db_int_get(global_prefs, key, &value);

	return value;
}

int
ewl_prefs_int_set(char *key, int value)
{
    if (!user_prefs || !key)
        return FALSE;

    e_db_int_set(user_prefs, key, value);
    return TRUE;
}

char *
ewl_prefs_str_get(char *key)
{
    char *value = NULL;

    if (!key)
        return NULL;

	if (!user_prefs || !global_prefs)
		return NULL;

    if (user_prefs) {
        value = e_db_str_get(user_prefs, key);
        if (value)
            return value;
    }
    return e_db_str_get(global_prefs, key);
}

int
ewl_prefs_str_set(char *key, char *value)
{
    if (!user_prefs || !key)
        return FALSE;

    e_db_str_set(user_prefs, key, value);

    return TRUE;
}

float
ewl_prefs_float_get(char *key)
{
    float value;

    if (user_prefs) {
        if (e_db_float_get(user_prefs, key, &value))
            return value;
    }

    e_db_float_get(global_prefs, key, &value);

    return value;
}

int
ewl_prefs_float_set(char *key, float value)
{
    if (!user_prefs || !key)
        return FALSE;

    e_db_float_set(user_prefs, key, value);
    return TRUE;
}

char *
ewl_prefs_theme_name_get()
{
	char * str = NULL;

	str = ewl_prefs_str_get("/theme/name");

	if (!str || !strlen(str))
		return strdup("default");

	return str;
}

Evas_Render_Method
ewl_prefs_render_method_get()
{
	char * str = NULL;

	str = ewl_prefs_str_get("/evas/render_method");

	if (str) {
		if (!strcasecmp(str, "software"))
			return RENDER_METHOD_ALPHA_SOFTWARE;
		else if (!strcasecmp(str, "hardware"))
			return RENDER_METHOD_3D_HARDWARE;
		else if (!strcasecmp(str, "x11"))
			return RENDER_METHOD_BASIC_HARDWARE;
	}

	return RENDER_METHOD_ALPHA_SOFTWARE;
}
