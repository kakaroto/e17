#include "config.h"
#ifndef X_DISPLAY_MISSING
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#endif
#include <string.h>
#include <stdarg.h>
#include "common.h"
#include "colormod.h"
#include "image.h"
#include "scale.h"
#include "blend.h"
#ifndef X_DISPLAY_MISSING
#include "context.h"
#include "color.h"
#include "grab.h"
#include "rend.h"
#include "rgba.h"
#include "ximage.h"
#include "draw.h"
#endif
#include "file.h"
#include "updates.h"
#include "rgbadraw.h"
#include "Imlib2.h"
#include <ft2build.h>
#include FT_FREETYPE_H
/*#ifdef HAVE_FREETYPE1_FREETYPE_FREETYPE_H
#include <freetype1/freetype/freetype.h>
#elif defined(HAVE_FREETYPE_FREETYPE_H)
#include <freetype/freetype.h>
#else
#include <freetype.h>
#endif
*/
#include "font.h"
#include "grad.h"
#include "rotate.h"
#include "filter.h"
#include "dynamic_filters.h"
#include "script.h"
#include <math.h>
#include "color_helpers.h"

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

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* internal typedefs for function pointers */
typedef void        (*Imlib_Internal_Progress_Function) (void *, char, int, int,
                                                         int, int);
typedef void        (*Imlib_Internal_Data_Destructor_Function) (void *, void *);

struct _imlibcontext;
typedef struct _imlibcontext ImlibContext;

struct _imlibcontext {
#ifndef X_DISPLAY_MISSING
   Display            *display;
   Visual             *visual;
   Colormap            colormap;
   int                 depth;
   Drawable            drawable;
   Pixmap              mask;
#endif
   char                anti_alias;
   char                dither;
   char                blend;
   Imlib_Color_Modifier color_modifier;
   Imlib_Operation     operation;
   Imlib_Font          font;
   Imlib_Text_Direction direction;
   double              angle;
   Imlib_Color         color;
   Imlib_Color_Range   color_range;
   Imlib_Image         image;
   Imlib_Progress_Function progress_func;
   char                progress_granularity;
   char                dither_mask;
   Imlib_Filter        filter;
   Imlib_Rectangle     cliprect;
   Imlib_TTF_Encoding  encoding;

   int                 references;
   char                dirty;
};

struct _imlibcontextitem;
typedef struct _imlibcontextitem ImlibContextItem;
struct _imlibcontextitem {
   ImlibContext       *context;
   ImlibContextItem   *below;
};

/* a stack of contexts -- only used by context-handling functions. */
static ImlibContextItem *contexts = NULL;       /* (ImlibContext*) imlib_context_new(); */

/* this is the context all functions use rely on */
static ImlibContext *ctx = NULL;        /* contexts->context; */

/* frees the given context including all its members */
void
__imlib_free_context(ImlibContext * context)
{
   ImlibContextItem   *next = contexts;

   if (ctx == context)
     {
        next = contexts->below;
        free(contexts);
        contexts = next;
     }

   ctx = context;

   if (ctx->image)
     {
        imlib_free_image();
        ctx->image = NULL;
     }
   if (ctx->font)
     {
        imlib_free_font();
        ctx->font = NULL;
     }
   if (ctx->color_modifier)
     {
        imlib_free_color_modifier();
        ctx->color_modifier = NULL;
     }
   if (ctx->filter)
     {
        imlib_free_filter();
        ctx->filter = NULL;
     }

   free(ctx);
   ctx = next->context;
}

Imlib_Context
imlib_context_new(void)
{
   ImlibContext       *context = malloc(sizeof(ImlibContext));

#ifndef X_DISPLAY_MISSING
   context->display = NULL;
   context->visual = NULL;
   context->colormap = 0;
   context->depth = 0;
   context->drawable = 0;
   context->mask = 0;
#endif
   context->anti_alias = 1;
   context->dither = 0;
   context->blend = 1;
   context->color_modifier = NULL;
   context->operation = IMLIB_OP_COPY;
   context->font = NULL;
   context->direction = IMLIB_TEXT_TO_RIGHT;
   context->angle = 0.0;
   context->color = (Imlib_Color)
   {
   255, 255, 255, 255};
   context->color_range = NULL;
   context->image = NULL;
   context->progress_func = NULL;
   context->progress_granularity = 0;
   context->dither_mask = 0;
   context->filter = NULL;
   context->cliprect = (Imlib_Rectangle)
   {
   0, 0, 0, 0};
   context->encoding = IMLIB_TTF_ENCODING_ISO_8859_1;

   context->references = 0;
   context->dirty = 0;

   return (Imlib_Context) context;
}

/* frees the given context if it doesn't have any reference anymore. The
   last (default) context can never be freed. 
   If context is the current context, the context below will be made the
   current context.
*/
void
imlib_context_free(Imlib_Context context)
{
   ImlibContext       *c = (ImlibContext *) context;

   CHECK_PARAM_POINTER("imlib_context_free", "context", context);
   if (c == ctx && !contexts->below)
      return;

   if (c->references == 0)
      __imlib_free_context(c);
   else
      c->dirty = 1;
}

void
imlib_context_push(Imlib_Context context)
{
   ImlibContextItem   *item;

   CHECK_PARAM_POINTER("imlib_context_push", "context", context);
   ctx = (ImlibContext *) context;

   item = malloc(sizeof(ImlibContextItem));
   item->context = ctx;
   item->below = contexts;
   contexts = item;

   ctx->references++;
}

void
imlib_context_pop(void)
{
   ImlibContextItem   *item = contexts;
   ImlibContext       *current_ctx = item->context;

   if (!item->below)
      return;

   contexts = item->below;
   ctx = contexts->context;
   current_ctx->references--;
   if (current_ctx->dirty && current_ctx->references <= 0)
      __imlib_free_context(current_ctx);

   free(item);
}

Imlib_Context
imlib_context_get(void)
{
   return (Imlib_Context) ctx;
}

/* context setting/getting functions */

void
imlib_context_set_cliprect(int x, int y, int w, int h)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->cliprect.x = x;
   ctx->cliprect.y = y;
   ctx->cliprect.w = w;
   ctx->cliprect.h = h;
}

void
imlib_context_get_cliprect(int *x, int *y, int *w, int *h)
{
   if (!ctx)
      ctx = imlib_context_new();
   *x = ctx->cliprect.x;
   *y = ctx->cliprect.y;
   *w = ctx->cliprect.w;
   *h = ctx->cliprect.h;
}

#ifndef X_DISPLAY_MISSING
void
imlib_context_set_display(Display * display)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->display = display;
}

Display            *
imlib_context_get_display(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->display;
}

void
imlib_context_set_visual(Visual * visual)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->visual = visual;
   ctx->depth = imlib_get_visual_depth(ctx->display, ctx->visual);
}

Visual             *
imlib_context_get_visual(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->visual;
}

void
imlib_context_set_colormap(Colormap colormap)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->colormap = colormap;
}

Colormap
imlib_context_get_colormap(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->colormap;
}

void
imlib_context_set_drawable(Drawable drawable)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->drawable = drawable;
}

Drawable
imlib_context_get_drawable(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->drawable;
}

void
imlib_context_set_mask(Pixmap mask)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->mask = mask;
}

Pixmap
imlib_context_get_mask(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->mask;
}
#endif

void
imlib_context_set_dither_mask(char dither_mask)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->dither_mask = dither_mask;
}

char
imlib_context_get_dither_mask(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->dither_mask;
}

void
imlib_context_set_anti_alias(char anti_alias)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->anti_alias = anti_alias;
}

char
imlib_context_get_anti_alias(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->anti_alias;
}

void
imlib_context_set_dither(char dither)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->dither = dither;
}

char
imlib_context_get_dither(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->dither;
}

void
imlib_context_set_blend(char blend)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->blend = blend;
}

char
imlib_context_get_blend(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->blend;
}

void
imlib_context_set_color_modifier(Imlib_Color_Modifier color_modifier)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->color_modifier = color_modifier;
}

Imlib_Color_Modifier
imlib_context_get_color_modifier(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->color_modifier;
}

void
imlib_context_set_operation(Imlib_Operation operation)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->operation = operation;
}

Imlib_Operation
imlib_context_get_operation(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->operation;
}

void
imlib_context_set_font(Imlib_Font font)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->font = font;
}

Imlib_Font
imlib_context_get_font(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->font;
}

void
imlib_context_set_direction(Imlib_Text_Direction direction)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->direction = direction;
}

void
imlib_context_set_angle(double angle)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->angle = angle;
}

double
imlib_context_get_angle(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->angle;
}

Imlib_Text_Direction
imlib_context_get_direction(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->direction;
}

void
imlib_context_set_color(int red, int green, int blue, int alpha)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->color.red = red;
   ctx->color.green = green;
   ctx->color.blue = blue;
   ctx->color.alpha = alpha;
}

void
imlib_context_get_color(int *red, int *green, int *blue, int *alpha)
{
   if (!ctx)
      ctx = imlib_context_new();
   *red = ctx->color.red;
   *green = ctx->color.green;
   *blue = ctx->color.blue;
   *alpha = ctx->color.alpha;
}

Imlib_Color        *
imlib_context_get_imlib_color(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return &ctx->color;
}

void
imlib_context_set_color_hsva(float hue, float saturation, float value,
                             int alpha)
{
   int                 r, g, b;

   __imlib_hsv_to_rgb(hue, saturation, value, &r, &g, &b);
   imlib_context_set_color(r, g, b, alpha);
}

