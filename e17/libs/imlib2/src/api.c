#include "config.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <string.h>
#include "common.h"
#include "colormod.h"
#include "scale.h"
#include "image.h"
#include "context.h"
#include "rgba.h"
#include "color.h"
#include "file.h"
#include "grab.h"
#include "blend.h"
#include "rend.h"
#include "draw.h"
#include "updates.h"
#include "ximage.h"
#include "rgbadraw.h"
#include "Imlib2.h"
#ifdef HAVE_FREETYPE_FREETYPE_H
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif
#include "font.h"
#include "grad.h"
#include "rotate.h"
#include <math.h>

/* convenience macros */
#define   CAST_IMAGE(im, image) (im) = (ImlibImage *)(image)
#define   CHECK_PARAM_POINTER_RETURN(func, sparam, param, ret) \
if (!(param)) \
{ \
  fprintf(stderr, "***** Imlib2 Developer Warning ***** :\n" \
                  "\tThis program is calling the Imlib call:\n\n" \
                  "\t%s();\n\n" \
                  "\tWith the parameter:\n\n" \
                  "\t%s\n\n" \
                  "\tbeing NULL. Please fix your program.\n", func, sparam); \
  return ret; \
}

#define   CHECK_PARAM_POINTER(func, sparam, param) \
if (!(param)) \
{ \
  fprintf(stderr, "***** Imlib2 Developer Warning ***** :\n" \
                  "\tThis program is calling the Imlib call:\n\n" \
                  "\t%s();\n\n" \
                  "\tWith the parameter:\n\n" \
                  "\t%s\n\n" \
                  "\tbeing NULL. Please fix your program.\n", func, sparam); \
  return; \
}

/* internal typedefs for function pointers */
typedef void (*Imlib_Internal_Progress_Function)(void *, char, 
						 int, int, int, int);
typedef void (*Imlib_Internal_Data_Destructor_Function)(void *, void *);

/* context - all operations use this context for their state */
static Display                *ctxt_display              = NULL;
static Visual                 *ctxt_visual               = NULL;
static Colormap                ctxt_colormap             = 0;
static int                     ctxt_depth                = 0;
static Drawable                ctxt_drawable             = 0;
static Pixmap                  ctxt_mask                 = 0;
static char                    ctxt_anti_alias           = 1;
static char                    ctxt_dither               = 0;
static char                    ctxt_blend                = 1;
static Imlib_Color_Modifier    ctxt_color_modifier       = NULL;
static Imlib_Operation         ctxt_operation            = IMLIB_OP_COPY;
static Imlib_Font              ctxt_font                 = NULL;
static Imlib_Text_Direction    ctxt_direction            = IMLIB_TEXT_TO_RIGHT;
static Imlib_Color             ctxt_color                = {255, 255, 255, 255};
static Imlib_Color_Range       ctxt_color_range          = NULL;
static Imlib_Image             ctxt_image                = NULL;
static Imlib_Progress_Function ctxt_progress_func        = NULL;
static char                    ctxt_progress_granularity = 0;
static char                    ctxt_dither_mask          = 0;

/* context setting/getting functions */
void
imlib_context_set_display(Display *display)
{
   ctxt_display = display;
}

Display *
imlib_context_get_display(void)
{
    return ctxt_display;
}

void
imlib_context_set_visual(Visual *visual)
{
   ctxt_visual = visual;
   ctxt_depth = imlib_get_visual_depth(ctxt_display, ctxt_visual);
}

Visual *
imlib_context_get_visual(void)
{
    return ctxt_visual;
}

void
imlib_context_set_colormap(Colormap colormap)
{
   ctxt_colormap = colormap;
}

Colormap
imlib_context_get_colormap(void)
{
    return ctxt_colormap;
}

void
imlib_context_set_drawable(Drawable drawable)
{
   ctxt_drawable = drawable;
}

Drawable
imlib_context_get_drawable(void)
{
    return ctxt_drawable;
}

void
imlib_context_set_mask(Pixmap mask)
{
   ctxt_mask = mask;
}

Pixmap
imlib_context_get_mask(void)
{
    return ctxt_mask;
}

void
imlib_context_set_dither_mask(char dither_mask)
{
   ctxt_dither_mask = dither_mask;
}

char
imlib_context_get_dither_mask(void)
{
    return ctxt_dither_mask;
}

void
imlib_context_set_anti_alias(char anti_alias)
{
   ctxt_anti_alias = anti_alias;
}

char
imlib_context_get_anti_alias(void)
{
    return ctxt_anti_alias;
}

void
imlib_context_set_dither(char dither)
{
   ctxt_dither = dither;
}

char
imlib_context_get_dither(void)
{
    return ctxt_dither;
}

void
imlib_context_set_blend(char blend)
{
   ctxt_blend = blend;
}

char
imlib_context_get_blend(void)
{
    return ctxt_blend;
}

void
imlib_context_set_color_modifier(Imlib_Color_Modifier color_modifier)
{
   ctxt_color_modifier = color_modifier;
}

Imlib_Color_Modifier
imlib_context_get_color_modifier(void)
{
    return ctxt_color_modifier;
}

void
imlib_context_set_operation(Imlib_Operation operation)
{
   ctxt_operation = operation;
}

Imlib_Operation
imlib_context_get_operation(void)
{
    return ctxt_operation;
}

void
imlib_context_set_font(Imlib_Font font)
{
   ctxt_font = font;
}

Imlib_Font
imlib_context_get_font(void)
{
    return ctxt_font;
}

void
imlib_context_set_direction(Imlib_Text_Direction direction)
{
   ctxt_direction = direction;
}

Imlib_Text_Direction
imlib_context_get_direction(void)
{
    return ctxt_direction;
}

void
imlib_context_set_color(int red, int green, int blue, int alpha)
{
   ctxt_color.red   = red;
   ctxt_color.green = green;
   ctxt_color.blue  = blue;
   ctxt_color.alpha = alpha;
}

void
imlib_context_get_color(int *red, int *green, int *blue, int *alpha)
{
    *red = ctxt_color.red;
    *green = ctxt_color.green;
    *blue = ctxt_color.blue;
    *alpha = ctxt_color.alpha;
}

Imlib_Color *
imlib_context_get_imlib_color(void)
{
    return &ctxt_color;
}

void
imlib_context_set_color_range(Imlib_Color_Range color_range)
{
   ctxt_color_range = color_range;
}

Imlib_Color_Range
imlib_context_get_color_range(void)
{
    return ctxt_color_range;
}

void
imlib_context_set_progress_function(Imlib_Progress_Function progress_function)
{
   ctxt_progress_func = progress_function;
}

Imlib_Progress_Function
imlib_context_get_progress_function(void)
{
    return ctxt_progress_func;
}

