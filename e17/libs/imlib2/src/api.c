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
#include <freetype.h>
#include "font.h"
#include "grad.h"

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

typedef void (*Imlib_Internal_Progress_Function)(void *, char, 
						 int, int, int, int);
typedef void (*Imlib_Internal_Data_Destructor_Function)(void *, void *);

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

int 
imlib_get_visual_depth(Display *display, Visual *visual)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_visual_depth", "display", display, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_get_visual_depth", "visual", visual, 0);
   return __imlib_XActualDepth(display, visual);
}

Visual *
imlib_get_best_visual(Display *display, int screen,
		      int *depth_return)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_best_visual", "display", display, NULL);
   return __imlib_BestVisual(display, screen, depth_return);
}

Imlib_Image 
imlib_load_image(char *file)
{
   CHECK_PARAM_POINTER_RETURN("imlib_load_image", "file", file, NULL);
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 0, 0, NULL);
}

Imlib_Image 
imlib_load_image_with_progress_callback(char *file,
					Imlib_Progress_Function progress_function,
					char progress_granulatiy)
{
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_with_progress_callback", "file", file, NULL);
   return (Imlib_Image) 
      __imlib_LoadImage(file, (Imlib_Internal_Progress_Function)progress_function, progress_granulatiy, 0, 0, NULL);
}

Imlib_Image 
imlib_load_image_immediately(char *file)
{
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_immediately", "file", file, NULL);
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 1, 0, NULL);
}

Imlib_Image 
imlib_load_image_without_cache(char *file)
{
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_without_cache", "file", file, NULL);
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 0, 1, NULL);
}

Imlib_Image 
imlib_load_image_with_progress_callback_without_cache (char *file,
						       Imlib_Progress_Function progress_function,
						       char progress_granulatiy)
{
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_with_progress_callback_without_cache", "file", file, NULL);
   return (Imlib_Image) 
      __imlib_LoadImage(file, (Imlib_Internal_Progress_Function)progress_function, progress_granulatiy, 0, 1, NULL);
}

Imlib_Image 
imlib_load_image_immediately_without_cache(char *file)
{
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_immediately_without_cache", "file", file, NULL);
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 1, 1, NULL);
}

Imlib_Image 
imlib_load_image_with_progress_callback_and_error_return (char *file,
							  Imlib_Progress_Function progress_function,
							  char progress_granulatiy,
							  Imlib_Load_Error *error_return)
{
   Imlib_Image im = NULL;
   ImlibLoadError er;

   CHECK_PARAM_POINTER_RETURN("imlib_load_image_with_progress_callback_and_error_return", "file", file, NULL);
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
   im = (Imlib_Image)__imlib_LoadImage(file, (Imlib_Internal_Progress_Function)progress_function, progress_granulatiy, 0, 0, &er);
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
imlib_free_image(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_free_image", "image", image);
   CAST_IMAGE(im, image);
   __imlib_FreeImage(im);
}

void 
imlib_free_image_and_decache(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_free_image_and_decache", "image", image);
   CAST_IMAGE(im, image);
   SET_FLAG(im->flags, F_INVALID);
   __imlib_FreeImage(im);
}

int 
imlib_image_get_width(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_width", "image", image, 0);
   CAST_IMAGE(im, image);
   return im->w;
}

int 
imlib_image_get_height(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_height", "image", image, 0);
   CAST_IMAGE(im, image);
   return im->h;
}

DATA32 *
imlib_image_get_data(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_data", "image", image, NULL);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   return im->data;
}

DATA32 *
imlib_image_get_data_for_reading_only(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_get_data_for_reading_only", "image", image, NULL);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   return im->data;
}

void 
imlib_image_put_back_data(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_put_back_data", "image", image);
   CAST_IMAGE(im, image);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
}

char 
imlib_image_has_alpha(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_has_alpha", "image", image, 0);
   CAST_IMAGE(im, image);
   if (IMAGE_HAS_ALPHA(im))
      return 1;
   return 0;       
}

void 
imlib_image_set_never_changes_on_disk(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_never_changes_on_disk", "image", image);
   CAST_IMAGE(im, image);
   UNSET_FLAG(im->flags, F_ALWAYS_CHECK_DISK); 
}

void 
imlib_image_get_border(Imlib_Image image, Imlib_Border *border)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_get_border", "image", image);
   CHECK_PARAM_POINTER("imlib_image_get_border", "border", border);
   CAST_IMAGE(im, image);
   border->left = im->border.left;
   border->right = im->border.right;
   border->top = im->border.top;
   border->bottom = im->border.bottom;
}