void
imlib_context_get_color_hsva(float *hue, float *saturation, float *value,
                             int *alpha)
{
   int                 r, g, b, f;
   float               i, j, k, max, min, d;

   imlib_context_get_color(&r, &g, &b, alpha);
   __imlib_rgb_to_hsv(r, g, b, hue, saturation, value);
}

void
imlib_context_set_color_hlsa(float hue, float lightness, float saturation,
                             int alpha)
{
   int                 r, g, b;

   __imlib_hls_to_rgb(hue, lightness, saturation, &r, &g, &b);
   imlib_context_set_color(r, g, b, alpha);
}

void
imlib_context_get_color_hlsa(float *hue, float *lightness, float *saturation,
                             int *alpha)
{
   int                 r, g, b;

   imlib_context_get_color(&r, &g, &b, alpha);
   __imlib_rgb_to_hls(r, g, b, hue, lightness, saturation);
}

void
imlib_context_set_color_cmya(int cyan, int magenta, int yellow, int alpha)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->color.red = 255 - cyan;
   ctx->color.green = 255 - magenta;
   ctx->color.blue = 255 - yellow;
   ctx->color.alpha = alpha;
}

void
imlib_context_get_color_cmya(int *cyan, int *magenta, int *yellow, int *alpha)
{
   if (!ctx)
      ctx = imlib_context_new();
   *cyan = 255 - ctx->color.red;
   *magenta = 255 - ctx->color.green;
   *yellow = 255 - ctx->color.blue;
   *alpha = ctx->color.alpha;
}

void
imlib_context_set_color_range(Imlib_Color_Range color_range)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->color_range = color_range;
}

Imlib_Color_Range
imlib_context_get_color_range(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->color_range;
}

void
imlib_context_set_progress_function(Imlib_Progress_Function progress_function)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->progress_func = progress_function;
}

Imlib_Progress_Function
imlib_context_get_progress_function(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->progress_func;
}

void
imlib_context_set_progress_granularity(char progress_granularity)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->progress_granularity = progress_granularity;
}

char
imlib_context_get_progress_granularity(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->progress_granularity;
}

void
imlib_context_set_image(Imlib_Image image)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->image = image;
}

Imlib_Image
imlib_context_get_image(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->image;
}

void
imlib_context_set_TTF_encoding(Imlib_TTF_Encoding encoding)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->encoding = encoding;
}

Imlib_TTF_Encoding
imlib_context_get_TTF_encoding(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->encoding;
}

/* imlib api */
int
imlib_get_cache_size(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return __imlib_GetCacheSize();
}

void
imlib_set_cache_size(int bytes)
{
   if (!ctx)
      ctx = imlib_context_new();
   __imlib_SetCacheSize(bytes);
}

int
imlib_get_color_usage(void)
{
   if (!ctx)
      ctx = imlib_context_new();
#ifndef X_DISPLAY_MISSING
   return (int)_max_colors;
#else
   return 256;
#endif
}

void
imlib_set_color_usage(int max)
{
   if (!ctx)
      ctx = imlib_context_new();
#ifndef X_DISPLAY_MISSING
   if (max < 2)
      max = 2;
   else if (max > 256)
      max = 256;
   _max_colors = max;
#endif
}

void
imlib_flush_loaders(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   __imlib_RemoveAllLoaders();
   LTDL_Exit();
}

#ifndef X_DISPLAY_MISSING
int
imlib_get_visual_depth(Display * display, Visual * visual)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_visual_depth", "display", display, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_get_visual_depth", "visual", visual, 0);
   return __imlib_XActualDepth(display, visual);
}

Visual             *
imlib_get_best_visual(Display * display, int screen, int *depth_return)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_best_visual", "display", display,
                              NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_get_best_visual", "depth_return",
                              depth_return, NULL);
   return __imlib_BestVisual(display, screen, depth_return);
}
#endif

Imlib_Image
imlib_load_image(const char *file)
{
   Imlib_Image         im = NULL;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_load_image", "file", file, NULL);
   prev_ctxt_image = ctx->image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction) ctx->progress_func,
                          ctx->progress_granularity, 0, 0, NULL);
   ctx->image = prev_ctxt_image;
   return (Imlib_Image) im;
}

Imlib_Image
imlib_load_image_immediately(const char *file)
{
   Imlib_Image         im = NULL;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_immediately", "file", file,
                              NULL);
   prev_ctxt_image = ctx->image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction) ctx->progress_func,
                          ctx->progress_granularity, 1, 0, NULL);
   ctx->image = prev_ctxt_image;
   return (Imlib_Image) im;
}

Imlib_Image
imlib_load_image_without_cache(const char *file)
{
   Imlib_Image         im = NULL;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_without_cache", "file",
                              file, NULL);
   prev_ctxt_image = ctx->image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction) ctx->progress_func,
                          ctx->progress_granularity, 0, 1, NULL);
   ctx->image = prev_ctxt_image;
   return (Imlib_Image) im;
}

Imlib_Image
imlib_load_image_immediately_without_cache(const char *file)
{
   Imlib_Image         im = NULL;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_immediately_without_cache",
                              "file", file, NULL);
   prev_ctxt_image = ctx->image;
   im = __imlib_LoadImage(file, (ImlibProgressFunction) ctx->progress_func,
                          ctx->progress_granularity, 1, 1, NULL);
   ctx->image = prev_ctxt_image;
   return (Imlib_Image) im;
}

Imlib_Image
imlib_load_image_with_error_return(const char *file,
                                   Imlib_Load_Error * error_return)
{
   Imlib_Image         im = NULL;
   ImlibLoadError      er;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_load_image_with_error_return", "file",
                              file, NULL);
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
   prev_ctxt_image = ctx->image;
   im = (Imlib_Image) __imlib_LoadImage(file,
                                        (ImlibProgressFunction)
                                        ctx->progress_func,
                                        ctx->progress_granularity, 1, 0, &er);
   ctx->image = prev_ctxt_image;
   if (im)
      *error_return = IMLIB_LOAD_ERROR_NONE;
   else
     {
        if (er == IMLIB_LOAD_ERROR_NONE)
           *error_return = IMLIB_LOAD_ERROR_NO_LOADER_FOR_FILE_FORMAT;
        else
           *error_return = (Imlib_Load_Error) er;
     }
   return im;
}

void
imlib_free_image(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_free_image", "image", ctx->image);
   __imlib_FreeImage(ctx->image);
   ctx->image = NULL;
}

void
imlib_free_image_and_decache(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_free_image_and_decache", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   SET_FLAG(im->flags, F_INVALID);
   __imlib_FreeImage(im);
   ctx->image = NULL;
}

int
imlib_image_get_width(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_width", "image", ctx->image, 0);
   CAST_IMAGE(im, ctx->image);
   return im->w;
}

int
imlib_image_get_height(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_height", "image", ctx->image, 0);
   CAST_IMAGE(im, ctx->image);
   return im->h;
}

const char         *
imlib_image_get_filename(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_filename", "image", ctx->image,
                              0);
   CAST_IMAGE(im, ctx->image);
   /* strdup() the returned value if you want to alter it! */
   return (const char *)(im->file);
}

DATA32             *
imlib_image_get_data(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_data", "image", ctx->image,
                              NULL);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return NULL;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   return im->data;
}

DATA32             *
imlib_image_get_data_for_reading_only(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_data_for_reading_only",
                              "image", ctx->image, NULL);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return NULL;
   return im->data;
}

void
imlib_image_put_back_data(DATA32 * data)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_put_back_data", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_put_back_data", "data", data);
   CAST_IMAGE(im, ctx->image);
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   data = NULL;
}

char
imlib_image_has_alpha(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_has_alpha", "image", ctx->image, 0);
   CAST_IMAGE(im, ctx->image);
   if (IMAGE_HAS_ALPHA(im))
      return 1;
   return 0;
}

void
imlib_image_set_changes_on_disk(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_never_changes_on_disk", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
   SET_FLAG(im->flags, F_ALWAYS_CHECK_DISK);
}

void
imlib_image_get_border(Imlib_Border * border)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_get_border", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_get_border", "border", border);
   CAST_IMAGE(im, ctx->image);
   border->left = im->border.left;
   border->right = im->border.right;
   border->top = im->border.top;
   border->bottom = im->border.bottom;
}

void
imlib_image_set_border(Imlib_Border * border)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_border", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_set_border", "border", border);
   CAST_IMAGE(im, ctx->image);
   if ((im->border.left == border->left)
       && (im->border.right == border->right)
       && (im->border.top == border->top)
       && (im->border.bottom == border->bottom))
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_format", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_set_format", "format", format);
   CAST_IMAGE(im, ctx->image);
   if (im->format)
      free(im->format);
   if (format)
      im->format = strdup(format);
   else
      im->format = NULL;
   if (!(im->flags & F_FORMAT_IRRELEVANT))
     {
        __imlib_DirtyImage(im);
        __imlib_DirtyPixmapsForImage(im);
     }
}

void
imlib_image_set_irrelevant_format(char irrelevant)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_format", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_border", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_irrelevant_alpha", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if (irrelevant)
     {
        SET_FLAG(im->flags, F_ALPHA_IRRELEVANT);
     }
   else
     {
        UNSET_FLAG(im->flags, F_ALPHA_IRRELEVANT);
     }
}

