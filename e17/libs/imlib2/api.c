#include <X11/Xlib.h>
#include "common.h"
#include "scale.h"
#include "image.h"
#include "rend.h"
#include "context.h"
#include "rgba.h"
#include "color.h"
#include "file.h"
#include "grab.h"
#include "blend.h"
#include "draw.h"
#include "api.h"

#define   CAST_IMAGE(im, image) (im) = (ImlibImage *)(image)

typedef void (*Imlib_Internal_Progress_Function)(ImlibImage*, char, 
						 int, int, int, int);

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

Imlib_Image 
imlib_load_image(char *file)
{
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 0, 0);
}

Imlib_Image 
imlib_load_image_with_progress_callback(char *file,
					Imlib_Progress_Function progress_function,
					char progress_granulatiy)
{
   return (Imlib_Image) 
      __imlib_LoadImage(file, (Imlib_Internal_Progress_Function)progress_function, progress_granulatiy, 0, 0);
}

Imlib_Image 
imlib_load_image_immediately(char *file)
{
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 1, 0);
}

Imlib_Image 
imlib_load_image_without_cache(char *file)
{
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 0, 1);
}

Imlib_Image 
imlib_load_image_with_progress_callback_without_cache (char *file,
						       Imlib_Progress_Function progress_function,
						       char progress_granulatiy)
{
   return (Imlib_Image) 
      __imlib_LoadImage(file, (Imlib_Internal_Progress_Function)progress_function, progress_granulatiy, 0, 1);
}

Imlib_Image 
imlib_load_image_immediately_without_cache(char *file)
{
   return (Imlib_Image) 
      __imlib_LoadImage(file, NULL, 0, 1, 1);
}

void 
imlib_free_image(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);   
   __imlib_FreeImage(im);
}

void 
imlib_free_image_and_decache(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   SET_FLAG(im->flags, F_INVALID);
   __imlib_FreeImage(im);
}

int 
imlib_image_get_width(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   return im->w;
}

int 
imlib_image_get_height(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   return im->h;
}

DATA32 *
imlib_image_get_data(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   if (!(im->data))
      im->loader->load(im, NULL, 0, 1);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   return im->data;
}

void 
imlib_image_put_back_data(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
}

char 
imlib_image_has_alpha(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   if (IMAGE_HAS_ALPHA(im))
      return 1;
   return 0;       
}

void 
imlib_image_set_never_changes_on_disk(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   UNSET_FLAG(im->flags, F_ALWAYS_CHECK_DISK); 
}

void 
imlib_image_get_border(Imlib_Image image, Imlib_Border *border)
{
   ImlibImage *im;

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

   CAST_IMAGE(im, image);
   if (im->format)
      free(im->format);
   im->format = strdup(format);
}

char *
imlib_image_format(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   return im->format;
}

void 
imlib_image_set_has_alpha(Imlib_Image image, char has_alpha)
{
   ImlibImage *im;
   
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

   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader))
      im->loader->load(im, NULL, 0, 1);
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

   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader))
      im->loader->load(im, NULL, 0, 1);
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
			       Imlib_Color_Modifier color_modifier)
{
   ImlibImage *im;
   ImlibColorModifier *cm;
   
   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader))
      im->loader->load(im, NULL, 0, 1);
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       0, 0, im->w, im->h, x, y, im->w, im->h,
		       0,
		       dithered_rendering,
		       alpha_blending, 0, 
		       cm);
}

void
imlib_render_image_on_drawable_at_size(Imlib_Image image, Display *display,
				       Drawable drawable, Visual *visual,
				       Colormap colormap, int depth,
				       char anti_aliased_scaling,
				       char dithered_rendering,
				       char alpha_blending,
				       int x, int y, int width, int height,
				       Imlib_Color_Modifier color_modifier)
{
   ImlibImage *im;
   ImlibColorModifier *cm;

   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader))
      im->loader->load(im, NULL, 0, 1);
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       0, 0, im->w, im->h, x, y, width, height,
		       anti_aliased_scaling,
		       dithered_rendering,
		       alpha_blending, 0, 
		       cm);
}

