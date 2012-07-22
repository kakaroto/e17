
#include "config.h"

#include "evas_common.h"
#include "evas_private.h"
#include "evas_gl_common.h"
#include "evas_engine.h"
#include "Evas.h"
#include "Evas_Engine_GL_PSL1GHT.h"

#include <malloc.h>
#include <EGL/egl.h>
#include <sysutil/video.h>

int _evas_engine_gl_psl1ght_log_dom = -1;

/* function tables - filled in later (func and parent func) */
static Evas_Func func, pfunc;

/* engine struct data */
typedef struct _Render_Engine Render_Engine;

static int eng_image_alpha_get(void *data __UNUSED__, void *image);
static int eng_image_colorspace_get(void *data __UNUSED__, void *image);

#define MAX_BUFFERS 2

struct _Render_Engine
{
   Evas_Engine_Info_GL_PSL1GHT	*info;
   int			w, h;

   Evas_Engine_GL_Context *gl_context;
   Tilebuf        *tb;
   struct {
      int              redraw : 1;
      int              drew : 1;
      int              x1, y1, x2, y2;
   } draw;


   EGLContext       egl_context[1];
   EGLSurface       egl_surface[1];
   EGLConfig        egl_config;
   EGLDisplay       egl_disp;

   int             end : 1;
};

void gl_debug(GLsizei n,const GLchar * s)
{
  printf("%s", s);
}

static u8
getPreferredResolution(u16 width, u16 height)
{
   videoDeviceInfo info;
   videoResolution res;
   int area = width * height;
   int mode_area;
   int min_area_diff = abs (area - (720 * 480));
   int area_diff;
   u8 resolution = VIDEO_RESOLUTION_480;
   int i;

   videoGetDeviceInfo(0, 0, &info);

   for (i = 0; i < info.availableModeCount; i++) {
        videoGetResolution (info.availableModes[i].resolution, &res);
        mode_area = res.width * res.height;
        area_diff = abs (area - mode_area);
        if (area_diff < min_area_diff)
          {
             min_area_diff = area_diff;
             resolution = info.availableModes[i].resolution;
          }
     }

   return resolution;
}

static int
setResolution(u16 *width, u16 *height)
{
   videoState state;
   videoConfiguration vconfig;
   videoResolution res;
   u8 resolution;

   resolution = getPreferredResolution (*width, *height);

   /* Get the state of the display */
   if (videoGetState (0, 0, &state) != 0)
     return FALSE;

   /* Make sure display is enabled */
   if (state.state != 0)
     return FALSE;

   if (videoGetResolution (resolution, &res) != 0)
     return FALSE;

   /* Configure the buffer format to xRGB */
   memset (&vconfig, 0, sizeof(videoConfiguration));
   vconfig.resolution = resolution;
   vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
   vconfig.pitch = res.width * sizeof(u32);
   vconfig.aspect = VIDEO_ASPECT_AUTO;

   if (videoConfigure (0, &vconfig, NULL, 0) != 0)
     return FALSE;

   *width = res.width;
   *height = res.height;

   return TRUE;
}