char               *
imlib_image_format(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_format", "image", ctx->image, NULL);
   CAST_IMAGE(im, ctx->image);
   return im->format;
}

void
imlib_image_set_has_alpha(char has_alpha)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_set_has_alpha", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if (has_alpha)
      SET_FLAG(im->flags, F_HAS_ALPHA);
   else
      UNSET_FLAG(im->flags, F_HAS_ALPHA);
}

#ifndef X_DISPLAY_MISSING
void
imlib_render_pixmaps_for_whole_image(Pixmap * pixmap_return,
                                     Pixmap * mask_return)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image",
                       "pixmap_return", pixmap_return);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image", "mask_return",
                       mask_return);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_CreatePixmapsForImage(ctx->display, ctx->drawable, ctx->visual,
                                 ctx->depth, ctx->colormap, im, pixmap_return,
                                 mask_return, 0, 0, im->w, im->h, im->w,
                                 im->h, 0, ctx->dither, ctx->dither_mask,
                                 ctx->color_modifier);
}

void
imlib_render_pixmaps_for_whole_image_at_size(Pixmap * pixmap_return,
                                             Pixmap * mask_return, int width,
                                             int height)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size",
                       "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size",
                       "pixmap_return", pixmap_return);
   CHECK_PARAM_POINTER("imlib_render_pixmaps_for_whole_image_at_size",
                       "mask_return", mask_return);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);

   if (!(im->data))
      return;
   __imlib_CreatePixmapsForImage(ctx->display, ctx->drawable, ctx->visual,
                                 ctx->depth, ctx->colormap, im, pixmap_return,
                                 mask_return, 0, 0, im->w, im->h, width,
                                 height, ctx->anti_alias, ctx->dither,
                                 ctx->dither_mask, ctx->color_modifier);
}

void
imlib_free_pixmap_and_mask(Pixmap pixmap)
{
   if (!ctx)
      ctx = imlib_context_new();
   __imlib_FreePixmap(ctx->display, pixmap);
}

void
imlib_render_image_on_drawable(int x, int y)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(ctx->display, im, ctx->drawable, ctx->mask,
                       ctx->visual, ctx->colormap, ctx->depth, 0, 0, im->w,
                       im->h, x, y, im->w, im->h, 0, ctx->dither, ctx->blend,
                       ctx->dither_mask, ctx->color_modifier, ctx->operation);
}

void
imlib_render_image_on_drawable_at_size(int x, int y, int width, int height)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_size", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(ctx->display, im, ctx->drawable, ctx->mask,
                       ctx->visual, ctx->colormap, ctx->depth, 0, 0, im->w,
                       im->h, x, y, width, height, ctx->anti_alias,
                       ctx->dither, ctx->blend, ctx->dither_mask,
                       ctx->color_modifier, ctx->operation);
}

void
imlib_render_image_part_on_drawable_at_size(int source_x, int source_y,
                                            int source_width,
                                            int source_height, int x, int y,
                                            int width, int height)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_image_part_on_drawable_at_size", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_RenderImage(ctx->display, im, ctx->drawable, 0, ctx->visual,
                       ctx->colormap, ctx->depth, source_x, source_y,
                       source_width, source_height, x, y, width, height,
                       ctx->anti_alias, ctx->dither, ctx->blend, 0,
                       ctx->color_modifier, ctx->operation);
}

DATA32
imlib_render_get_pixel_color(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return __imlib_RenderGetPixel(ctx->display, ctx->drawable, ctx->visual,
                                 ctx->colormap, ctx->depth,
                                 (DATA8) ctx->color.red,
                                 (DATA8) ctx->color.green,
                                 (DATA8) ctx->color.blue);
}

#endif

void
imlib_blend_image_onto_image(Imlib_Image source_image, char merge_alpha,
                             int source_x, int source_y, int source_width,
                             int source_height, int destination_x,
                             int destination_y, int destination_width,
                             int destination_height)
{
   ImlibImage         *im_src, *im_dst;
   int                 aa;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image", "source_image",
                       source_image);
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image", "image", ctx->image);
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, ctx->image);
   if ((!(im_src->data)) && (im_src->loader) && (im_src->loader->load))
      im_src->loader->load(im_src, NULL, 0, 1);
   if (!(im_src->data))
      return;
   if ((!(im_dst->data)) && (im_dst->loader) && (im_dst->loader->load))
      im_dst->loader->load(im_dst, NULL, 0, 1);
   if (!(im_dst->data))
      return;
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   /* FIXME: hack to get around infinite loops for scaling down too far */
   aa = ctx->anti_alias;
   if ((abs(destination_width) < (source_width >> 7))
       || (abs(destination_height) < (source_height >> 7)))
      aa = 0;
   __imlib_BlendImageToImage(im_src, im_dst, aa, ctx->blend,
                             merge_alpha, source_x, source_y, source_width,
                             source_height, destination_x, destination_y,
                             destination_width, destination_height,
                             ctx->color_modifier, ctx->operation,
                             ctx->cliprect.x, ctx->cliprect.y,
                             ctx->cliprect.w, ctx->cliprect.h);
}

Imlib_Image
imlib_create_image(int width, int height)
{
   DATA32             *data;

   if (!ctx)
      ctx = imlib_context_new();
   if ((width <= 0) || (height <= 0))
      return NULL;
   data = malloc(width * height * sizeof(DATA32));
   if (data)
      return (Imlib_Image) __imlib_CreateImage(width, height, data);
   return NULL;
}

Imlib_Image
imlib_create_image_using_data(int width, int height, DATA32 * data)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_using_data", "data", data,
                              NULL);
   if ((width <= 0) || (height <= 0))
      return NULL;
   im = __imlib_CreateImage(width, height, data);
   if (im)
      SET_FLAG(im->flags, F_DONT_FREE_DATA);
   return (Imlib_Image) im;
}

Imlib_Image
imlib_create_image_using_copied_data(int width, int height, DATA32 * data)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_create_image_using_copied_data", "data",
                              data, NULL);
   if ((width <= 0) || (height <= 0))
      return NULL;
   im = __imlib_CreateImage(width, height, NULL);
   if (!im)
      return NULL;
   im->data = malloc(width * height * sizeof(DATA32));
   if (data)
     {
        memcpy(im->data, data, width * height * sizeof(DATA32));
        return (Imlib_Image) im;
     }
   else
      __imlib_FreeImage(im);
   return NULL;
}

#ifndef X_DISPLAY_MISSING
Imlib_Image
imlib_create_image_from_drawable(Pixmap mask, int x, int y, int width,
                                 int height, char need_to_grab_x)
{
   ImlibImage         *im;
   char                domask = 0;

   if (!ctx)
      ctx = imlib_context_new();
   if (mask)
      domask = 1;
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height * sizeof(DATA32));
   __imlib_GrabDrawableToRGBA(im->data, 0, 0, width, height, ctx->display,
                              ctx->drawable, mask, ctx->visual, ctx->colormap,
                              ctx->depth, x, y, width, height, domask,
                              need_to_grab_x);
   return (Imlib_Image) im;
}

Imlib_Image
imlib_create_image_from_ximage(XImage * image, XImage * mask, int x, int y,
                               int width, int height, char need_to_grab_x)
{
   ImlibImage         *im;
   char                domask = 0;

   if (!ctx)
      ctx = imlib_context_new();
   if (mask)
      domask = 1;
   im = __imlib_CreateImage(width, height, NULL);
   im->data = malloc(width * height * sizeof(DATA32));
   __imlib_GrabXImageToRGBA(im->data, 0, 0, width, height,
                            ctx->display, image, mask, ctx->visual,
                            ctx->depth, x, y, width, height, need_to_grab_x);
   return (Imlib_Image) im;
}

Imlib_Image
imlib_create_scaled_image_from_drawable(Pixmap mask, int source_x,
                                        int source_y, int source_width,
                                        int source_height,
                                        int destination_width,
                                        int destination_height,
                                        char need_to_grab_x,
                                        char get_mask_from_shape)
{
   ImlibImage         *im;
   char                domask = 0, tmpmask = 0;
   int                 x, xx;
   XGCValues           gcv;
   GC                  gc = 0, mgc = 0;
   Pixmap              p, m;

   if (!ctx)
      ctx = imlib_context_new();
   if ((mask) || (get_mask_from_shape))
      domask = 1;
   p = XCreatePixmap(ctx->display, ctx->drawable, destination_width,
                     source_height, ctx->depth);
   gcv.foreground = 0;
   gcv.subwindow_mode = IncludeInferiors;
   if (domask)
     {
        m = XCreatePixmap(ctx->display, ctx->drawable, destination_width,
                          source_height, 1);
        mgc = XCreateGC(ctx->display, m, GCForeground, &gcv);
     }
   else
      m = None;
   gc = XCreateGC(ctx->display, ctx->drawable, GCSubwindowMode, &gcv);
   if ((domask) && (!mask))
     {
        XRectangle         *rect;
        int                 rect_num, rect_ord;

        tmpmask = 1;
        mask =
            XCreatePixmap(ctx->display, ctx->drawable, source_width,
                          source_height, 1);
        rect =
            XShapeGetRectangles(ctx->display, ctx->drawable, ShapeBounding,
                                &rect_num, &rect_ord);
        XFillRectangle(ctx->display, mask, mgc, 0, 0, source_width,
                       source_height);
        if (rect)
          {
             XSetForeground(ctx->display, mgc, 1);
             for (x = 0; x < rect_num; x++)
                XFillRectangle(ctx->display, mask, mgc, rect[x].x, rect[x].y,
                               rect[x].width, rect[x].height);
             XFree(rect);
          }
        /* build mask from window shape rects */
     }
   for (x = 0; x < destination_width; x++)
     {
        xx = (source_width * x) / destination_width;
        XCopyArea(ctx->display, ctx->drawable, p, gc, source_x + xx, 0, 1,
                  source_height, xx, 0);
        if (m != None)
           XCopyArea(ctx->display, mask, m, mgc, xx, 0, 1, source_height, xx,
                     0);
     }
   for (x = 0; x < destination_height; x++)
     {
        xx = (source_height * x) / destination_height;
        XCopyArea(ctx->display, ctx->drawable, p, gc, 0, source_y + xx,
                  destination_width, 1, 0, xx);
        if (m != None)
           XCopyArea(ctx->display, mask, m, mgc, 0, source_y + xx,
                     destination_width, 1, 0, xx);
     }
   im = __imlib_CreateImage(destination_width, destination_height, NULL);
   im->data = malloc(destination_width * destination_height * sizeof(DATA32));
   __imlib_GrabDrawableToRGBA(im->data, 0, 0, destination_width,
                              destination_height, ctx->display, p, m,
                              ctx->visual, ctx->colormap, ctx->depth, 0, 0,
                              source_width, source_height, domask,
                              need_to_grab_x);
   XFreePixmap(ctx->display, p);
   if (m != None)
     {
        XFreeGC(ctx->display, mgc);
        XFreePixmap(ctx->display, m);
        if (tmpmask)
           XFreePixmap(ctx->display, mask);
     }
   XFreeGC(ctx->display, gc);
   return (Imlib_Image) im;
}

