#include <stdlib.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "scanline.h"
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/
Enesim_Scanline * enesim_scanline_new(void)
{
	Enesim_Scanline *sl;

	sl = calloc(1, sizeof(Enesim_Scanline));
	return sl;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/

/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void enesim_scanline_delete(Enesim_Scanline *sl)
{
	sl->funcs->free(sl->data);
	free(sl);
}
