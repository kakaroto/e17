#include <unistd.h>

#include "../../config.h"
#include "etox_style_private.h"

static Ecore_Hash *color_table = NULL;
static E_DB_File *sys_colors = NULL;
static E_DB_File *user_colors = NULL;

/**
 * _etox_style_color_instance - get a pointer to a color structure
 * @r: the red value of the color
 * @g: the green value of the color
 * @b: the blue value of the color
 * @a: the alpha value of the color
 *
 * Returns a pointer to the color structure for use by contexts, NULL returned
 * on failure.
 */
Etox_Style_Color *_etox_style_color_instance(int r, int g, int b, int a)
{
	unsigned int value;
	Etox_Style_Color *ret;

	/*
	 * Check the hash table for a previous malloc of the color
	 */
	value = ETOX_STYLE_HASH_COLOR(r, g, b, a);
	ret = (Etox_Style_Color *) ecore_hash_get(color_table, (void *) value);
	if (!ret) {
		ret = (Etox_Style_Color *) malloc(sizeof(Etox_Style_Color));
		ret->r = r;
		ret->g = g;
		ret->b = b;
		ret->a = a;
		ecore_hash_set(color_table, (void *) value, ret);
	}

	return ret;
}

/**
 * _etox_style_color_instance_db - get a pointer to a color structure by name
 * @color: the name of the color to retrieve from the colors dbs
 *
 * Returns a pointer to the color structure for use by contexts, NULL returned
 * on failure.
 */
Etox_Style_Color *_etox_style_color_instance_db(char *color)
{
	int r, g, b, a;
	unsigned int value;
	char key[PATH_MAX];
	Etox_Style_Color *found;

	/*
	 * Retrieve the red value
	 */
	snprintf(color, PATH_MAX, "/%s/r", color);
	e_db_int_get(user_colors, key, &r);
	if (!r)
		e_db_int_get(sys_colors, key, &r);

	/*
	 * Retrieve the green value
	 */
	snprintf(color, PATH_MAX, "/%s/g", color);
	e_db_int_get(user_colors, key, &g);
	if (!g)
		e_db_int_get(sys_colors, key, &g);

	/*
	 * Retrieve the blue value
	 */
	snprintf(color, PATH_MAX, "/%s/b", color);
	e_db_int_get(user_colors, key, &b);
	if (!b)
		e_db_int_get(sys_colors, key, &b);

	/*
	 * Retrieve the alpha value
	 */
	snprintf(color, PATH_MAX, "/%s/a", color);
	e_db_int_get(user_colors, key, &a);
	if (!a)
		e_db_int_get(sys_colors, key, &a);

	value = ETOX_STYLE_HASH_COLOR(r, g, b, a);

	/*
	 * Check for a previous allocation for this color.
	 */
	found = (Etox_Style_Color *) ecore_hash_get(color_table, (void *) value);
	if (!found) {
		found = (Etox_Style_Color *) malloc(sizeof(Etox_Style_Color));
		found->r = r;
		found->g = g;
		found->b = b;
		found->a = a;
		ecore_hash_set(color_table, (void *) value, found);
	}

	return found;
}

/**
 * etox_style_colors_init - setup some internal variables for colors
 *
 * Returns no value. Sets up necessary internal variables for color handling.
 */
void _etox_style_color_init()
{
	char *home;
	char user_path[PATH_MAX];

	sys_colors = e_db_open_read(PACKAGE_DATA_DIR "/colors.db");

	home = getenv("HOME");
	if (home) {
		snprintf(user_path, PATH_MAX, "%s/.e/etox/colors.db",
			 home);
      if (!access(user_path, R_OK))
   		sys_colors = e_db_open_read(user_path);
	}

	color_table = ecore_hash_new(NULL, NULL);
}