void 
imlib_image_set_border(Imlib_Image image, Imlib_Border *border)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_border", "image", image);
   CHECK_PARAM_POINTER("imlib_image_set_border", "border", border);
   CAST_IMAGE(im, image);
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
imlib_image_set_format(Imlib_Image image, char *format)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_format", "image", image);
   CHECK_PARAM_POINTER("imlib_image_set_format", "format", format);
   CAST_IMAGE(im, image);
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
imlib_image_set_irrelevant_format(Imlib_Image image, char irrelevant)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_format", "image", image);
   CAST_IMAGE(im, image);
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
imlib_image_set_irrelevant_border(Imlib_Image image, char irrelevant)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_border", "image", image);
   CAST_IMAGE(im, image);
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
imlib_image_set_irrelevant_alpha(Imlib_Image image, char irrelevant)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_alpha", "image", image);
   CAST_IMAGE(im, image);
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
imlib_image_format(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER_RETURN("imlib_image_format", "image", image, NULL);
   CAST_IMAGE(im, image);
   return im->format;
}

void 
imlib_image_set_has_alpha(Imlib_Image image, char has_alpha)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_set_has_alpha", "image", image);
   CAST_IMAGE(im, image);
   if (has_alpha)
      SET_FLAG(im->flags, F_HAS_ALPHA);
   else      
      UNSET_FLAG(im->flags, F_HAS_ALPHA);
}

void 
imlib_render_pixmaps_for_whole_image(Imlib_Image image, Display *display,
				     Drawable drawable, Visual *visual,
				     Colormap colormap, int depth,
				     Pixmap *pixmap_return,
				     Pixmap *mask_return,
				     char dithered_rendering,
				     char create_dithered_mask,
				     Imlib_Color_Modifier color_modifier)
{
   ImlibImage *im;
   ImlibColorModifier *cm;

   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "image", image);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "display", display);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "drawable", drawable);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "visual", visual);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "colormap", colormap);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "depth", depth);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "pixmap_return", pixmap_return);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "mask_return", mask_return);
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_CreatePixmapsForImage(display, drawable, visual, depth, colormap, 
				 im, pixmap_return, mask_return, 0, 0, 
				 im->w, im->h, im->w, im->h,
				 0,
				 dithered_rendering,
				 create_dithered_mask,
				 color_modifier);
}

void 
imlib_render_pixmaps_for_whole_image_at_size(Imlib_Image image, Display *display,
					     Drawable drawable, Visual *visual,
					     Colormap colormap, int depth,
					     Pixmap *pixmap_return,
					     Pixmap *mask_return,
					     char anti_aliased_scaling,
					     char dithered_rendering,
					     char create_dithered_mask,
					     int width, int height,
					     Imlib_Color_Modifier color_modifier)
{
   ImlibImage *im;
   ImlibColorModifier *cm;

   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "image", image);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "display", display);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "drawable", drawable);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "visual", visual);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "colormap", colormap);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "depth", depth);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "pixmap_return", pixmap_return);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size", "mask_return", mask_return);
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_CreatePixmapsForImage(display, drawable, visual, depth, colormap, 
				 im, pixmap_return, mask_return, 0, 0, 
				 im->w, im->h, width, height,
				 anti_aliased_scaling,
				 dithered_rendering,
				 create_dithered_mask,
				 color_modifier);
}

void 
imlib_render_image_on_drawable(Imlib_Image image, Display *display,
			       Drawable drawable, Visual *visual,
			       Colormap colormap, int depth,
			       char dithered_rendering,
			       char alpha_blending,
			       int x, int y,
			       Imlib_Color_Modifier color_modifier,
			       Imlib_Operation operation)
{
   ImlibImage *im;
   ImlibColorModifier *cm;
   
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "image", image);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "display", display);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "drawable", drawable);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "visual", visual);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "colormap", colormap);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "depth", depth);
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       0, 0, im->w, im->h, x, y, im->w, im->h,
		       0,
		       dithered_rendering,
		       alpha_blending, 0, 
		       cm, (ImlibOp)operation);
}

void
imlib_render_image_on_drawable_at_size(Imlib_Image image, Display *display,
				       Drawable drawable, Visual *visual,
				       Colormap colormap, int depth,
				       char anti_aliased_scaling,
				       char dithered_rendering,
				       char alpha_blending,
				       int x, int y, int width, int height,
				       Imlib_Color_Modifier color_modifier,
				       Imlib_Operation operation)
{
   ImlibImage *im;
   ImlibColorModifier *cm;

   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "image", image);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "display", display);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "drawable", drawable);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "visual", visual);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "colormap", colormap);
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "depth", depth);
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       0, 0, im->w, im->h, x, y, width, height,
		       anti_aliased_scaling,
		       dithered_rendering,
		       alpha_blending, 0, 
		       cm, (ImlibOp)operation);
}

