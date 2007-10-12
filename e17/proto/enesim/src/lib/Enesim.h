#ifndef _ENESIM_H
#define _ENESIM_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/**
 * @mainpage Enesim
 * @section intro Introduction
 * Enesim is a library that ...
 * @image html vector.png
 * The raster sub-library is ...
 * @image html raster.png 
 * 
 * @file
 * @brief Enesim API
 * @defgroup Enesim_Group API
 * @{
 *
 * TODO normalize this errors
 *
 */
enum
{
	ENESIM_ERROR_NONE 	= 1,
	ENESIM_ERROR_INVAL,
	ENESIM_ERROR_IO,
	ENESIM_ERRORS
};
/**
 * @defgroup Enesim_Raster_Group Raster
 * @{
 */
typedef unsigned int 		DATA32;
typedef unsigned short int 	DATA16;
typedef unsigned char 		DATA8;
/**
 * @defgroup Enesim_Scanline_Group Scanline
 * @{
 */

typedef struct _Enesim_Scanline Enesim_Scanline; /**< Scanline Handler */
EAPI Enesim_Scanline 	*enesim_scanline_alias_new(void);
EAPI void 		enesim_scanline_delete(Enesim_Scanline *sl);

/** @} */ //End of Enesim_Scanline_Group

/**
 * @defgroup Enesim_Rasterizer_Group Rasterizer
 * @{
 */
typedef struct _Enesim_Rasterizer Enesim_Rasterizer; /**< Rasterizer Handler */
EAPI void enesim_rasterizer_vertex_add(Enesim_Rasterizer *r, float x, float y);
EAPI void enesim_rasterizer_generate(Enesim_Rasterizer *r, Enesim_Scanline *sl);
EAPI void enesim_rasterizer_boundings_set(Enesim_Rasterizer *r, int x, int y, int w, int h);
EAPI Enesim_Rasterizer * enesim_rasterizer_new(void);

/** @} */ //End of Enesim_Rasterizer_Group

/**
 * @defgroup Enesim_Surface_Group Surface
 * @{
 */
typedef struct _Enesim_Surface 	Enesim_Surface; /**< Surface Handler */
typedef enum
{
	ENESIM_SURFACE_ALPHA =	(1 << 0),
	ENESIM_SURFACE_DIRTY =	(1 << 1)
} Enesim_Surface_Flag;

typedef enum
{
	ENESIM_SURFACE_ARGB8888,
	ENESIM_SURFACE_RGB565,
	ENESIM_SURFACE_FORMATS,
} Enesim_Surface_Format;

EAPI Enesim_Surface * enesim_surface_new(Enesim_Surface_Format f, int w, int h, Enesim_Surface_Flag flags, ...);
EAPI void enesim_surface_size_get(Enesim_Surface *s, int *w, int *h);
EAPI void enesim_surface_size_set(Enesim_Surface *s, int w, int h);
EAPI void enesim_surface_data_get(Enesim_Surface *s, ...);
EAPI Enesim_Surface_Format enesim_surface_format_get(Enesim_Surface *s);
EAPI int enesim_surface_flag_get(Enesim_Surface *s);
EAPI void enesim_surface_flag_set(Enesim_Surface *s, Enesim_Surface_Flag flags);
EAPI void enesim_surface_data_set(Enesim_Surface *s, Enesim_Surface_Format f, ...);
//EAPI void enesim_surface_resize(Enesim_Surface *s, Enesim_Rectangle *srect, Enesim_Surface *d, Enesim_Rectangle *drect, int mode);

/** @} */ //End of Enesim_Surface_Group

/**
 * @defgroup Enesim_Renderer_Group Renderer
 * @{
 */
typedef struct _Enesim_Renderer Enesim_Renderer; /**< Renderer Handler */
typedef enum
{
	ENESIM_RENDERER_BLEND,
	ENESIM_RENDERER_ROPS
} Enesim_Renderer_Rop;
EAPI void enesim_renderer_rop_set(Enesim_Renderer *r, int rop);
EAPI void enesim_renderer_delete(Enesim_Renderer *r);
EAPI void enesim_renderer_draw(Enesim_Renderer *r, Enesim_Scanline *sl, Enesim_Surface *dst);
/**
 * @defgroup Enesim_Renderer_Fill_Color_Group Fill Color
 * @{
 */
EAPI Enesim_Renderer * enesim_fill_color_new(void);
EAPI void enesim_fill_color_color_set(Enesim_Renderer *r, DATA32 color);
/** @} */ //End of Enesim_Renderer_Fill_Color_Group
/**
 * @defgroup Enesim_Renderer_Fill_Surface_Group Fill Surface
 * @{
 */
enum
{
	ENESIM_SURFACE_REPEAT_NONE  = 0, /**< Don't repeat on any axis */
	ENESIM_SURFACE_REPEAT_X     = (1 << 0), /**< Repeat on X axis */
	ENESIM_SURFACE_REPEAT_Y     = (1 << 1), /**< Repeat on Y axis */
};
EAPI Enesim_Renderer * enesim_fill_surface_new(void);
EAPI void enesim_fill_surface_surface_set(Enesim_Renderer *r, Enesim_Surface *s);
EAPI void enesim_fill_surface_mode_set(Enesim_Renderer *r, int mode);
EAPI void enesim_fill_surface_area_set(Enesim_Renderer *r, int x, int y, int w, int h);
/** @} */ //End of Enesim_Renderer_Fill_Surface_Group
/** @} */ //End of Enesim_Renderer_Group
/** @} */ //End of Enesim_Raster_Group

/**
 * @defgroup Enesim_Vector_Group Vector
 * @{
 * @defgroup Enesim_Component_Group Component
 * @{
 */
typedef struct _Enesim_Component Enesim_Component; /**< Component Handler */
typedef struct _Enesim_Component_Reader Enesim_Component_Reader; /**< Reader Handler */
EAPI void enesim_component_delete(Enesim_Component *c);
EAPI int enesim_component_source_set(Enesim_Component *c, Enesim_Component *src);
enum
{
	ENESIM_CMD_MOVE_TO,
	ENESIM_CMD_LINE_TO,
	ENESIM_CMD_END,
	ENESIM_CMDS
};

/**
 * @defgroup Enesim_Component_Transform_Group Transform Component
 * @brief This Component applies a transformation matrix.
 * Reads vertex values from another component that is linked with the XXX
 * function and applies a transformation matrix to them.
 * @{
 */
EAPI Enesim_Component * enesim_transform_new(void);
EAPI void enesim_transform_matrix_set(Enesim_Component *c, float *matrix);
/** @} //End of Enesim_Component_Transform_Group */
/**
 * @defgroup Enesim_Component_Csv_Source_Group CSV Source Component
 * @brief This Component reads data from a csv (Comma Separated Values) file.
 * @{
 */
EAPI Enesim_Component * enesim_source_csv_new(void);
EAPI int enesim_source_csv_file_set(Enesim_Component *c, const char *path);
/** @} //End of Enesim_Component_Csv_Source_Group */

/**
 * @defgroup Enesim_Component_Path_Group Path Source Component
 * @{
 */
EAPI Enesim_Component * enesim_path_new(void);
EAPI void enesim_path_move_to(Enesim_Component *p, int x, int y);
EAPI void enesim_path_line_to(Enesim_Component *p, int x, int y);
EAPI void enesim_path_curve3(Enesim_Component *p, float x1, float y1, float x2,
	float y2, float x3, float y3);
EAPI void enesim_path_curve4(Enesim_Component *p, float x1, float y1, float x2,
	float y2, float x3, float y3, float x4, float y4);
EAPI void enesim_path_close(Enesim_Component *p);
/** @} //End of Enesim_Component_Path_Group */
/** @} //End of Enesim_Component_Group */
/** 
 * @defgroup Enesim_Component_Reader_Group Reader
 * @{
 */
EAPI Enesim_Component_Reader * enesim_reader_new(Enesim_Component *c);
EAPI void enesim_reader_rewind(Enesim_Component_Reader *r);
EAPI int enesim_reader_vertex_get(Enesim_Component_Reader *r, float *x, float *y);
/** @} */ //End of Enesim_Component_Reader_Group
/** @} */ //End of Enesim_Vector_Group
/** @} */ //End of Enesim_Group

#endif
