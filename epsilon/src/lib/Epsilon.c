#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "Epsilon.h"
#define X_DISPLAY_MISSING 1
#include "Epsilon_Plugin.h"
#ifdef HAVE_PNG_H
#include <png.h>
#endif
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "md5.h"
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
#ifdef HAVE_EPEG_H
#include <Epeg.h>
#endif
#define THUMB_SIZE_NORMAL 128
#define THUMB_SIZE_LARGE 256
#define THUMB_SIZE_CUSTOM 0
#define THUMB_SIZE_FAIL -1
#include "exiftags/exif.h"

#include <Ecore.h>

#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <Edje.h>
#include <dlfcn.h>

static char *PATH_DIR_LARGE = NULL;
static char *PATH_DIR_NORMAL = NULL;
static char *PATH_DIR_CUSTOM = NULL;
static char *PATH_DIR_FAIL = NULL;
static unsigned LEN_DIR_LARGE = 0;
static unsigned LEN_DIR_NORMAL = 0;
static unsigned LEN_DIR_CUSTOM = 0;
static unsigned LEN_DIR_FAIL = 0;


static Ecore_Hash* plugins_mime;

extern int epsilon_info_exif_props_as_int_get (Epsilon_Info * ei, unsigned
					       short lvl, long prop);
extern void epsilon_exif_info_free (Epsilon_Exif_Info * eei);
/*
 * epsilon_exif_info_get
 * NULL on no exif data present
 * Returns a valid pointer to an Epsilon_Exif_Info object 
 */
extern Epsilon_Exif_Info *epsilon_exif_info_get (Epsilon * e);

static int _epsilon_exists_ext(Epsilon *e, const char *ext, char *path, int path_size, time_t *mtime);
static char *epsilon_hash (const char *file);
#ifdef HAVE_PNG_H
static FILE *_epsilon_open_png_file_reading (const char *filename);
static int _epsilon_png_write (const char *file, unsigned int * ptr,
			       int tw, int th, int sw, int sh, char *imformat,
			       int mtime, char *uri);
#endif
static int _epsilon_jpg_write (const char *file, unsigned int * ptr,
			       Evas_Object *eo, int w, int h);

Epsilon *
epsilon_new (const char *file)
{
  Epsilon *result = NULL;
  if (file)
    {
      if (file[0] == '/')
	{
	  result = calloc (1, sizeof (Epsilon));
	  result->src = strdup (file);
	  result->tw = THUMB_SIZE_LARGE;
	  result->th = THUMB_SIZE_LARGE;
	  result->format = EPSILON_THUMB_FDO;
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
      if (e->thumb)
	free (e->thumb);
      free (e);
    }
}


static Epsilon_Plugin*
epsilon_plugin_load(char* path)
{
	Epsilon_Plugin* plugin = NULL;
	void* dl_ref;
	Epsilon_Plugin* (*epsilon_plugin_init)();

	dl_ref = dlopen(path, RTLD_LAZY);
	if (dl_ref) {
		epsilon_plugin_init = dlsym(dl_ref, "epsilon_plugin_init");
		if (!epsilon_plugin_init) {
		   fprintf(stderr, "Failed to load %s: %s", path, dlerror());
		   dlclose(dl_ref);
		   return NULL;
		}
		plugin = (*epsilon_plugin_init)();
	}

	return plugin;
}

static int epsilon_init_count = 0;
int
epsilon_init (void)
{
  char buf[PATH_MAX];
  int base_len;
  char *home;
  struct dirent *de;
  char* type;
  DIR *dir;
  Epsilon_Plugin *plugin;
  char plugin_path[PATH_MAX];

  if (epsilon_init_count) return ++epsilon_init_count;

  home = getenv("HOME");
  base_len = snprintf(buf, sizeof(buf), "%s/.thumbnails", home);
  if (!PATH_DIR_LARGE) {
     strncpy(buf + base_len, "/large", PATH_MAX - base_len);
     PATH_DIR_LARGE = strdup(buf);
     LEN_DIR_LARGE = strlen(buf);
  }
  if (!PATH_DIR_NORMAL) {
     strncpy(buf + base_len, "/normal", PATH_MAX - base_len);
     PATH_DIR_NORMAL = strdup(buf);
     LEN_DIR_NORMAL = strlen(buf);
  }
  if (!PATH_DIR_FAIL) {
     strncpy(buf + base_len, "/fail/epsilon", PATH_MAX - base_len);
     PATH_DIR_FAIL = strdup(buf);
     LEN_DIR_FAIL = strlen(buf);
  }

  ecore_file_mkpath(PATH_DIR_LARGE);
  ecore_file_mkpath(PATH_DIR_NORMAL);
  ecore_file_mkpath(PATH_DIR_FAIL);

  plugins_mime = ecore_hash_new(ecore_str_hash, ecore_str_compare);

  /*Initialise plugins*/
  dir = opendir(PACKAGE_LIB_DIR "/epsilon/plugins/");
  if (dir) {
	while ((de = readdir(dir))) {
		if (!strncmp(de->d_name + strlen(de->d_name) - 3, ".so", 3)) {
			   snprintf(plugin_path, 1024, "%s/%s",
                           	PACKAGE_LIB_DIR "/epsilon/plugins", de->d_name);

			   if ((plugin = epsilon_plugin_load(plugin_path))) {
				   /*Append the mime types for this plugin*/
				   ecore_list_first_goto(plugin->mime_types);
				   while ( (type = ecore_list_next(plugin->mime_types))) {
					ecore_hash_set(plugins_mime, type, plugin);
				   }
			   }

		}
	}

	closedir(dir);
  }

  return ++epsilon_init_count;
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
      e->w = w;
      e->h = h;
    }
}