char
imlib_copy_drawable_to_image(Pixmap mask, int x, int y, int width, int height,
                             int destination_x, int destination_y,
                             char need_to_grab_x)
{
   ImlibImage         *im;
   char                domask = 0;
   int                 pre_adj;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_copy_drawable_to_image", "image",
                              ctx->image, 0);
   if (mask)
      domask = 1;
   CAST_IMAGE(im, ctx->image);

   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return 0;

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
                                     im->w, im->h, ctx->display,
                                     ctx->drawable, mask, ctx->visual,
                                     ctx->colormap, ctx->depth, x, y, width,
                                     height, domask, need_to_grab_x);
}
#endif

Imlib_Image
imlib_clone_image(void)
{
   ImlibImage         *im, *im_old;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_clone_image", "image", ctx->image, NULL);
   CAST_IMAGE(im_old, ctx->image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   im = __imlib_CreateImage(im_old->w, im_old->h, NULL);
   if (!(im))
      return NULL;
   im->data = malloc(im->w * im->h * sizeof(DATA32));
   if (!(im->data))
     {
        __imlib_FreeImage(im);
        return NULL;
     }
   memcpy(im->data, im_old->data, im->w * im->h * sizeof(DATA32));
   im->flags = im_old->flags;
   SET_FLAG(im->flags, F_UNCACHEABLE);
   im->moddate = im_old->moddate;
   im->border = im_old->border;
   im->loader = im_old->loader;
   if (im_old->format)
     {
        im->format = malloc(strlen(im_old->format) + 1);
        strcpy(im->format, im_old->format);
     }
   if (im_old->file)
     {
        im->file = malloc(strlen(im_old->file) + 1);
        strcpy(im->file, im_old->file);
     }
   return (Imlib_Image) im;
}

Imlib_Image
imlib_create_cropped_image(int x, int y, int width, int height)
{
   ImlibImage         *im, *im_old;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_create_cropped_image", "image",
                              ctx->image, NULL);
   CAST_IMAGE(im_old, ctx->image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   im = __imlib_CreateImage(abs(width), abs(height), NULL);
   im->data = malloc(abs(width * height) * sizeof(DATA32));
   if (!(im->data))
     {
        __imlib_FreeImage(im);
        return NULL;
     }
   __imlib_BlendImageToImage(im_old, im, 0, 0, 0, x, y, abs(width),
                             abs(height), 0, 0, width, height, NULL,
                             IMLIB_OP_COPY,
                             ctx->cliprect.x, ctx->cliprect.y,
                             ctx->cliprect.w, ctx->cliprect.h);
   return (Imlib_Image) im;
}

Imlib_Image
imlib_create_cropped_scaled_image(int source_x, int source_y,
                                  int source_width, int source_height,
                                  int destination_width, int destination_height)
{
   ImlibImage         *im, *im_old;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_create_cropped_scaled_image", "image",
                              ctx->image, NULL);
   CAST_IMAGE(im_old, ctx->image);
   if ((!(im_old->data)) && (im_old->loader) && (im_old->loader->load))
      im_old->loader->load(im_old, NULL, 0, 1);
   if (!(im_old->data))
      return NULL;
   im = __imlib_CreateImage(abs(destination_width), abs(destination_height),
                            NULL);
   im->data =
       malloc(abs(destination_width * destination_height) * sizeof(DATA32));
   if (!(im->data))
     {
        __imlib_FreeImage(im);
        return NULL;
     }
   if (IMAGE_HAS_ALPHA(im_old))
     {
        SET_FLAG(im->flags, F_HAS_ALPHA);
        __imlib_BlendImageToImage(im_old, im, ctx->anti_alias, 0, 1, source_x,
                                  source_y, source_width, source_height, 0, 0,
                                  destination_width, destination_height, NULL,
                                  IMLIB_OP_COPY,
                                  ctx->cliprect.x, ctx->cliprect.y,
                                  ctx->cliprect.w, ctx->cliprect.h);
     }
   else
     {
        __imlib_BlendImageToImage(im_old, im, ctx->anti_alias, 0, 0, source_x,
                                  source_y, source_width, source_height, 0, 0,
                                  destination_width, destination_height, NULL,
                                  IMLIB_OP_COPY,
                                  ctx->cliprect.x, ctx->cliprect.y,
                                  ctx->cliprect.w, ctx->cliprect.h);
     }
   return (Imlib_Image) im;
}

Imlib_Updates
imlib_updates_clone(Imlib_Updates updates)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   return (Imlib_Updates) __imlib_DupUpdates(u);
}

Imlib_Updates
imlib_update_append_rect(Imlib_Updates updates, int x, int y, int w, int h)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   return (Imlib_Updates) __imlib_AddUpdate(u, x, y, w, h);
}

Imlib_Updates
imlib_updates_merge(Imlib_Updates updates, int w, int h)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   return (Imlib_Updates) __imlib_MergeUpdate(u, w, h, 0);
}

Imlib_Updates
imlib_updates_merge_for_rendering(Imlib_Updates updates, int w, int h)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   return (Imlib_Updates) __imlib_MergeUpdate(u, w, h, 3);
}

void
imlib_updates_free(Imlib_Updates updates)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   __imlib_FreeUpdates(u);
}

Imlib_Updates
imlib_updates_get_next(Imlib_Updates updates)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   return (Imlib_Updates) (u->next);
}

void
imlib_updates_get_coordinates(Imlib_Updates updates, int *x_return,
                              int *y_return, int *width_return,
                              int *height_return)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_updates_get_coordinates", "updates", updates);
   u = (ImlibUpdate *) updates;
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
imlib_updates_set_coordinates(Imlib_Updates updates, int x, int y, int width,
                              int height)
{
   ImlibUpdate        *u;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_updates_set_coordinates", "updates", updates);
   u = (ImlibUpdate *) updates;
   u->x = x;
   u->y = y;
   u->w = width;
   u->h = height;
}

#ifndef X_DISPLAY_MISSING
void
imlib_render_image_updates_on_drawable(Imlib_Updates updates, int x, int y)
{
   ImlibUpdate        *u;
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_image_updates_on_drawable", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
   u = (ImlibUpdate *) updates;
   if (!updates)
      return;
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_SetMaxXImageCount(ctx->display, 10);
   for (; u; u = u->next)
     {
        __imlib_RenderImage(ctx->display, im, ctx->drawable, 0, ctx->visual,
                            ctx->colormap, ctx->depth, u->x, u->y, u->w, u->h,
                            x + u->x, y + u->y, u->w, u->h, 0, ctx->dither, 0,
                            0, ctx->color_modifier, OP_COPY);
     }
   __imlib_SetMaxXImageCount(ctx->display, 0);
}
#endif

Imlib_Updates
imlib_updates_init(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return (Imlib_Updates) NULL;
}

Imlib_Updates
imlib_updates_append_updates(Imlib_Updates updates,
                             Imlib_Updates appended_updates)
{
   ImlibUpdate        *u, *uu;

   if (!ctx)
      ctx = imlib_context_new();
   u = (ImlibUpdate *) updates;
   uu = (ImlibUpdate *) appended_updates;
   if (!uu)
      return (Imlib_Updates) u;
   if (!u)
      return (Imlib_Updates) uu;
   while (u)
     {
        if (!(u->next))
          {
             u->next = uu;
             return updates;
          }
        u = u->next;
     }
   return (Imlib_Updates) u;
}

