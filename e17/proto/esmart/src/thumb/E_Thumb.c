#include "E_Thumb.h"
#define X_DISPLAY_MISSING 1
#include <Imlib2.h>
#include <png.h>
#include "md5.h"
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define E_THUMBNAIL_SIZE 128

static int _e_thumb_write (Evas_Object * o, DATA32 * ptr);
static int _e_thumb_read_file_attributes (Evas_Object * o);
static void _e_thumb_add (Evas_Object * o);
static void _e_thumb_del (Evas_Object * o);
static void _e_thumb_show (Evas_Object * o);
static void _e_thumb_hide (Evas_Object * o);
static void _e_thumb_raise (Evas_Object * o);
static void _e_thumb_lower (Evas_Object * o);
static void _e_thumb_generate (Evas_Object * o);
static void _e_thumb_hash_name (Evas_Object * o);
static void _e_thumb_clip_unset (Evas_Object * o);
static void _e_thumb_layer_set (Evas_Object * o, int layer);
static void _e_thumb_move (Evas_Object * o, double x, double y);
static void _e_thumb_resize (Evas_Object * o, double w, double h);
static void _e_thumb_clip_set (Evas_Object * o, Evas_Object * clip);
static void _e_thumb_stack_above (Evas_Object * o, Evas_Object * above);
static void _e_thumb_stack_below (Evas_Object * o, Evas_Object * below);
static void _e_thumb_color_set (Evas_Object * o, int r, int g, int b, int a);

void
e_thumb_init (void)
{
    int i = 0;
    struct stat status;
    char buf[PATH_MAX];
    char *dirs[] = { ".thumbnails", ".thumbnails/normal", 
		    ".thumbnails/large", ".thumbnails/fail" };
    
    for(i = 0; i < 4; i++)
    {
	snprintf(buf, PATH_MAX, "%s/%s", getenv("HOME"), dirs[i]);
	if(!stat(buf, &status)) continue;
	else mkdir(buf, S_IRUSR | S_IWUSR | S_IXUSR);
    }
}

Evas_Object *
e_thumb_new (Evas * evas, const char *file)
{
  Evas_Object *result = NULL;
  if (file && file[0] == '/')
    {
      E_Thumb *e = NULL;
      char uri[PATH_MAX];
      Evas_Smart *s = NULL;
      s = evas_smart_new ("E_Thumb",
			  _e_thumb_add,
			  _e_thumb_del,
			  _e_thumb_layer_set,
			  _e_thumb_raise,
			  _e_thumb_lower,
			  _e_thumb_stack_above,
			  _e_thumb_stack_below,
			  _e_thumb_move,
			  _e_thumb_resize,
			  _e_thumb_show,
			  _e_thumb_hide,
			  _e_thumb_color_set,
			  _e_thumb_clip_set, _e_thumb_clip_unset, NULL);
      result = evas_object_smart_add (evas, s);

      if ((e = (E_Thumb *) evas_object_smart_data_get (result)))
	{
	  snprintf (uri, PATH_MAX, "%s", file);
	  e->file.name = strdup (uri);
	  snprintf (uri, PATH_MAX, "file://%s", file);
	  e->uri = strdup (uri);
	  _e_thumb_hash_name (result);
	  if (!e->image)
	    {
	      evas_object_del (result);
	      result = NULL;
	    }
	}
    }
  return (result);
}

void
e_thumb_free (Evas_Object * o)
{
  if (o)
    evas_object_del (o);
}
const char *
e_thumb_file_get (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	return (e->file.name);
    }
  return (NULL);
}

