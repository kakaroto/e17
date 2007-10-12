#ifndef _SURFACE_H
#define _SURFACE_H

/**
 *
 * @todo, resize code (enesim_surface_resize())
 */

typedef struct _Argb8888_Data
{
	DATA32	*data;
} Argb8888_Data;

typedef struct _Rgb565_Data
{
	DATA16	*data;
	DATA8 	*alpha;
} Rgb565_Data;

typedef union
{
	Rgb565_Data 	rgb565;	
	Argb8888_Data 	argb8888;	
} Enesim_Surface_Data;

typedef void (*Span_Color_Func) (Enesim_Surface_Data *data, int off, DATA32 c, int w);

typedef struct _Enesim_Surface_Func
{
	Span_Color_Func	sp_color;
} Enesim_Surface_Func;

extern Enesim_Surface_Func rgb565_funcs[ENESIM_RENDERER_ROPS];
extern Enesim_Surface_Func argb8888_funcs[ENESIM_RENDERER_ROPS];

struct _Enesim_Surface
{
	int 				w;
	int 				h;
	Enesim_Surface_Format		format;
	int 				flags;
	Enesim_Surface_Data 		data;
};

Span_Color_Func enesim_surface_span_color_get(Enesim_Surface *s, int rop);

#endif
