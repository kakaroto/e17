#include "Epsilon.h"
#define X_DISPLAY_MISSING 1
#include <Imlib2.h>
#include <png.h>
#include "md5.h"
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../config.h"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifdef HAVE_EPEG_H
#include <Epeg.h>
#endif
#define THUMBNAIL_SIZE 256
#include "exiftags/exif.h"

#include <Evas.h>
#include <Ecore_Evas.h>
#include <Edje.h>

extern int epsilon_info_exif_props_as_int_get (Epsilon_Info * ei, unsigned
					       short lvl, long prop);
extern void epsilon_exif_info_free (Epsilon_Exif_Info * eei);
/*
 * epsilon_exif_info_get
 * NULL on no exif data present
 * Returns a valid pointer to an Epsilon_Exif_Info object 
 */
extern Epsilon_Exif_Info *epsilon_exif_info_get (Epsilon * e);

static char *epsilon_hash (const char *file);
static int _epsilon_png_mtime_get (const char *file);
#ifdef HAVE_EPEG_H
static int _epsilon_jpg_mtime_get (const char *file);
#endif
static FILE *_epsilon_open_png_file_reading (const char *filename);
static int _epsilon_png_write (const char *file, DATA32 * ptr,
			       int tw, int th, int sw, int sh, char *imformat,
			       int mtime, char *uri);

Epsilon *
epsilon_new (const char *file)
{
  Epsilon *result = NULL;
  if (file)
    {
      if (file[0] == '/')
	{
	  result = malloc (sizeof (Epsilon));
	  memset (result, 0, sizeof (Epsilon));
	  result->src = strdup (file);
	}
      else
	{
	  fprintf (stderr, "Invalid filename given: %s\n", file);
	  fprintf (stderr, "Epsilon expects the full path to file\n");
	}
    }
  return (result);
}

void
epsilon_free (Epsilon * e)
{
  if (e)
    {
      if (e->key)
	free (e->key);
      if (e->hash)
	free (e->hash);
      if (e->src)
	free (e->src);
      free (e);
    }
}
void
epsilon_init (void)
{
  int i = 0;
  struct stat status;
  char buf[PATH_MAX];
  char *dirs[] = { ".thumbnails", ".thumbnails/normal",
    ".thumbnails/large", ".thumbnails/fail"
  };

  for (i = 0; i < 4; i++)
    {
      snprintf (buf, PATH_MAX, "%s/%s", getenv ("HOME"), dirs[i]);
      if (!stat (buf, &status))
	continue;
      else
	mkdir (buf, S_IRUSR | S_IWUSR | S_IXUSR);
    }
}

void
epsilon_key_set (Epsilon * e, const char *key)
{
  if (e)
    {
      if (e->key)
	free (e->key);
      if (key)
	e->key = strdup (key);
      else
	e->key = NULL;
    }
}

void
epsilon_resolution_set (Epsilon * e, int w, int h)
{
  if (e && w > 0 && h > 0)
    {
      char buf[PATH_MAX];
      e->w = w;
      e->h = h;
    }
}

const char *
epsilon_file_get (Epsilon * e)
{
  char *result = NULL;
  if (e)
    result = e->src;
  return (result);
}
const char *
epsilon_thumb_file_get (Epsilon * e)
{
  int i = 0;
  struct stat status;
  char buf[PATH_MAX];
  char *dirs[] = { ".thumbnails/normal", ".thumbnails/large",
    ".thumbnails/fail"
  };

  if (!e)
    return (NULL);
  if (e->thumb)
    return (e->thumb);
  for (i = 0; i < 3; i++)
    {
#ifdef HAVE_EPEG_H
      snprintf (buf, PATH_MAX, "%s/%s/%s.jpg", getenv ("HOME"), dirs[i],
		e->hash);
      if (stat (buf, &status) == 0)
	{
	  e->thumb = strdup (buf);
	  break;
	}
#endif
      snprintf (buf, PATH_MAX, "%s/%s/%s.png", getenv ("HOME"), dirs[i],
		e->hash);
      if (stat (buf, &status) == 0)
	{
	  if (e->thumb)
	    free (e->thumb);
	  e->thumb = strdup (buf);
	  break;
	}
    }
  return (e->thumb);
}
static char *
epsilon_hash (const char *file)
{
  int n;
  MD5_CTX ctx;
  char md5out[(2 * MD5_HASHBYTES) + 1];
  unsigned char hash[MD5_HASHBYTES];
  static const char hex[] = "0123456789abcdef";

  char uri[PATH_MAX];

  if (!file)
    return (NULL);
  snprintf (uri, PATH_MAX, "file://%s", file);

  MD5Init (&ctx);
  MD5Update (&ctx, uri, strlen (uri));
  MD5Final (hash, &ctx);

  for (n = 0; n < MD5_HASHBYTES; n++)
    {
      md5out[2 * n] = hex[hash[n] >> 4];
      md5out[2 * n + 1] = hex[hash[n] & 0x0f];
    }
  md5out[2 * n] = '\0';
  return (strdup (md5out));
}
static Epsilon_Info *
epsilon_info_new (void)
{
  Epsilon_Info *result = NULL;
  result = malloc (sizeof (Epsilon_Info));
  memset (result, 0, sizeof (Epsilon_Info));
  return (result);
}