/* internal engine routines */
static void *
_output_setup(int w, int h)
{
   Render_Engine *re;
   EGLint version0 = 0,version1 = 0;
   EGLBoolean result;
   EGLint attribs[] = {
     EGL_RED_SIZE,8,
     EGL_BLUE_SIZE,8,
     EGL_GREEN_SIZE,8,
     EGL_ALPHA_SIZE,8,
     EGL_DEPTH_SIZE,16,
     EGL_NONE
   };
   EGLConfig config;
   EGLint nconfig = 0;
   u16 width, height;

   printf ("_output_setup called : %dx%d\n", w, h);
   re = calloc(1, sizeof(Render_Engine));
   if (!re)
     return NULL;

   glInitDebug(1024*256, gl_debug);

   width = w;
   height = h;
   setResolution (&width, &height);
   printf ("Resolution changed to : %dx%d\n", width, height);

   /* Setup EGL display and config */
   re->egl_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   if (re->egl_disp == EGL_NO_DISPLAY)
     {
	free(re);
	return NULL;
     }

   result = eglInitialize(re->egl_disp, &version0, &version1);
   if (!result)
     {
	eglTerminate(re->egl_disp);
	free(re);
	return NULL;
     }

   result = eglChooseConfig(re->egl_disp, attribs, &re->egl_config, 1, &nconfig);
   if (nconfig == 0)
     {
	eglTerminate(re->egl_disp);
	free(re);
	return NULL;
     }

   re->egl_surface[0] = eglCreateWindowSurface(re->egl_disp, re->egl_config, 0, 0);
   if (re->egl_surface[0] == EGL_NO_SURFACE)
     {
	eglTerminate(re->egl_disp);
	free(re);
	return NULL;
     }


   re->egl_context[0] = eglCreateContext(re->egl_disp, re->egl_config, 0, 0);
   if (re->egl_context[0] == EGL_NO_CONTEXT)
     {
	eglTerminate(re->egl_disp);
	free(re);
	return NULL;
     }

   result = eglMakeCurrent(re->egl_disp, re->egl_surface[0], re->egl_surface[0], re->egl_context[0]);
   if (!result)
     {
	eglDestroyContext(re->egl_disp, re->egl_context[0]);
	eglTerminate(re->egl_disp);
	free(re);
	return NULL;
     }

   eglQuerySurface(re->egl_disp, re->egl_surface[0], EGL_WIDTH, &re->w);
   eglQuerySurface(re->egl_disp, re->egl_surface[0], EGL_HEIGHT, &re->h);

   printf ("Created EGL surface : %dx%d\n", re->w, re->h);

   /* Setup GL context */
   re->gl_context = evas_gl_common_context_new();
   if (!re->gl_context)
     {
	free(re);
	return NULL;
     }
   evas_gl_common_context_use(re->gl_context);
   evas_gl_common_context_resize(re->gl_context, re->w, re->h, re->gl_context->rot);

   /* if we haven't initialized - init (automatic abort if already done) */
   evas_common_cpu_init();
   evas_common_blend_init();
   evas_common_image_init();
   evas_common_convert_init();
   evas_common_scale_init();
   evas_common_rectangle_init();
   evas_common_polygon_init();
   evas_common_line_init();
   evas_common_font_init();
   evas_common_draw_init();
   evas_common_tilebuf_init();

   re->tb = evas_common_tilebuf_new(re->w, re->h);

   /* in preliminary tests 16x16 gave highest framerates */
   evas_common_tilebuf_set_tile_size(re->tb, TILESIZE, TILESIZE);

   return re;
}

/* engine api this module provides */
static void *
eng_info(Evas *e)
{
   Evas_Engine_Info_GL_PSL1GHT *info;

   printf ("eng_info called\n");
   info = calloc(1, sizeof(Evas_Engine_Info_GL_PSL1GHT));
   if (!info)
     return NULL;

   info->magic.magic = rand();
   info->render_mode = EVAS_RENDER_MODE_BLOCKING;

   return info;
}

static void
eng_info_free(Evas *e __UNUSED__, void *info)
{
   Evas_Engine_Info_GL_PSL1GHT *in;

   printf ("eng_info_free called\n");
   in = (Evas_Engine_Info_GL_PSL1GHT *)info;
   free(in);
}

static int
eng_setup(Evas *e, void *in)
{
   Evas_Engine_Info_GL_PSL1GHT *info;

   printf ("eng_setup called\n");
   info = (Evas_Engine_Info_GL_PSL1GHT *)in;

   e->engine.data.output = _output_setup(e->output.w, e->output.h);
   if (!e->engine.data.output)
     return 0;

   e->engine.func = &func;
   e->engine.data.context = e->engine.func->context_new(e->engine.data.output);

   return 1;
}

static Eina_Bool
eng_canvas_alpha_get(void *data, void *context __UNUSED__)
{
   return EINA_TRUE;
}

static void
eng_output_free(void *data)
{
   Render_Engine *re;

   re = (Render_Engine *)data;


   if (re)
     {
       evas_gl_common_context_free(re->gl_context);
       eglMakeCurrent(re->egl_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
       eglDestroySurface(re->egl_disp, re->egl_surface[0]);
       eglDestroyContext(re->egl_disp, re->egl_context[0]);
       eglTerminate(re->egl_disp);
       free(re);

       evas_common_font_shutdown();
       evas_common_image_shutdown();

     }
}

static void
eng_output_resize(void *data, int w, int h)
{
   Render_Engine *re;
   u16 width, height;

   printf ("eng_output_resize called : %dx%d\n", w, h);
   re = (Render_Engine *)data;

   /* FIXME: do we need to swap buffers to make sure the rsx is inactive? */
   eglSwapBuffers(re->egl_disp, re->egl_surface[0]);

   width = w;
   height = h;
   setResolution (&width, &height);
   printf ("Resolution changed to : %dx%d\n", width, height);

   eglQuerySurface(re->egl_disp, re->egl_surface[0], EGL_WIDTH, &re->w);
   eglQuerySurface(re->egl_disp, re->egl_surface[0], EGL_HEIGHT, &re->h);
   printf ("EGL surface size is : %dx%d\n", re->w, re->h);

   evas_gl_common_context_resize(re->gl_context, re->w, re->h, re->gl_context->rot);
   evas_common_tilebuf_free(re->tb);
   re->tb = evas_common_tilebuf_new(re->w, re->h);
   if (re->tb)
     evas_common_tilebuf_set_tile_size(re->tb, TILESIZE, TILESIZE);
}

static void
eng_output_tile_size_set(void *data, int w, int h)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_common_tilebuf_set_tile_size(re->tb, w, h);
}