void 
imlib_render_image_part_on_drawable_at_size(Imlib_Image image, Display *display,
					    Drawable drawable, Visual *visual,
					    Colormap colormap, int depth,
					    char anti_aliased_scaling,
					    char dithered_rendering,
					    char alpha_blending,
					    int source_x, int source_y,
					    int source_width, int source_height,
					    int x, int y, int width, int height,
					    Imlib_Color_Modifier color_modifier,
					    Imlib_Operation operation)
{
   ImlibImage *im;
   ImlibColorModifier *cm;

   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "image", image);
   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "display", display);
   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "drawable", drawable);
   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "visual", visual);
   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "colormap", colormap);
   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "depth", depth);
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       source_x, source_y, 
		       source_width, source_height, x, y, width, height,
		       anti_aliased_scaling,
		       dithered_rendering,
		       alpha_blending, 0, 
		       cm, (ImlibOp)operation);
}

void 
imlib_blend_image_onto_image(Imlib_Image source_image,
			     Imlib_Image destination_image,
			     char antialias, char blend, char merge_alpha,
			     int source_x, int source_y,
			     int source_width, int source_height,
			     int destination_x, int destination_y,
			     int destination_width, int destination_height,
			     Imlib_Color_Modifier color_modifier,
			     Imlib_Operation operation)
{
   ImlibImage *im_src, *im_dst;
   ImlibColorModifier *cm;
   
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image", "source_image", source_image);
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image", "destination_image", destination_image);
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, destination_image);
   cm = (ImlibColorModifier *)color_modifier;
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   __imlib_BlendImageToImage(im_src, im_dst, antialias, blend, merge_alpha,
			     source_x, source_y, source_width, source_height,
			     destination_x, destination_y, 
			     destination_width, destination_height,
			     cm, (ImlibOp)operation);
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
imlib_create_image_from_drawable(Display *display,
				 Drawable drawable,
				 Pixmap mask, Visual *visual,
				 Colormap colormap, int depth,
				 int x, int y,
				 int width, int height, char need_to_grab_x)
{
   ImlibImage *im;
   char domask = 0;
   
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_from_drawable", "display", display, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_from_drawable", "drawable", drawable, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_from_drawable", "visual", visual, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_from_drawable", "colormap", colormap, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_from_drawable", "depth", depth, NULL);
   if (mask)
      domask = 1;
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height * sizeof(DATA32));
   __imlib_GrabDrawableToRGBA(im->data, 0, 0, width, height,
			      display, drawable, mask, visual,
			      colormap, depth, x, y, width, height,
			      domask, need_to_grab_x);
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_scaled_image_from_drawable(Display *display,
					Drawable drawable,
					Pixmap mask, Visual *visual,
					Colormap colormap, int depth,
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
   
   CHECK_PARAM_POINTER_RETURN("imlib_create_scaled_image_from_drawable", "display", display, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_scaled_image_from_drawable", "drawable", drawable, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_scaled_image_from_drawable", "visual", visual, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_scaled_image_from_drawable", "colormap", colormap, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_create_scaled_image_from_drawable", "depth", depth, NULL);
   if ((mask) || (get_mask_from_shape))
      domask = 1;
   p = XCreatePixmap(display, drawable, destination_width, 
		     source_height, depth);
   gcv.foreground = 0;
   gcv.subwindow_mode = IncludeInferiors;
   if (domask)
     {
	m = XCreatePixmap(display, drawable, destination_width, 
			  source_height, 1);
	mgc = XCreateGC(display, m, GCForeground, &gcv);
     }
   else
      m = None;
   gc = XCreateGC(display, drawable, GCSubwindowMode, &gcv);
   if ((domask) && (!mask))
     {
	XRectangle *rect;
	int         rect_num, rect_ord;
	
	tmpmask = 1;
	mask = XCreatePixmap(display, drawable, source_width, 
			     source_height, 1);
	rect = XShapeGetRectangles(display, drawable, ShapeBounding, &rect_num, &rect_ord);
	XFillRectangle(display, mask, mgc, 0, 0, source_width, source_height);
	if (rect)
	  {
	     XSetForeground(display, mgc, 1);
	     for (x = 0; x < rect_num; x++)
		XFillRectangle(display, mask, mgc, rect[x].x, rect[x].y, 
			       rect[x].width, rect[x].height);
	     XFree(rect);
	  }
	/* build mask from window shape rects */
     }
   for (x = 0; x < destination_width; x++)
     {
	xx = (source_width * x) / destination_width;
	XCopyArea(display, drawable, p, gc, source_x + xx, 0, 1, source_height,
		  xx, 0);
	if (m != None)
	   XCopyArea(display, mask, m, mgc, xx, 0, 1, source_height,
		     xx, 0);	   
     }
   for (x = 0; x < destination_height; x++)
     {
	xx = (source_height * x) / destination_height;
	XCopyArea(display, drawable, p, gc, 0, source_y + xx, destination_width, 1,
		  0, xx);
	if (m != None)
	   XCopyArea(display, mask, m, mgc, 0, source_y + xx, destination_width, 1,
		     0, xx);
     }
   im = __imlib_CreateImage(destination_width, destination_height, NULL);
   im->data = malloc(destination_width * destination_height * sizeof(DATA32));
   __imlib_GrabDrawableToRGBA(im->data, 0, 0, destination_width, 
			      destination_height,
			      display, p, m, visual,
			      colormap, depth, 0, 0, source_width, 
			      source_height,
			      domask, need_to_grab_x);
   XFreePixmap(display, p);
   if (m != None)
     {
	XFreeGC(display, mgc);
	XFreePixmap(display, m);
	if (tmpmask)
	   XFreePixmap(display, mask);
     }      
   XFreeGC(display, gc);
   return (Imlib_Image)im;
}

