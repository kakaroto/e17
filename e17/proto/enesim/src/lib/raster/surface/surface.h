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

typedef union _Enesim_Surface_Data
{
	Rgb565_Data 	rgb565;	
	Argb8888_Data 	argb8888;	
} Enesim_Surface_Data;

typedef void (*Span_Color_Func) (Enesim_Surface_Data *data, int off, DATA32 c, int w);
typedef void (*Span_Color_Mask_Func) (Enesim_Surface_Data *data, int off, DATA32 c, int w, DATA8 *m);

typedef struct _Surface_Func
{
	Span_Color_Func	sp_color;
	Span_Color_Mask_Func sp_color_mask;
} Surface_Rop_Func;

typedef void (*Surface_Conv_Func) (Enesim_Surface_Data *sdata, Enesim_Surface_Data *ddata, int w, int h);

typedef struct _Surface_Backend
{
	Surface_Rop_Func rops[ENESIM_RENDERER_ROPS];
	Surface_Conv_Func conv[ENESIM_SURFACE_FORMATS - 1];
	void (*premul)(Enesim_Surface_Data *sdata, int len);
	void (*unpremul)(Enesim_Surface_Data *sdata, Enesim_Surface_Data *ddata, int len);
} Surface_Backend;

extern Surface_Backend argb8888_backend;
extern Surface_Backend rgb565_backend;

struct _Enesim_Surface
{
	int 				w;
	int 				h;
	Enesim_Surface_Format		format;
	int 				flags;
	Enesim_Surface_Data 		data;
};

Span_Color_Func enesim_surface_span_color_get(Enesim_Surface *s, int rop);
Span_Color_Mask_Func enesim_surface_span_color_mask_get(Enesim_Surface *s, int rop);

void enesim_surface_premul(Enesim_Surface *s);

#endif
