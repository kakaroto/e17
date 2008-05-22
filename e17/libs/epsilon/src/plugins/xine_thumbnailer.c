#include "Epsilon.h"
#include "Epsilon_Plugin.h"
#include "../config.h"

#include <dlfcn.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <xine.h>
#include <xine/xineutils.h>

#define XINE_THUMB_RETRY -10
#define XINE_THUMB_FAIL -11
#define XINE_THUMB_SUCCESS -9
#define XINE_THUMBNAILER_DEBUG 0

typedef struct _epsilon_xine_param
{
  xine_t *xine;
  xine_stream_t *stream;
  xine_video_port_t *vo_port;
  xine_audio_port_t *ao_port;
} epsilon_xine_param;

static int clip (int val);
static void i_yuy2_to_yv12 (const unsigned char *src, unsigned char *dest, int width,
		     int height);
static int yv12_to_rgb (const char *name, int w, int h, unsigned char ** buf,
		 Epsilon_Image ** rp);
static Epsilon_Image *epsilon_generate_thumb (Epsilon * e);
EAPI Epsilon_Plugin *epsilon_plugin_init (void);

/*----------------------------------------------*/

static int
clip (int val)
{
  if (val < 0)
    return 0;
  return (val > 255) ? 255 : val;
}


static void
i_yuy2_to_yv12 (const unsigned char *src, unsigned char *dest, int width,
		int height)
{
  register int stride = width >> 1;
  register int i, j;
  register unsigned char *img_y = dest;
  register unsigned char *img_cb = img_y + (width * height);
  register unsigned char *img_cr = img_cb + stride * (height >> 1);

  for (i = 0; i < height; i++)
    {
      for (j = 0; j < stride; j++)
	{
	  if (i % 2)
	    {
	      *img_y++ = *src++;
	      src++;
	      *img_y++ = *src++;
	      src++;
	    }
	  else
	    {
	      *img_y++ = *src++;
	      *img_cb++ = (src[width << 1] + src[0]) >> 1;
	      src++;
	      *img_y++ = *src++;
	      *img_cr++ = (src[width << 1] + src[0]) >> 1;
	      src++;
	    }
	}
    }
}

static int
yv12_to_rgb (const char *name, int w, int h, unsigned char ** buf, Epsilon_Image ** rp)
{
  unsigned char *_y = *buf,		/* video-data (Y, base) */
    *_u = _y + (w * h),		/* video-data (U, base) */
    *_v = _y + (w * h) + (w * h) / 4;	/* video-data (V, base) */
  unsigned int *rgb;			/* video-data (RGB, base) */

  int hw = ((w + 1) / 2), hh = ((h + 1) / 2);

  int i, j;			/* indices */

  int y, u, v;			/* video-data (YUV) */
  int r, g, b;			/* video-data (RGB) */

  int io, jo;			/* offsets */

  long sx, sy = 0;		/* sums */
  int ymin = 255, ymax = 0, yt;	/* delta */

  int ret = 0;

  Epsilon_Image *dst;		/* destination image (ARGB) */

  if (rp)
    *rp = NULL;

  dst = calloc(1, sizeof(Epsilon_Image));
  if (!dst) {
    if (XINE_THUMBNAILER_DEBUG) printf("Couldn't create  yv12_to_rgb imlib image..\n");
    return XINE_THUMB_FAIL;
  }
  dst->w = w;
  dst->h = h;
  dst->alpha = 1;
  dst->data = malloc(dst->w * dst->h * sizeof(int));
  if (!dst->data)
     {
	free(dst);
	if (XINE_THUMBNAILER_DEBUG) printf("Couldn't create  yv12_to_rgb imlib image..\n");
	return XINE_THUMB_FAIL;
     }
  dst->alpha = 1;

  rgb = dst->data;

  for (i = 0; i < h; ++i)
    {
      io = ((i * hh) / h);
      sx = 0;

      for (j = 0; j < w; ++j)
	{
	  jo = ((j * hw) / w) + (io * hw);

	  y = _y[(i * w) + j] - 16;
	  u = _u[jo] - 128;
	  v = _v[jo] - 128;

	  yt = _y[(i * w) + j];
	  if (yt > ymax)
	    ymax = yt;
	  if (yt < ymin)
	    ymin = yt;

	  sx += yt;

	  r = ((1.1644 * y) + (1.5960 * v));
	  g = ((1.1644 * y) - (0.3918 * u) - (0.8130 * v));
	  b = ((1.1644 * y) + (2.0172 * u));

	  rgb[0] = 0xff000000 |
	     (clip(b) << 16) |
	     (clip(g) << 8 ) |
	     (clip(b)      );
	  rgb++;
	}

      sy += (sx / w);

    }

  sy = (sy / h);

  if ((sy < 32) || (sy > 223))
    {
      if (XINE_THUMBNAILER_DEBUG) printf("Image too dark!..\n");
      ret = XINE_THUMB_RETRY;
    }
  else {
    ret = XINE_THUMB_SUCCESS;

    free (*buf);
    *buf = NULL;
  }

  if (rp)
    *rp = dst;

  return ret;
}

