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

char 
imlib_init(void)
{
   return 1;
}

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
imlib_get_image_width(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   return im->w;
}

int 
imlib_get_image_height(Imlib_Image image)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   return im->h;
}

DATA32 *
imlib_get_image_data(Imlib_Image image)
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
imlib_put_back_image_data(Imlib_Image image)
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
   UNSET_FLAG(im->flags, F_HAS_ALPHA);
}

void 
imlib_render_pixmaps_for_whole_image(Imlib_Image image, Display *display,
				     Drawable drawable, Visual *visual,
				     Colormap colormap, int depth,
				     Pixmap *pixmap_return,
				     Pixmap *mask_return,
				     char anti_aliased_scaling,
				     char dithered_rendering,
				     char create_dithered_mask)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   __imlib_CreatePixmapsForImage(display, drawable, visual, depth, colormap, 
				 im, pixmap_return, mask_return, 0, 0, 
				 im->w, im->h, im->w, im->h,
				 anti_aliased_scaling,
				 dithered_rendering,
				 create_dithered_mask);
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
					     int width, int height)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   __imlib_CreatePixmapsForImage(display, drawable, visual, depth, colormap, 
				 im, pixmap_return, mask_return, 0, 0, 
				 im->w, im->h, width, height,
				 anti_aliased_scaling,
				 dithered_rendering,
				 create_dithered_mask);
}

void 
imlib_render_image_on_drawable(Imlib_Image image, Display *display,
			       Drawable drawable, Visual *visual,
			       Colormap colormap, int depth,
			       char anti_aliased_scaling,
			       char dithered_rendering,
			       char alpha_blending,
			       int x, int y)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       0, 0, im->w, im->h, x, y, im->w, im->h,
		       anti_aliased_scaling,
		       dithered_rendering,
		       alpha_blending, 0);
}

void
imlib_render_image_on_drawable_at_size(Imlib_Image image, Display *display,
				       Drawable drawable, Visual *visual,
				       Colormap colormap, int depth,
				       char anti_aliased_scaling,
				       char dithered_rendering,
				       char alpha_blending,
				       int x, int y, int width, int height)
{
   ImlibImage *im;

   CAST_IMAGE(im, image);
   __imlib_RenderImage(display, im, drawable, 0, visual, colormap, depth, 
		       0, 0, width, height, x, y, width, height,
		       anti_aliased_scaling,
		       dithered_rendering,
		       alpha_blending, 0);
}

void 
imlib_blend_image_onto_image(Imlib_Image source_image,
			     Imlib_Image destination_image,
			     int source_x, int source_y,
			     int source_width, int source_height,
			     int destination_x, int destination_y,
			     int destination_width, int destination_height)
{
   ImlibImage *im_src, *im_dst;
   
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, destination_image);
   /* FIXME: doesnt do clipping in any way or form - must fix */
   
   __imlib_BlendRGBAToRGBA(im_src->data, 0, im_dst->data, 0, 
			   source_width, source_height);
}

Imlib_Image 
imlib_create_image_using_data(int width, int height,
			      DATA32 *data)
{
   return (Imlib_Image)__imlib_CreateImage(width, height, data);
}

Imlib_Image 
imlib_create_image_using_copied_data(int width, int height,
				     DATA32 *data)
{
   ImlibImage *im;
   
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height *sizeof(DATA32));
   memcpy(im->data, data, width * height *sizeof(DATA32));
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_create_image_from_drawable(Display *display,
				 Drawable drawable,
				 Pixmap mask, Visual *visual,
				 Colormap colormap, int depth,
				 int x, int y,
				 int width, int height)
{
   ImlibImage *im;
   char domask = 0;
   
   if (mask)
      domask = 1;
   im = __imlib_CreateImage(width, height, NULL);
   im->data = __imlib_GrabDrawableToRGBA(display, drawable, mask, visual,
					 colormap, depth, x, y, width, height,
					 domask);
   return (Imlib_Image)im;
}

Imlib_Image 
imlib_clone_image(Imlib_Image image)
{
   ImlibImage *im, *im_old;

   CAST_IMAGE(im_old, image);
   im = __imlib_CreateImage(im_old->w, im_old->h, NULL);
   im->data = malloc(im->w * im->h *sizeof(DATA32));
   memcpy(im->data, im_old->data, im->w * im->h *sizeof(DATA32));
   return (Imlib_Image)im;
}