void
epsilon_format_set (Epsilon * e, Epsilon_Thumb_Format f)
{
  if (e && (f == EPSILON_THUMB_FDO || f == EPSILON_THUMB_JPEG))
    e->format = f;
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
  time_t mtime;
  char buf[PATH_MAX];

  if (!e)
    return (NULL);
  if (e->thumb)
    return (e->thumb);

  if (_epsilon_exists_ext(e, "jpg", buf, sizeof(buf), &mtime))
    {
       e->thumb = strdup(buf);
       return (e->thumb);
    }
#ifdef HAVE_PNG_H
  if (_epsilon_exists_ext(e, "png", buf, sizeof(buf), &mtime))
    {
       e->thumb = strdup (buf);
       return (e->thumb);
    }
#endif
  return (NULL);
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
  snprintf (uri, sizeof(uri), "file://%s", file);

  MD5Init (&ctx);
  MD5Update (&ctx, (unsigned char const*)uri, (unsigned)strlen (uri));
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
      !strcasecmp (&e->thumb[len - 3], "jpg") &&
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
#ifdef HAVE_PNG_H
  if ((fp = _epsilon_open_png_file_reading (e->thumb)))
#endif
    {
#ifdef HAVE_PNG_H
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
#endif
    }
  else
     return NULL;

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


/*This needs to be worked into some kind of 'mime-magic' ID 
 * section, to work out what file type we're dealing with*/
static const char *
epsilon_mime_for_extension_get(const char* extension)
{
	if ((!strcasecmp(extension, "mpg")) ||
			(!strcasecmp(extension, "mpeg"))) return "video/mpeg";
	else if (!strcasecmp(extension, "wmv")) return "video/x-ms-wmv";
	else if (!strcasecmp(extension, "avi")) return "video/x-msvideo";
	else if (!strcasecmp(extension, "mov")) return "video/quicktime";
	else if (!strcasecmp(extension, "pdf")) return "application/pdf";
	else if (!strcasecmp(extension, "dvi")) return "application/dvi";
	else return NULL;
}

static void
_epsilon_file_name(unsigned thumb_size, const char *hash, const char *ext, char *path, int path_size)
{
   char *dir;
   int dir_len;

   if (thumb_size == THUMB_SIZE_LARGE)
     {
	dir = PATH_DIR_LARGE;
	dir_len = LEN_DIR_LARGE;
     }
   else if (thumb_size == THUMB_SIZE_NORMAL)
     {
	dir = PATH_DIR_NORMAL;
	dir_len = LEN_DIR_NORMAL;
     }
   else if (thumb_size == THUMB_SIZE_CUSTOM)
     {
	dir = PATH_DIR_CUSTOM;
	dir_len = LEN_DIR_CUSTOM;
     }
   else
     {
	dir = PATH_DIR_FAIL;
	dir_len = LEN_DIR_FAIL;
     }

   if (dir) strncpy(path, dir, path_size);
   path_size -= dir_len;
   snprintf(path + dir_len, path_size, "/%s.%s", hash, ext);
}

static int
_epsilon_exists_ext_int(unsigned thumb_size, const char *hash, const char *ext, char *path, int path_size, time_t *mtime)
{
   struct stat filestatus;

   _epsilon_file_name(thumb_size, hash, ext, path, path_size);
   if (stat(path, &filestatus) == 0)
     {
	*mtime = filestatus.st_mtime;
	return 1;
     }
   else return 0;
}

static int
_epsilon_exists_ext(Epsilon *e, const char *ext, char *path, int path_size, time_t *mtime)
{
   if (_epsilon_exists_ext_int(e->tsize, e->hash, ext, path, path_size, mtime))
     return 1;

   return _epsilon_exists_ext_int(THUMB_SIZE_FAIL, e->hash, ext, path, path_size, mtime);
}

int
epsilon_exists (Epsilon * e)
{
  int ok = 0;
  struct stat st;
  time_t srcmtime;

  if (!e || !e->src)
    return (EPSILON_FAIL);

  if (stat(e->src, &st) != 0)
    return (EPSILON_FAIL);
  srcmtime = st.st_mtime;

  if (!e->hash)
    {
       char hash_seed[PATH_MAX] = "";
       int idx = 0, len = sizeof(hash_seed);

       if (e->key)
	 {
	    int size;
	    size = snprintf (hash_seed, len, "%s:%s", e->src, e->key);
	    idx += size;
	    len -= size;
	 }

       if ((e->w > 0) && (e->h > 0))
	 snprintf (hash_seed + idx, len, ":%dx%d", e->w, e->h);

       if (hash_seed[0] != 0)
	 e->hash = epsilon_hash (hash_seed);
       else
	 e->hash = epsilon_hash (e->src);
    }

  if (!e->hash)
    return (EPSILON_FAIL);

  if (!ok)
    {
       char path[PATH_MAX];
       time_t filemtime;
       if (_epsilon_exists_ext(e, "jpg", path, sizeof(path), &filemtime)) {
	  if (filemtime >= srcmtime)
	    return (EPSILON_OK);
	  /* XXX compare with time from e->src exif tag? */
       }
    }
#ifdef HAVE_PNG_H
  if (!ok)
    {
       char path[PATH_MAX];
       time_t filemtime;
       if (_epsilon_exists_ext(e, "png", path, sizeof(path), &filemtime)) {
	  if (filemtime >= srcmtime)
	    return (EPSILON_OK);
	  /* XXX compare with time from e->src png tag? */
       }
    }
#endif
  return (EPSILON_FAIL);
}

int
epsilon_generate (Epsilon * e)
{
   char buf[PATH_MAX], buf2[PATH_MAX], buf3[256], *ext = NULL;
   Evas *evas = NULL, *evas_im = NULL;
   Ecore_Evas *ee = NULL, *ee_im = NULL;
   Evas_Object *im = NULL, *edje = NULL;
   int ret = EPSILON_FAIL;
   int iw, ih, alpha, ww, hh;
   int *data = NULL;
   struct stat filestatus;
   time_t mtime = 0;
   const char *mime = NULL;
   Epsilon_Image *img_thm = NULL;
   Epsilon_Plugin *plugin = NULL;
   
   if (stat(e->src, &filestatus) != 0)
     return ret;
   
   mtime = filestatus.st_mtime;
   evas_init();
   ecore_init();
   ecore_evas_init();
   edje_init();
   
   edje_file_cache_set(0);
   edje_collection_cache_set(0);
   ee = ecore_evas_buffer_new(1, 1);
   evas = ecore_evas_get(ee);
   evas_image_cache_set(evas, 0);
   evas_font_cache_set(evas, 0);
   ww = 0;
   hh = 0;
   alpha = 1;
   
   ext = strrchr(e->src, '.');
   if (ext)
     {
	mime = epsilon_mime_for_extension_get(ext + 1);
	if ((plugin = ecore_hash_get(plugins_mime, mime)))
	  img_thm = (plugin->epsilon_generate_thumb)(e);
     }
   
   if (img_thm)
     {
	iw = img_thm->w;
	ih = img_thm->h;
	im = evas_object_image_add(evas);
	alpha = img_thm->alpha;
	evas_object_image_alpha_set(im, alpha);
	evas_object_image_size_set(im, iw, ih);
	evas_object_image_data_set(im, img_thm->data);
	evas_object_image_data_update_add(im, 0, 0, iw, ih);
	if ((iw > 0) && (ih > 0))
	  {
	     if ((iw > ih && e->tw > 0) || e->th <= 0)
	       {
		  ww = e->tw;
		  hh = (e->tw * ih) / iw;
	       }
	     else
	       {
		  hh = e->th;
		  ww = (e->th * iw) / ih;
	       }
	     evas_object_image_fill_set(im, 0, 0, ww, hh);
	  }
	ret = EPSILON_OK;
     }
   else if ((ext) &&
       (!strcasecmp(ext, ".edj")))
     {
	ww = e->tw;
	hh = e->th;
	im = ecore_evas_object_image_new(ee);
	ee_im = evas_object_data_get(im, "Ecore_Evas");
	evas_im = ecore_evas_get(ee_im);
	evas_image_cache_set(evas_im, 0);
	evas_font_cache_set(evas_im, 0);
	evas_object_image_size_set(im, ww * 2, hh * 2);
	evas_object_image_fill_set(im, 0, 0, ww, hh);
	edje = edje_object_add(evas_im);
	if ((edje_object_file_set(edje, e->src, "e/desktop/background")) ||
	    (edje_object_file_set(edje, e->src, "e/init/splash")) ||
	    (edje_object_file_set(edje, e->src, "icon")))
	  {
	     ret = EPSILON_OK;
	     evas_object_move(edje, 0, 0);
	     evas_object_resize(edje, ww * 2, hh * 2);
	     evas_object_show(edje);
	  }
     }
   else
     {
	im = evas_object_image_add(evas);
	evas_object_image_load_size_set(im, e->tw, e->th);
	evas_object_image_file_set(im, e->src, NULL);
	if (evas_object_image_load_error_get(im) == EVAS_LOAD_ERROR_NONE)
	  ret = EPSILON_OK;
	iw = 0; ih = 0;
	evas_object_image_size_get(im, &iw, &ih);
	alpha = evas_object_image_alpha_get(im);
	if ((iw > 0) && (ih > 0))
	  {
	     if ((iw > ih && e->tw > 0) || e->th <= 0)
	       {
		  ww = e->tw;
		  hh = (e->tw * ih) / iw;
	       }
	     else
	       {
		  hh = e->th;
		  ww = (e->th * iw) / ih;
	       }
	     evas_object_image_fill_set(im, 0, 0, ww, hh);
	  }
     }
   evas_object_move(im, 0, 0);
   evas_object_resize(im, ww, hh);
   ecore_evas_resize(ee, ww, hh);
   evas_object_show(im);
   if ((ret == EPSILON_OK) && (ww > 0))
     {
	data = (int *)ecore_evas_buffer_pixels_get(ee);
	if (data)
	  {
	     snprintf(buf, sizeof(buf), "file://%s", e->src);
	     if (e->format == EPSILON_THUMB_FDO)
	       {
	        _epsilon_file_name(e->tsize, e->hash, "png", buf2, sizeof(buf2));
	        /* this is wrong - but hey! good enough? */
	        if (ext) snprintf(buf3, sizeof(buf3), "image/%s", ext);
	        else snprintf(buf3, sizeof(buf3), "image/png");
	        if (_epsilon_png_write(buf2,
				       data, ww, hh, iw, ih,
				       buf3, mtime, buf))
	          {
		     ret = EPSILON_FAIL;
	          }
               }
	     else
	     {
		  _epsilon_file_name(e->tsize, e->hash, "jpg", buf2, sizeof(buf2));
	          if (_epsilon_jpg_write(buf2,
					 data, im, ww, hh))
		    {
		       ret = EPSILON_FAIL;
		    }
	     }
	  }
     }
   if (edje) evas_object_del(edje);
   if (ee_im) ecore_evas_free(ee_im);
   else if (im) evas_object_del(im);
   
   if (img_thm)
     {
	free(img_thm->data);
	free(img_thm);
     }

   ecore_evas_free(ee);
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   evas_shutdown();
   return ret;
}

void
epsilon_thumb_size(Epsilon *e, Epsilon_Thumb_Size size)
{
   if (!e) return;
   
   switch (size)
     {
      case EPSILON_THUMB_NORMAL:
	e->tw = THUMB_SIZE_NORMAL;
	e->th = THUMB_SIZE_NORMAL;
	e->tsize = THUMB_SIZE_NORMAL;
	break;
      case EPSILON_THUMB_LARGE:
	e->tw = THUMB_SIZE_LARGE;
	e->th = THUMB_SIZE_LARGE;
	e->tsize = THUMB_SIZE_LARGE;
	break;
     }
}

void
epsilon_custom_thumb_size (Epsilon * e, int w, int h, const char *dir)
{
  char buf[PATH_MAX];
  int base_len;
  char *home;

  if (e && (w > 0 || h > 0))
    {
      e->tw = w;
      e->th = h;
      e->tsize = THUMB_SIZE_CUSTOM;

      home = getenv("HOME");
      base_len = snprintf(buf, sizeof(buf), "%s/.thumbnails/", home);
      strncpy(buf + base_len, dir, PATH_MAX - base_len);

      if (PATH_DIR_CUSTOM)
        free(PATH_DIR_CUSTOM);

      PATH_DIR_CUSTOM = strdup(buf);
      LEN_DIR_CUSTOM = strlen(buf);
      ecore_file_mkpath(PATH_DIR_CUSTOM);
    }
}

#ifdef HAVE_PNG_H
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
	  if ((ret = png_check_sig ((png_bytep)buf, bytes)))
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

#define GET_TMPNAME(_tmpbuf,_file) { \
  int _l,_ll; \
  char _buf[21]; \
  _l=snprintf(_tmpbuf,sizeof(_tmpbuf),"%s",_file); \
  _ll=snprintf(_buf,sizeof(_buf),"epsilon-%06d.png",(int)getpid());  \
  strncpy(&tmpfile[_l-35],_buf,_ll+1); }

