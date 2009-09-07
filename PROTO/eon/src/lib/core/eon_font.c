/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Eon.h"
#include "eon_private.h"

#include <fontconfig.h>
#include <ft2build.h>
#include FT_FREETYPE_H

/* TODO create a cache for fonts */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Eon_Font
{
	const char *file;
	FT_Face face;
} Eon_Font;

static FT_Library library;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* TODO use fontconfig to retrieve the font mathcing the pattern, instead of
 * a file
 */
Eon_Font * eon_font_load(const char *name)
{
	Eon_Font *font;
	FcPattern *pat;
	FcFontSet *set;
	FcResult res;
	FT_Error err;
	FT_Face face;

	pat = FcNameParse((FcChar8 *)name);
	set = FcFontSort(NULL, pat, FcTrue, NULL, &res);
	if (!set)
		goto sort_failed;

	err = FT_New_Face(library, file, 0, &face);

	/* TODO the face loading can be done on a font engine */
	/* load the fonts */
	FcFontSetDestroy(set);

sort_failed:
	FcPatternDestroy(pat);
}

void eon_font_boundings_get(Eon_Font *f, const char *text, Eina_Rectangle *rect)
{

}

void eon_font_init(void)
{
	FT_Init_FreeType(&library);
}

void eon_font_shutdown(void)
{

}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