void
e_thumb_geometry_get (Evas_Object * o, int *w, int *h)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  if (w)
	    *w = e->file.w;
	  if (h)
	    *h = e->file.h;
	}
    }
}
Evas_Object *
e_thumb_evas_object_get (Evas_Object * o)
{
  Evas_Object *result = NULL;
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  Imlib_Image tmp = NULL;

	  tmp = imlib_load_image_immediately_without_cache (e->file.name);
	  if (tmp)
	    {
	      imlib_context_set_image (tmp);

	      result = evas_object_image_add (evas_object_evas_get (o));
	      evas_object_image_alpha_set (result, 1);
	      e->file.w = imlib_image_get_width ();
	      e->file.h = imlib_image_get_height ();
	      evas_object_image_size_set (result, e->file.w, e->file.h);

	      evas_object_image_data_copy_set (result,
					       imlib_image_get_data_for_reading_only
					       ());
	      imlib_free_image_and_decache ();
	    }
	}
    }
  return (result);
}

/*==========================================================================
 * Smart Object Functions
 *========================================================================*/
static void
_e_thumb_add (Evas_Object * o)
{
  E_Thumb *e = NULL;

  e = (E_Thumb *) malloc (sizeof (E_Thumb));
  memset (e, 0, sizeof (E_Thumb));

  evas_object_smart_data_set (o, e);
}

/**
 * _e_thumb_del - used when deleting the object from the evas
 * @o - Our Smart Evas Object
 */
static void
_e_thumb_del (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)) == NULL)
	{
	  if (e->image)
	    evas_object_del (e->image);
	  if (e->file.name)
	    free (e->file.name);
	  if (e->thumb.name)
	    free (e->thumb.name);
	  if (e->uri)
	    free (e->uri);
	  if (e->format)
	    free (e->format);
	  free (e);
	}
    }
}

/**
 * _e_thumb_set - used when setting the thumbnail's layer
 * @o - Our Smart Evas Object
 */
static void
_e_thumb_layer_set (Evas_Object * o, int layer)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  if (e->image)
	    evas_object_layer_set (e->image, layer);
	}
    }
}

/**
 * _e_thumb_raise - send the thumbnail a "raise" request
 * @o - Our Smart Evas Object
 */
static void
_e_thumb_raise (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_raise (e->image);
	}
    }
}

/**
 * _e_thumb_raise - send the thumbnail a "lower" request
 * @o - Our Smart Evas Object
 */
static void
_e_thumb_lower (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_lower (e->image);
	}
    }
}

/**
 * _e_thumb_stack_above - used when placing the object above
 * another object in the same layer
 * @o - Our Smart Evas Object
 * @below - the object we want above
 */
static void
_e_thumb_stack_above (Evas_Object * o, Evas_Object * above)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_stack_above (e->image, above);
	}
    }
}

/**
 * _e_thumb_stack_below - used when placing this object below
 * another object in the same layer
 * @o - Our Smart Evas Object
 * @below - the object we want below
 */
static void
_e_thumb_stack_below (Evas_Object * o, Evas_Object * below)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_stack_below (e->image, below);
	}
    }
}

/**
 * _e_thumb_move - move the smart text to the specified coordinates
 * @o - Our Smart Object
 * @x - the x-coordinate
 * @y - the y-coordinate 
 */
static void
_e_thumb_move (Evas_Object * o, double x, double y)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  e->x = x;
	  e->y = y;
	  _e_thumb_resize (o, e->w, e->h);
	}
    }
}

/**
 * _e_thumb_resize - resize the smart text to the specified values
 * @o - Our Smart Evas Object, an Evas_Text_Smart
 * @w - the new width
 * @h - the new height
 */
static void
_e_thumb_resize (Evas_Object * o, double w, double h)
{
  if (o && (w > 1) && (h > 1))
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  double ww = w, hh = h;
	  e->w = w;
	  e->h = h;
	  if (e->thumb.w > e->thumb.h)
	    hh *= (double) e->thumb.h / (double) e->thumb.w;
	  else
	    ww *= (double) e->thumb.w / (double) e->thumb.h;

	  evas_object_resize (e->image, ww, hh);
	  evas_object_image_fill_set (e->image, 0.0, 0.0, ww, hh);
	  evas_object_move (e->image, e->x + ((w - ww) / 2),
			    e->y + ((h - hh) / 2));
	}
    }
}