static void
eng_output_redraws_rect_add(void *data, int x, int y, int w, int h)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_gl_common_context_resize(re->gl_context, re->w, re->h, re->gl_context->rot);
   evas_common_tilebuf_add_redraw(re->tb, x, y, w, h);

   RECTS_CLIP_TO_RECT(x, y, w, h, 0, 0, re->w, re->h);
   if ((w <= 0) || (h <= 0)) return;
   if (!re->draw.redraw)
     {
#if 1
	re->draw.x1 = x;
	re->draw.y1 = y;
	re->draw.x2 = x + w - 1;
	re->draw.y2 = y + h - 1;
#else
	re->draw.x1 = 0;
	re->draw.y1 = 0;
	re->draw.x2 = re->w - 1;
	re->draw.y2 = re->h - 1;
#endif
     }
   else
     {
	if (x < re->draw.x1) re->draw.x1 = x;
	if (y < re->draw.y1) re->draw.y1 = y;
	if ((x + w - 1) > re->draw.x2) re->draw.x2 = x + w - 1;
	if ((y + h - 1) > re->draw.y2) re->draw.y2 = y + h - 1;
     }
   re->draw.redraw = 1;
}

static void
eng_output_redraws_rect_del(void *data, int x, int y, int w, int h)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_common_tilebuf_del_redraw(re->tb, x, y, w, h);
}

static void
eng_output_redraws_clear(void *data)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_common_tilebuf_clear(re->tb);
}

static void *
eng_output_redraws_next_update_get(void *data, int *x, int *y, int *w, int *h, int *cx, int *cy, int *cw, int *ch)
{
   Render_Engine *re;
   Tilebuf_Rect *rects;

   re = (Render_Engine *)data;
   /* get the upate rect surface - return engine data as dummy */
   rects = evas_common_tilebuf_get_render_rects(re->tb);
   if (rects)
     {
        evas_common_tilebuf_free_render_rects(rects);
        evas_common_tilebuf_clear(re->tb);
        evas_gl_common_context_flush(re->gl_context);
        evas_gl_common_context_newframe(re->gl_context);
        if (x) *x = 0;
        if (y) *y = 0;
        if (w) *w = re->w;
        if (h) *h = re->h;
        if (cx) *cx = 0;
        if (cy) *cy = 0;
        if (cw) *cw = re->w;
        if (ch) *ch = re->h;
        return re->gl_context->def_surface;
     }
   return NULL;
}

static void
eng_output_redraws_next_update_push(void *data, void *surface, int x, int y, int w, int h)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   /* put back update surface.. in this case just unflag redraw */
   re->draw.redraw = 0;
   re->draw.drew = 1;
   evas_gl_common_context_flush(re->gl_context);
}

static void
eng_context_cutout_add(void *data __UNUSED__, void *context, int x, int y, int w, int h)
{
   evas_common_draw_context_add_cutout(context, x, y, w, h);
}

static void
eng_context_cutout_clear(void *data __UNUSED__, void *context)
{
   evas_common_draw_context_clear_cutouts(context);
}

static void
eng_output_flush(void *data)
{
   Render_Engine *re;
   EGLBoolean result;

   re = (Render_Engine *)data;
   if (!re->draw.drew) return;
   re->draw.drew = 0;

   result = eglSwapBuffers(re->egl_disp, re->egl_surface[0]);

   if(!result)
     printf("Swap sync timed-out: %x\n", eglGetError ());
}

static void
eng_output_idle_flush(void *data)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
}

static void
eng_output_dump(void *data)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_common_image_image_all_unload();
   evas_common_font_font_all_unload();
   evas_gl_common_image_all_unload(re->gl_context);
}

static void
eng_rectangle_draw(void *data, void *context, void *surface, int x, int y, int w, int h)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_gl_common_context_target_surface_set(re->gl_context, surface);
   re->gl_context->dc = context;
   evas_gl_common_rect_draw(re->gl_context, x, y, w, h);
}