void
imlib_context_set_progress_granularity(char progress_granularity)
{
   ctxt_progress_granularity = progress_granularity;
}

char
imlib_context_get_progress_granularity(void)
{
    return ctxt_progress_granularity;
}

void
imlib_context_set_image(Imlib_Image image)
{
   ctxt_image = image;
}

Imlib_Image
imlib_context_get_image(void)
{
    return ctxt_image;
}

/* imlib api */
int 
imlib_get_cache_size(void)
{
   return __imlib_GetCacheSize();
}

void 
imlib_set_cache_size(int bytes)
{
   __imlib_SetCacheSize(bytes);
}

int
imlib_get_color_usage(void)
{
   return (int)_max_colors;
}

void 
imlib_set_color_usage(int max)
{
   if (max < 2)
      max = 2;
   else if (max > 256)
      max = 256;
   _max_colors = max;
}

void
imlib_flush_loaders(void)
{
   __imlib_RemoveAllLoaders();
   LTDL_Exit();
}

int 
imlib_get_visual_depth(Display *display, Visual *visual)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_visual_depth", "display", display, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_get_visual_depth", "visual", visual, 0);
   return __imlib_XActualDepth(display, visual);
}

Visual *
imlib_get_best_visual(Display *display, int screen, int *depth_return)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_best_visual", "display", display, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_get_best_visual", "depth_return", depth_return, NULL);
   return __imlib_BestVisual(display, screen, depth_return);
}

Imlib_Image 
imlib_load_image(const char *file)
{
   Imlib_Image im = NULL;
   Imlib_Image prev_ctxt_image;
   
   CHECK_PARAM_POINTER_RETURN("imlib_load_image", "file", file, NULL);
   prev_ctxt_image = ctxt_image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction)ctxt_progress_func,
			  ctxt_progress_granularity, 0, 0, NULL);
   ctxt_image = prev_ctxt_image;
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_load_image_immediately(const char *file)
{
   Imlib_Image im = NULL;
   Imlib_Image prev_ctxt_image;
   
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_immediately", "file", file, NULL);
   prev_ctxt_image = ctxt_image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction)ctxt_progress_func,
			  ctxt_progress_granularity, 1, 0, NULL);
   ctxt_image = prev_ctxt_image;
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_load_image_without_cache(const char *file)
{
   Imlib_Image im = NULL;
   Imlib_Image prev_ctxt_image;
   
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_without_cache", "file", file, NULL);
   prev_ctxt_image = ctxt_image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction)ctxt_progress_func,
			  ctxt_progress_granularity, 0, 1, NULL);
   ctxt_image = prev_ctxt_image;
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_load_image_immediately_without_cache(const char *file)
{
   Imlib_Image im = NULL;
   Imlib_Image prev_ctxt_image;
   
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_immediately_without_cache", "file", file, NULL);
   prev_ctxt_image = ctxt_image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction)ctxt_progress_func,
			  ctxt_progress_granularity, 1, 1, NULL);
   ctxt_image = prev_ctxt_image;
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_load_image_with_error_return(const char *file, Imlib_Load_Error *error_return)
{
   Imlib_Image im = NULL;
   ImlibLoadError er;
   Imlib_Image prev_ctxt_image;
   
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_with_error_return", "file", file, NULL);
   if (!__imlib_FileExists(file))
     {
	*error_return = IMLIB_LOAD_ERROR_FILE_DOES_NOT_EXIST;
	return NULL;
     }
   if (__imlib_FileIsDir(file))
     {
	*error_return = IMLIB_LOAD_ERROR_FILE_IS_DIRECTORY;
	return NULL;
     }
   if (!__imlib_FileCanRead(file))
     {
	*error_return = IMLIB_LOAD_ERROR_PERMISSION_DENIED_TO_READ;
	return NULL;
     }
   prev_ctxt_image = ctxt_image;
   im = (Imlib_Image)__imlib_LoadImage(file, (ImlibProgressFunction)ctxt_progress_func, 
				       ctxt_progress_granularity, 1, 0, &er);
   ctxt_image = prev_ctxt_image;
   if (im)
      *error_return = IMLIB_LOAD_ERROR_NONE;
   else
     {
	if (er == IMLIB_LOAD_ERROR_NONE)
	   *error_return = IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT;
	else
           *error_return = (Imlib_Load_Error)er;	   
     }
   return im;
}

void 
imlib_free_image(void)
{
   CHECK_PARAM_POINTER("imlib_free_image", "image", ctxt_image);
   __imlib_FreeImage(ctxt_image);
   ctxt_image = NULL;
}

void 
imlib_free_image_and_decache(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_free_image_and_decache", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   SET_FLAG(im->flags, F_INVALID);
   __imlib_FreeImage(im);
   ctxt_image = NULL;
}

int 
imlib_image_get_width(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_width", "image", ctxt_image, 0);
   CAST_IMAGE(im, ctxt_image);
   return im->w;
}

int 
imlib_image_get_height(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_height", "image", ctxt_image, 0);
   CAST_IMAGE(im, ctxt_image);
   return im->h;
}

DATA32 *
imlib_image_get_data(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_data", "image", ctxt_image, NULL);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return NULL;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   return im->data;
}

DATA32 *
imlib_image_get_data_for_reading_only(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_data_for_reading_only", "image", ctxt_image, NULL);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return NULL;
   return im->data;
}

void 
imlib_image_put_back_data(DATA32 *data)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_put_back_data", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_put_back_data", "data", data);
   CAST_IMAGE(im, ctxt_image);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   data = NULL;
}

char 
imlib_image_has_alpha(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_has_alpha", "image", ctxt_image, 0);
   CAST_IMAGE(im, ctxt_image);
   if (IMAGE_HAS_ALPHA(im))
      return 1;
   return 0;       
}

void 
imlib_image_set_never_changes_on_disk(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_never_changes_on_disk", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   UNSET_FLAG(im->flags, F_ALWAYS_CHECK_DISK); 
}

void 
imlib_image_get_border(Imlib_Border *border)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_get_border", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_get_border", "border", border);
   CAST_IMAGE(im, ctxt_image);
   border->left = im->border.left;
   border->right = im->border.right;
   border->top = im->border.top;
   border->bottom = im->border.bottom;
}

void 
imlib_image_set_border(Imlib_Border *border)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_border", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_set_border", "border", border);
   CAST_IMAGE(im, ctxt_image);
   if ((im->border.left == border->left) &&
       (im->border.right == border->right) &&
       (im->border.top == border->top) &&
       (im->border.bottom == border->bottom))
      return;
   im->border.left = border->left;
   im->border.right = border->right;
   im->border.top = border->top;
   im->border.bottom = border->bottom;
   __imlib_DirtyPixmapsForImage(im);
}