static int
_epsilon_png_write (const char *file, unsigned int * ptr, int tw, int th, int sw,
		    int sh, char *imformat, int mtime, char *uri)
{
  FILE *fp = NULL;
  char mtimebuf[32], widthbuf[10], heightbuf[10], tmpfile[PATH_MAX] = "";
  int i, j, k, has_alpha = 1, ret = 0;

/*
  unsigned int      *ptr=NULL;
*/
  png_infop info_ptr;
  png_color_8 sig_bit;
  png_structp png_ptr;
  png_text text_ptr[5];
  png_bytep row_ptr, row_data = NULL;

  /*If the image has no width or no height, leave here, otherwise libpng gives a crash*/
  if (!th || !tw) {
	  return 1;
  }

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

      snprintf (mtimebuf, sizeof(mtimebuf), "%d", mtime);
      text_ptr[1].key = "Thumb::MTime";
      text_ptr[1].text = mtimebuf;
      text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (widthbuf, sizeof(widthbuf), "%d", sw);
      text_ptr[2].key = "Thumb::Image::Width";
      text_ptr[2].text = widthbuf;
      text_ptr[2].compression = PNG_TEXT_COMPRESSION_NONE;

      snprintf (heightbuf, sizeof(heightbuf), "%d", sh);
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
#endif

static int
_epsilon_jpg_write (const char *file, unsigned int * ptr, Evas_Object *eo, int w, int h)
{
  char tmpfile[PATH_MAX] = "";

  int l,ll;
  char buf[21];
  l=snprintf(tmpfile,sizeof(tmpfile),"%s",file);
  ll=snprintf(buf,sizeof(buf),"epsilon-%06d.jpg",(int)getpid());
  strncpy(&tmpfile[l-35], buf,ll+1);

  evas_object_image_data_set(eo, NULL);
  evas_object_image_size_set(eo, w, h);
  evas_object_image_data_set(eo, ptr);

  if (!evas_object_image_save(eo, tmpfile, NULL, NULL))
    return 1;

  if (!rename (tmpfile, file))
    {
       if (chmod (file, S_IWUSR | S_IRUSR))
	 fprintf (stderr,
		  "epsilon: could not set permissions on \"%s\"!?\n",
		  file);
    }

  return 0;
}