static void
eng_line_draw(void *data, void *context, void *surface, int x1, int y1, int x2, int y2)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_gl_common_context_target_surface_set(re->gl_context, surface);
   re->gl_context->dc = context;
   evas_gl_common_line_draw(re->gl_context, x1, y1, x2, y2);
}

static void *
eng_polygon_point_add(void *data, void *context __UNUSED__, void *polygon, int x, int y)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   return evas_gl_common_poly_point_add(polygon, x, y);
}

static void *
eng_polygon_points_clear(void *data, void *context __UNUSED__, void *polygon)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   return evas_gl_common_poly_points_clear(polygon);
}

static void
eng_polygon_draw(void *data, void *context, void *surface, void *polygon, int x, int y)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_gl_common_context_target_surface_set(re->gl_context, surface);
   re->gl_context->dc = context;
   evas_gl_common_poly_draw(re->gl_context, polygon, x, y);
}

static void *
eng_image_load(void *data, const char *file, const char *key, int *error, Evas_Image_Load_Opts *lo)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   *error = EVAS_LOAD_ERROR_NONE;
   return evas_gl_common_image_load(re->gl_context, file, key, lo, error);
}

static void *
eng_image_new_from_data(void *data, int w, int h, DATA32 *image_data, int alpha, int cspace)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   return evas_gl_common_image_new_from_data(re->gl_context, w, h, image_data, alpha, cspace);
}

static void *
eng_image_new_from_copied_data(void *data, int w, int h, DATA32 *image_data, int alpha, int cspace)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   return evas_gl_common_image_new_from_copied_data(re->gl_context, w, h, image_data, alpha, cspace);
}

static void
eng_image_free(void *data, void *image)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   if (!image) return;
   evas_gl_common_image_free(image);
}

static void
eng_image_size_get(void *data __UNUSED__, void *image, int *w, int *h)
{
   if (!image)
     {
        *w = 0;
        *h = 0;
        return;
     }
   if (w) *w = ((Evas_GL_Image *)image)->w;
   if (h) *h = ((Evas_GL_Image *)image)->h;
}

static void *
eng_image_size_set(void *data, void *image, int w, int h)
{
   Render_Engine *re;
   Evas_GL_Image *im = image;
   Evas_GL_Image *im_old;

   re = (Render_Engine *)data;
   if (!im) return NULL;
   if (im->native.data)
     {
        im->w = w;
        im->h = h;
        return image;
     }

   if ((im->tex) && (im->tex->pt->dyn.img))
     {
        evas_gl_common_texture_free(im->tex);
        im->tex = NULL;
        im->w = w;
        im->h = h;
        im->tex = evas_gl_common_texture_dynamic_new(im->gc, im);
        return image;
     }
   im_old = image;

   switch (eng_image_colorspace_get(data, image))
     {
      case EVAS_COLORSPACE_YCBCR422P601_PL:
      case EVAS_COLORSPACE_YCBCR422P709_PL:
      case EVAS_COLORSPACE_YCBCR422601_PL:
      case EVAS_COLORSPACE_YCBCR420NV12601_PL:
      case EVAS_COLORSPACE_YCBCR420TM12601_PL:
         w &= ~0x1;
         break;
     }

   if ((im_old) &&
       ((int)im_old->im->cache_entry.w == w) &&
       ((int)im_old->im->cache_entry.h == h))
     return image;
   if (im_old)
     {
        im = evas_gl_common_image_new(re->gl_context, w, h,
                                      eng_image_alpha_get(data, image),
                                      eng_image_colorspace_get(data, image));
        evas_gl_common_image_free(im_old);
     }
   else
     im = evas_gl_common_image_new(re->gl_context, w, h, 1, EVAS_COLORSPACE_ARGB8888);
   return im;
}

static void *
eng_image_dirty_region(void *data, void *image, int x, int y, int w, int h)
{
   Render_Engine *re;
   Evas_GL_Image *im = image;

   re = (Render_Engine *)data;
   if (!image) return NULL;
   if (im->native.data) return image;
   evas_gl_common_image_dirty(image, x, y, w, h);
   return image;
}