void 
imlib_image_set_format(const char *format)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_format", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_set_format", "format", format);
   CAST_IMAGE(im, ctxt_image);
   if (im->format)
      free(im->format);
   im->format = strdup(format);
   if (!(im->flags & F_FORMAT_IRRELEVANT))
     {
	__imlib_DirtyImage(im);
	__imlib_DirtyPixmapsForImage(im);
     }
}

void
imlib_image_set_irrelevant_format(char irrelevant)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_format", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if (irrelevant)
     {
	SET_FLAG(im->flags, F_FORMAT_IRRELEVANT); 
     }
   else
     {
	UNSET_FLAG(im->flags, F_FORMAT_IRRELEVANT); 
     }
}

void
imlib_image_set_irrelevant_border(char irrelevant)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_border", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if (irrelevant)
     {
	SET_FLAG(im->flags, F_BORDER_IRRELEVANT); 
     }
   else
     {
	UNSET_FLAG(im->flags, F_BORDER_IRRELEVANT); 
     }
}

void
imlib_image_set_irrelevant_alpha(char irrelevant)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_alpha", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if (irrelevant)
     {
	SET_FLAG(im->flags, F_ALPHA_IRRELEVANT); 
     }
   else
     {
	UNSET_FLAG(im->flags, F_ALPHA_IRRELEVANT); 
     }
}

char *
imlib_image_format(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_format", "image", ctxt_image, NULL);
   CAST_IMAGE(im, ctxt_image);
   return im->format;
}

void 
imlib_image_set_has_alpha(char has_alpha)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_set_has_alpha", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if (has_alpha)
      SET_FLAG(im->flags, F_HAS_ALPHA);
   else      
      UNSET_FLAG(im->flags, F_HAS_ALPHA);
}

void 
imlib_render_pixmaps_for_whole_image(Pixmap *pixmap_return,
				     Pixmap *mask_return,
				     char create_dithered_mask)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "pixmap_return", pixmap_return);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "mask_return", mask_return);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_CreatePixmapsForImage(ctxt_display, ctxt_drawable, ctxt_visual,
				 ctxt_depth, ctxt_colormap, im, pixmap_return, 
				 mask_return, 0, 0, im->w, im->h, im->w, im->h,
				 0, ctxt_dither, create_dithered_mask,
				 ctxt_color_modifier);
}

void 
imlib_render_pixmaps_for_whole_image_at_size(Pixmap *pixmap_return,
					     Pixmap *mask_return,
					     char create_dithered_mask,
					     int width, int height)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "pixmap_return", pixmap_return);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "mask_return", mask_return);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_CreatePixmapsForImage(ctxt_display, ctxt_drawable, ctxt_visual, 
				 ctxt_depth, ctxt_colormap,  im, 
				 pixmap_return, mask_return, 0, 0, im->w, 
				 im->h, width, height, ctxt_anti_alias,
				 ctxt_dither, create_dithered_mask,
				 ctxt_color_modifier);
}

void
imlib_free_pixmap_and_mask(Pixmap pixmap)
{
   __imlib_FreePixmap(ctxt_display, pixmap);
}

void 
imlib_render_image_on_drawable(int x, int y)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(ctxt_display, im, ctxt_drawable, ctxt_mask, ctxt_visual, 
		       ctxt_colormap, ctxt_depth, 0, 0, im->w, im->h, x, y, 
		       im->w, im->h, 0, ctxt_dither, ctxt_blend, ctxt_dither_mask, 
		       ctxt_color_modifier, ctxt_operation);
}

void
imlib_render_image_on_drawable_at_size(int x, int y, int width, int height)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(ctxt_display, im, ctxt_drawable, ctxt_mask, ctxt_visual, 
		       ctxt_colormap, ctxt_depth, 0, 0, im->w, im->h, x, y, 
		       width, height, ctxt_anti_alias, ctxt_dither,
		       ctxt_blend, ctxt_dither_mask, ctxt_color_modifier, ctxt_operation);
}

void 
imlib_render_image_part_on_drawable_at_size(int source_x, int source_y,
					    int source_width, int source_height,
					    int x, int y, int width, int height)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(ctxt_display, im, ctxt_drawable, 0, ctxt_visual, 
		       ctxt_colormap, ctxt_depth, source_x, source_y, 
		       source_width, source_height, x, y, width, height,
		       ctxt_anti_alias, ctxt_dither, ctxt_blend, 0, 
		       ctxt_color_modifier, ctxt_operation);
}

void 
imlib_blend_image_onto_image(Imlib_Image source_image,
			     char merge_alpha,
			     int source_x, int source_y,
			     int source_width, int source_height,
			     int destination_x, int destination_y,
			     int destination_width, int destination_height)
{
   ImlibImage *im_src, *im_dst;
   
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image", "source_image", source_image);
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image", "image", ctxt_image);
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, ctxt_image);
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   __imlib_BlendImageToImage(im_src, im_dst, ctxt_anti_alias, ctxt_blend, 
			     merge_alpha, source_x, source_y, source_width, 
			     source_height, destination_x, destination_y, 
			     destination_width, destination_height,
			     ctxt_color_modifier, ctxt_operation);
}

Imlib_Image 
imlib_create_image(int width, int height)
{
   DATA32 *data;
   
   if ((width <= 0) || (height <= 0))
      return NULL;
   data = malloc(width * height * sizeof(DATA32));
   if (data)
      return (Imlib_Image)__imlib_CreateImage(width, height, data);
   return NULL;
}

Imlib_Image 
imlib_create_image_using_data(int width, int height,
			      DATA32 *data)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_create_image_using_data", "data", data, NULL);
   if ((width <= 0) || (height <= 0))
      return NULL;
   im = __imlib_CreateImage(width, height, data);
   if (im)
      SET_FLAG(im->flags, F_DONT_FREE_DATA);
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_image_using_copied_data(int width, int height,
				     DATA32 *data)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_using_copied_data", "data", data, NULL);
   if ((width <= 0) || (height <= 0))
      return NULL;
   im = __imlib_CreateImage(width, height, NULL);
   if (!im)
      return NULL;
   im->data = malloc(width * height *sizeof(DATA32));
   if (data)
     {
	memcpy(im->data, data, width * height *sizeof(DATA32));
	return (Imlib_Image)im;
     }
   else
      __imlib_FreeImage(im);
   return NULL;
}