/**
 * _e_thumb_show - used sending the smart text a "show" request
 * @o - Our Smart Evas Object, an Evas_Text_Smart
 */
static void
_e_thumb_show (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_show (e->image);
	}
    }
}

/**
 * _e_thumb_hide - used sending the smart text a "hide" request
 * @o - Our Smart Evas Object, an Evas_Text_Smart
 */
static void
_e_thumb_hide (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_hide (e->image);
	}
    }
}

/**
 * _e_thumb_color_set - set the smart text color
 * @o - Our Smart Evas Object, an Evas_Text_Smart
 * @r - the new r value
 * @g - the new g value
 * @b - the new b value
 * @a - the new a value
 */
static void
_e_thumb_color_set (Evas_Object * o, int r, int g, int b, int a)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_color_set (e->image, r, g, b, a);
	}
    }
}

/**
 * _e_thumb_clip_set - clip the smart text to the object clip
 * @o - Our Smart Evas Object, an Evas_Text_Smart
 * @clip - The Evas_Object we want to clip the smart text to
 */
static void
_e_thumb_clip_set (Evas_Object * o, Evas_Object * clip)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_clip_set (e->image, clip);
	}
    }
}

/**
 * _e_thumb_clip_unset - remove the smart text from the clipped area
 * @o - Our Smart Evas Object, an Evas_Text_Smart
 */
static void
_e_thumb_clip_unset (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  evas_object_clip_unset (e->image);
	}
    }
}

static char *
_e_thumb_hash_uri (const char *uri)
{
  int n;
  MD5_CTX ctx;
  char md5out[32];
  char nn[PATH_MAX];
  struct stat thumbstatus;
  unsigned char hash[MD5_HASHBYTES];
  static const char hex[] = "0123456789abcdef";

  char *dirs[] = { "normal", "large", "fail/evidence", "normal" };
  /* use normal as the fallback */

  if (!uri)
    return (NULL);

  MD5Init (&ctx);
  MD5Update (&ctx, uri, strlen (uri));
  MD5Final (hash, &ctx);

  for (n = 0; n < MD5_HASHBYTES; n++)
    {
      md5out[2 * n] = hex[hash[n] >> 4];
      md5out[2 * n + 1] = hex[hash[n] & 0x0f];
    }
  md5out[2 * n] = '\0';

  for (n = 0; n < 4; n++)
    {
      snprintf (nn, PATH_MAX, "%s/.thumbnails/%s/%s.png", getenv ("HOME"),
		dirs[n], md5out);
      if (!stat (nn, &thumbstatus))
	break;
    }
  return (strdup (nn));

}
static void
_e_thumb_hash_name (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  int ok = 0;
	  struct stat filestatus;

	  if (!stat (e->file.name, &filestatus))
	    e->file.mtime = filestatus.st_mtime;
	  if ((e->thumb.name = _e_thumb_hash_uri (e->uri)))
	    {
	      if (!_e_thumb_read_file_attributes (o))
		{
		  if (e->file.mtime == e->thumb.mtime)
		    {
		      e->image =
			evas_object_image_add (evas_object_evas_get (o));
		      evas_object_image_file_set (e->image, e->thumb.name,
						  NULL);
		      if (!evas_object_image_load_error_get (e->image))
			{
			  ok = 1;
			  evas_object_image_size_get (e->image,
						      &e->thumb.w,
						      &e->thumb.h);
			}
		      else
			{
			  evas_object_del (e->image);
			  e->image = NULL;
			}
		    }
		}
	    }
	  if (!ok)
	    {
	      int ret = 0;
	      e->thumb.w = e->thumb.h = E_THUMBNAIL_SIZE;
	      _e_thumb_generate (o);
	      e->image = evas_object_image_add (evas_object_evas_get (o));
	      evas_object_image_file_set (e->image, e->thumb.name, NULL);
	      if ((ret = evas_object_image_load_error_get (e->image)) != 0)
		{
		  evas_object_del (e->image);
		  e->image = NULL;
		}

	    }
	}
    }
}