char
imlib_copy_drawable_to_image(Imlib_Image image, Display *display,
			     Drawable drawable, Pixmap mask, Visual *visual,
			     Colormap colormap, int depth, int x, int y,
			     int width, int height,
			     int destination_x, int destination_y,
			     char need_to_grab_x)
{
   ImlibImage *im;
   char domask = 0;
   int pre_adj;
   
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "image", image, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "display", display, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "drawable", drawable, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "visual", visual, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "colormap", colormap, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "depth", depth, 0);
   if (mask)
      domask = 1;   
   CAST_IMAGE(im, image);

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
				     im->w, im->h, display, drawable, 
				     mask, visual, colormap, depth, 
				     x, y, width, height,
				     domask, need_to_grab_x);   
}

Imlib_Image 
imlib_clone_image(Imlib_Image image)
{
   ImlibImage *im, *im_old;

   CHECK_PARAM_POINTER_RETURN("imlib_clone_image", "image", image, NULL);
   CAST_IMAGE(im_old, image);
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
imlib_create_cropped_image(Imlib_Image image, int x, int y, int width, 
			   int height)
{
   ImlibImage *im, *im_old;

   CHECK_PARAM_POINTER_RETURN("imlib_create_cropped_image", "image", image, NULL);
   CAST_IMAGE(im_old, image);
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
imlib_create_cropped_scaled_image(Imlib_Image image, char antialias, 
				  int source_x, int source_y, int source_width,
				  int source_height, int destination_width,
				  int destination_height)
{
   ImlibImage *im, *im_old;
   
   CHECK_PARAM_POINTER_RETURN("imlib_create_cropped_scaled_image", "image", image, NULL);
   CAST_IMAGE(im_old, image);
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
	__imlib_BlendImageToImage(im_old, im, antialias, 0, 1,
				  source_x, source_y, source_width, source_height,
				  0, 0, destination_width, destination_height,
				  NULL, IMLIB_OP_COPY);
     }
   else
     {
	__imlib_BlendImageToImage(im_old, im, antialias, 0, 0,
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

Imlib_Updates imlib_updates_get_next(Imlib_Updates updates)
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
imlib_render_image_updates_on_drawable(Imlib_Image image, 
				       Imlib_Updates updates,
				       Display *display,
				       Drawable drawable, Visual *visual,
				       Colormap colormap, int depth,
				       char dithered_rendering,
				       int x, int y,
				       Imlib_Color_Modifier color_modifier)
{
   ImlibUpdate *u;
   ImlibImage *im;
   ImlibColorModifier *cm;
   
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "image", image);
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "display", display);
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "drawable", drawable);
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "visual", visual);
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "colormap", colormap);
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "depth", depth);
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   u = (ImlibUpdate *)updates;
   if (!updates)
      return;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_SetMaxXImageCount(display, 10);
   for (; u; u = u->next)
     {
	__imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
			    u->x, u->y, u->w, u->h, x + u->x, y + u->y, 
			    u->w, u->h,
			    0,
			    dithered_rendering,
			    0, 0, 
			    cm, OP_COPY);
     }
   __imlib_SetMaxXImageCount(display, 0);
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
   while(u)
     {
	if (!u->next)
	  {
	     u->next = uu;
	     return u;
	  }
	u = u->next;
     }
   return u;
}

