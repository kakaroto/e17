#include "Estyle_private.h"

static Ewd_Hash *fonts = NULL;

/**
 * estyle_font_instance - retrieve an instance of a font for use in an estyle
 * @name: the name of the font to retrieve
 *
 * Returns a pointer to a the font on success, NULL on failure.
 */
Estyle_Font *estyle_font_instance(char *name)
{
	Estyle_Font *fn;

	CHECK_PARAM_POINTER_RETURN("name", name, NULL);

	/*
	 * No fonts have been loaded at this point, so create the hash
	 * table for storing font info for later.
	 */
	if (!fonts)
		fonts = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	fn = ewd_hash_get(fonts, name);
	if (!fn) {
		fn = (Estyle_Font *)malloc(sizeof(Estyle_Font));
		fn->name = strdup(name);

		ewd_hash_set(fonts, name, fn);
	}

	return fn;
}