static void
_e_thumb_generate (Evas_Object * o)
{
  if (o)
    {
      E_Thumb *e = NULL;
      if ((e = (E_Thumb *) evas_object_smart_data_get (o)))
	{
	  Imlib_Image tmp = NULL;
	  Imlib_Image src = NULL;

	  tmp = imlib_load_image_immediately_without_cache (e->file.name);
	  if (tmp)
	    {
	      imlib_context_set_image (tmp);
	      e->format = strdup (imlib_image_format ());
	      e->file.w = imlib_image_get_width ();
	      e->file.h = imlib_image_get_height ();

	      if (e->file.w > e->file.h)
		{
		  e->thumb.w =
		    (int) (((double) e->thumb.w / (double) e->file.w) *
			   (double) e->file.w);
		  e->thumb.h =
		    (int) (((double) e->thumb.h / (double) e->file.w) *
			   (double) e->file.h);
		}
	      else
		{
		  e->thumb.w =
		    (int) (((double) e->thumb.w / (double) e->file.h) *
			   (double) e->file.w);
		  e->thumb.h =
		    (int) (((double) e->thumb.h / (double) e->file.h) *
			   (double) e->file.h);
		}
	      imlib_context_set_cliprect (0, 0, e->thumb.w, e->thumb.h);
	      if ((src = imlib_create_cropped_scaled_image (0, 0,
							    e->file.w,
							    e->file.h,
							    e->thumb.w,
							    e->thumb.h)))
		{
		  imlib_free_image_and_decache ();
		  imlib_context_set_image (src);
		  /*
		     imlib_context_set_anti_alias (1);
		   */
		  imlib_image_set_has_alpha (1);
		  imlib_image_set_format ("argb");
		  if (_e_thumb_write (o, imlib_image_get_data ()) == 0)
		    fprintf (stderr, "Cached %s successfully\n",
			     e->file.name);
		  else
		    fprintf (stderr, "Unable to cache %s\n", e->file.name);
		}
	      imlib_free_image_and_decache ();
	    }
	  else
	    {
	      evas_object_del (e->image);
	      e->image = NULL;
	    }
	}
    }
}

static FILE *
_e_thumb_open_png_file_reading (char *filename)
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
_e_thumb_read_file_attributes (Evas_Object * o)
{
  int result = 0;
  FILE *fp = NULL;
  E_Thumb *e = NULL;

  if ((e = (E_Thumb *) evas_object_smart_data_get (o)) == NULL)
    return 1;

  if ((fp = _e_thumb_open_png_file_reading (e->thumb.name)))
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
	  return 1;
	}

      if (!(info_ptr = png_create_info_struct (png_ptr)))
	{
	  png_destroy_read_struct (&png_ptr, (png_infopp) NULL,
				   (png_infopp) NULL);
	  fclose (fp);
	  return 1;
	}
      png_init_io (png_ptr, fp);
      png_read_info (png_ptr, info_ptr);

      num_text = png_get_text (png_ptr, info_ptr, &text_ptr, &num_text);
      for (i = 0; (i < num_text) && (i < 10); i++)
	{
	  png_text text = text_ptr[i];

	  if (!strcmp (text.key, "Thumb::MTime"))
	    e->thumb.mtime = atoi (text.text);
	  if (!strcmp (text.key, "Thumb::Image::Width"))
	    e->thumb.w = atoi (text.text);
	  if (!strcmp (text.key, "Thumb::Image::Height"))
	    e->thumb.h = atoi (text.text);
	  if (!strcmp (text.key, "Thumb::URI"))
	    if (strcmp (text.text, e->uri))
	      result = 1;
	}
      /* png_read_end(png_ptr,info_ptr); */
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
      fclose (fp);
    }
  else
    {
      result = 1;
    }
  return (result);
}

