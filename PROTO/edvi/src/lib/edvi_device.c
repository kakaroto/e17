#include <stdlib.h>

#include "edvi_enum.h"
#include "edvi_forward.h"
#include "edvi_private.h"
#include "edvi_main.h"

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif


static Edvi_Framebuffer *_edvi_device_fb = NULL;
static Evas_Object      *_edvi_device_o = NULL;
static double            _edvi_device_offset_x = EDVI_DEFAULT_OFFSET_X;
static double            _edvi_device_offset_y = EDVI_DEFAULT_OFFSET_Y;
static int               _edvi_device_draw_page_finished = 0;

static void
_edvi_device_aa_put (long x,
                     long y,
                     long w,
                     long h)
{
  unsigned short *aa_buff = NULL;
  unsigned int   *m = NULL;
  unsigned int   *colormap;
  long            i;
  long            j;
  long            fb_w;
  long            fb_h;
  long            aa_bw;
  long            aa_bh;

  if (x < 0) {
    w += x;
    x = 0;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (x + w >= _edvi_device_fb->dvi_framebuffer->width)
    w = _edvi_device_fb->dvi_framebuffer->width - x - 1;
  if (y + h >= _edvi_device_fb->dvi_framebuffer->height)
    h = _edvi_device_fb->dvi_framebuffer->height - y - 1;
  if ((w <= 0) || (h <= 0))
    return;

  aa_buff = DVI_fb_antialias(_edvi_device_fb->dvi_framebuffer, edvi_aa_get (),
                             x, y, w, h,
                             &aa_bw, &aa_bh);
  if (!aa_buff)
    return;

  fb_w = _edvi_device_fb->dvi_framebuffer->width / edvi_aa_get ();
  fb_h = _edvi_device_fb->dvi_framebuffer->height / edvi_aa_get ();
  evas_object_image_size_set (_edvi_device_o, fb_w, fb_h);
  evas_object_image_fill_set (_edvi_device_o, 0, 0, fb_w, fb_h);
  m = (unsigned int *)evas_object_image_data_get (_edvi_device_o, 1);
  if (!m)
    goto sortie;
  colormap = (unsigned int *)edvi_color_map_get ();
  for (j = 0; j < aa_bh; j++) {
    for (i = 0; i < aa_bw; i++) {
      if ((colormap[aa_buff[aa_bw * j + i]] >> 24) == 0) {
        m[(j + y/edvi_aa_get ()) * fb_w + x/edvi_aa_get () + i] =
          colormap[aa_buff[aa_bw * j + i]];
      }
    }
  }

  evas_object_image_data_update_add (_edvi_device_o, x/edvi_aa_get (), y/edvi_aa_get (), aa_bw, aa_bh);
  evas_object_resize (_edvi_device_o, fb_w, fb_h);

 sortie:
  free (aa_buff);
}

static void
DEV_put_bitmap_rgb(DVI_DEVICE dev __UNUSED__,
                   DVI        dvi __UNUSED__,
                   DVI_BITMAP bm,
                   int        font_id __UNUSED__,
		   long       dvipos __UNUSED__,
                   long       code_point __UNUSED__,
                   long       x,
                   long       y)
{
  DVI_fb_put_bitmap (_edvi_device_fb->dvi_framebuffer, bm,
                     x + _edvi_device_offset_x,
                     y + _edvi_device_offset_y);
  _edvi_device_aa_put (x + _edvi_device_offset_x - edvi_aa_get () / 2,
                       y + _edvi_device_offset_y - edvi_aa_get () / 2,
                       bm->width + edvi_aa_get (), bm->height + edvi_aa_get ());
}

static int
DEV_poll(DVI_DEVICE dev __UNUSED__,
         DVI        dvi __UNUSED__,
         int        poll_type)
{
  static int  t = 0;

  if ((poll_type != DVI_POLL_PAGE) || ((--t) > 0)) {
    return 0;
  }
  t = 30;
  if (!_edvi_device_draw_page_finished)
    return 0;

  return 1;
}


static void
_edvi_put_rectangle(DVI_DEVICE dev __UNUSED__,
                    DVI        dvi __UNUSED__,
                    long       x,
                    long       y,
                    long       w,
                    long       h)
{
  DVI_fb_put_rectangle (_edvi_device_fb->dvi_framebuffer,
                        x + _edvi_device_offset_x,
                        y + _edvi_device_offset_y,
                        w, h);
  _edvi_device_aa_put (x + _edvi_device_offset_x - edvi_aa_get () / 2,
                       y + _edvi_device_offset_y - edvi_aa_get () / 2,
                       w + edvi_aa_get (), h + edvi_aa_get ());
}


Edvi_Device *
edvi_device_new (int h_dpi, int v_dpi)
{
  Edvi_Device *device;

  device = (Edvi_Device *)malloc (sizeof (Edvi_Device));
  if (!device)
    return NULL;

  device->dvi_device = DVI_DEVICE_ALLOC();
  if (!device->dvi_device) {
    free (device);
    return NULL;
  }

  device->dvi_device->h_dpi = h_dpi;
  device->dvi_device->v_dpi = v_dpi;
  device->dvi_device->device_polling = DEV_poll;
  device->dvi_device->put_rectangle = _edvi_put_rectangle;
  device->dvi_device->put_bitmap = DEV_put_bitmap_rgb;

  return device;
}

void
edvi_device_delete (Edvi_Device *device)
{
  if (!device)
    return;

  free (device->dvi_device);
  free (device);
  device = NULL;
}

void
edvi_device_evas_object_set (Edvi_Device *device, Evas_Object *o)
{
  if (!device || !o)
    return;

  _edvi_device_o = o;
}

void
edvi_device_framebuffer_set (Edvi_Device *device, Edvi_Framebuffer *fb)
{
  if (!device || !fb)
    return;

  if (_edvi_device_fb)
    DVI_fb_dispose(_edvi_device_fb->dvi_framebuffer);
  _edvi_device_fb = fb;
}

void
edvi_device_offset_set (Edvi_Device *device, double offset_x, double offset_y)
{
  if (!device)
    return;

  _edvi_device_offset_x = offset_x;
  _edvi_device_offset_y = offset_y;
}

void
edvi_device_draw_page_finished (int is_finished)
{
  _edvi_device_draw_page_finished = is_finished;
}