void 
imlib_image_flip_horizontal(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_flip_horizontal", "image", image);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FlipImageHoriz(im);
}

void 
imlib_image_flip_vertical(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_flip_vertical", "image", image);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FlipImageVert(im);
}

void 
imlib_image_flip_diagonal(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_flip_diagonal", "image", image);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FlipImageDiagonal(im);
}

void 
imlib_image_blur(Imlib_Image image, int radius)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_blur", "image", image);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_BlurImage(im, radius);
}

void 
imlib_image_sharpen(Imlib_Image image, int radius)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   CHECK_PARAM_POINTER("imlib_image_sharpen", "image", image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_SharpenImage(im, radius);
}

void 
imlib_image_tile_horizontal(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_tile_horizontal", "image", image);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_TileImageHoriz(im);
}

void 
imlib_image_tile_vertical(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_tile_vertical", "image", image);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_TileImageVert(im);
}

void 
imlib_image_tile(Imlib_Image image)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_tile", "image", image);
   CAST_IMAGE(im, image);
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
imlib_load_font(char *font_name)
{
   return (Imlib_Font)__imlib_load_font(font_name);   
}

void 
imlib_free_font(Imlib_Font font)
{
   CHECK_PARAM_POINTER("imlib_free_font", "font", font);
   __imlib_free_font(font);
}

void 
imlib_text_draw(Imlib_Font font, Imlib_Image image, int x, int y,
		Imlib_Text_Direction direction, char *text,
		Imlib_Color *color, Imlib_Operation operation)
{
   ImlibImage *im;
   ImlibFont *fn;
   
   CHECK_PARAM_POINTER("imlib_text_draw", "font", font);
   CHECK_PARAM_POINTER("imlib_text_draw", "image", image);
   CHECK_PARAM_POINTER("imlib_text_draw", "text", text);
   CHECK_PARAM_POINTER("imlib_text_draw", "color", color);
   CAST_IMAGE(im, image);
   fn = (ImlibFont *)font;
   __imlib_render_str(im, fn, x, y, text, (DATA8)color->red, 
		      (DATA8)color->green, (DATA8)color->blue, 
		      (DATA8)color->alpha, (char)direction, 
		      NULL, NULL, 0, NULL, NULL,
		      (ImlibOp)operation);
}

void 
imlib_text_draw_with_return_metrics(Imlib_Font font, Imlib_Image image, int x, 
				    int y, Imlib_Text_Direction direction, 
				    char *text, Imlib_Color *color, 
				    Imlib_Operation operation,
				    int *width_return, int *height_return,
				    int *horizontal_advance_return,
				    int *vertical_advance_return)
{
   ImlibImage *im;
   ImlibFont *fn;
   
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "font", font);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "image", image);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "text", text);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "color", color);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   fn = (ImlibFont *)font;
   __imlib_render_str(im, fn, x, y, text, (DATA8)color->red, 
		      (DATA8)color->green, (DATA8)color->blue, 
		      (DATA8)color->alpha, (char)direction, 
		      width_return, height_return, 0, 
		      horizontal_advance_return, vertical_advance_return,
		      (ImlibOp)operation);
}

void 
imlib_get_text_size(Imlib_Font font, Imlib_Text_Direction direction,
		    char *text, int *width_return, int *height_return)
{
   ImlibFont *fn;
   int w, h;
   
   CHECK_PARAM_POINTER("imlib_get_text_size", "font", font);
   CHECK_PARAM_POINTER("imlib_get_text_size", "text", text);
   fn = (ImlibFont *)font;
   __imlib_calc_size(fn, &w, &h, text);
   switch(direction)
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
imlib_add_path_to_font_path(char *path)
{
   CHECK_PARAM_POINTER("imlib_add_path_to_font_path", "path", path);
   __imlib_add_font_path(path);
}

void 
imlib_remove_path_from_font_path(char *path)
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
imlib_text_get_index_and_location(Imlib_Font font,
				  Imlib_Text_Direction direction,
				  char *text, int x, int y,
				  int *char_x_return, int *char_y_return,
				  int *char_width_return,
				  int *char_height_return)
{
   ImlibFont *fn;
   int w, h, cx, cy, cw, ch, cp, xx, yy;
   
   CHECK_PARAM_POINTER_RETURN("imlib_text_get_index_and_location", "font", font , -1);
   CHECK_PARAM_POINTER_RETURN("imlib_text_get_index_and_location", "text", text, -1);
   fn = (ImlibFont *)font;
   switch(direction)
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
imlib_get_font_ascent(Imlib_Font font)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_ascent", "font", font , 0);
   return ((ImlibFont *)font)->ascent;
}

int
imlib_get_font_descent(Imlib_Font font)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_descent", "font", font , 0);
   return ((ImlibFont *)font)->descent;
}

