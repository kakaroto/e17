#include "../estyle-config.h"
#include "Estyle_private.h"

static Ewd_Hash *color_table = NULL;
static E_DB_File *sys_colors = NULL;
static E_DB_File *user_colors = NULL;

static int __estyle_color_cmp(void *c1, void *c2);

/**
 * estyle_color_instance - get a pointer to a color structure
 * @r: the red value of the color
 * @g: the green value of the color
 * @b: the blue value of the color
 * @a: the alpha value of the color
 *
 * Returns a pointer to the color structure for use by contexts, NULL returned
 * on failure.
 */
Estyle_Color *estyle_color_instance(int r, int g, int b, int a)
{
	unsigned int value;
	Estyle_Color *ret;

	/*
	 * Check the hash table for a previous malloc of the color
	 */
	value = ESTYLE_HASH_COLOR(r, g, b, a);
	ret = (Estyle_Color *) ewd_hash_get(color_table, (void *) value);
	if (!ret) {
		ret = (Estyle_Color *) malloc(sizeof(Estyle_Color));
		ret->r = r;
		ret->g = g;
		ret->b = b;
		ret->a = a;
		ewd_hash_set(color_table, (void *) value, ret);
	}

	return ret;
}

/**
 * estyle_color_instance_db - get a pointer to a color structure by name
 * @color: the name of the color to retrieve from the colors dbs
 *
 * Returns a pointer to the color structure for use by contexts, NULL returned
 * on failure.
 */
Estyle_Color *estyle_color_instance_db(char *color)
{
	int r, g, b, a;
	unsigned int value;
	char key[PATH_MAX];
	Estyle_Color *found;

	CHECK_PARAM_POINTER_RETURN("color", color, NULL);

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

	value = ESTYLE_HASH_COLOR(r, g, b, a);

	/*
	 * Check for a previous allocation for this color.
	 */
	found = (Estyle_Color *) ewd_hash_get(color_table, (void *) value);
	if (!found) {
		found = (Estyle_Color *) malloc(sizeof(Estyle_Color));
		found->r = r;
		found->g = g;
		found->b = b;
		found->a = a;
		ewd_hash_set(color_table, (void *) value, found);
	}

	return found;
}

/**
 * estyle_colors_init - setup some internal variables for colors
 *
 * Returns no value. Sets up necessary internal variables for color handling.
 */
void estyle_color_init()
{
	char *home;
	char user_path[PATH_MAX];

	sys_colors = e_db_open_read(PACKAGE_DATA_DIR "colors.db");

	home = getenv("HOME");
	if (home) {
		snprintf(user_path, PATH_MAX, "%s/.e/estyle/colors.db",
			 home);
		sys_colors = e_db_open_read(user_path);
	}

	color_table = ewd_hash_new(NULL, __estyle_color_cmp);
}

/**
 * Comparison function for hashing colors.
 */
static int __estyle_color_cmp(void *c1, void *c2)
{
	Estyle_Color *col1 = (Estyle_Color *) c1;
	Estyle_Color *col2 = (Estyle_Color *) c2;

	/*
	 * Check for cases of NULL being passed in.
	 */
	if (c1 == c2)
		return 0;

	if (!c1 && c2)
		return -1;

	if (!c2 && c1)
		return 1;

	/*
	 * Check red colors first
	 */
	if (col1->r > col2->r)
		return -1;

	if (col1->r < col2->r)
		return 1;

	/*
	 * Check green colors next
	 */
	if (col1->g > col2->g)
		return -1;

	if (col1->g < col2->g)
		return 1;

	/*
	 * Check blue colors now
	 */
	if (col1->b > col2->b)
		return -1;

	if (col1->b < col2->b)
		return 1;

	/*
	 * Finally check alpha colors
	 */
	if (col1->a > col2->a)
		return -1;

	if (col1->a < col2->a)
		return 1;

	return 0;
}
