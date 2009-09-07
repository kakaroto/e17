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

#include <fontconfig/fontconfig.h>

/* TODO create a cache for fonts */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Eon_Font
{
	const char **file;
	/* TODO add more properties */
} Eon_Font;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* TODO use fontconfig to retrieve the font mathcing the pattern, instead of
 * a file
 */
Eon_Font * eon_font_load(const char *style)
{
	Eon_Font *font = NULL;
	FcPattern *pat;
	FcFontSet *set;
	FcResult res;

	pat = FcNameParse((FcChar8 *)style);
	set = FcFontSort(NULL, pat, FcTrue, NULL, &res);
	if (!set)
		goto sort_failed;

	font = malloc(sizeof(Eon_Font));
	/* TODO iterate over the set and get the real file name */

	FcFontSetDestroy(set);

sort_failed:
	FcPatternDestroy(pat);
	return font;
}

void eon_font_init(void)
{
}

void eon_font_shutdown(void)
{

}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