void imlib_render_image_part_on_drawable_at_size(Imlib_Image image, Display *display,
						 Drawable drawable, Visual *visual,
						 Colormap colormap, int depth,
						 char anti_aliased_scaling,
						 char dithered_rendering,
						 char alpha_blending,
						 int source_x, int source_y,
						 int source_width, int source_height,
						 int x, int y, int width, int height,
						 Imlib_Color_Modifier color_modifier)
{
   ImlibImage *im;
   ImlibColorModifier *cm;

   CAST_IMAGE(im, image);
   cm = (ImlibColorModifier *)color_modifier;
   if ((!(im->data)) && (im->loader))
      im->loader->load(im, NULL, 0, 1);
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       source_x, source_y, 
		       source_width, source_height, x, y, width, height,
		       anti_aliased_scaling,
		       dithered_rendering,
		       alpha_blending, 0, 
		       cm);
}

void 
imlib_blend_image_onto_image(Imlib_Image source_image,
			     Imlib_Image destination_image,
			     char antialias, char blend, char merge_alpha,
			     int source_x, int source_y,
			     int source_width, int source_height,
			     int destination_x, int destination_y,
			     int destination_width, int destination_height)
{
   ImlibImage *im_src, *im_dst;
   char anitalias = 1;
   
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, destination_image);
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   __imlib_BlendImageToImage(im_src, im_dst, antialias, blend, merge_alpha,
			     source_x, source_y, source_width, source_height,
			     destination_x, destination_y, 
			     destination_width, destination_height);
}

Imlib_Image 
imlib_create_image(int width, int height)
{
   DATA32 *data;
   
   data = malloc(width *height * sizeof(DATA32));
   if (data)
      return (Imlib_Image)__imlib_CreateImage(width, height, data);
   return NULL;
}

Imlib_Image 
imlib_create_image_using_data(int width, int height,
			      DATA32 *data)
{
   ImlibImage *im;

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
   
   im = __imlib_CreateImage(width, height, NULL);
   if (!im)
      return NULL;
   im->data = malloc(width * height *sizeof(DATA32));
   if (data)
     {
	memcpy(im->data, data, width * height *sizeof(DATA32));
	return (Imlib_Image)im;
     }
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

   CAST_IMAGE(im_old, image);
   if ((!(im_old->data)) && (im_old->loader))
      im_old->loader->load(im_old, NULL, 0, 1);
   im = __imlib_CreateImage(im_old->w, im_old->h, NULL);
   im->data = malloc(im->w * im->h *sizeof(DATA32));
   memcpy(im->data, im_old->data, im->w * im->h *sizeof(DATA32));
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_cropped_image(Imlib_Image image, int x, int y, int width, 
			   int height)
{
   ImlibImage *im, *im_old;

   CAST_IMAGE(im_old, image);
   if ((!(im_old->data)) && (im_old->loader))
      im_old->loader->load(im_old, NULL, 0, 1);
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height *sizeof(DATA32));
   __imlib_BlendImageToImage(im_old, im, 0, 0, 0,
			     x, y, width, height,
			     0, 0, width, height);
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_cropped_scaled_image(Imlib_Image image, char antialias, 
				  int source_x, int source_y, int source_width,
				  int source_height, int destination_width,
				  int destination_height)
{
   ImlibImage *im, *im_old;
   
   CAST_IMAGE(im_old, image);
   if ((!(im_old->data)) && (im_old->loader))
      im_old->loader->load(im_old, NULL, 0, 1);
   im = __imlib_CreateImage(destination_width, destination_height, NULL);
   im->data = malloc(destination_width * destination_height *sizeof(DATA32));
   __imlib_BlendImageToImage(im_old, im, antialias, 0, 0,
			     source_x, source_y, source_width, source_height,
			     0, 0, destination_width, destination_height);
   return (Imlib_Image)im;
}

