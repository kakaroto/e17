#include <X11/Xlib.h>
#include "common.h"
#include "scale.h"
#include "image.h"
#include "rend.h"
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
   __imlib_RGBA_init();
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
imlib_set_image_never_changes_on_disk(Imlib_Image image)
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