void
imlib_image_flip_horizontal(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_flip_horizontal", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_flip_vertical", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_flip_diagonal", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_orientate", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   switch (orientation)
     {
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_blur", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CAST_IMAGE(im, ctx->image);
   CHECK_PARAM_POINTER("imlib_image_sharpen", "image", ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_tile_horizontal", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_tile_vertical", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_tile", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   return imlib_font_load_joined(font_name);
}

void
imlib_free_font(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_free_font", "font", ctx->font);
   imlib_font_free(ctx->font);
   ctx->font = NULL;
}

void
imlib_text_draw(int x, int y, const char *text)
{
   if (!ctx)
      ctx = imlib_context_new();
   imlib_text_draw_with_return_metrics(x, y, text, NULL, NULL, NULL, NULL);
}

void
imlib_text_draw_with_return_metrics(int x, int y, const char *text,
                                    int *width_return, int *height_return,
                                    int *horizontal_advance_return,
                                    int *vertical_advance_return)
{
   ImlibImage         *im;
   ImlibFont          *fn;
   char               *tt;
   int                 tx, ty;
   int                 dir;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "font",
                       ctx->font);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_text_draw_with_return_metrics", "text", text);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   fn = (ImlibFont *) ctx->font;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);

   dir = ctx->direction;
   if (ctx->direction == IMLIB_TEXT_TO_ANGLE && ctx->angle == 0.0)
      dir = IMLIB_TEXT_TO_RIGHT;

   imlib_render_str(im, fn, x, y, text, (DATA8) ctx->color.red,
                    (DATA8) ctx->color.green, (DATA8) ctx->color.blue,
                    (DATA8) ctx->color.alpha, (char)dir,
                    ctx->angle, width_return, height_return, 0,
                    horizontal_advance_return, vertical_advance_return,
                    ctx->operation,
                    ctx->cliprect.x, ctx->cliprect.y,
                    ctx->cliprect.w, ctx->cliprect.h);
}

void
imlib_get_text_size(const char *text, int *width_return, int *height_return)
{
   ImlibFont          *fn;
   int                 w, h;
   int                 dir;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_get_text_size", "font", ctx->font);
   CHECK_PARAM_POINTER("imlib_get_text_size", "text", text);
   fn = (ImlibFont *) ctx->font;

   dir = ctx->direction;
   if (ctx->direction == IMLIB_TEXT_TO_ANGLE && ctx->angle == 0.0)
      dir = IMLIB_TEXT_TO_RIGHT;

   imlib_font_query_size(fn, text, &w, &h);

   switch (dir)
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
       case IMLIB_TEXT_TO_ANGLE:
          if (width_return || height_return)
            {
               double              sa, ca;

               sa = sin(ctx->angle);
               ca = cos(ctx->angle);

               if (width_return)
                 {
                    double              x1, x2, xt;

                    x1 = x2 = 0.0;
                    xt = ca * w;
                    if (xt < x1)
                       x1 = xt;
                    if (xt > x2)
                       x2 = xt;
                    xt = -(sa * h);
                    if (xt < x1)
                       x1 = xt;
                    if (xt > x2)
                       x2 = xt;
                    xt = ca * w - sa * h;
                    if (xt < x1)
                       x1 = xt;
                    if (xt > x2)
                       x2 = xt;
                    *width_return = (int)(x2 - x1);
                 }
               if (height_return)
                 {
                    double              y1, y2, yt;

                    y1 = y2 = 0.0;
                    yt = sa * w;
                    if (yt < y1)
                       y1 = yt;
                    if (yt > y2)
                       y2 = yt;
                    yt = ca * h;
                    if (yt < y1)
                       y1 = yt;
                    if (yt > y2)
                       y2 = yt;
                    yt = sa * w + ca * h;
                    if (yt < y1)
                       y1 = yt;
                    if (yt > y2)
                       y2 = yt;
                    *height_return = (int)(y2 - y1);
                 }
            }
          break;
       default:
          break;
     }
}

void
imlib_get_text_advance(const char *text, int *horizontal_advance_return,
                       int *vertical_advance_return)
{
   ImlibFont          *fn;
   int                 w, h;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_get_text_advance", "font", ctx->font);
   CHECK_PARAM_POINTER("imlib_get_text_advance", "text", text);
   fn = (ImlibFont *) ctx->font;
   imlib_font_query_advance(fn, text, &w, &h);
   if (horizontal_advance_return)
      *horizontal_advance_return = w;
   if (vertical_advance_return)
      *vertical_advance_return = h;
}

int
imlib_get_text_inset(const char *text)
{
   ImlibFont          *fn;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_text_advance", "font", ctx->font, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_get_text_advance", "text", text, 0);
   fn = (ImlibFont *) ctx->font;
   return imlib_font_query_inset(fn, text);
}

void
imlib_add_path_to_font_path(const char *path)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_add_path_to_font_path", "path", path);
   if (!imlib_font_path_exists(path))
      imlib_font_add_font_path(path);
}

void
imlib_remove_path_from_font_path(const char *path)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_remove_path_from_font_path", "path", path);
   imlib_font_del_font_path(path);
}

char              **
imlib_list_font_path(int *number_return)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_list_font_path", "number_return",
                              number_return, NULL);
   return imlib_font_list_font_path(number_return);
}

int
imlib_text_get_index_and_location(const char *text, int x, int y,
                                  int *char_x_return, int *char_y_return,
                                  int *char_width_return,
                                  int *char_height_return)
{
   ImlibFont          *fn;
   int                 w, h, cx, cy, cw, ch, cp, xx, yy;
   int                 dir;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_text_get_index_and_location", "font",
                              ctx->font, -1);
   CHECK_PARAM_POINTER_RETURN("imlib_text_get_index_and_location", "text",
                              text, -1);
   fn = (ImlibFont *) ctx->font;

   dir = ctx->direction;
   if (ctx->direction == IMLIB_TEXT_TO_ANGLE && ctx->angle == 0.0)
      dir = IMLIB_TEXT_TO_RIGHT;

   imlib_get_text_size(text, &w, &h);

   switch (dir)
     {
       case IMLIB_TEXT_TO_RIGHT:
          xx = x;
          yy = y;
          break;
       case IMLIB_TEXT_TO_LEFT:
          xx = w - x;
          yy = h - y;
          break;
       case IMLIB_TEXT_TO_DOWN:
          xx = y;
          yy = w - x;
          break;
       case IMLIB_TEXT_TO_UP:
          xx = h - y;
          yy = x;
          break;
       default:
          return -1;
     }

   cp = imlib_font_query_text_at_pos(fn, text, xx, yy, &cx, &cy, &cw, &ch);

   switch (dir)
     {
       case IMLIB_TEXT_TO_RIGHT:
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
       case IMLIB_TEXT_TO_LEFT:
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

void
imlib_text_get_location_at_index(const char *text, int index,
                                 int *char_x_return, int *char_y_return,
                                 int *char_width_return,
                                 int *char_height_return)
{
   ImlibFont          *fn;
   int                 cx, cy, cw, ch, w, h;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_text_get_index_and_location", "font", ctx->font);
   CHECK_PARAM_POINTER("imlib_text_get_index_and_location", "text", text);
   fn = (ImlibFont *) ctx->font;

   imlib_font_query_char_coords(fn, text, index, &cx, &cy, &cw, &ch);

   imlib_get_text_size(text, &w, &h);

   switch (ctx->direction)
     {
       case IMLIB_TEXT_TO_RIGHT:
          if (char_x_return)
             *char_x_return = cx;
          if (char_y_return)
             *char_y_return = cy;
          if (char_width_return)
             *char_width_return = cw;
          if (char_height_return)
             *char_height_return = ch;
          return;
          break;
       case IMLIB_TEXT_TO_LEFT:
          cx = 1 + w - cx - cw;
          if (char_x_return)
             *char_x_return = cx;
          if (char_y_return)
             *char_y_return = cy;
          if (char_width_return)
             *char_width_return = cw;
          if (char_height_return)
             *char_height_return = ch;
          return;
          break;
       case IMLIB_TEXT_TO_DOWN:
          if (char_x_return)
             *char_x_return = cy;
          if (char_y_return)
             *char_y_return = cx;
          if (char_width_return)
             *char_width_return = ch;
          if (char_height_return)
             *char_height_return = cw;
          return;
          break;
       case IMLIB_TEXT_TO_UP:
          cy = 1 + h - cy - ch;
          if (char_x_return)
             *char_x_return = cy;
          if (char_y_return)
             *char_y_return = cx;
          if (char_width_return)
             *char_width_return = ch;
          if (char_height_return)
             *char_height_return = cw;
          return;
          break;
       default:
          return;
          break;
     }
}

char              **
imlib_list_fonts(int *number_return)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_list_fonts", "number_return",
                              number_return, NULL);
   return imlib_font_list_fonts(number_return);
}

void
imlib_free_font_list(char **font_list, int number)
{
   __imlib_FileFreeDirList(font_list, number);
}

int
imlib_get_font_cache_size(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return imlib_font_cache_get();
}

void
imlib_set_font_cache_size(int bytes)
{
   if (!ctx)
      ctx = imlib_context_new();
   imlib_font_cache_set(bytes);
}

void
imlib_flush_font_cache(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   imlib_font_flush();
}

int
imlib_get_font_ascent(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_ascent", "font", ctx->font, 0);
   return imlib_font_ascent_get(ctx->font);
}

int
imlib_get_font_descent(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_ascent", "font", ctx->font, 0);
   return imlib_font_descent_get(ctx->font);
}

int
imlib_get_maximum_font_ascent(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_ascent", "font", ctx->font, 0);
   return imlib_font_max_ascent_get(ctx->font);
}

