#ifndef _RASTERIZER_H
#define _RASTERIZER_H

/**
 * @todo
 * - on vertex add, calculate the bounding box
 */


typedef struct _Enesim_Rasterizer_Func
{
	void *(*create)(void);
	void (*vertex_add)(void *r, float x, float y);
	void (*generate)(void *r, Enesim_Scanline *sl);
	/* TODO free */
} Enesim_Rasterizer_Func;

extern Enesim_Rasterizer_Func cpsc;
extern Enesim_Rasterizer_Func faap;

struct _Enesim_Rasterizer
{
	Enesim_Rectangle 	bound;
	Enesim_Rasterizer_Func 	*funcs;
	void 			*data;
};

#endif