Imlib_Image 
imlib_create_image_from_drawable(Pixmap mask,
				 int x, int y,
				 int width, int height, char need_to_grab_x)
{
   ImlibImage *im;
   char domask = 0;
   
   if (mask)
      domask = 1;
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height * sizeof(DATA32));
   __imlib_GrabDrawableToRGBA(im->data, 0, 0, width, height, ctxt_display, 
			      ctxt_drawable, mask, ctxt_visual, ctxt_colormap,
			      ctxt_depth, x, y, width, height, domask, 
			      need_to_grab_x);
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_scaled_image_from_drawable(Pixmap mask,
					int source_x, int source_y,
					int source_width, int source_height,
					int destination_width, int destination_height,
					char need_to_grab_x,
					char get_mask_from_shape)
{
   ImlibImage *im;
   char        domask = 0, tmpmask = 0;
   int         x, xx;
   XGCValues   gcv;
   GC          gc = 0, mgc = 0;
   Pixmap      p, m;
   
   if ((mask) || (get_mask_from_shape))
      domask = 1;
   p = XCreatePixmap(ctxt_display, ctxt_drawable, destination_width, 
		     source_height, ctxt_depth);
   gcv.foreground = 0;
   gcv.subwindow_mode = IncludeInferiors;
   if (domask)
     {
	m = XCreatePixmap(ctxt_display, ctxt_drawable, destination_width, 
			  source_height, 1);
	mgc = XCreateGC(ctxt_display, m, GCForeground, &gcv);
     }
   else
      m = None;
   gc = XCreateGC(ctxt_display, ctxt_drawable, GCSubwindowMode, &gcv);
   if ((domask) && (!mask))
     {
	XRectangle *rect;
	int         rect_num, rect_ord;
	
	tmpmask = 1;
	mask = XCreatePixmap(ctxt_display, ctxt_drawable, source_width, 
			     source_height, 1);
	rect = XShapeGetRectangles(ctxt_display, ctxt_drawable, ShapeBounding, 
				   &rect_num, &rect_ord);
	XFillRectangle(ctxt_display, mask, mgc, 0, 0, source_width, 
		       source_height);
	if (rect)
	  {
	     XSetForeground(ctxt_display, mgc, 1);
	     for (x = 0; x < rect_num; x++)
		XFillRectangle(ctxt_display, mask, mgc, rect[x].x, rect[x].y, 
			       rect[x].width, rect[x].height);
	     XFree(rect);
	  }
	/* build mask from window shape rects */
     }
   for (x = 0; x < destination_width; x++)
     {
	xx = (source_width * x) / destination_width;
	XCopyArea(ctxt_display, ctxt_drawable, p, gc, source_x + xx, 0, 1, 
		  source_height, xx, 0);
	if (m != None)
	   XCopyArea(ctxt_display, mask, m, mgc, xx, 0, 1, source_height,
		     xx, 0);	   
     }
   for (x = 0; x < destination_height; x++)
     {
	xx = (source_height * x) / destination_height;
	XCopyArea(ctxt_display, ctxt_drawable, p, gc, 0, source_y + xx, 
		  destination_width, 1, 0, xx);
	if (m != None)
	   XCopyArea(ctxt_display, mask, m, mgc, 0, source_y + xx, 
		     destination_width, 1, 0, xx);
     }
   im = __imlib_CreateImage(destination_width, destination_height, NULL);
   im->data = malloc(destination_width * destination_height * sizeof(DATA32));
   __imlib_GrabDrawableToRGBA(im->data, 0, 0, destination_width, 
			      destination_height, ctxt_display, p, m, 
			      ctxt_visual, ctxt_colormap, ctxt_depth, 0, 0, 
			      source_width, source_height, domask, 
			      need_to_grab_x);
   XFreePixmap(ctxt_display, p);
   if (m != None)
     {
	XFreeGC(ctxt_display, mgc);
	XFreePixmap(ctxt_display, m);
	if (tmpmask)
	   XFreePixmap(ctxt_display, mask);
     }      
   XFreeGC(ctxt_display, gc);
   return (Imlib_Image)im;
}

char
imlib_copy_drawable_to_image(Pixmap mask, int x, int y,
			     int width, int height,
			     int destination_x, int destination_y,
			     char need_to_grab_x)
{
   ImlibImage *im;
   char domask = 0;
   int pre_adj;
   
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "image", ctxt_image, 0);
   if (mask)
      domask = 1;   
   CAST_IMAGE(im, ctxt_image);

   pre_adj = 0;
   if (x < 0)
     {
	width += x;
	pre_adj = x;
	x = 0;
     }
   if (width < 0)
      width = 0;
   if (destination_x < 0)
     {
	width += destination_x;
	x -= destination_x - pre_adj;
	destination_x = 0;
     }
   if ((destination_x + width) >= im->w)
      width = im->w - destination_x;

   pre_adj = 0;
   if (y < 0)
     {
	height += y;
	pre_adj = y;
	y = 0;
     }
   if (height < 0)
      height = 0;
   if (destination_y < 0)
     {
	height += destination_y;
	y -= destination_y - pre_adj;
	destination_y = 0;
     }
   if ((destination_y + height) >= im->h)
      height = im->h - destination_y;

   if ((width <= 0) || (height <= 0))
      return 0;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   return __imlib_GrabDrawableToRGBA(im->data, destination_x, destination_y,
				     im->w, im->h, ctxt_display, 
				     ctxt_drawable, mask, ctxt_visual, 
				     ctxt_colormap, ctxt_depth, 
				     x, y, width, height,
				     domask, need_to_grab_x);   
}