int
imlib_get_maximum_font_descent(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_get_font_ascent", "font", ctx->font, 0);
   return imlib_font_max_descent_get(ctx->font);
}

Imlib_Color_Modifier
imlib_create_color_modifier(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return (Imlib_Color_Modifier) __imlib_CreateCmod();
}

void
imlib_free_color_modifier(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_free_color_modifier", "color_modifier",
                       ctx->color_modifier);
   __imlib_FreeCmod((ImlibColorModifier *) ctx->color_modifier);
   ctx->color_modifier = NULL;
}

void
imlib_modify_color_modifier_gamma(double gamma_value)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_gamma", "color_modifier",
                       ctx->color_modifier);
   __imlib_CmodModGamma((ImlibColorModifier *) ctx->color_modifier,
                        gamma_value);
}

void
imlib_modify_color_modifier_brightness(double brightness_value)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_brightness",
                       "color_modifier", ctx->color_modifier);
   __imlib_CmodModBrightness((ImlibColorModifier *) ctx->color_modifier,
                             brightness_value);
}

void
imlib_modify_color_modifier_contrast(double contrast_value)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_modify_color_modifier_contrast",
                       "color_modifier", ctx->color_modifier);
   __imlib_CmodModContrast((ImlibColorModifier *) ctx->color_modifier,
                           contrast_value);
}

void
imlib_set_color_modifier_tables(DATA8 * red_table, DATA8 * green_table,
                                DATA8 * blue_table, DATA8 * alpha_table)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_set_color_modifier_tables", "color_modifier",
                       ctx->color_modifier);
   __imlib_CmodSetTables((ImlibColorModifier *) ctx->color_modifier,
                         red_table, green_table, blue_table, alpha_table);
}

void
imlib_get_color_modifier_tables(DATA8 * red_table, DATA8 * green_table,
                                DATA8 * blue_table, DATA8 * alpha_table)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_get_color_modifier_tables", "color_modifier",
                       ctx->color_modifier);
   __imlib_CmodGetTables((ImlibColorModifier *) ctx->color_modifier,
                         red_table, green_table, blue_table, alpha_table);
}

void
imlib_reset_color_modifier(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_rset_color_modifier", "color_modifier",
                       ctx->color_modifier);
   __imlib_CmodReset((ImlibColorModifier *) ctx->color_modifier);
}

void
imlib_apply_color_modifier(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_apply_color_modifier", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_apply_color_modifier", "color_modifier",
                       ctx->color_modifier);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_DataCmodApply(im->data, im->w, im->h, 0, &(im->flags),
                         (ImlibColorModifier *) ctx->color_modifier);
}

void
imlib_apply_color_modifier_to_rectangle(int x, int y, int width, int height)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_apply_color_modifier_to_rectangle", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_apply_color_modifier_to_rectangle",
                       "color_modifier", ctx->color_modifier);
   CAST_IMAGE(im, ctx->image);
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
                         im->w - width, &(im->flags),
                         (ImlibColorModifier *) ctx->color_modifier);
}

Imlib_Updates
imlib_image_draw_pixel(int x, int y, char make_updates)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_draw_pixel", "image", ctx->image,
                              NULL);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return NULL;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   if (ctx->cliprect.w == 0)
     {
        __imlib_draw_set_point(im, x, y,
                               (DATA8) ctx->color.red,
                               (DATA8) ctx->color.green,
                               (DATA8) ctx->color.blue,
                               (DATA8) ctx->color.alpha, ctx->operation);
     }
   else
     {
        __imlib_draw_set_point_clipped(im, x, y,
                                       ctx->cliprect.x,
                                       ctx->cliprect.x +
                                       ctx->cliprect.w - 1,
                                       ctx->cliprect.y,
                                       ctx->cliprect.y +
                                       ctx->cliprect.h - 1,
                                       (DATA8) ctx->color.red,
                                       (DATA8) ctx->color.green,
                                       (DATA8) ctx->color.blue,
                                       (DATA8) ctx->color.alpha,
                                       ctx->operation);
     }
   if (!make_updates)
      return (Imlib_Updates) NULL;
   return (Imlib_Updates) __imlib_AddUpdate(NULL, x, y, 1, 1);
}

Imlib_Updates
imlib_image_draw_line(int x1, int y1, int x2, int y2, char make_updates)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_draw_line", "image", ctx->image,
                              NULL);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return NULL;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   if (ctx->cliprect.w)
     {
        return (Imlib_Updates) __imlib_draw_line_clipped(im, x1, y1, x2, y2,
                                                         ctx->cliprect.x,
                                                         ctx->cliprect.x +
                                                         ctx->cliprect.w - 1,
                                                         ctx->cliprect.y,
                                                         ctx->cliprect.y +
                                                         ctx->cliprect.h - 1,
                                                         (DATA8) ctx->color.red,
                                                         (DATA8) ctx->color.
                                                         green,
                                                         (DATA8) ctx->color.
                                                         blue,
                                                         (DATA8) ctx->color.
                                                         alpha, ctx->operation,
                                                         (char)make_updates);
     }
   else
     {
        return (Imlib_Updates) __imlib_draw_line(im, x1, y1, x2, y2,
                                                 (DATA8) ctx->color.red,
                                                 (DATA8) ctx->color.green,
                                                 (DATA8) ctx->color.blue,
                                                 (DATA8) ctx->color.alpha,
                                                 ctx->operation,
                                                 (char)make_updates);
     }
}

void
imlib_image_draw_rectangle(int x, int y, int width, int height)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_draw_rectangle", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   if (ctx->cliprect.w)
     {
        __imlib_draw_box_clipped(im, x, y, width, height, ctx->cliprect.x,
                                 ctx->cliprect.x + ctx->cliprect.w,
                                 ctx->cliprect.y,
                                 ctx->cliprect.y + ctx->cliprect.h,
                                 ctx->color.red, ctx->color.green,
                                 ctx->color.blue, ctx->color.alpha,
                                 ctx->operation);
     }
   else
     {
        __imlib_draw_box(im, x, y, width, height, ctx->color.red,
                         ctx->color.green, ctx->color.blue, ctx->color.alpha,
                         ctx->operation);
     }
}

void
imlib_image_fill_rectangle(int x, int y, int width, int height)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_fill_rectangle", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   if (ctx->cliprect.w)
     {
        __imlib_draw_filled_box_clipped(im, x, y, width, height,
                                        ctx->cliprect.x,
                                        ctx->cliprect.x + ctx->cliprect.w,
                                        ctx->cliprect.y,
                                        ctx->cliprect.y + ctx->cliprect.h,
                                        ctx->color.red, ctx->color.green,
                                        ctx->color.blue, ctx->color.alpha,
                                        ctx->operation);
     }
   else
     {
        __imlib_draw_filled_box(im, x, y, width, height, ctx->color.red,
                                ctx->color.green, ctx->color.blue,
                                ctx->color.alpha, ctx->operation);
     }
}

void
imlib_image_copy_alpha_to_image(Imlib_Image image_source, int x, int y)
{
   ImlibImage         *im, *im2;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_to_image", "image_source",
                       image_source);
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_to_image", "image_destination",
                       ctx->image);
   CAST_IMAGE(im, image_source);
   CAST_IMAGE(im2, ctx->image);
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
imlib_image_copy_alpha_rectangle_to_image(Imlib_Image image_source, int x,
                                          int y, int width, int height,
                                          int destination_x, int destination_y)
{
   ImlibImage         *im, *im2;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_rectangle_to_image",
                       "image_source", image_source);
   CHECK_PARAM_POINTER("imlib_image_copy_alpha_rectangle_to_image",
                       "image_destination", ctx->image);
   CAST_IMAGE(im, image_source);
   CAST_IMAGE(im2, ctx->image);
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
   ImlibImage         *im;
   int                 xx, yy, w, h, nx, ny;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_scroll_rect", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
imlib_image_copy_rect(int x, int y, int width, int height, int new_x, int new_y)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_copy_rect", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
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
   if (!ctx)
      ctx = imlib_context_new();
   return (Imlib_Color_Range) __imlib_CreateRange();
}

void
imlib_free_color_range(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_free_color_range", "color_range",
                       ctx->color_range);
   __imlib_FreeRange((ImlibRange *) ctx->color_range);
   ctx->color_range = NULL;
}

void
imlib_add_color_to_color_range(int distance_away)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_add_color_to_color_range", "color_range",
                       ctx->color_range);
   __imlib_AddRangeColor((ImlibRange *) ctx->color_range, ctx->color.red,
                         ctx->color.green, ctx->color.blue, ctx->color.alpha,
                         distance_away);
}

void
imlib_image_fill_color_range_rectangle(int x, int y, int width, int height,
                                       double angle)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle",
                       "color_range", ctx->color_range);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_DrawGradient(im, x, y, width, height,
                        (ImlibRange *) ctx->color_range, angle,
                        ctx->operation,
                        ctx->cliprect.x, ctx->cliprect.y,
                        ctx->cliprect.w, ctx->cliprect.h);
}

void
imlib_image_fill_hsva_color_range_rectangle(int x, int y, int width, int height,
                                            double angle)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_image_fill_color_range_rectangle",
                       "color_range", ctx->color_range);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_DrawHsvaGradient(im, x, y, width, height,
                            (ImlibRange *) ctx->color_range, angle,
                            ctx->operation,
                            ctx->cliprect.x, ctx->cliprect.y,
                            ctx->cliprect.w, ctx->cliprect.h);
}