static void *
eng_image_data_get(void *data, void *image, int to_write, DATA32 **image_data, int *err)
{
   Render_Engine *re;
   Evas_GL_Image *im;
   int error;

   re = (Render_Engine *)data;
   if (!image)
     {
        *image_data = NULL;
        if (err) *err = EVAS_LOAD_ERROR_GENERIC;
        return NULL;
     }
   im = image;
   if (im->native.data)
     {
        *image_data = NULL;
        if (err) *err = EVAS_LOAD_ERROR_NONE;
        return im;
     }

   if ((im->tex) && (im->tex->pt) && (im->tex->pt->dyn.data))
     {
        *image_data = im->tex->pt->dyn.data;
        if (err) *err = EVAS_LOAD_ERROR_NONE;
        return im;
     }

   /* Engine can be fail to create texture after cache drop like eng_image_content_hint_set function,
        so it is need to add code which check im->im's NULL value*/

   if (!im->im)
    {
       *image_data = NULL;
       if (err) *err = EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED;
       return NULL;
    }

   error = evas_cache_image_load_data(&im->im->cache_entry);
   switch (im->cs.space)
     {
      case EVAS_COLORSPACE_ARGB8888:
         if (to_write)
           {
              if (im->references > 1)
                {
                   Evas_GL_Image *im_new;

                   im_new = evas_gl_common_image_new_from_copied_data
                      (im->gc, im->im->cache_entry.w, im->im->cache_entry.h,
                       im->im->image.data,
                       eng_image_alpha_get(data, image),
                       eng_image_colorspace_get(data, image));
                   if (!im_new)
                     {
                        *image_data = NULL;
                        if (err) *err = EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED;
                        return NULL;
                     }
                   evas_gl_common_image_free(im);
                   im = im_new;
                }
              else
                evas_gl_common_image_dirty(im, 0, 0, 0, 0);
           }
         *image_data = im->im->image.data;
         break;
      case EVAS_COLORSPACE_YCBCR422P601_PL:
      case EVAS_COLORSPACE_YCBCR422P709_PL:
      case EVAS_COLORSPACE_YCBCR422601_PL:
      case EVAS_COLORSPACE_YCBCR420NV12601_PL:
      case EVAS_COLORSPACE_YCBCR420TM12601_PL:
         *image_data = im->cs.data;
         break;
      default:
         abort();
         break;
     }
   if (err) *err = error;
   return im;
}

static void *
eng_image_data_put(void *data, void *image, DATA32 *image_data)
{
   Render_Engine *re;
   Evas_GL_Image *im, *im2;

   re = (Render_Engine *)data;
   if (!image) return NULL;
   im = image;
   if (im->native.data) return image;
   if ((im->tex) && (im->tex->pt)
       && (im->tex->pt->dyn.data)
       && (im->cs.space == EVAS_COLORSPACE_ARGB8888))
     {
        int w, h;

        if (im->tex->pt->dyn.data == image_data)
          {
             if (im->tex->pt->dyn.checked_out > 0)
               {
                 im->tex->pt->dyn.checked_out--;
               }

             return image;
          }

        w = im->im->cache_entry.w;
        h = im->im->cache_entry.h;
        im2 = eng_image_new_from_data(data, w, h, image_data,
                                      eng_image_alpha_get(data, image),
                                      eng_image_colorspace_get(data, image));
        if (!im2) return im;
        evas_gl_common_image_free(im);
        im = im2;
        evas_gl_common_image_dirty(im, 0, 0, 0, 0);
        return im;
     }
   switch (im->cs.space)
     {
      case EVAS_COLORSPACE_ARGB8888:
         if (image_data != im->im->image.data)
           {
              int w, h;

              w = im->im->cache_entry.w;
              h = im->im->cache_entry.h;
              im2 = eng_image_new_from_data(data, w, h, image_data,
                                            eng_image_alpha_get(data, image),
                                            eng_image_colorspace_get(data, image));
              if (!im2) return im;
              evas_gl_common_image_free(im);
              im = im2;
           }
         break;
      case EVAS_COLORSPACE_YCBCR422P601_PL:
      case EVAS_COLORSPACE_YCBCR422P709_PL:
      case EVAS_COLORSPACE_YCBCR422601_PL:
      case EVAS_COLORSPACE_YCBCR420NV12601_PL:
      case EVAS_COLORSPACE_YCBCR420TM12601_PL:
         if (image_data != im->cs.data)
           {
              if (im->cs.data)
                {
                   if (!im->cs.no_free) free(im->cs.data);
                }
              im->cs.data = image_data;
           }
         evas_gl_common_image_dirty(im, 0, 0, 0, 0);
         break;
      default:
         abort();
         break;
     }
   return im;
}

static void
eng_image_data_preload_request(void *data __UNUSED__, void *image, const void *target)
{
   Evas_GL_Image *gim = image;
   RGBA_Image *im;

   if (!gim) return;
   if (gim->native.data) return;
   im = (RGBA_Image *)gim->im;
   if (!im) return;
   evas_cache_image_preload_data(&im->cache_entry, target);
}