Imlib_Image 
imlib_clone_image(void)
{
   ImlibImage *im, *im_old;

   CHECK_PARAM_POINTER_RETURN("imlib_clone_image", "image", ctxt_image, NULL);
   CAST_IMAGE(im_old, ctxt_image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   im = __imlib_CreateImage(im_old->w, im_old->h, NULL);
   if (!(im))
      return NULL;
   im->data = malloc(im->w * im->h *sizeof(DATA32));
   if (!(im->data))
     {
	__imlib_FreeImage(im);
	return NULL;
     }
   memcpy(im->data, im_old->data, im->w * im->h *sizeof(DATA32));
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_cropped_image(int x, int y, int width, int height)
{
   ImlibImage *im, *im_old;

   CHECK_PARAM_POINTER_RETURN("imlib_create_cropped_image", "image", ctxt_image, NULL);
   CAST_IMAGE(im_old, ctxt_image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height *sizeof(DATA32));
   if (!(im->data))
     {
	__imlib_FreeImage(im);
	return NULL;
     }
   __imlib_BlendImageToImage(im_old, im, 0, 0, 0,
			     x, y, width, height,
			     0, 0, width, height, NULL, IMLIB_OP_COPY);
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_cropped_scaled_image(int source_x, int source_y, int source_width,
				  int source_height, int destination_width,
				  int destination_height)
{
   ImlibImage *im, *im_old;
   
   CHECK_PARAM_POINTER_RETURN("imlib_create_cropped_scaled_image", "image", ctxt_image, NULL);
   CAST_IMAGE(im_old, ctxt_image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   im = __imlib_CreateImage(destination_width, destination_height, NULL);
   im->data = malloc(destination_width * destination_height *sizeof(DATA32));
   if (!(im->data))
     {
	__imlib_FreeImage(im);
	return NULL;
     }
   if (IMAGE_HAS_ALPHA(im_old))
     {
	SET_FLAG(im->flags, F_HAS_ALPHA);
	__imlib_BlendImageToImage(im_old, im, ctxt_anti_alias, 0, 1,
				  source_x, source_y, source_width, source_height,
				  0, 0, destination_width, destination_height,
				  NULL, IMLIB_OP_COPY);
     }
   else
     {
	__imlib_BlendImageToImage(im_old, im, ctxt_anti_alias, 0, 0,
				  source_x, source_y, source_width, source_height,
				  0, 0, destination_width, destination_height,
				  NULL, IMLIB_OP_COPY);
     }
   return (Imlib_Image)im;
}

Imlib_Updates
imlib_updates_clone(Imlib_Updates updates)
{
   ImlibUpdate *u;
   
   u = (ImlibUpdate *)updates;
   return (Imlib_Updates)__imlib_DupUpdates(u);   
}

Imlib_Updates 
imlib_update_append_rect(Imlib_Updates updates, int x, int y, int w, int h)
{
   ImlibUpdate *u;
   
   u = (ImlibUpdate *)updates;
   return (Imlib_Updates)__imlib_AddUpdate(u, x, y, w, h);
}

Imlib_Updates 
imlib_updates_merge(Imlib_Updates updates, int w, int h)
{
   ImlibUpdate *u;
   
   u = (ImlibUpdate *)updates;
   return (Imlib_Updates)__imlib_MergeUpdate(u, w, h, 0);
}

Imlib_Updates 
imlib_updates_merge_for_rendering(Imlib_Updates updates, int w, int h)
{
   ImlibUpdate *u;
   
   u = (ImlibUpdate *)updates;
   return (Imlib_Updates)__imlib_MergeUpdate(u, w, h, 3);
}

void 
imlib_updates_free(Imlib_Updates updates)
{
   ImlibUpdate *u;
   
   u = (ImlibUpdate *)updates;
   __imlib_FreeUpdates(u);
}

Imlib_Updates
imlib_updates_get_next(Imlib_Updates updates)
{
   ImlibUpdate *u;
   
   u = (ImlibUpdate *)updates;
   return (Imlib_Updates)(u->next);
}


void 
imlib_updates_get_coordinates(Imlib_Updates updates,
			      int *x_return, int *y_return,
			      int *width_return, int *height_return)
{
   ImlibUpdate *u;
   
   CHECK_PARAM_POINTER("imlib_updates_get_coordinates", "updates", updates);
   u = (ImlibUpdate *)updates;
   if (x_return)
      *x_return = u->x;
   if (y_return)
      *y_return = u->y;
   if (width_return)
      *width_return = u->w;
   if (height_return)
      *height_return = u->h;
}

void 
imlib_updates_set_coordinates(Imlib_Updates updates, int x, int y, 
			      int width, int height)
{
   ImlibUpdate *u;
   
   CHECK_PARAM_POINTER("imlib_updates_set_coordinates", "updates", updates);
   u = (ImlibUpdate *)updates;
   u->x = x;
   u->y = y;
   u->w = width;
   u->h = height;
}

void 
imlib_render_image_updates_on_drawable(Imlib_Updates updates,
				       int x, int y)
{
   ImlibUpdate *u;
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   u = (ImlibUpdate *)updates;
   if (!updates)
      return;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_SetMaxXImageCount(ctxt_display, 10);
   for (; u; u = u->next)
     {
	__imlib_RenderImage(ctxt_display, im, ctxt_drawable, 0, ctxt_visual, 
			    ctxt_colormap, ctxt_depth, u->x, u->y, u->w, u->h,
			    x + u->x, y + u->y, u->w, u->h, 0, ctxt_dither,
			    0, 0, ctxt_color_modifier, OP_COPY);
     }
   __imlib_SetMaxXImageCount(ctxt_display, 0);
}

Imlib_Updates 
imlib_updates_init(void)
{
   return (Imlib_Updates)NULL;
}

Imlib_Updates 
imlib_updates_append_updates(Imlib_Updates updates,
			     Imlib_Updates appended_updates)
{
   ImlibUpdate *u, *uu;

   u = (ImlibUpdate *)updates;
   uu = (ImlibUpdate *)appended_updates;
   if (!uu)
      return (Imlib_Updates)u;
   if (!u)
      return (Imlib_Updates)uu;
   while (u)
     {
	if (!(u->next))
	  {
	     u->next = uu;
	     return updates;
	  }
	u = u->next;
     }
   return (Imlib_Updates)u;
}

void 
imlib_image_flip_horizontal(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_flip_horizontal", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FlipImageHoriz(im);
}

void 
imlib_image_flip_vertical(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_flip_vertical", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FlipImageVert(im);
}

void 
imlib_image_flip_diagonal(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_flip_diagonal", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FlipImageDiagonal(im, 0);
}

void
imlib_image_orientate(int orientation)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_orientate", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   switch (orientation) {
   default:
   case 0:
      break;
   case 1:
      __imlib_FlipImageDiagonal(im, 1);
      break;
   case 2:
      __imlib_FlipImageBoth(im);
      break;
   case 3:
      __imlib_FlipImageDiagonal(im, 2);
      break;
   case 4:
      __imlib_FlipImageHoriz(im);
      break;
   case 5:
      __imlib_FlipImageDiagonal(im, 3);
      break;
   case 6:
      __imlib_FlipImageVert(im);
      break;
   case 7:
      __imlib_FlipImageDiagonal(im, 0);
      break;
   }
}

void 
imlib_image_blur(int radius)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_blur", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_BlurImage(im, radius);
}

void 
imlib_image_sharpen(int radius)
{
   ImlibImage *im;

   CAST_IMAGE(im, ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_sharpen", "image", ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_SharpenImage(im, radius);
}

void 
imlib_image_tile_horizontal(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_tile_horizontal", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_TileImageHoriz(im);
}

void 
imlib_image_tile_vertical(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_tile_vertical", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_TileImageVert(im);
}

void 
imlib_image_tile(void)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_tile", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_TileImageHoriz(im);
   __imlib_TileImageVert(im);
}

Imlib_Font
imlib_load_font(const char *font_name)
{
   return (Imlib_Font)__imlib_load_font(font_name);   
}

void 
imlib_free_font(void)
{
   CHECK_PARAM_POINTER("imlib_free_font", "font", ctxt_font);
   __imlib_free_font(ctxt_font);
   ctxt_font = NULL;
}

void 
imlib_text_draw(int x, int y, const char *text)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_text_draw", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_text_draw", "text", text);
   CAST_IMAGE(im, ctxt_image);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_render_str(im, ctxt_font, x, y, text, (DATA8)ctxt_color.red, 
		      (DATA8)ctxt_color.green, (DATA8)ctxt_color.blue, 
		      (DATA8)ctxt_color.alpha, (char)ctxt_direction, 
		      NULL, NULL, 0, NULL, NULL,
		      ctxt_operation);
}