void
imlib_image_query_pixel(int x, int y, Imlib_Color * color_return)
{
   ImlibImage         *im;
   DATA32             *p;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "color_return", color_return);
   CAST_IMAGE(im, ctx->image);
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
imlib_image_query_pixel_hsva(int x, int y, float *hue, float *saturation,
                             float *value, int *alpha)
{
   ImlibImage         *im;
   DATA32             *p;
   int                 r, g, b, f;
   float               i, j, k, max, min, d;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if ((x < 0) || (x >= im->w) || (y < 0) || (y >= im->h))
     {
        *hue = 0;
        *saturation = 0;
        *value = 0;
        *alpha = 0;
        return;
     }
   p = im->data + (im->w * y) + x;
   r = ((*p) >> 16) & 0xff;
   g = ((*p) >> 8) & 0xff;
   b = (*p) & 0xff;
   *alpha = ((*p) >> 24) & 0xff;

   __imlib_rgb_to_hsv(r, g, b, hue, saturation, value);
}

void
imlib_image_query_pixel_hlsa(int x, int y, float *hue, float *lightness,
                             float *saturation, int *alpha)
{
   ImlibImage         *im;
   DATA32             *p;
   int                 r, g, b, f;
   float               i, j, k, max, min, d;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if ((x < 0) || (x >= im->w) || (y < 0) || (y >= im->h))
     {
        *hue = 0;
        *lightness = 0;
        *saturation = 0;
        *alpha = 0;
        return;
     }
   p = im->data + (im->w * y) + x;
   r = ((*p) >> 16) & 0xff;
   g = ((*p) >> 8) & 0xff;
   b = (*p) & 0xff;
   *alpha = ((*p) >> 24) & 0xff;

   __imlib_rgb_to_hls(r, g, b, hue, lightness, saturation);
}

void
imlib_image_query_pixel_cmya(int x, int y, int *cyan, int *magenta, int *yellow,
                             int *alpha)
{
   ImlibImage         *im;
   DATA32             *p;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_query_pixel", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   if ((x < 0) || (x >= im->w) || (y < 0) || (y >= im->h))
     {
        *cyan = 0;
        *magenta = 0;
        *yellow = 0;
        *alpha = 0;
        return;
     }
   p = im->data + (im->w * y) + x;
   *cyan = 255 - (((*p) >> 16) & 0xff);
   *magenta = 255 - (((*p) >> 8) & 0xff);
   *yellow = 255 - ((*p) & 0xff);
   *alpha = ((*p) >> 24) & 0xff;
}

void
imlib_image_attach_data_value(const char *key, void *data, int value,
                              Imlib_Internal_Data_Destructor_Function
                              destructor_function)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_attach_data_value", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_attach_data_value", "key", key);
   CAST_IMAGE(im, ctx->image);
   __imlib_AttachTag(im, key, value, data,
                     (ImlibDataDestructorFunction) destructor_function);
}

void               *
imlib_image_get_attached_data(const char *key)
{
   ImlibImageTag      *t;
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_data", "image",
                              ctx->image, NULL);
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_data", "key", key,
                              NULL);
   CAST_IMAGE(im, ctx->image);
   t = __imlib_GetTag(im, key);
   if (t)
      return t->data;
   return NULL;
}

int
imlib_image_get_attached_value(const char *key)
{
   ImlibImageTag      *t;
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_value", "image",
                              ctx->image, 0);
   CHECK_PARAM_POINTER_RETURN("imlib_image_get_attached_value", "key", key, 0);
   CAST_IMAGE(im, ctx->image);
   t = __imlib_GetTag(im, key);
   if (t)
      return t->val;
   return 0;
}

void
imlib_image_remove_attached_data_value(const char *key)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_remove_attached_data_value", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_image_remove_attached_data_value", "key", key);
   CAST_IMAGE(im, ctx->image);
   __imlib_RemoveTag(im, key);
}

void
imlib_image_remove_and_free_attached_data_value(const char *key)
{
   ImlibImageTag      *t;
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_remove_and_free_attached_data_value",
                       "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_remove_and_free_attached_data_value",
                       "key", key);
   CAST_IMAGE(im, ctx->image);
   t = __imlib_RemoveTag(im, key);
   __imlib_FreeTag(im, t);
}

void
imlib_save_image(const char *filename)
{
   ImlibImage         *im;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_save_image", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_save_image", "filename", filename);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return;
   prev_ctxt_image = ctx->image;
   __imlib_SaveImage(im, filename, (ImlibProgressFunction) ctx->progress_func,
                     ctx->progress_granularity, NULL);
   ctx->image = prev_ctxt_image;
}

void
imlib_save_image_with_error_return(const char *filename,
                                   Imlib_Load_Error * error_return)
{
   ImlibImage         *im;
   Imlib_Image         prev_ctxt_image;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "image",
                       ctx->image);
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "filename",
                       filename);
   CHECK_PARAM_POINTER("imlib_save_image_with_error_return", "error_return",
                       error_return);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!im->data)
      return;
   prev_ctxt_image = ctx->image;
   __imlib_SaveImage(im, filename, (ImlibProgressFunction) ctx->progress_func,
                     ctx->progress_granularity, error_return);
   ctx->image = prev_ctxt_image;
}

Imlib_Image
imlib_create_rotated_image(double angle)
{
   ImlibImage         *im, *im_old;
   int                 x, y, dx, dy, sz;
   double              x1, y1, d;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_create_rotated_image", "image",
                              ctx->image, NULL);
   CAST_IMAGE(im_old, ctx->image);
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

   if (ctx->anti_alias)
     {
#ifdef DO_MMX_ASM
        if (__imlib_get_cpuid() & CPUID_MMX)
           __imlib_mmx_RotateAA(im_old->data, im->data, im_old->w, im_old->w,
                                im_old->h, im->w, sz, sz, x, y, dx, dy, -dy,
                                dx);
        else
#endif
           __imlib_RotateAA(im_old->data, im->data, im_old->w, im_old->w,
                            im_old->h, im->w, sz, sz, x, y, dx, dy, -dy, dx);
     }
   else
     {
        __imlib_RotateSample(im_old->data, im->data, im_old->w, im_old->w,
                             im_old->h, im->w, sz, sz, x, y, dx, dy, -dy, dx);
     }
   SET_FLAG(im->flags, F_HAS_ALPHA);

   return (Imlib_Image) im;
}

void
imlib_blend_image_onto_image_at_angle(Imlib_Image source_image,
                                      char merge_alpha, int source_x,
                                      int source_y, int source_width,
                                      int source_height, int destination_x,
                                      int destination_y, int angle_x,
                                      int angle_y)
{
   ImlibImage         *im_src, *im_dst;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image_at_angle",
                       "source_image", source_image);
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image_at_angle", "image",
                       ctx->image);
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, ctx->image);
   if ((!(im_src->data)) && (im_src->loader) && (im_src->loader->load))
      im_src->loader->load(im_src, NULL, 0, 1);
   if (!(im_src->data))
      return;
   if ((!(im_dst->data)) && (im_dst->loader) && (im_dst->loader->load))
      im_dst->loader->load(im_dst, NULL, 0, 1);
   if (!(im_dst->data))
      return;
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   __imlib_BlendImageToImageSkewed(im_src, im_dst, ctx->anti_alias,
                                   ctx->blend, merge_alpha, source_x,
                                   source_y, source_width, source_height,
                                   destination_x, destination_y, angle_x,
                                   angle_y, 0, 0, ctx->color_modifier,
                                   ctx->operation,
                                   ctx->cliprect.x, ctx->cliprect.y,
                                   ctx->cliprect.w, ctx->cliprect.h);
}

void
imlib_blend_image_onto_image_skewed(Imlib_Image source_image,
                                    char merge_alpha, int source_x,
                                    int source_y, int source_width,
                                    int source_height, int destination_x,
                                    int destination_y, int h_angle_x,
                                    int h_angle_y, int v_angle_x, int v_angle_y)
{
   ImlibImage         *im_src, *im_dst;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image_skewed", "source_image",
                       source_image);
   CHECK_PARAM_POINTER("imlib_blend_image_onto_image_skewed", "image",
                       ctx->image);
   CAST_IMAGE(im_src, source_image);
   CAST_IMAGE(im_dst, ctx->image);
   if ((!(im_src->data)) && (im_src->loader) && (im_src->loader->load))
      im_src->loader->load(im_src, NULL, 0, 1);
   if (!(im_src->data))
      return;
   if ((!(im_dst->data)) && (im_dst->loader) && (im_dst->loader->load))
      im_dst->loader->load(im_dst, NULL, 0, 1);
   if (!(im_dst->data))
      return;
   __imlib_DirtyImage(im_dst);
   __imlib_DirtyPixmapsForImage(im_dst);
   __imlib_BlendImageToImageSkewed(im_src, im_dst, ctx->anti_alias,
                                   ctx->blend, merge_alpha, source_x,
                                   source_y, source_width, source_height,
                                   destination_x, destination_y, h_angle_x,
                                   h_angle_y, v_angle_x, v_angle_y,
                                   ctx->color_modifier, ctx->operation,
                                   ctx->cliprect.x, ctx->cliprect.y,
                                   ctx->cliprect.w, ctx->cliprect.h);
}

