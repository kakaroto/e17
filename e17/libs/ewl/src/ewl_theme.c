
#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#define EWL_GLOBAL_THEMES			PACKAGE_DATA_DIR"/themes"
#define EWL_USER_THEMES				EWL_USER_DIR"/themes"

E_DB_File * theme;

char theme_path[PATH_LEN];
char font_path[PATH_LEN];

void
ewl_theme_init(void)
{
    char *theme_name = NULL;
    char theme_file[PATH_LEN];

    theme_name = ewl_prefs_theme_name_get();


    if (!theme && theme_name[0]) {
        snprintf(theme_path, PATH_LEN, "%s/%s", EWL_GLOBAL_THEMES,
                theme_name);
        snprintf(theme_file, PATH_LEN, "%s/theme.db", theme_path);
        theme = e_db_open_read(theme_file);
    }

    /* Got a theme name so lets try to open the theme */
    if (!theme && theme_name) {
        /* Absolute path given, so open it there */
        if (theme_name[0] == '/')
            snprintf(theme_path, PATH_LEN, "%s",
                    theme_name);
        /* Relative path given so look in user's directory */
        else
            snprintf(theme_path, PATH_LEN, "%s/%s/%s",
                    getenv("HOME"), EWL_USER_THEMES,
                    theme_name);

        snprintf(theme_file, PATH_LEN, "%s/theme.db", theme_path);
        theme = e_db_open_read(theme_file);
    }

    /* Ok, stupid user specified a non-existant theme or no theme,
     * so open the default */
    if (!theme) {
        snprintf(theme_path, PATH_LEN, "%s/default", EWL_GLOBAL_THEMES);
        snprintf(theme_file, PATH_LEN, "%s/theme.db", theme_path);
        theme = e_db_open_read(theme_file);
    }

    snprintf(font_path, PATH_LEN, "%s/appearance/fonts", theme_path);
}

char *
ewl_theme_path()
{
	return strdup(theme_path);
}

char * ewl_theme_font_path()
{
	return strdup(font_path);
}

char *
ewl_theme_ebit_get(char * widget, char * type, char * state)
{
	char temp[PATH_LEN];
	char * value = NULL;
	struct stat st;

	snprintf(temp, PATH_LEN, "%s/appearance/%s/%s/%s.bits.db",
					theme_path, widget, type, state);

	value = strdup(temp);

	stat(value, &st);

	if (S_ISREG(st.st_mode))
		return value;
	else {
		DERROR("File\n%s\ndoes not exist, exiting...\n", value);
		ewl_main_quit();
	}

	return NULL;
}

int
ewl_theme_int_get(char * key)
{
	int value;

	e_db_int_get(theme, key, &value);

	return value;
}

void
ewl_theme_int_set(char * key, int value)
{
	e_db_int_set(theme, key, value);
}


float
ewl_theme_float_get(char * key)
{
	float value;

	e_db_float_get(theme, key, &value);

	return value;
}

void
ewl_theme_float_set(char * key, float value)
{
	e_db_float_set(theme, key, value);
}

char *
ewl_theme_str_get(char * key)
{
	return e_db_str_get(theme, key);
}

void
ewl_theme_str_set(char * key, char * value)
{
	e_db_str_set(theme, key, value);
}
