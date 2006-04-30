#include "Epsilon.h"
#include "epsilon_plugin.h"
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
#include <Imlib2.h>

#define XINE_THUMB_RETRY -10
#define XINE_THUMB_FAIL -11
#define XINE_THUMB_SUCCESS -9

typedef struct _epsilon_xine_param
{
  xine_t *xine;
  xine_stream_t *stream;
  xine_video_port_t *vo_port;
  xine_audio_port_t *ao_port;
} epsilon_xine_param;

Imlib_Image epsilon_thumb_imlib_standardize ();
int clip (int val);
void i_yuy2_to_yv12 (const unsigned char *src, unsigned char *dest, int width,
		     int height);
int yv12_to_rgb (const char *name, int w, int h, DATA8 ** buf,
		 Imlib_Image * rp);
Imlib_Image epsilon_generate_thumb (Epsilon * e);
Epsilon_Plugin *epsilon_plugin_init ();

/*----------------------------------------------*/
Imlib_Image
epsilon_thumb_imlib_standardize ()
{
  Imlib_Image dst = NULL;
  int dw, dh;
  int sw = imlib_image_get_width (), sh = imlib_image_get_height ();
  int s = 128;

  if (sw > sh)
    {
      dw = s;
      dh = (s * sh) / sw;
    }
  else
    {
      dh = s;
      dw = (s * sw) / sh;
    }

  imlib_context_set_cliprect (0, 0, dw, dh);

  if ((dst = imlib_create_cropped_scaled_image (0, 0, sw, sh, dw, dh)))
    {

      imlib_context_set_image (dst);
      imlib_context_set_anti_alias (1);
      imlib_image_set_has_alpha (1);
    }

  return dst;
}

int
clip (int val)
{
  if (val < 0)
    return 0;
  return (val > 255) ? 255 : val;
}


void
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

int
yv12_to_rgb (const char *name, int w, int h, DATA8 ** buf, Imlib_Image * rp)
{
  DATA8 *_y = *buf,		/* video-data (Y, base) */
    *_u = _y + (w * h),		/* video-data (U, base) */
    *_v = _y + (w * h) + (w * h) / 4;	/* video-data (V, base) */
  DATA8 *_rgb,			/* video-data (RGB, base) */
   *__rgb;			/* video-data (RGB, current row) */

  int hw = ((w + 1) / 2), hh = ((h + 1) / 2);

  int i, j;			/* indices */

  int y, u, v;			/* video-data (YUV) */
  int r, g, b;			/* video-data (RGB) */

  int io, jo;			/* offsets */

  long sx, sy = 0;		/* sums */
  int ymin = 255, ymax = 0, yt;	/* delta */

  int ret = 0;

  Imlib_Image dst;		/* destination image (ARGB) */

  if (rp)
    *rp = NULL;

  if (!(dst = imlib_create_image (w, h)))
    return XINE_THUMB_FAIL;

  imlib_context_set_image (dst);
  imlib_image_set_format ("argb");
  imlib_image_set_has_alpha (1);

  if (!(_rgb = (DATA8 *) imlib_image_get_data ()))
    {
      imlib_free_image ();
      return XINE_THUMB_FAIL;
    }

#define BPP 4

  __rgb = _rgb;

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

	  __rgb[(j * BPP) + 0] = clip (b);
	  __rgb[(j * BPP) + 1] = clip (g);
	  __rgb[(j * BPP) + 2] = clip (r);
	  __rgb[(j * BPP) + 3] = 255;
	}

      sy += (sx / w);

      __rgb += (w * BPP);
    }

  sy = (sy / h);

  if ((sy < 32) || (sy > 223))
    {
      ret = XINE_THUMB_RETRY;
    }
  else
    ret = XINE_THUMB_SUCCESS;

  free (*buf);
  *buf = NULL;

  imlib_image_put_back_data ((DATA32 *) _rgb);

  if (rp)
    *rp = dst;

  return ret;
}

float
pti (int ti)
{
  if (ti < 0)
    return 0;
  return (ti / 60000.0);
}

Imlib_Image
epsilon_generate_thumb (Epsilon * e)
{
  int ret = XINE_THUMB_SUCCESS;
  char *p;
  Imlib_Image img = NULL;
  DATA8 *buf = NULL;
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
      if (attempts < 4)
	{
	  if (req_time < 10000)
	    req_time += 1000;
	  if (req_perc < 10000)
	    req_perc += 1000;
	  goto try_get_chance;

	}
      else
	{
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}
    }
  else if ((w <= 0) || (h <= 0))
    {
      ret = XINE_THUMB_FAIL;
      goto close_stream;
    }
  else
    {
      char *v;

      /* know we know the size, get a buffer for the snapshot */
      if (!(buf = malloc (w * h * 2)))
	{
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}

      /* make a snapshot! */
      if (!xine_get_current_frame
	  (param->stream, &w, &h, &ratio, &format, buf))
	{
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}

      else if ((w <= 0) || (h <= 0))
	{
	  ret = XINE_THUMB_FAIL;
	  goto close_stream;
	}

      if (format == XINE_IMGFMT_YUY2)
	{
	  char *buf2 = malloc (w * h * 2);
	  char *tmp = buf;
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
		  if (++attempts > 4)
		    goto close_stream;
		  imlib_free_image ();
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
		      ret = XINE_THUMB_FAIL;
		      goto close_stream;
		    }
		}

	      else
		{
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
	      imlib_free_image ();
	      img = NULL;
	    }
	}
      else
	img = epsilon_thumb_imlib_standardize (e);

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
}

/*---------------------------*/



Epsilon_Plugin *
epsilon_plugin_init ()
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