static void
eng_image_data_preload_cancel(void *data __UNUSED__, void *image, const void *target)
{
   Evas_GL_Image *gim = image;
   RGBA_Image *im;

   if (!gim) return;
   if (gim->native.data) return;
   im = (RGBA_Image *)gim->im;
   if (!im) return;
   evas_cache_image_preload_cancel(&im->cache_entry, target);
}

static void *
eng_image_alpha_set(void *data, void *image, int has_alpha)
{
   Render_Engine *re;
   Evas_GL_Image *im;

   re = (Render_Engine *)data;
   if (!image) return NULL;
   im = image;
   if (im->alpha == has_alpha) return image;
   if (im->native.data)
     {
        im->alpha = has_alpha;
        return image;
     }
   if ((im->tex) && (im->tex->pt->dyn.img))
     {
        im->alpha = has_alpha;
        im->tex->alpha = im->alpha;
        return image;
     }
   /* FIXME: can move to gl_common */
   if (im->cs.space != EVAS_COLORSPACE_ARGB8888) return im;
   if ((has_alpha) && (im->im->cache_entry.flags.alpha)) return image;
   else if ((!has_alpha) && (!im->im->cache_entry.flags.alpha)) return image;
   if (im->references > 1)
     {
        Evas_GL_Image *im_new;

        im_new = evas_gl_common_image_new_from_copied_data
           (im->gc, im->im->cache_entry.w, im->im->cache_entry.h,
               im->im->image.data,
               eng_image_alpha_get(data, image),
               eng_image_colorspace_get(data, image));
        if (!im_new) return im;
        evas_gl_common_image_free(im);
        im = im_new;
     }
   else
     evas_gl_common_image_dirty(im, 0, 0, 0, 0);
   return evas_gl_common_image_alpha_set(im, has_alpha ? 1 : 0);
}

static int
eng_image_alpha_get(void *data __UNUSED__, void *image)
{
   Evas_GL_Image *im;

   if (!image) return 1;
   im = image;
   return im->alpha;
}

static void *
eng_image_border_set(void *data, void *image, int l __UNUSED__, int r __UNUSED__, int t __UNUSED__, int b __UNUSED__)
{
   /* even gl_x11 doesn't do anything here */
   return image;
}

static void
eng_image_border_get(void *data, void *image __UNUSED__, int *l __UNUSED__, int *r __UNUSED__, int *t __UNUSED__, int *b __UNUSED__)
{
   /* even gl_x11 doesn't do anything here */
}

static void
eng_image_draw(void *data, void *context, void *surface, void *image, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h, int smooth)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   if (!image) return;
   evas_gl_common_context_target_surface_set(re->gl_context, surface);
   re->gl_context->dc = context;
   evas_gl_common_image_draw(re->gl_context, image,
                             src_x, src_y, src_w, src_h,
                             dst_x, dst_y, dst_w, dst_h,
                             smooth);
}

static char *
eng_image_comment_get(void *data, void *image, char *key __UNUSED__)
{
   Evas_GL_Image *im;

   if (!image) return NULL;
   im = image;
   if (!im->im) return NULL;
   return im->im->info.comment;
}

static char *
eng_image_format_get(void *data, void *image)
{
   /* even gl_x11 doesn't do anything here */
   return NULL;
}

static void
eng_image_colorspace_set(void *data, void *image, int cspace)
{
   Render_Engine *re;
   Evas_GL_Image *im;

   re = (Render_Engine *)data;
   if (!image) return;
   im = image;
   if (im->native.data) return;
   /* FIXME: can move to gl_common */
   if (im->cs.space == cspace) return;
   evas_cache_image_colorspace(&im->im->cache_entry, cspace);
   switch (cspace)
     {
      case EVAS_COLORSPACE_ARGB8888:
	if (im->cs.data)
	  {
	     if (!im->cs.no_free) free(im->cs.data);
	     im->cs.data = NULL;
	     im->cs.no_free = 0;
	  }
	break;
      case EVAS_COLORSPACE_YCBCR422P601_PL:
      case EVAS_COLORSPACE_YCBCR422P709_PL:
      case EVAS_COLORSPACE_YCBCR422601_PL:
      case EVAS_COLORSPACE_YCBCR420NV12601_PL:
      case EVAS_COLORSPACE_YCBCR420TM12601_PL:
        if (im->tex) evas_gl_common_texture_free(im->tex);
        im->tex = NULL;
	if (im->cs.data)
	  {
	     if (!im->cs.no_free) free(im->cs.data);
	  }
	im->cs.data = calloc(1, im->im->cache_entry.h * sizeof(unsigned char *) * 2);
	im->cs.no_free = 0;
	break;
      default:
	abort();
	break;
     }
   im->cs.space = cspace;
}