#ifndef X_DISPLAY_MISSING
void
imlib_render_image_on_drawable_skewed(int source_x, int source_y,
                                      int source_width, int source_height,
                                      int destination_x, int destination_y,
                                      int h_angle_x, int h_angle_y,
                                      int v_angle_x, int v_angle_y)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_skewed", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   CAST_IMAGE(im, ctx->image);
   __imlib_RenderImageSkewed(ctx->display, im, ctx->drawable, ctx->mask,
                             ctx->visual, ctx->colormap, ctx->depth, source_x,
                             source_y, source_width, source_height,
                             destination_x, destination_y, h_angle_x,
                             h_angle_y, v_angle_x, v_angle_y, ctx->anti_alias,
                             ctx->dither, ctx->blend, ctx->dither_mask,
                             ctx->color_modifier, ctx->operation);
}

void
imlib_render_image_on_drawable_at_angle(int source_x, int source_y,
                                        int source_width, int source_height,
                                        int destination_x, int destination_y,
                                        int angle_x, int angle_y)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_render_image_on_drawable_at_angle", "image",
                       ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   CAST_IMAGE(im, ctx->image);
   __imlib_RenderImageSkewed(ctx->display, im, ctx->drawable, ctx->mask,
                             ctx->visual, ctx->colormap, ctx->depth, source_x,
                             source_y, source_width, source_height,
                             destination_x, destination_y, angle_x, angle_y,
                             0, 0, ctx->anti_alias, ctx->dither, ctx->blend,
                             ctx->dither_mask, ctx->color_modifier,
                             ctx->operation);
}
#endif

void
imlib_image_filter(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_filter", "image", ctx->image);
   CHECK_PARAM_POINTER("imlib_image_filter", "filter", ctx->filter);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_FilterImage(im, (ImlibFilter *) ctx->filter);
}

Imlib_Filter
imlib_create_filter(int initsize)
{
   if (!ctx)
      ctx = imlib_context_new();
   return (Imlib_Filter) __imlib_CreateFilter(initsize);
}

void
imlib_free_filter(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_free_filter", "filter", ctx->filter);
   __imlib_FreeFilter((ImlibFilter *) ctx->filter);
   ctx->filter = NULL;
}

void
imlib_context_set_filter(Imlib_Filter filter)
{
   if (!ctx)
      ctx = imlib_context_new();
   ctx->filter = filter;
}

Imlib_Filter
imlib_context_get_filter(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return ctx->filter;
}

void
imlib_filter_set(int xoff, int yoff, int a, int r, int g, int b)
{
   ImlibFilter        *fil;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_set", "filter", ctx->filter);
   fil = (ImlibFilter *) ctx->filter;
   __imlib_FilterSetColor(&fil->alpha, xoff, yoff, a, 0, 0, 0);
   __imlib_FilterSetColor(&fil->red, xoff, yoff, 0, r, 0, 0);
   __imlib_FilterSetColor(&fil->green, xoff, yoff, 0, 0, g, 0);
   __imlib_FilterSetColor(&fil->blue, xoff, yoff, 0, 0, 0, b);
}

void
imlib_filter_set_alpha(int xoff, int yoff, int a, int r, int g, int b)
{
   ImlibFilter        *fil;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_set_alpha", "filter", ctx->filter);
   fil = (ImlibFilter *) ctx->filter;
   __imlib_FilterSetColor(&fil->alpha, xoff, yoff, a, r, g, b);
}

void
imlib_filter_set_red(int xoff, int yoff, int a, int r, int g, int b)
{
   ImlibFilter        *fil;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_set_red", "filter", ctx->filter);
   fil = (ImlibFilter *) ctx->filter;
   __imlib_FilterSetColor(&fil->red, xoff, yoff, a, r, g, b);
}

void
imlib_filter_set_green(int xoff, int yoff, int a, int r, int g, int b)
{
   ImlibFilter        *fil;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_set_green", "filter", ctx->filter);
   fil = (ImlibFilter *) ctx->filter;
   __imlib_FilterSetColor(&fil->green, xoff, yoff, a, r, g, b);
}

void
imlib_filter_set_blue(int xoff, int yoff, int a, int r, int g, int b)
{
   ImlibFilter        *fil;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_set_blue", "filter", ctx->filter);
   fil = (ImlibFilter *) ctx->filter;
   __imlib_FilterSetColor(&fil->blue, xoff, yoff, a, r, g, b);
}

void
imlib_filter_constants(int a, int r, int g, int b)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_constants", "filter", ctx->filter);
   __imlib_FilterConstants((ImlibFilter *) ctx->filter, a, r, g, b);
}

void
imlib_filter_divisors(int a, int r, int g, int b)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_filter_divisors", "filter", ctx->filter);
   __imlib_FilterDivisors((ImlibFilter *) ctx->filter, a, r, g, b);
}

void
imlib_apply_filter(char *script, ...)
{
   va_list             param_list;
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   __imlib_dynamic_filters_init();
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   va_start(param_list, script);
   __imlib_script_parse(im, script, param_list);
   va_end(param_list);
}

ImlibPolygon
imlib_polygon_new(void)
{
   if (!ctx)
      ctx = imlib_context_new();
   return (ImlibPolygon) __imlib_polygon_new();
}

void
imlib_polygon_add_point(ImlibPolygon poly, int x, int y)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_polygon_add_point", "polygon", poly);
   __imlib_polygon_add_point(poly, x, y);
}

void
imlib_polygon_free(ImlibPolygon poly)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_polygon_free", "polygon", poly);
   __imlib_polygon_free(poly);
}

void
imlib_image_draw_polygon(ImlibPolygon poly, unsigned char closed)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_draw_polygon", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   if (ctx->cliprect.w)
     {
        __imlib_draw_polygon_clipped(im, poly, closed, ctx->cliprect.x,
                                     ctx->cliprect.x + ctx->cliprect.w - 1,
                                     ctx->cliprect.y,
                                     ctx->cliprect.y + ctx->cliprect.h - 1,
                                     ctx->color.red, ctx->color.green,
                                     ctx->color.blue, ctx->color.alpha,
                                     ctx->operation);
     }
   else
     {
        __imlib_draw_polygon(im, poly, closed, ctx->color.red, ctx->color.green,
                             ctx->color.blue, ctx->color.alpha, ctx->operation);
     }
}

void
imlib_image_fill_polygon(ImlibPolygon poly)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_fill_polygon", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   __imlib_draw_polygon_filled(im, poly, ctx->cliprect.x,
                               ctx->cliprect.x + ctx->cliprect.w - 1,
                               ctx->cliprect.y,
                               ctx->cliprect.y + ctx->cliprect.h - 1,
                               ctx->color.red, ctx->color.green,
                               ctx->color.blue, ctx->color.alpha,
                               ctx->operation, ctx->anti_alias);
}

void
imlib_polygon_get_bounds(ImlibPolygon poly, int *px1, int *py1, int *px2,
                         int *py2)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_polygon_get_bounds", "polygon", poly);
   __imlib_polygon_get_bounds(poly, px1, py1, px2, py2);
}

void
imlib_image_draw_ellipse(int xc, int yc, int a, int b)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_draw_ellipse", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);

   if (ctx->cliprect.w)
     {
        __imlib_draw_ellipse_clipped(im, xc, yc, a, b, ctx->cliprect.x,
                                     ctx->cliprect.x + ctx->cliprect.w - 1,
                                     ctx->cliprect.y,
                                     ctx->cliprect.y + ctx->cliprect.h - 1,
                                     ctx->color.red, ctx->color.green,
                                     ctx->color.blue, ctx->color.alpha,
                                     ctx->operation);
     }
   else
     {
        __imlib_draw_ellipse_clipped(im, xc, yc, a, b, 0,
                                     im->w - 1, 0, im->h - 1,
                                     ctx->color.red, ctx->color.green,
                                     ctx->color.blue, ctx->color.alpha,
                                     ctx->operation);
     }
}

void
imlib_image_fill_ellipse(int xc, int yc, int a, int b)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_fill_ellipse", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);

   __imlib_fill_ellipse(im, xc, yc, a, b, ctx->cliprect.x,
                        ctx->cliprect.x + ctx->cliprect.w - 1,
                        ctx->cliprect.y,
                        ctx->cliprect.y + ctx->cliprect.h - 1, ctx->color.red,
                        ctx->color.green, ctx->color.blue, ctx->color.alpha,
                        ctx->operation, ctx->anti_alias);
}

unsigned char
imlib_polygon_contains_point(ImlibPolygon poly, int x, int y)
{
   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER_RETURN("imlib_polygon_contains_point", "polygon", poly,
                              0);
   return __imlib_polygon_contains_point(poly, x, y);
}

void
imlib_image_clear(void)
{
   ImlibImage         *im;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_clear", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   memset(im->data, 0, im->w * im->h * sizeof(DATA32));
}

void
imlib_image_clear_color(int r, int g, int b, int a)
{
   ImlibImage         *im;
   int                 i, max;
   DATA32              col;

   if (!ctx)
      ctx = imlib_context_new();
   CHECK_PARAM_POINTER("imlib_image_clear_color", "image", ctx->image);
   CAST_IMAGE(im, ctx->image);
   if ((!(im->data)) && (im->loader) && (im->loader->load))
      im->loader->load(im, NULL, 0, 1);
   if (!(im->data))
      return;
   __imlib_DirtyImage(im);
   __imlib_DirtyPixmapsForImage(im);
   max = im->w * im->h;
   WRITE_RGBA(&col, r, g, b, a);
   for (i = 0; i < max; i++)
      im->data[i] = col;
}