void
epsilon_info_free (Epsilon_Info * info)
{
  if (info)
    {
      if (info->uri)
	free (info->uri);
      if (info->mimetype)
	free (info->mimetype);
      if (info->eei)
	epsilon_exif_info_free (info->eei);
      free (info);
    }
}
Epsilon_Info *
epsilon_info_get (Epsilon * e)
{
  FILE *fp = NULL;
  Epsilon_Info *p = NULL;
#ifdef HAVE_EPEG_H
  Epeg_Image *im;
  Epeg_Thumbnail_Info info;
  int len = 0;
#endif

  if (!e || !epsilon_thumb_file_get (e))
    return (p);
#ifdef HAVE_EPEG_H
  len = strlen (e->thumb);
  if ((len > 4) &&
      (!strcmp (&e->thumb[len - 3], "jpg") ||
       !strcmp (&e->thumb[len - 3], "JPG")) &&
      (im = epeg_file_open (e->thumb)))
    {
      epeg_thumbnail_comments_get (im, &info);
      if (info.mimetype)
	{
	  p = epsilon_info_new ();
	  p->mtime = info.mtime;
	  p->w = info.w;
	  p->h = info.h;
	  if (info.uri)
	    p->uri = strdup (info.uri);
	  if (info.mimetype)
	    p->mimetype = strdup (info.mimetype);
	}
      epeg_close (im);
    }
  else
#endif
  if ((fp = _epsilon_open_png_file_reading (e->thumb)))
    {
      png_structp png_ptr = NULL;
      png_infop info_ptr = NULL;
      png_textp text_ptr;
      int num_text = 0, i;

      if (!
	  (png_ptr =
	   png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
	{
	  fclose (fp);
	  return (p);
	}

      if (!(info_ptr = png_create_info_struct (png_ptr)))
	{
	  png_destroy_read_struct (&png_ptr, (png_infopp) NULL,
				   (png_infopp) NULL);
	  fclose (fp);
	  return (p);
	}
      png_init_io (png_ptr, fp);
      png_read_info (png_ptr, info_ptr);

      p = epsilon_info_new ();
      num_text = png_get_text (png_ptr, info_ptr, &text_ptr, &num_text);
      for (i = 0; (i < num_text) && (i < 10); i++)
	{
	  png_text text = text_ptr[i];

	  if (!strcmp (text.key, "Thumb::MTime"))
	    p->mtime = atoi (text.text);
	  if (!strcmp (text.key, "Thumb::Image::Width"))
	    p->w = atoi (text.text);
	  if (!strcmp (text.key, "Thumb::Image::Height"))
	    p->h = atoi (text.text);
	  if (!strcmp (text.key, "Thumb::URI"))
	    p->uri = strdup (text.text);
	  if (!strcmp (text.key, "Thumb::Mimetype"))
	    p->mimetype = strdup (text.text);
	}
      /* png_read_end(png_ptr,info_ptr); */
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
      fclose (fp);
    }
  if ((p->eei = epsilon_exif_info_get (e)))
    {
      if (p->w == 0)
	{
	  p->w =
	    epsilon_info_exif_props_as_int_get (p, EPSILON_ED_IMG, 0xa002);
	}
      if (p->h == 0)
	{
	  p->h =
	    epsilon_info_exif_props_as_int_get (p, EPSILON_ED_IMG, 0xa003);
	}
    }
  return (p);
}

int
epsilon_info_exif_get (Epsilon_Info * info)
{
  if (info)
    {
      if (info->eei)
	return (1);
    }
  return (0);
}

int
epsilon_exists (Epsilon * e)
{
  int ok = 0, i = 0;
  int filemtime = 0, epsilonmtime = 0;
  struct stat filestatus;
  char *dirs[] = { "large", "normal", "fail/epsilon" };
  char home[PATH_MAX], buf[PATH_MAX];

  char *hash_seed = NULL;

  if (!e || !e->src)
    return (EPSILON_FAIL);

  if (!e->hash)
    {
      hash_seed = malloc (PATH_MAX * sizeof (char));
      memset (hash_seed, 0, PATH_MAX * sizeof (char));

      if (e->key)
	{
	  snprintf (buf, PATH_MAX, "%s:%s", e->src, e->key);
	  strcat (hash_seed, buf);
	}

      if ((e->w > 0) && (e->h > 0))
	{
	  snprintf (buf, PATH_MAX, ":%dx%d", e->w, e->h);
	  strcat (hash_seed, buf);
	}

      if (hash_seed)
	e->hash = epsilon_hash (hash_seed);
      else
	e->hash = epsilon_hash (e->src);
    }

  if (!e->hash)
    return (EPSILON_FAIL);

  snprintf (home, PATH_MAX, "%s", getenv ("HOME"));
  for (i = 0; i < 3; i++)
    {
#ifdef HAVE_EPEG_H
      snprintf (buf, PATH_MAX, "%s/.thumbnails/%s/%s.jpg", home,
		dirs[i], e->hash);
      if (!stat (buf, &filestatus))
	{
	  ok = 1;
	  break;
	}
#endif
      snprintf (buf, PATH_MAX, "%s/.thumbnails/%s/%s.png", home,
		dirs[i], e->hash);
      if (!stat (buf, &filestatus))
	{
	  ok = 2;
	  break;
	}
    }
  if (!ok)
    return (EPSILON_FAIL);
  if (!stat (e->src, &filestatus))
    {
      filemtime = filestatus.st_mtime;
#ifdef HAVE_EPEG_H
      if (ok == 1)
	epsilonmtime = _epsilon_jpg_mtime_get (buf);
      else
#endif
	epsilonmtime = _epsilon_png_mtime_get (buf);
      if (filemtime == epsilonmtime)
	return (EPSILON_OK);
    }
  return (EPSILON_FAIL);

}

int
epsilon_generate (Epsilon * e)
{
  int iw, ih;
  int tw = THUMBNAIL_SIZE, th = THUMBNAIL_SIZE;
  char outfile[PATH_MAX];
#ifdef HAVE_EPEG_H
  Epeg_Image *im;
  Epeg_Thumbnail_Info info;
  int len = 0;
#endif

  if (!e || !e->src || !e->hash)
    return (EPSILON_FAIL);
#ifdef HAVE_EPEG_H
  len = strlen (e->src);
  if ((len > 4) &&
      (!strcmp (&e->src[len - 3], "jpg") ||
       !strcmp (&e->src[len - 3], "JPG")) && (im = epeg_file_open (e->src)))
    {
      snprintf (outfile, PATH_MAX, "%s/.thumbnails/large/%s.jpg",
		getenv ("HOME"), e->hash);
      epeg_thumbnail_comments_get (im, &info);
      epeg_size_get (im, &iw, &ih);
      if (iw > ih)
	{
	  th = THUMBNAIL_SIZE * ((double) ih / (double) iw);
	}
      else
	{
	  tw = THUMBNAIL_SIZE * ((double) iw / (double) ih);
	}
      epeg_decode_size_set (im, tw, th);
      epeg_quality_set (im, 100);
      epeg_thumbnail_comments_enable (im, 1);
      epeg_file_output_set (im, outfile);
      if (!epeg_encode (im))
	{
	  epeg_close (im);
	  return (EPSILON_OK);
	}
      epeg_close (im);
    }
#endif
  {
    int mtime;
    char uri[PATH_MAX];
    char format[32];
    struct stat filestatus;
    int isedje = 0;
    Imlib_Image tmp = NULL;
    Imlib_Image src = NULL;
    Ecore_Evas *ee = NULL;

    if (stat (e->src, &filestatus) != 0)
      return (EPSILON_FAIL);

    mtime = filestatus.st_mtime;

    len = strlen (e->src);

    if ((len > 4) && (!strcmp (&e->src[len - 3], "edj")))
      {
	Evas *evas = NULL;
	Evas_Object *edje = NULL;
	const int *pixels;
	int w, h;
	edje_init ();
	if (!e->key)
	  {
	    fprintf (stderr, "Key required for this file type! ERROR!!\n");
	    return (EPSILON_FAIL);
	  }

	isedje = 1;
	if (e->w > 0)
	  w = e->w;
	else
	  w = THUMBNAIL_SIZE;

	if (e->h > 0)
	  h = e->h;
	else
	  h = THUMBNAIL_SIZE;

	ee = ecore_evas_buffer_new (w, h);
	if (ee)
	  {
	    evas = ecore_evas_get (ee);
	    edje = edje_object_add (evas);
	    if (edje_object_file_set (edje, e->src, e->key))
	      {
		evas_object_move (edje, 0, 0);
		evas_object_resize (edje, w, h);
		evas_object_show (edje);
		edje_message_signal_process ();

		pixels = ecore_evas_buffer_pixels_get (ee);
		tmp = imlib_create_image_using_data (w, h, (DATA32 *) pixels);

		imlib_context_set_image (tmp);
		snprintf (format, 32, "image/edje");
	      }
	    else
	      {
		printf ("Cannot load file %s, group %s\n", e->src, e->key);
		return (EPSILON_FAIL);
	      }
	  }
	else
	  {
	    fprintf (fprintf, "Cannot create buffer canvas! ERROR!\n");
	    return (EPSILON_FAIL);
	  }
      }


    if (!tmp)
      {
	tmp = imlib_load_image_immediately_without_cache (e->src);
	imlib_context_set_image (tmp);
	snprintf (format, 32, "image/%s", imlib_image_format ());
      }

    if (tmp)
      {
	iw = imlib_image_get_width ();
	ih = imlib_image_get_height ();
	if (iw > ih)
	  {
	    th = THUMBNAIL_SIZE * ((double) ih / (double) iw);
	  }
	else
	  {
	    tw = THUMBNAIL_SIZE * ((double) iw / (double) ih);
	  }
	imlib_context_set_cliprect (0, 0, tw, th);
	if ((src = imlib_create_cropped_scaled_image (0, 0, iw, ih, tw, th)))
	  {
	    imlib_free_image_and_decache ();
	    imlib_context_set_image (src);
	    imlib_image_set_has_alpha (1);
	    imlib_image_set_format ("argb");
	    snprintf (uri, PATH_MAX, "file://%s", e->src);
	    snprintf (outfile, PATH_MAX, "%s/.thumbnails/large/%s.png",
		      getenv ("HOME"), e->hash);
	    if (!_epsilon_png_write (outfile,
				     imlib_image_get_data (), tw, th, iw, ih,
				     format, mtime, uri))
	      {
		imlib_free_image_and_decache ();
		return (EPSILON_OK);
	      }
	    imlib_free_image_and_decache ();
	  }

      }
  }
  return (EPSILON_FAIL);
}

#ifdef HAVE_EPEG_H
static int
_epsilon_jpg_mtime_get (const char *file)
{
  int result = 0;
  Epeg_Image *im;
  Epeg_Thumbnail_Info info;

  if ((im = epeg_file_open (file)))
    {
      epeg_thumbnail_comments_get (im, &info);
      if (info.mimetype)
	result = info.mtime;
      epeg_close (im);
    }
  return (result);
}
#endif

static FILE *
_epsilon_open_png_file_reading (const char *filename)
{
  FILE *fp = NULL;

  if ((fp = fopen (filename, "rb")))
    {
      char buf[4];
      int bytes = sizeof (buf);
      int ret;

      ret = fread (buf, sizeof (char), bytes, fp);
      if (ret != bytes)
	{
	  fclose (fp);
	  fp = NULL;
	}
      else
	{
	  if ((ret = png_check_sig (buf, bytes)))
	    rewind (fp);
	  else
	    {
	      fclose (fp);
	      fp = NULL;
	    }
	}
    }
  return fp;
}

static int
_epsilon_png_mtime_get (const char *file)
{
  int result = 0;
  FILE *fp = NULL;

  if ((fp = _epsilon_open_png_file_reading (file)))
    {
      png_structp png_ptr = NULL;
      png_infop info_ptr = NULL;
      png_textp text_ptr;
      int num_text = 0, i;

      if (!
	  (png_ptr =
	   png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
	{
	  fclose (fp);
	  return (result);
	}

      if (!(info_ptr = png_create_info_struct (png_ptr)))
	{
	  png_destroy_read_struct (&png_ptr, (png_infopp) NULL,
				   (png_infopp) NULL);
	  fclose (fp);
	  return (result);
	}
      png_init_io (png_ptr, fp);
      png_read_info (png_ptr, info_ptr);

      num_text = png_get_text (png_ptr, info_ptr, &text_ptr, &num_text);
      for (i = 0; (i < num_text) && (i < 10); i++)
	{
	  png_text text = text_ptr[i];

	  if (!strcmp (text.key, "Thumb::MTime"))
	    result = atoi (text.text);
	}
      /* png_read_end(png_ptr,info_ptr); */
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
      fclose (fp);
    }
  return (result);
}

#define GET_TMPNAME(_tmpbuf,_file) { \
  int _l,_ll; \
  char _buf[21]; \
  _l=snprintf(_tmpbuf,PATH_MAX,"%s",_file); \
  _ll=snprintf(_buf,21,"epsilon-%06d.png",(int)getpid());  \
  strncpy(&tmpfile[_l-35],_buf,_ll+1); }

static int
_epsilon_png_write (const char *file, DATA32 * ptr, int tw, int th, int sw,
		    int sh, char *imformat, int mtime, char *uri)
{
  FILE *fp = NULL;
  char mtimebuf[32], widthbuf[10], heightbuf[10], tmpfile[PATH_MAX] = "";
  int i, j, k, has_alpha = 1, ret = 0;

/*
  DATA32      *ptr=NULL;
*/
  png_infop info_ptr;
  png_color_8 sig_bit;
  png_structp png_ptr;
  png_text text_ptr[5];
  png_bytep row_ptr, row_data = NULL;

  GET_TMPNAME (tmpfile, file);

/*
  has_alpha = evas_object_image_alpha_get (e->image);
 */
  if ((fp = fopen (tmpfile, "wb")))
    {
      if (!
	  (png_ptr =
	   png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
	{
	  ret = 1;
	}
      if (!(info_ptr = png_create_info_struct (png_ptr)))
	{
	  png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
	  ret = 1;
	}
      if (setjmp (png_ptr->jmpbuf))
	{
	  png_destroy_write_struct (&png_ptr, &info_ptr);
	  ret = 1;
	}

      png_init_io (png_ptr, fp);

#ifdef PNG_TEXT_SUPPORTED
      /* setup tags here */
      text_ptr[0].key = "Thumb::URI";
      text_ptr[0].text = uri;
      text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (mtimebuf, 32, "%d", mtime);
      text_ptr[1].key = "Thumb::MTime";
      text_ptr[1].text = mtimebuf;
      text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (widthbuf, PATH_MAX, "%d", sw);
      text_ptr[2].key = "Thumb::Image::Width";
      text_ptr[2].text = widthbuf;
      text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (heightbuf, PATH_MAX, "%d", sh);
      text_ptr[3].key = "Thumb::Image::Height";
      text_ptr[3].text = heightbuf;
      text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;

      text_ptr[4].key = "Thumb::Mimetype";
      text_ptr[4].text = imformat;
      text_ptr[4].compression = PNG_TEXT_COMPRESSION_NONE;

      png_set_text (png_ptr, info_ptr, text_ptr, 5);
#endif
      if (has_alpha)
	{
	  png_set_IHDR (png_ptr, info_ptr, tw, th, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
#ifdef WORDS_BIGENDIAN
	  png_set_swap_alpha (png_ptr);
#else
	  png_set_bgr (png_ptr);
#endif
	}
      else
	{
	  png_set_IHDR (png_ptr, info_ptr, tw, th, 8,
			PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	  row_data = (png_bytep) malloc (tw * 3 * sizeof (char));
	}

      sig_bit.red = 8;
      sig_bit.green = 8;
      sig_bit.blue = 8;
      sig_bit.alpha = 8;
      png_set_sBIT (png_ptr, info_ptr, &sig_bit);

      png_set_compression_level (png_ptr, 9);	/* 0?? ### */
      png_write_info (png_ptr, info_ptr);
      png_set_shift (png_ptr, &sig_bit);
      png_set_packing (png_ptr);

      for (i = 0; i < th; i++)
	{
	  if (has_alpha)
	    row_ptr = (png_bytep) ptr;
	  else
	    {
	      for (j = 0, k = 0; j < tw; k++)
		{
		  row_data[j++] = (ptr[k] >> 16) & 0xff;
		  row_data[j++] = (ptr[k] >> 8) & 0xff;
		  row_data[j++] = (ptr[k]) & 0xff;
		}
	      row_ptr = (png_bytep) row_data;
	    }
	  png_write_row (png_ptr, row_ptr);
	  ptr += tw;
	}

      png_write_end (png_ptr, info_ptr);
      png_destroy_write_struct (&png_ptr, &info_ptr);
      png_destroy_info_struct (png_ptr, &info_ptr);

      if (!rename (tmpfile, file))
	{
	  if (chmod (file, S_IWUSR | S_IRUSR))
	    fprintf (stderr,
		     "epsilon: could not set permissions on \"%s\"!?\n",
		     file);
	}
    }
  else
    fprintf (stderr, "epsilon: Unable to open \"%s\" for writing\n", tmpfile);

  fflush (fp);
  if (fp)
    fclose (fp);
  if (row_data)
    free (row_data);

  return (ret);
}