static int
eng_image_colorspace_get(void *data __UNUSED__, void *image)
{
   Evas_GL_Image *im;

   if (!image) return EVAS_COLORSPACE_ARGB8888;
   im = image;
   return im->cs.space;
}

static Eina_Bool
eng_image_can_region_get(void *data __UNUSED__, void *image)
{
   Evas_GL_Image *gim = image;
   Image_Entry *im;
   if (!gim) return EINA_FALSE;
   im = (Image_Entry *)gim->im;
   if (!im) return EINA_FALSE;
   return ((Evas_Image_Load_Func*) im->info.loader)->do_region;
}

static void
eng_image_mask_create(void *data __UNUSED__, void *image)
{
   Evas_GL_Image *im;

   if (!image) return;
   im = image;
   if (!im->im->image.data)
      evas_cache_image_load_data(&im->im->cache_entry);
   if (!im->tex)
      im->tex = evas_gl_common_texture_new(im->gc, im->im);
}

static void *
eng_image_native_set(void *data, void *image, void *native)
{
   /* TODO eventually... doesn't seem to be needed for now though */
   return NULL;
}

static void *
eng_image_native_get(void *data, void *image)
{
   /* TODO eventually... doesn't seem to be needed for now though */
   return NULL;
}

static void
eng_font_draw(void *data, void *context, void *surface, Evas_Font_Set *font, int x, int y, int w __UNUSED__, int h __UNUSED__, int ow __UNUSED__, int oh __UNUSED__, Evas_Text_Props *intl_props)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   evas_gl_common_context_target_surface_set(re->gl_context, surface);
   re->gl_context->dc = context;
     {
        // FIXME: put im into context so we can free it
	static RGBA_Image *im = NULL;

        if (!im)
          im = (RGBA_Image *)evas_cache_image_empty(evas_common_image_cache_get());
        im->cache_entry.w = re->w;
        im->cache_entry.h = re->h;
        evas_common_draw_context_font_ext_set(context,
   					      re->gl_context,
   					      evas_gl_font_texture_new,
   					      evas_gl_font_texture_free,
   					      evas_gl_font_texture_draw);
	evas_common_font_draw_prepare(intl_props);
	evas_common_font_draw(im, context, x, y, intl_props);
	evas_common_draw_context_font_ext_set(context,
					      NULL,
					      NULL,
					      NULL,
					      NULL);
     }
}

static void
eng_image_scale_hint_set(void *data __UNUSED__, void *image, int hint)
{
   if (image) evas_gl_common_image_scale_hint_set(image, hint);
}

static int
eng_image_scale_hint_get(void *data __UNUSED__, void *image)
{
   Evas_GL_Image *gim = image;
   if (!gim) return EVAS_IMAGE_SCALE_HINT_NONE;
   return gim->scale_hint;
}

static void
eng_image_stride_get(void *data __UNUSED__, void *image, int *stride)
{
   Evas_GL_Image *im = image;

   if ((im->tex) && (im->tex->pt->dyn.img))
     *stride = im->tex->pt->dyn.stride;
   else
     *stride = im->w * 4;
}

static void
eng_image_map_draw(void *data, void *context, void *surface, void *image, RGBA_Map *m, int smooth, int level)
{
   Evas_GL_Image *gim = image;
   Render_Engine *re;

   re = (Render_Engine *)data;
   if (!image) return;
   evas_gl_common_context_target_surface_set(re->gl_context, surface);
   re->gl_context->dc = context;
   if (m->count != 4)
     {
        // FIXME: nash - you didn't fix this
        abort();
     }
   if ((m->pts[0].x == m->pts[3].x) &&
       (m->pts[1].x == m->pts[2].x) &&
       (m->pts[0].y == m->pts[1].y) &&
       (m->pts[3].y == m->pts[2].y) &&
       (m->pts[0].x <= m->pts[1].x) &&
       (m->pts[0].y <= m->pts[2].y) &&
       (m->pts[0].u == 0) &&
       (m->pts[0].v == 0) &&
       (m->pts[1].u == (gim->w << FP)) &&
       (m->pts[1].v == 0) &&
       (m->pts[2].u == (gim->w << FP)) &&
       (m->pts[2].v == (gim->h << FP)) &&
       (m->pts[3].u == 0) &&
       (m->pts[3].v == (gim->h << FP)) &&
       (m->pts[0].col == 0xffffffff) &&
       (m->pts[1].col == 0xffffffff) &&
       (m->pts[2].col == 0xffffffff) &&
       (m->pts[3].col == 0xffffffff))
     {
        int dx, dy, dw, dh;

        dx = m->pts[0].x >> FP;
        dy = m->pts[0].y >> FP;
        dw = (m->pts[2].x >> FP) - dx;
        dh = (m->pts[2].y >> FP) - dy;
        eng_image_draw(data, context, surface, image,
                       0, 0, gim->w, gim->h, dx, dy, dw, dh, smooth);
     }
   else
     {
        evas_gl_common_image_map_draw(re->gl_context, image, m->count, &m->pts[0],
                                      smooth, level);
     }
}

