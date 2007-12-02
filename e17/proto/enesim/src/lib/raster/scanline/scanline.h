#ifndef _SCANLINE_H
#define _SCANLINE_H

/**
 * @file
 * @brief Scanline
 * @defgroup Scanline_Internal_Group Scanline
 * @ingroup Enesim_Internal_Group
 * @{
 * @todo
 * - Remove this interface
 * - Export all scanline implementations on Enesim.h so users can create
 *   their own scanlines outside enesim and call a renderer on them
 */

typedef void *(*Enesim_Scanline_Create)(void);
typedef void (*Enesim_Scanline_Add)(void *d, int y, int x0, int x1, int coverage);
typedef void (*Enesim_Scanline_Free)(void *d);

#define ENESIM_SCANLINE_FREE(func) ((Enesim_Scanline_Free)func)
#define ENESIM_SCANLINE_ADD(func) ((Enesim_Scanline_Add)func)

typedef struct _Enesim_Scanline_Func
{
	Enesim_Scanline_Add 	add;
	Enesim_Scanline_Free 	free;
} Enesim_Scanline_Func;

struct _Enesim_Scanline
{
	Enesim_Scanline_Func 	*funcs;
	void 			*data;
};

Enesim_Scanline * enesim_scanline_new(void);

/** @} */

#endif