#define GET_TMPNAME(_tmpbuf,_file) { \
  int _l,_ll; \
  char _buf[21]; \
  _l=snprintf(_tmpbuf,PATH_MAX,"%s",_file); \
  _ll=snprintf(_buf,21,"e_thumb-%06d.png",(int)getpid());  \
  strncpy(&tmpfile[_l-35],_buf,_ll+1); }

/*****************************************************************************/

static int
_e_thumb_write (Evas_Object * o, DATA32 * ptr)
{
  FILE *fp = NULL;
  char mtimebuf[PATH_MAX], widthbuf[10], heightbuf[10], formatbuf[32],
    tmpfile[PATH_MAX] = "";
  int i, j, k, has_alpha = 1, ret = 0;

/*
  DATA32      *ptr=NULL;
*/
  png_infop info_ptr;
  png_color_8 sig_bit;
  png_structp png_ptr;
  png_text text_ptr[5];
  png_bytep row_ptr, row_data = NULL;

  E_Thumb *e = NULL;

  if ((e = (E_Thumb *) evas_object_smart_data_get (o)) == NULL)
    return 1;
  GET_TMPNAME (tmpfile, e->thumb.name);

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
      text_ptr[0].text = e->uri;
      text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (mtimebuf, PATH_MAX, "%d", (int) e->file.mtime);
      text_ptr[1].key = "Thumb::MTime";
      text_ptr[1].text = mtimebuf;
      text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (widthbuf, PATH_MAX, "%d", e->file.w);
      text_ptr[2].key = "Thumb::Image::Width";
      text_ptr[2].text = widthbuf;
      text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (heightbuf, PATH_MAX, "%d", e->file.h);
      text_ptr[3].key = "Thumb::Image::Height";
      text_ptr[3].text = heightbuf;
      text_ptr[3].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (formatbuf, PATH_MAX, "image/%s", e->format);
      text_ptr[4].key = "Thumb::Mimetype";
      text_ptr[4].text = formatbuf;
      text_ptr[4].compression = PNG_TEXT_COMPRESSION_NONE;

      png_set_text (png_ptr, info_ptr, text_ptr, 5);
#endif
      if (has_alpha)
	{
	  png_set_IHDR (png_ptr, info_ptr, e->thumb.w, e->thumb.h, 8,
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
	  png_set_IHDR (png_ptr, info_ptr, e->thumb.w, e->thumb.h, 8,
			PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	  row_data = (png_bytep) malloc (e->thumb.w * 3 * sizeof (char));
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
/*
    ptr=evas_object_image_data_get(e->image,1);
*/
      for (i = 0; i < e->thumb.h; i++)
	{
	  if (has_alpha)
	    row_ptr = (png_bytep) ptr;
	  else
	    {
	      for (j = 0, k = 0; j < e->thumb.w; k++)
		{
		  row_data[j++] = (ptr[k] >> 16) & 0xff;
		  row_data[j++] = (ptr[k] >> 8) & 0xff;
		  row_data[j++] = (ptr[k]) & 0xff;
		}
	      row_ptr = (png_bytep) row_data;
	    }
	  png_write_row (png_ptr, row_ptr);
	  ptr += e->thumb.w;
	}

      png_write_end (png_ptr, info_ptr);
      png_destroy_write_struct (&png_ptr, &info_ptr);
      png_destroy_info_struct (png_ptr, &info_ptr);

      if (!rename (tmpfile, e->thumb.name))
	{
	  if (chmod (e->thumb.name, S_IWUSR | S_IRUSR))
	    printf ("e_thumb: could not set permissions on \"%s\"!?\n",
		    e->thumb.name);
	}
    }
  else
    printf ("e_thumb: Unable to open \"%s\" for writing\n", tmpfile);

  fflush (fp);
  if (fp)
    fclose (fp);
  if (row_data)
    free (row_data);

  return (ret);
}