void 
imlib_text_draw_with_return_metrics(int x, int y, const char *text,
				    int *width_return, int *height_return,
				    int *horizontal_advance_return,
				    int *vertical_advance_return)
{
   ImlibImage *im;
   ImlibFont *fn;
   
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "font", ctxt_font);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "text", text);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   fn = (ImlibFont *)ctxt_font;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_render_str(im, fn, x, y, text, (DATA8)ctxt_color.red, 
		      (DATA8)ctxt_color.green, (DATA8)ctxt_color.blue, 
		      (DATA8)ctxt_color.alpha, (char)ctxt_direction, 
		      width_return, height_return, 0, 
		      horizontal_advance_return, vertical_advance_return,
		      ctxt_operation);
}

void 
imlib_get_text_size(const char *text, int *width_return, int *height_return)
{
   ImlibFont *fn;
   int w, h;
   
   CHECK_PARAM_POINTER("imlib_get_text_size", "font", ctxt_font);
   CHECK_PARAM_POINTER("imlib_get_text_size", "text", text);
   fn = (ImlibFont *)ctxt_font;
   __imlib_calc_size(fn, &w, &h, text);
   switch(ctxt_direction)
     {
     case IMLIB_TEXT_TO_RIGHT:
     case IMLIB_TEXT_TO_LEFT:
	if (width_return)
	   *width_return = w;
	if (height_return)
	   *height_return = h;
	break;
     case IMLIB_TEXT_TO_DOWN:
     case IMLIB_TEXT_TO_UP:
	if (width_return)
	   *width_return = h;
	if (height_return)
	   *height_return = w;
	break;
     default:
	break;
     }
}

void 
imlib_add_path_to_font_path(const char *path)
{
   CHECK_PARAM_POINTER("imlib_add_path_to_font_path", "path", path);
   __imlib_add_font_path(path);
}

void 
imlib_remove_path_from_font_path(const char *path)
{
   CHECK_PARAM_POINTER("imlib_remove_path_from_font_path", "path", path);
   __imlib_del_font_path(path);
}

char **
imlib_list_font_path(int *number_return)
{
   CHECK_PARAM_POINTER_RETURN("imlib_list_font_path", "number_return", number_return, NULL);
   return __imlib_list_font_path(number_return);
}

int
imlib_text_get_index_and_location(const char *text, int x, int y,
				  int *char_x_return, int *char_y_return,
				  int *char_width_return,
				  int *char_height_return)
{
   ImlibFont *fn;
   int w, h, cx, cy, cw, ch, cp, xx, yy;
   
   CHECK_PARAM_POINTER_RETURN("imlib_text_get_index_and_location", "font", ctxt_font, -1);
   CHECK_PARAM_POINTER_RETURN("imlib_text_get_index_and_location", "text", text, -1);
   fn = (ImlibFont *)ctxt_font;
   switch(ctxt_direction)
     {
     case IMLIB_TEXT_TO_RIGHT:
	return __imlib_char_pos(fn, text, x, y, char_x_return, char_y_return, 
				char_width_return, char_height_return);
	break;
     case IMLIB_TEXT_TO_LEFT:
	__imlib_calc_size(fn, &w, &h, text);
	xx = w - x;
	yy = h - y;
	cp = __imlib_char_pos(fn, text, xx, yy, &cx, &cy, &cw, &ch);
	cx = 1 + w - cx - cw;
	if (char_x_return)
	   *char_x_return = cx;
	if (char_y_return)
	   *char_y_return = cy;
	if (char_width_return)
	   *char_width_return = cw;
	if (char_height_return)
	   *char_height_return = ch;
	return cp;
	break;
     case IMLIB_TEXT_TO_DOWN:
	__imlib_calc_size(fn, &w, &h, text);
	xx = h - y;
	yy = x;
	cp = __imlib_char_pos(fn, text, xx, yy, &cx, &cy, &cw, &ch);
	if (char_x_return)
	   *char_x_return = cy;
	if (char_y_return)
	   *char_y_return = cx;
	if (char_width_return)
	   *char_width_return = ch;
	if (char_height_return)
	   *char_height_return = cw;
	return cp;
	break;
     case IMLIB_TEXT_TO_UP:
	__imlib_calc_size(fn, &w, &h, text);
	xx = w - y;
	yy = x;
	cp = __imlib_char_pos(fn, text, xx, yy, &cx, &cy, &cw, &ch);
	cy = 1 + h - cy - ch;
	if (char_x_return)
	   *char_x_return = cy;
	if (char_y_return)
	   *char_y_return = cx;
	if (char_width_return)
	   *char_width_return = ch;
	if (char_height_return)
	   *char_height_return = cw;
	return cp;
	break;
     default:
	return -1;
	break;
     }
   return -1;
}

char **
imlib_list_fonts(int *number_return)
{
   CHECK_PARAM_POINTER_RETURN("imlib_list_fonts", "number_return", number_return, NULL);
   return __imlib_list_fonts(number_return);
}

void 
imlib_free_font_list(char **font_list, int number)
{
   CHECK_PARAM_POINTER("imlib_free_font_list", "font_list", font_list);
   CHECK_PARAM_POINTER("imlib_free_font_list", "number", number);
   __imlib_free_font_list(font_list, number);
}

int 
imlib_get_font_cache_size(void)
{
   return __imlib_get_font_cache_size();
}

void 
imlib_set_font_cache_size(int bytes)
{
   __imlib_set_font_cache_size(bytes);
}

void 
imlib_flush_font_cache(void)
{
   __imlib_purge_font_cache();
}

int
imlib_get_font_ascent(void)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_ascent", "font", ctxt_font , 0);
   return ((ImlibFont *)ctxt_font)->ascent;
}

int
imlib_get_font_descent(void)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_descent", "font", ctxt_font , 0);
   return ((ImlibFont *)ctxt_font)->descent;
}

int
imlib_get_maximum_font_ascent(Imlib_Font font)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_maximum_font_ascent", "font", ctxt_font , 0);
   return ((ImlibFont *)ctxt_font)->max_ascent;
}

int
imlib_get_maximum_font_descent(Imlib_Font font)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_maximum_font_descent", "font", ctxt_font , 0);
   return ((ImlibFont *)ctxt_font)->max_ascent;
}


Imlib_Color_Modifier 
imlib_create_color_modifier(void)
{
   return (Imlib_Color_Modifier)__imlib_CreateCmod();
}

void 
imlib_free_color_modifier(void)
{
   CHECK_PARAM_POINTER("imlib_free_color_modifier", "color_modifier", ctxt_color_modifier);
   __imlib_FreeCmod((ImlibColorModifier *)ctxt_color_modifier);
   ctxt_color_modifier = NULL;
}

