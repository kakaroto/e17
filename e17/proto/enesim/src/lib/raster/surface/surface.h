#ifndef _SURFACE_H
#define _SURFACE_H

/**
 * @todo resize code (enesim_surface_resize())
 * @todo clean this file
 */

#define MATRIX_XX 0
#define MATRIX_XY 1
#define MATRIX_XZ 2
#define MATRIX_YX 3
#define MATRIX_YY 4
#define MATRIX_YZ 5
#define MATRIX_ZX 6
#define MATRIX_ZY 7
#define MATRIX_ZZ 8
#define MATRIX_SIZE 9

#define ENESIM_SURFACE_TRANSFORMATIONS 3

typedef enum
{
	ENESIM_SURFACE_TRANSFORMATION_IDENTITY = (1 << 0),
	ENESIM_SURFACE_TRANSFORMATION_AFFINE = (1 << 1),
	ENESIM_SURFACE_TRANSFORMATION_PROJECTIVE = (1 << 2),
} Enesim_Surface_Transformation_Type;

typedef enum
{
	ENESIM_SURFACE_NO_SCALE,
	ENESIM_SURFACE_SCALE_X,
	ENESIM_SURFACE_SCALE_Y,
	ENESIM_SURFACE_SCALE_ALL,
	ENESIM_SURFACE_SCALES,
} Enesim_Surface_Scale_Direction;

#if 0
/* Handle different qualities */
typedef enum
{
	ENESIM_SURFACE_QUALITY_SMOOTH,
	ENESIM_SURFACE_QUALITY_FAST,
	ENESIM_SURFACE_QUALITIES,
} Enesim_Surface_Quality;
#endif

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
typedef void (*Span_Pixel_Func) (Enesim_Surface_Data *data, int off, int w, Enesim_Surface_Data *src, int soff);

typedef struct _Surface_Func
{
	Span_Color_Func	sp_color;
	Span_Color_Mask_Func sp_color_mask;
	Span_Pixel_Func sp_pixel;
} Surface_Rop_Func;

typedef struct _Surface_Draw_Func
{
	
} Surface_Draw_Func;

typedef void (*Surface_Conv_Func) (Enesim_Surface_Data *sdata, Enesim_Surface_Data *ddata, int w, int h);

typedef struct _Surface_Backend
{
	Surface_Rop_Func rops[ENESIM_RENDERER_ROPS];
	Surface_Conv_Func conv[ENESIM_SURFACE_FORMATS - 1];
	Surface_Draw_Func draw[ENESIM_RENDERER_ROPS];
	void (*premul)(Enesim_Surface_Data *sdata, int len);
	void (*unpremul)(Enesim_Surface_Data *sdata, Enesim_Surface_Data *ddata, int len);
} Surface_Backend;

extern Surface_Backend argb8888_backend;
extern Surface_Backend rgb565_backend;

struct _Enesim_Surface
{
	int w;
	int h;
	Enesim_Surface_Format format;
	int flags;
	struct
	{
		float matrix[MATRIX_SIZE];
		Enesim_Surface_Transformation_Type type;
		int quality;
	} transformation;
	Enesim_Surface_Data 		data;
};

Span_Color_Func enesim_surface_span_color_func_get(Enesim_Surface *s, int rop);
Span_Color_Mask_Func enesim_surface_span_color_mask_func_get(Enesim_Surface *s, int rop);
Span_Pixel_Func enesim_surface_pixel_func_get(Enesim_Surface *s, Enesim_Surface *src, int rop);

void enesim_surface_premul(Enesim_Surface *s);

#endif
