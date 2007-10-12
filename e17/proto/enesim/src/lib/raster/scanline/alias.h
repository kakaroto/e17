#ifndef _ALIAS_H
#define _ALIAS_H

/**
 * @file
 * @brief
 * @defgroup Scanline_Alias_Internal_Group Aliased Scanline
 * @ingroup Scanline_Internal_Group
 * @{
 */

/**
 * To be documented
 * FIXME: To be fixed
 */
typedef struct _Scanline_Alias_Sl
{
	int y;
	int x;
	int w;
} Scanline_Alias_Sl;

/**
 * To be documented
 * FIXME: To be fixed
 */
typedef struct _Scanline_Alias
{
	Edata_Array 		*a;
	Scanline_Alias_Sl	*sls;
	int 			num_sls;
} Scanline_Alias;

/**
 * To be documented
 * FIXME: To be fixed
 */
static inline int
scanline_alias_split(Scanline_Alias_Sl *sl, Scanline_Alias_Sl *rsl, int x)
{
	if ((sl->x <= x) && (sl->x + sl->w > x))
	{
		int x2;

		x2 = sl->x + sl->w;
		sl->w = x - sl->x;
		rsl->y = sl->y;
		rsl->x = x;
		rsl->w = x2 - x;
		return 1;
	}
	return 0;
}

/** @} */

#endif