void 
imlib_modify_color_modifier_gamma(double gamma_value)
{
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_gamma", "color_modifier", ctxt_color_modifier);
   __imlib_CmodModGamma((ImlibColorModifier *)ctxt_color_modifier, 
			gamma_value);
}

void 
imlib_modify_color_modifier_brightness(double brightness_value)
{
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_brightness", "color_modifier", ctxt_color_modifier);
   __imlib_CmodModBrightness((ImlibColorModifier *)ctxt_color_modifier, 
			     brightness_value);
}

void 
imlib_modify_color_modifier_contrast(double contrast_value)
{
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_contrast", "color_modifier", ctxt_color_modifier);
   __imlib_CmodModContrast((ImlibColorModifier *)ctxt_color_modifier, 
			   contrast_value);
}

void 
imlib_set_color_modifier_tables(DATA8 *red_table,
				DATA8 *green_table,
				DATA8 *blue_table,
				DATA8 *alpha_table)
{
   CHECK_PARAM_POINTER("imlib_set_color_modifier_tables", "color_modifier", ctxt_color_modifier);
   __imlib_CmodSetTables((ImlibColorModifier *)ctxt_color_modifier,
			 red_table, green_table, blue_table, alpha_table);
}

void 
imlib_get_color_modifier_tables(DATA8 *red_table,
				DATA8 *green_table,
				DATA8 *blue_table,
				DATA8 *alpha_table)
{
   CHECK_PARAM_POINTER("imlib_get_color_modifier_tables", "color_modifier", ctxt_color_modifier);
   __imlib_CmodGetTables((ImlibColorModifier *)ctxt_color_modifier,
			 red_table, green_table, blue_table, alpha_table);
}

void
imlib_reset_color_modifier(void)
{
   CHECK_PARAM_POINTER("imlib_rset_color_modifier", "color_modifier", ctxt_color_modifier);
   __imlib_CmodReset((ImlibColorModifier *)ctxt_color_modifier);
}

void 
imlib_apply_color_modifier(void)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_apply_color_modifier", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_apply_color_modifier", "color_modifier", ctxt_color_modifier);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_DataCmodApply(im->data, im->w, im->h, 0, 
			 (ImlibColorModifier *)ctxt_color_modifier);
}

void 
imlib_apply_color_modifier_to_rectangle(int x, int y, int width, int height)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_apply_color_modifier_to_rectangle", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_apply_color_modifier_to_rectangle", "color_modifier", ctxt_color_modifier);
   CAST_IMAGE(im, ctxt_image);
   if (x < 0)
     {
	width += x;
	x = 0;
     }
   if (width <= 0)
      return;
   if ((x + width) > im->w)
      width = (im->w - x);
   if (width <= 0)
      return;
   if (y < 0)
     {
	height += y;
	y = 0;
     }
   if (height <= 0)
      return;
   if ((y + height) > im->h)
      height = (im->h - y);
   if (height <= 0)
      return;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_DataCmodApply(im->data + (y * im->w) + x, width, height, 
			 im->w - width, 
			 (ImlibColorModifier *)ctxt_color_modifier);
}

Imlib_Updates
imlib_image_draw_line(int x1, int y1, int x2, int y2, char make_updates)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER_RETURN("imlib_image_draw_line", "image", ctxt_image, NULL);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return NULL;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   return (Imlib_Updates)__imlib_draw_line(im, x1, y1, x2, y2, 
					   (DATA8)ctxt_color.red, 
					   (DATA8)ctxt_color.green, 
					   (DATA8)ctxt_color.blue, 
					   (DATA8)ctxt_color.alpha, 
					   ctxt_operation,
					   (char)make_updates);
}

void 
imlib_image_draw_rectangle(int x, int y, int width, int height)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_draw_rectangle", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_draw_box(im, x, y, width, height, ctxt_color.red, ctxt_color.green,
		    ctxt_color.blue, ctxt_color.alpha, ctxt_operation);
}

void 
imlib_image_fill_rectangle(int x, int y, int width, int height)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_fill_rectangle", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_draw_filled_box(im, x, y, width, height, ctxt_color.red, 
			   ctxt_color.green, ctxt_color.blue, 
			   ctxt_color.alpha, ctxt_operation);
}

void 
imlib_image_copy_alpha_to_image(Imlib_Image image_source,
				int x, int y)
{
   ImlibImage *im, *im2;
   
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_to_image", "image_source", image_source);
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_to_image", "image_destination", ctxt_image);
   CAST_IMAGE(im, image_source);
   CAST_IMAGE(im2, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if ((!(im2->data)) && (im2->loader) && (im2->loader->load))
      im2->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if (!(im2->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_copy_alpha_data(im, im2, 0, 0, im->w, im->h, x, y);
}

void 
imlib_image_copy_alpha_rectangle_to_image(Imlib_Image image_source,
					  int x, int y, int width,
					  int height, int destination_x,
					  int destination_y)
{
   ImlibImage *im, *im2;
   
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_rectangle_to_image", "image_source", image_source);
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_rectangle_to_image", "image_destination", ctxt_image);
   CAST_IMAGE(im, image_source);
   CAST_IMAGE(im2, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if ((!(im2->data)) && (im2->loader) && (im2->loader->load))
      im2->loader->load(im2, NULL, 0, 1);
   if (!(im->data))
      return;
   if (!(im2->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_copy_alpha_data(im, im2, x, y, width, height, destination_x, 
			   destination_y);
}

void 
imlib_image_scroll_rect(int x, int y, int width, int height, int delta_x, 
			int delta_y)
{
   ImlibImage *im;
   int xx, yy, w, h, nx, ny;
   
   CHECK_PARAM_POINTER("imlib_image_scroll_rect", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if (delta_x > 0)
     {
	xx = x;
	nx = x + delta_x;
	w = width - delta_x;
     }
   else
     {
	xx = x - delta_x;
	nx = x;
	w = width + delta_x;
     }
   if (delta_y > 0)
     {
	yy = y;
	ny = y + delta_y;
	h = height - delta_y;
     }
   else
     {
	yy = y - delta_y;
	ny = y;
	h = height + delta_y;
     }   
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_copy_image_data(im, xx, yy, w, h, nx, ny);
}

void 
imlib_image_copy_rect(int x, int y, int width, int height, int new_x,int new_y)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_copy_rect", "image", ctxt_image);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_copy_image_data(im, x, y, width, height, new_x, new_y);
}

Imlib_Color_Range 
imlib_create_color_range(void)
{
   return (Imlib_Color_Range)__imlib_CreateRange();
}

void 
imlib_free_color_range(void)
{
   CHECK_PARAM_POINTER("imlib_free_color_range", "color_range", ctxt_color_range);
   __imlib_FreeRange((ImlibRange *)ctxt_color_range);
   ctxt_color_range = NULL;
}

void 
imlib_add_color_to_color_range(int distance_away)
{
   CHECK_PARAM_POINTER("imlib_add_color_to_color_range", "color_range", ctxt_color_range);
   __imlib_AddRangeColor((ImlibRange *)ctxt_color_range,
			 ctxt_color.red, ctxt_color.green, ctxt_color.blue, 
			 ctxt_color.alpha, distance_away);
}

void 
imlib_image_fill_color_range_rectangle(int x, int y, int width, int height,
				       double angle)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle", "color_range", ctxt_color_range);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_DrawGradient(im, x, y, width, height, 
			(ImlibRange *)ctxt_color_range, angle, ctxt_operation);
}

void
imlib_image_query_pixel(int x, int y, Imlib_Color *color_return)
{
   ImlibImage *im;
   DATA32 *p;
   
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "color_return", color_return);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if ((x < 0) || (x >= im->w) || (y < 0) || (y >= im->h))
     {
	color_return->red = 0;
	color_return->green = 0;
	color_return->blue = 0;
	color_return->alpha = 0;
	return;
     }
   p = im->data + (im->w * y) + x;
   color_return->red = ((*p) >> 16) & 0xff;
   color_return->green = ((*p) >> 8) & 0xff;
   color_return->blue = (*p) & 0xff;
   color_return->alpha = ((*p) >> 24) & 0xff;
}

void 
imlib_image_attach_data_value(const char *key, void *data, int value,
			      Imlib_Internal_Data_Destructor_Function destructor_function)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_attach_data_value", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_attach_data_value", "key", key);
   CAST_IMAGE(im, ctxt_image);
   __imlib_AttachTag(im, key, value, data,
		     (ImlibDataDestructorFunction)destructor_function);
}

void *
imlib_image_get_attached_data(const char *key)
{
   ImlibImageTag *t;
   ImlibImage *im;
      
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_data", "image", ctxt_image, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_data", "key", key, NULL);
   CAST_IMAGE(im, ctxt_image);
   t = __imlib_GetTag(im, key);
   if (t)
      return t->data;
   return NULL;
}