static void *
eng_image_map_surface_new(void *data, int w, int h, int alpha)
{
   Render_Engine *re;

   re = (Render_Engine *)data;
   return evas_gl_common_image_surface_new(re->gl_context, w, h, alpha);
}

static void
eng_image_map_surface_free(void *data __UNUSED__, void *surface)
{
   evas_gl_common_image_free(surface);
}

static void
eng_image_map_clean(void *data, RGBA_Map *m)
{
}

static int
eng_image_load_error_get(void *data __UNUSED__, void *image)
{
   Evas_GL_Image *im;

   if (!image) return EVAS_LOAD_ERROR_NONE;
   im = image;
   return im->im->cache_entry.load_error;
}

/* module advertising code */
static int
module_open(Evas_Module *em)
{
   if (!em) return 0;
   if (!evas_gl_common_module_open()) return 0;
   /* get whatever engine module we inherit from */
   if (!_evas_module_engine_inherit(&pfunc, "software_generic")) return 0;
   _evas_engine_gl_psl1ght_log_dom = eina_log_domain_register
       ("evas-gl_psl1ght", EVAS_DEFAULT_LOG_COLOR);
   if (_evas_engine_gl_psl1ght_log_dom < 0)
     {
        EINA_LOG_ERR("Can not create a module log domain.");
        return 0;
     }

   /* store it for later use */
   func = pfunc;
   /* now to override methods */
#define ORD(f) EVAS_API_OVERRIDE(f, &func, eng_)
   ORD(info);
   ORD(info_free);
   ORD(setup);
   ORD(canvas_alpha_get);
   ORD(output_free);
   ORD(output_resize);
   ORD(output_tile_size_set);
   ORD(output_redraws_rect_add);
   ORD(output_redraws_rect_del);
   ORD(output_redraws_clear);
   ORD(output_redraws_next_update_get);
   ORD(output_redraws_next_update_push);
   ORD(context_cutout_add);
   ORD(context_cutout_clear);
   ORD(output_flush);
   ORD(output_idle_flush);
   ORD(output_dump);
   ORD(rectangle_draw);
   ORD(line_draw);
   ORD(polygon_point_add);
   ORD(polygon_points_clear);
   ORD(polygon_draw);

   ORD(image_load);
   ORD(image_new_from_data);
   ORD(image_new_from_copied_data);
   ORD(image_free);
   ORD(image_size_get);
   ORD(image_size_set);
   ORD(image_dirty_region);
   ORD(image_data_get);
   ORD(image_data_put);
   ORD(image_data_preload_request);
   ORD(image_data_preload_cancel);
   ORD(image_alpha_set);
   ORD(image_alpha_get);
   ORD(image_border_set);
   ORD(image_border_get);
   ORD(image_draw);
   ORD(image_comment_get);
   ORD(image_format_get);
   ORD(image_colorspace_set);
   ORD(image_colorspace_get);
   ORD(image_can_region_get);
   ORD(image_mask_create);
   ORD(image_native_set);
   ORD(image_native_get);
   ORD(font_draw);

   ORD(image_scale_hint_set);
   ORD(image_scale_hint_get);
   ORD(image_stride_get);

   ORD(image_map_draw);
   ORD(image_map_surface_new);
   ORD(image_map_surface_free);
   ORD(image_map_clean);

   ORD(image_load_error_get);

   /* now advertise out own api */
   em->functions = (void *)(&func);
   return 1;
}

static void
module_close(Evas_Module *em __UNUSED__)
{
   eina_log_domain_unregister(_evas_engine_gl_psl1ght_log_dom);
   evas_gl_common_module_close();
}

static Evas_Module_Api evas_modapi =
{
   EVAS_MODULE_API_VERSION,
   "gl_psl1ght",
   "none",
   {
      module_open,
      module_close
   }
};

EVAS_MODULE_DEFINE(EVAS_MODULE_TYPE_ENGINE, engine, gl_psl1ght);

#ifndef EVAS_STATIC_BUILD_GL_PSL1GHT
EVAS_EINA_MODULE_DEFINE(engine, gl_psl1ght);
#endif