int
imlib_get_maximum_font_ascent(Imlib_Font font)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_maximum_font_ascent", "font", font , 0);
   return ((ImlibFont *)font)->max_ascent;
}

int
imlib_get_maximum_font_descent(Imlib_Font font)
{
   CHECK_PARAM_POINTER_RETURN("imlib_get_maximum_font_descent", "font", font , 0);
   return ((ImlibFont *)font)->max_ascent;
}


Imlib_Color_Modifier 
imlib_create_color_modifier(void)
{
   return (Imlib_Color_Modifier)__imlib_CreateCmod();
}

void 
imlib_free_color_modifier(Imlib_Color_Modifier color_modifier)
{
   CHECK_PARAM_POINTER("imlib_free_color_modifier", "color_modifier", color_modifier);
   __imlib_FreeCmod((ImlibColorModifier *)color_modifier);
}

void 
imlib_modify_color_modifier_gamma(Imlib_Color_Modifier color_modifier,
				  double gamma_value)
{
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_gamma", "color_modifier", color_modifier);
   __imlib_CmodModGamma((ImlibColorModifier *)color_modifier, 
			gamma_value);
}

void 
imlib_modify_color_modifier_brightness(Imlib_Color_Modifier color_modifier,
				       double brightness_value)
{
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_brightness", "color_modifier", color_modifier);
   __imlib_CmodModBrightness((ImlibColorModifier *)color_modifier, 
			     brightness_value);
}

void 
imlib_modify_color_modifier_contrast(Imlib_Color_Modifier color_modifier,
				     double contrast_value)
{
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_contrast", "color_modifier", color_modifier);
   __imlib_CmodModContrast((ImlibColorModifier *)color_modifier, 
			   contrast_value);
}

void 
imlib_set_color_modifier_tables(Imlib_Color_Modifier color_modifier,
				DATA8 *red_table,
				DATA8 *green_table,
				DATA8 *blue_table,
				DATA8 *alpha_table)
{
   CHECK_PARAM_POINTER("imlib_set_color_modifier_tables", "color_modifier", color_modifier);
   __imlib_CmodSetTables((ImlibColorModifier *)color_modifier,
			 red_table, green_table, blue_table, alpha_table);
}

void 
imlib_get_color_modifier_tables(Imlib_Color_Modifier color_modifier,
				DATA8 *red_table,
				DATA8 *green_table,
				DATA8 *blue_table,
				DATA8 *alpha_table)
{
   CHECK_PARAM_POINTER("imlib_get_color_modifier_tables", "color_modifier", color_modifier);
   __imlib_CmodGetTables((ImlibColorModifier *)color_modifier,
			 red_table, green_table, blue_table, alpha_table);
}

void
imlib_reset_color_modifier(Imlib_Color_Modifier color_modifier)
{
   CHECK_PARAM_POINTER("imlib_rset_color_modifier", "color_modifier", color_modifier);
   __imlib_CmodReset((ImlibColorModifier *)color_modifier);
}

void 
imlib_apply_color_modifier(Imlib_Image image,
			   Imlib_Color_Modifier color_modifier)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_apply_color_modifier", "image", image);
   CHECK_PARAM_POINTER("imlib_apply_color_modifier", "color_modifier", color_modifier);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DataCmodApply(im->data, im->w, im->h, 0, 
			 (ImlibColorModifier *)color_modifier);
}

void 
imlib_apply_color_modifier_to_rectangle(Imlib_Image image, 
					Imlib_Color_Modifier color_modifier,
					int x, int y, int width, 
					int height)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_apply_color_modifier_to_rectangle", "image", image);
   CHECK_PARAM_POINTER("imlib_apply_color_modifier_to_rectangle", "color_modifier", color_modifier);
   CAST_IMAGE(im, image);
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
   __imlib_DataCmodApply(im->data + (y * im->w) + x, width, height, 
			 im->w - width, 
			 (ImlibColorModifier *)color_modifier);
}