int
imlib_image_get_attached_value(const char *key)
{
   ImlibImageTag *t;
   ImlibImage *im;
      
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_value", "image", ctxt_image, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_value", "key", key, 0);
   CAST_IMAGE(im, ctxt_image);
   t = __imlib_GetTag(im, key);
   if (t)
      return t->val;
   return 0;
}

void 
imlib_image_remove_attached_data_value(const char *key)
{
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_image_remove_attached_data_value", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_remove_attached_data_value", "key", key);
   CAST_IMAGE(im, ctxt_image);
   __imlib_RemoveTag(im, key);
}

void 
imlib_image_remove_and_free_attached_data_value(const char *key)
{
   ImlibImageTag *t;
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_image_remove_and_free_attached_data_value", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_image_remove_and_free_attached_data_value", "key", key);
   CAST_IMAGE(im, ctxt_image);
   t = __imlib_RemoveTag(im, key);
   __imlib_FreeTag(im, t);
}

void
imlib_save_image(const char *filename)
{
   ImlibImage *im;
   Imlib_Image prev_ctxt_image;
   
   CHECK_PARAM_POINTER("imlib_save_image", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_save_image", "filename", filename);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return;
   prev_ctxt_image = ctxt_image;
   __imlib_SaveImage(im, filename, (ImlibProgressFunction)ctxt_progress_func, 
		     ctxt_progress_granularity, NULL);
   ctxt_image = prev_ctxt_image;
}

void
imlib_save_image_with_error_return(const char *filename,
				   Imlib_Load_Error *error_return)
{
   ImlibImage *im;
   Imlib_Image prev_ctxt_image;
      
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "image", ctxt_image);
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "filename", filename);
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "error_return", error_return);
   CAST_IMAGE(im, ctxt_image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return;
   prev_ctxt_image = ctxt_image;
   __imlib_SaveImage(im, filename, (ImlibProgressFunction)ctxt_progress_func, 
		     ctxt_progress_granularity, error_return);
   ctxt_image = prev_ctxt_image;
}

Imlib_Image
imlib_create_rotated_image(double angle)
{
   ImlibImage *im, *im_old;
   DATA32 *data;
   int x, y, dx, dy, sz;
   double x1, y1, d;
   
   CHECK_PARAM_POINTER_RETURN("imlib_create_rotated_image", "image",
			      ctxt_image, NULL);
   CAST_IMAGE(im_old, ctxt_image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   
   d = hypot((double)(im_old->w + 4), (double)(im_old->h + 4)) / sqrt(2.0);
   
   x1 = (double)(im_old->w) / 2.0 - sin(angle + atan(1.0)) * d;
   y1 = (double)(im_old->h) / 2.0 - cos(angle + atan(1.0)) * d;
   
   sz = (int)(d * sqrt(2.0));
   x = (int)(x1 * _ROTATE_PREC_MAX);
   y = (int)(y1 * _ROTATE_PREC_MAX);
   dx = (int)(cos(angle) * _ROTATE_PREC_MAX);
   dy = -(int)(sin(angle) * _ROTATE_PREC_MAX);
   
   im = __imlib_CreateImage(sz, sz, NULL);
   im->data = calloc(sz * sz, sizeof(DATA32));
   if (!(im->data)) 
     {
	__imlib_FreeImage(im);
	return NULL;
     }
   
   if (ctxt_anti_alias) 
     {
	__imlib_RotateAA(im_old->data, im->data, im_old->w,
			 im_old->w, im_old->h, im->w, sz, sz, x, y, dx, dy);
     } else 
     {
	__imlib_RotateSample(im_old->data, im->data, im_old->w,
			     im_old->w, im_old->h, im->w, sz, sz, x, y, dx, dy);
     }
   SET_FLAG(im->flags, F_HAS_ALPHA);
   
   return (Imlib_Image)im;
}

void 
imlib_blend_image_onto_image_at_angle(Imlib_Image source_image,
				      char merge_alpha,
				      int source_x, int source_y,
				      int source_width, int source_height,
				      int destination_x1, int destination_y1,
				      int destination_x2, int destination_y2)
{
   ImlibImage *im_src, *im_dst;
   
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image_at_angle", "source_image", source_image);
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image_at_angle", "image", ctxt_image);
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, ctxt_image);
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   __imlib_BlendImageToImageAtAngle(im_src, im_dst, ctxt_anti_alias,
				    ctxt_blend, merge_alpha, source_x, source_y,
				    source_width, source_height, destination_x1,
				    destination_y1, destination_x2, destination_y2,
				    ctxt_color_modifier, ctxt_operation);
}
