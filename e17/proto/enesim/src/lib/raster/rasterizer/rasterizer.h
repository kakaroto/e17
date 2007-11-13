#ifndef _RASTERIZER_H
#define _RASTERIZER_H

/**
 * @todo
 * - on vertex add, calculate the bounding box
 * - make each rasterizer to export what kind of scanlines it supports
 * - add a function/struct element to set up window boundaries i.e 
 * destination surface/clip
 * - add a function/struct element to set up the odd/even/zero fill rule
 * - im not sure about using a rectangle for the boundaries, maybe only
 * width and height?
 */

typedef void (*Enesim_Rasterizer_Vertex_Add)(void *r, float x, float y);
typedef void (*Enesim_Rasterizer_Generate)(void *r, Enesim_Scanline *sl);
typedef void (*Enesim_Rasterizer_Delete)(void *r);
typedef struct _Enesim_Rasterizer_Func
{
	Enesim_Rasterizer_Vertex_Add vertex_add;
	Enesim_Rasterizer_Generate generate;
	Enesim_Rasterizer_Delete delete;
	/* TODO free */
} Enesim_Rasterizer_Func;

#define ENESIM_RASTERIZER_VERTEX_ADD(func) ((Enesim_Rasterizer_Vertex_Add)func)
#define ENESIM_RASTERIZER_GENERATE(func) ((Enesim_Rasterizer_Generate)func)
#define ENESIM_RASTERIZER_DELETE(func) ((Enesim_Rasterizer_Delete)func)

struct _Enesim_Rasterizer
{
	Enesim_Rectangle 	boundaries;
	Enesim_Rasterizer_Func 	*funcs;
	void 			*data;
};

Enesim_Rasterizer * enesim_rasterizer_new(void *data, Enesim_Rasterizer_Func *func,
		Enesim_Rectangle boundaries);

#endif