/*
   CHECK_PARAM_POINTER_RETURN("", "", , NULL);
   CHECK_PARAM_POINTER("", "", );
*/

Imlib_Updates
imlib_image_draw_line(Imlib_Image image, int x1, int y1, int x2, int y2,
		      Imlib_Color *color, Imlib_Operation operation,
		      int make_updates)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER_RETURN("imlib_image_draw_line", "image", image, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_image_draw_line", "color", color, NULL);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return NULL;
   return (Imlib_Updates)__imlib_draw_line(im, x1, y1, x2, y2, 
					   (DATA8)color->red, 
					   (DATA8)color->green, 
					   (DATA8)color->blue, 
					   (DATA8)color->alpha, 
					   (ImlibOp)operation,
					   (char)make_updates);
}

void 
imlib_image_draw_rectangle(Imlib_Image image, int x, int y, int width,
			   int height, Imlib_Color *color, 
			   Imlib_Operation operation)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_draw_rectangle", "image", image);
   CHECK_PARAM_POINTER("imlib_image_draw_rectangle", "color", color);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_draw_box(im, x, y, width, height, color->red, color->green,
		    color->blue, color->alpha, (ImlibOp)operation);
}

void 
imlib_image_fill_rectangle(Imlib_Image image, int x, int y, int width,
			   int height, Imlib_Color *color, 
			   Imlib_Operation operation)
{
   ImlibImage *im;
   
   CHECK_PARAM_POINTER("imlib_image_fill_rectangle", "image", image);
   CHECK_PARAM_POINTER("imlib_image_fill_rectangle", "color", color);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_draw_filled_box(im, x, y, width, height, color->red, color->green,
			   color->blue, color->alpha, (ImlibOp)operation);
}

void 
imlib_image_copy_alpha_to_image(Imlib_Image image_source,
				Imlib_Image image_destination,
				int x, int y)
{
   ImlibImage *im, *im2;
   
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_to_image", "image_source", image_source);
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_to_image", "image_destination", image_destination);
   CAST_IMAGE(im, image_source);
   CAST_IMAGE(im2, image_destination);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if ((!(im2->data)) && (im2->loader) && (im2->loader->load))
      im2->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if (!(im2->data))
      return;
   __imlib_copy_alpha_data(im, im2, 0, 0, im->w, im->h, x, y);
}

void 
imlib_image_copy_alpha_rectangle_to_image(Imlib_Image image_source,
					  Imlib_Image image_destination,
					  int x, int y, int width,
					  int height, int destination_x,
					  int destination_y)
{
   ImlibImage *im, *im2;
   
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_rectangle_to_image", "image_source", image_source);
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_rectangle_to_image", "image_destination", image_destination);
   CAST_IMAGE(im, image_source);
   CAST_IMAGE(im2, image_destination);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if ((!(im2->data)) && (im2->loader) && (im2->loader->load))
      im2->loader->load(im2, NULL, 0, 1);
   if (!(im->data))
      return;
   if (!(im2->data))
      return;
   __imlib_copy_alpha_data(im, im2, x, y, width, height, destination_x, 
			   destination_y);
}

void 
imlib_image_scroll_rect(Imlib_Image image, int x, int y, int width, 
			int height, int delta_x, int delta_y)
{
   ImlibImage *im;
   int xx, yy, w, h, nx, ny;
   
   CHECK_PARAM_POINTER("imlib_image_scroll_rect", "image", image);
   CAST_IMAGE(im, image);
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
   
   __imlib_copy_image_data(im, xx, yy, w, h, nx, ny);
}

void 
imlib_image_copy_rect(Imlib_Image image, int x, int y, int width, int height, 
		      int new_x,int new_y)
{
   ImlibImage *im;
   
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_copy_image_data(im, x, y, width, height, new_x, new_y);
}

Imlib_Color_Range 
imlib_create_color_range(void)
{
   return (Imlib_Color_Range)__imlib_CreateRange();
}

void 
imlib_free_color_range(Imlib_Color_Range color_range)
{
   CHECK_PARAM_POINTER("imlib_free_color_range", "color_range", color_range);
   __imlib_FreeRange((ImlibRange *)color_range);
}

void 
imlib_add_color_to_color_range(Imlib_Color_Range color_range,
			       Imlib_Color *color,
			       int distance_away)
{
   CHECK_PARAM_POINTER("imlib_add_color_to_color_range", "color_range", color_range);
   CHECK_PARAM_POINTER("imlib_add_color_to_color_range", "color", color);
   __imlib_AddRangeColor((ImlibRange *)color_range,
			 color->red, color->green, color->blue, color->alpha,
			 distance_away);
}