static float
pti (int ti)
{
  if (ti < 0)
    return 0;
  return (ti / 60000.0);
}

static Epsilon_Image *
epsilon_generate_thumb (Epsilon * e)
{
  int ret = XINE_THUMB_SUCCESS;
  Epsilon_Image *img = NULL;
  unsigned char *buf = NULL;
  int cnt, attempts = 0, length = -1, pos_perc, pos_time, new_perc, new_time,
  req_perc = 500, req_time = 500;
  static int old_length = -1;
  char cfg[PATH_MAX];
  epsilon_xine_param *param;

  param = calloc (1, sizeof (epsilon_xine_param));
  if (!(param->xine = xine_new ()))
    {
      return NULL;
    }
  snprintf (cfg, PATH_MAX, "%s%s", xine_get_homedir (), ".xine/config");
  xine_config_load (param->xine, cfg);
  xine_init (param->xine);

  /* opening xine output ports */
  if (!
      (param->vo_port =
       xine_open_video_driver (param->xine, "none", XINE_VISUAL_TYPE_NONE,
			       (void *) NULL)))
    {
      goto done;
    }

  param->ao_port = xine_open_audio_driver (param->xine, "none", NULL);

  /* open a xine stream connected to these ports */
  if (!
      (param->stream =
       xine_stream_new (param->xine, param->ao_port, param->vo_port)))
    {
      goto done;
    }


  if (!xine_open (param->stream, e->src))
    {
      ret = XINE_THUMB_FAIL;
      goto done;
    }

    if (XINE_THUMBNAILER_DEBUG) printf("Starting xine thumbnail process..\n");

try_get_chance:
  new_perc = -1;
  new_time = req_time;

  if (!xine_play (param->stream, 0, new_time))
    {
      new_time = -1;
      new_perc = req_perc;	/* 0..65535 */
      if (!xine_play (param->stream, new_perc, 0))
	{
	  ret = XINE_THUMB_FAIL;
	  if (XINE_THUMBNAILER_DEBUG) printf("Coudln't play video %s..\n", e->src);
	  goto close_stream;
	}
    }

  /* get position */
  for (cnt = 0; ((cnt < 50) &&
		 (!xine_get_pos_length
		  (param->stream, &pos_perc, &pos_time, &length)
		  || (length == old_length))); cnt++)
    usleep (1000);

  if (length < 0)
    {
      if (XINE_THUMBNAILER_DEBUG) printf("Video is 0 length!..\n");
      ret = XINE_THUMB_FAIL;
      goto close_stream;
    }

  /* wait till position changes */
  for (cnt = 0;
       (cnt < 50) &&
       (!xine_get_pos_length (param->stream, &new_perc, &new_time, &length) ||
	((new_perc == pos_perc) && (new_time == pos_time))); cnt++)
    {
      usleep (1000);
    }

  /* get snapshot */
  int w, h, ratio, format;

  old_length = length;

  /* ask what size a snapshot would be (if we can make one) */
  if (!xine_get_current_frame (param->stream, &w, &h, &ratio, &format, NULL))
    {
      attempts++;
      if (attempts < 10)
	{
	  if (req_time < 10000)
	    req_time += 1000;
	  if (req_perc < 10000)
	    req_perc += 1000;
	  goto try_get_chance;

	}
      else
	{
	  if (XINE_THUMBNAILER_DEBUG) printf("Exhausted attempts to thumbnail..\n");
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}
    }
  else if ((w <= 0) || (h <= 0))
    {
      if (XINE_THUMBNAILER_DEBUG) printf("Width/Height of video invalid: %d:%d..\n", w,h);
      ret = XINE_THUMB_FAIL;
      goto close_stream;
    }
  else
    {
      /* know we know the size, get a buffer for the snapshot */
      if (!(buf = malloc (w * h * 2)))
	{
	  if (XINE_THUMBNAILER_DEBUG) printf("Couldn't allocate memory for thumbnail buffer..\n");
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}

      /* make a snapshot! */
      if (!xine_get_current_frame
	  (param->stream, &w, &h, &ratio, &format, buf))
	{
	  if (XINE_THUMBNAILER_DEBUG) printf("Couldn't retrieve current video frame..\n");
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}

      else if ((w <= 0) || (h <= 0))
	{
	  if (XINE_THUMBNAILER_DEBUG) printf("Width/height of video invalid: %d:%d..\n",w,h);
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}

      if (format == XINE_IMGFMT_YUY2)
	{
	  unsigned char *buf2 = malloc (w * h * 2);
	  unsigned char *tmp = buf;
	  i_yuy2_to_yv12 (buf, buf2, w, h);

	  buf = buf2;
	  free (tmp);

	  format = XINE_IMGFMT_YV12;
	}

      if (format == XINE_IMGFMT_YV12)
	{
	  ret = yv12_to_rgb (e->src, w, h, &buf, &img);

	  if (ret != XINE_THUMB_SUCCESS)
	    {
	      if (img)
		{
		  if (++attempts > 10)
		    goto close_stream;
		  free(img->data);
		  free(img);
		  img = NULL;
		  if (ret == XINE_THUMB_RETRY)
		    {
		      if (buf)
			{
			  free (buf);
			  buf = NULL;
			  if ((req_perc += (attempts * 1000)) > 65535)
			    req_perc = 1000;
			  if ((req_time += (attempts * 2500)) > length)
			    req_time = 3000;
			  goto try_get_chance;
			}

	              if (XINE_THUMBNAILER_DEBUG) printf("Exhausted attempts to thumbnail pos 2..\n");
		      ret = XINE_THUMB_FAIL;
		      goto close_stream;
		    }
		}

	      else
		{
		  if (XINE_THUMBNAILER_DEBUG) printf("Couldn't complete yv12_to_rgb..\n");
		  ret = XINE_THUMB_FAIL;
		  goto close_stream;
		}
	    }
	}

      ret = XINE_THUMB_SUCCESS;

    close_stream:
      if (buf)
	{
	  free (buf);
	  buf = NULL;
	}
      xine_close (param->stream);

    done:
      if (ret != XINE_THUMB_SUCCESS)
	{
	  if (img)
	    {
	      free(img->data);
	      free(img);
	      img = NULL;
	    }
	}

      if (param->stream)
	{
	  xine_dispose (param->stream);
	  param->stream = NULL;
	}
      if (param->ao_port)
	{
	  xine_close_audio_driver (param->xine, param->ao_port);
	  param->ao_port = NULL;
	}
      if (param->vo_port)
	{
	  xine_close_video_driver (param->xine, param->vo_port);
	  param->vo_port = NULL;
	}

      free (param);

      return img;
    }
   return NULL;
}

/*---------------------------*/



Epsilon_Plugin *
epsilon_plugin_init (void)
{
  Epsilon_Plugin *plugin = calloc (1, sizeof (Epsilon_Plugin));
  plugin->epsilon_generate_thumb = &epsilon_generate_thumb;
  plugin->mime_types = ecore_list_new ();

  ecore_list_append (plugin->mime_types, "video/mpeg");
  ecore_list_append (plugin->mime_types, "video/x-ms-wmv");
  ecore_list_append (plugin->mime_types, "video/x-msvideo");
  ecore_list_append (plugin->mime_types, "video/quicktime");

  return plugin;
}
