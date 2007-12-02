#ifndef _RENDERER_H
#define _RENDERER_H

/**
 *
 * @todo
 * - move this interface to a new one, remove the scanline ptr and use
 *   the type instead
 */

typedef struct _Enesim_Renderer_Func
{
	void (*draw)(Enesim_Renderer *r, int type, void *sl, Enesim_Surface *dst);
	void (*free)(Enesim_Renderer *r);
} Enesim_Renderer_Func;

struct _Enesim_Renderer
{
	int 				rop;
	void 				*data;
	Enesim_Renderer_Func 		*funcs;
};

Enesim_Renderer * enesim_renderer_new(void);

#endif