void 
imlib_image_fill_color_range_rectangle(Imlib_Image image, int x, int y,
				       int width, int height,
				       Imlib_Color_Range color_range,
				       double angle,
				       Imlib_Operation operation)
{
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle", "image", image);
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle", "color_range", color_range);
   __imlib_DrawGradient((ImlibImage *)image, x, y, width, height,
			(ImlibRange *)color_range, angle,
			(ImlibOp)operation);
}

void
imlib_image_query_pixel(Imlib_Image image, int x, int y,
			Imlib_Color *color_return)
{
   ImlibImage *im;
   DATA32 *p;
   
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "image", image);
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "color_return", color_return);
   CAST_IMAGE(im, image);
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
   color_return->green = ((*p) >> 0) & 0xff;
   color_return->blue = (*p) & 0xff;
   color_return->alpha = ((*p) >> 24) & 0xff;
}

void 
imlib_image_attach_data_value(Imlib_Image image, char *key,
			      void *data, int value,
			      Imlib_Internal_Data_Destructor_Function destructor_function)
{
   ImlibImage *im;

   CHECK_PARAM_POINTER("imlib_image_attach_data_value", "image", image);
   CHECK_PARAM_POINTER("imlib_image_attach_data_value", "key", key);
   CAST_IMAGE(im, image);
   __imlib_AttachTag(im, key, value, data, destructor_function);
}

void *
imlib_image_get_attached_data(Imlib_Image image, char *key)
{
   ImlibImageTag *t;
   ImlibImage *im;
      
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_data", "image", image, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_data", "key", key, NULL);
   CAST_IMAGE(im, image);
   t = __imlib_GetTag(im, key);
   if (t)
      return t->data;
   return NULL;
}

int
imlib_image_get_attached_value(Imlib_Image image, char *key)
{
   ImlibImageTag *t;
   ImlibImage *im;
      
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_value", "image", image, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_value", "key", key, 0);
   CAST_IMAGE(im, image);
   t = __imlib_GetTag(im, key);
   if (t)
      return t->val;
   return 0;
}

void 
imlib_image_remove_attached_data_value(Imlib_Image image, char *key)
{
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_image_remove_attached_data_value", "image", image);
   CHECK_PARAM_POINTER("imlib_image_remove_attached_data_value", "key", key);
   CAST_IMAGE(im, image);
   __imlib_RemoveTag(im, key);
}

void 
imlib_image_remove_and_free_attached_data_value(Imlib_Image image, char *key)
{
   ImlibImageTag *t;
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_image_remove_and_free_attached_data_value", "image", image);
   CHECK_PARAM_POINTER("imlib_image_remove_and_free_attached_data_value", "key", key);
   CAST_IMAGE(im, image);
   t = __imlib_RemoveTag(im, key);
   __imlib_FreeTag(im, t);
}

void
imlib_save_image(Imlib_Image image, char *filename)
{
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_save_image", "image", image);
   CHECK_PARAM_POINTER("imlib_save_image", "filename", filename);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   __imlib_SaveImage(im, filename, NULL, 0, NULL);
}

void
imlib_save_image_with_progress_callback(Imlib_Image image, char *filename,
					Imlib_Internal_Progress_Function progress_function,
					char progress_granulatiy)
{
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback", "image", image);
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback", "filename", filename);
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback", "progress_function", progress_function);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   __imlib_SaveImage(im, filename, progress_function, 
		     progress_granulatiy, NULL);
}

void
imlib_save_image_with_error_return(Imlib_Image image, char *filename,
				   Imlib_Load_Error *error_return)
{
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "image", image);
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "filename", filename);
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "error_return", error_return);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   __imlib_SaveImage(im, filename, NULL, 0, error_return);
}

void
imlib_save_image_with_progress_callback_and_error_return(Imlib_Image image, 
							 char *filename,
							 Imlib_Internal_Progress_Function progress_function,
							 char progress_granulatiy,
							 Imlib_Load_Error *error_return)
{
   ImlibImage *im;
      
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback_and_error_return", "image", image);
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback_and_error_return", "filename", filename);
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback_and_error_return", "progress_function", progress_function);
   CHECK_PARAM_POINTER("imlib_save_image_with_progress_callback_and_error_return", "error_return", error_return);
   CAST_IMAGE(im, image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   __imlib_SaveImage(im, filename,progress_function, 
		     progress_granulatiy, error_return);
}


