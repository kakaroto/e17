/*

Copyright (C) 20002 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"
#include "color_values.h"

/* #define ICO_DBG */

#ifdef ICO_DBG
#define D(fmt, args...) \
{ \
  printf("Imlib2 ICO loader: "); \
  printf(fmt, ## args); \
}
#else
#define D(fmt, args...)
#endif

#define SWAP32(x) \
((((x) & 0x000000ff ) << 24) |\
 (((x) & 0x0000ff00 ) << 8) |\
 (((x) & 0x00ff0000 ) >> 8) |\
 (((x) & 0xff000000 ) >> 24))

#define SWAP16(x) \
((((x) & 0x00ff ) << 8) |\
 (((x) & 0xff00 ) >> 8))

#ifdef WORDS_BIGENDIAN
#define ENDIAN_SWAP(x) (SWAP32(x))
#define ENDIAN_SWAP16(x) (SWAP16(x))
#else
#define ENDIAN_SWAP(x) (x)
#define ENDIAN_SWAP16(x) (x)
#endif

typedef struct _MsIconEntry
{
  DATA8        width;        /* Width of icon in pixels */
  DATA8        height;       /* Height of icon in pixels */
  DATA8        num_colors;   /* Maximum number of colors */
  DATA8        reserved;     /* Not used */
  DATA16       num_planes;   /* Not used */
  DATA16       bpp;
  DATA32       size;         /* Length of icon bitmap in bytes */
  DATA32       offset;       /* Offset position of icon bitmap in file */
} MsIconEntry;

typedef struct _MsIconData
{
  /* Bitmap header data */
  DATA32       header_size;  /* = 40 Bytes */
  DATA32       width;
  DATA32       height;
  DATA16       planes;
  DATA16       bpp;
  DATA32       compression;  /* not used for icons */
  DATA32       image_size;   /* size of image */
  DATA32       x_res;
  DATA32       y_res;        
  DATA32       used_clrs;
  DATA32       important_clrs; 

  DATA32      *palette;      /* Color palette, only if bpp <= 8. */
  DATA8       *xor_map;      /* Icon bitmap */
  DATA8       *and_map;      /* Display bit mask */
} MsIconData;

typedef struct _MsIcon
{
  FILE        *fp;
  int          cp;
  char        *filename;

  DATA16       reserved;
  DATA16       resource_type;
  DATA16       icon_count;
  MsIconEntry *icon_dir;
  MsIconData  *icon_data;
} MsIcon;

static int        ico_read_int8 (FILE *fp, DATA8 *data, int count);
static int        ico_read_int16 (FILE *fp, DATA16 *data, int count);
static int        ico_read_int32 (FILE *fp, DATA32 *data, int count);
static MsIcon    *ico_init (char *filename);
static void       ico_cleanup (MsIcon *ico);
static void       ico_read_entry(MsIcon *ico, MsIconEntry* entry);
static void       ico_read_data(MsIcon *ico, int icon_num);
static void       ico_load (MsIcon *ico);
static int        ico_to_imlib (MsIcon *ico, ImlibImage *im);

char load(ImlibImage *im, ImlibProgressFunction progress,char progress_granularity, char immediate_load);
char save(ImlibImage *im, ImlibProgressFunction progress, char progress_granularity);
void formats(ImlibLoader *l);


static int
ico_read_int32 (FILE     *fp,
		DATA32   *data,
		int       count)
{
  int i, total;

  total = count;
  if (count > 0)
    {
      ico_read_int8 (fp, (DATA8*) data, count * 4);
      for (i = 0; i < count; i++)
         data[i] = ENDIAN_SWAP(data[i]);
    }

  return total * 4;
}


static int
ico_read_int16 (FILE     *fp,
		DATA16   *data,
		int       count)
{
  int i, total;

  total = count;
  if (count > 0)
    {
      ico_read_int8 (fp, (DATA8*) data, count * 2);
      for (i = 0; i < count; i++)
         data[i] = ENDIAN_SWAP16(data[i]);
    }

  return total * 2;
}


static int
ico_read_int8 (FILE     *fp,
	       DATA8    *data,
	       int       count)
{
  int total;
  int bytes;

  total = count;
  while (count > 0)
    {
      bytes = fread ((char*) data, sizeof (char), count, fp);
      if (bytes <= 0) /* something bad happened */
        break;
      count -= bytes;
      data += bytes;
    }

  return total;
}


static MsIcon    *
ico_init (char *filename)
{
  MsIcon *ico;

  if (! (ico = (MsIcon*) calloc(1, sizeof(MsIcon))))
    return NULL;
      
  if (! (ico->fp = fopen (filename, "r")))
    return NULL;

  ico->filename = filename;
  ico->cp += ico_read_int16(ico->fp, &ico->reserved, 1);
  ico->cp += ico_read_int16(ico->fp, &ico->resource_type, 1);

  /* Icon files use 1 as resource type, that's  what I wrote this for.
     From descriptions on the web it seems as if this loader should
     also be able to handle Win 3.11 - Win 95 cursor files (.cur),
     which use resource type 2. I haven't tested this though. */
  if (ico->reserved != 0 ||
      (ico->resource_type != 1 && ico->resource_type != 2))
    {
      ico_cleanup(ico);
      return NULL;
    }

  return ico;
}


static void
ico_cleanup (MsIcon *ico)
{
  int i;

  if (!ico)
    return;

  if (ico->fp)
    fclose(ico->fp);

  if (ico->icon_dir)
    free (ico->icon_dir);

  if (ico->icon_data)
    {
      for (i = 0; i < ico->icon_count; i++)
	{
	  /* There's not always a palette .. */
	  if (ico->icon_data[i].palette)
	    free (ico->icon_data[i].palette);
	  free (ico->icon_data[i].xor_map);
	  free (ico->icon_data[i].and_map);
	}
      free (ico->icon_data);
    }

  free (ico);
}


static void
ico_read_entry(MsIcon *ico, MsIconEntry* entry)
{
  if (!ico || !entry)
    return;

  ico->cp += ico_read_int8(ico->fp, &entry->width, 1);
  ico->cp += ico_read_int8(ico->fp, &entry->height, 1);
  ico->cp += ico_read_int8(ico->fp, &entry->num_colors, 1);
  ico->cp += ico_read_int8(ico->fp, &entry->reserved, 1);

  ico->cp += ico_read_int16(ico->fp, &entry->num_planes, 1);
  ico->cp += ico_read_int16(ico->fp, &entry->bpp, 1);

  ico->cp += ico_read_int32(ico->fp, &entry->size, 1);
  ico->cp += ico_read_int32(ico->fp, &entry->offset, 1);

  D("Read entry with w: %i, h: %i, num_colors: %i, bpp: %i\n",
	 entry->width, entry->height, entry->num_colors, entry->bpp);
}


static void
ico_read_data(MsIcon *ico, int icon_num)
{
  MsIconData *data;
  MsIconEntry *entry;
  int xor_len = 0, and_len = 0;

  if (!ico)
    return;

  entry = &ico->icon_dir[icon_num];
  data = &ico->icon_data[icon_num];

  ico->cp += ico_read_int32(ico->fp, &data->header_size, 1);
  ico->cp += ico_read_int32(ico->fp, &data->width, 1);
  ico->cp += ico_read_int32(ico->fp, &data->height, 1);
  
  ico->cp += ico_read_int16(ico->fp, &data->planes, 1);
  ico->cp += ico_read_int16(ico->fp, &data->bpp, 1);

  ico->cp += ico_read_int32(ico->fp, &data->compression, 1);
  ico->cp += ico_read_int32(ico->fp, &data->image_size, 1);
  ico->cp += ico_read_int32(ico->fp, &data->x_res, 1);
  ico->cp += ico_read_int32(ico->fp, &data->y_res, 1);
  ico->cp += ico_read_int32(ico->fp, &data->used_clrs, 1);
  ico->cp += ico_read_int32(ico->fp, &data->important_clrs, 1);

  if (data->used_clrs == 0)
    {
      switch (data->bpp)
	{
	case 1:
	  D("Allocating a 2-slot palette\n");
	  data->used_clrs = 2;
	  data->palette = (DATA32*) calloc(data->used_clrs, sizeof(DATA32));
	  ico->cp += ico_read_int32(ico->fp, data->palette, data->used_clrs);

	  if (((entry->width * entry->height) % 8) == 0)
	    xor_len = (entry->width * entry->height / 8);
	  else
	    xor_len = (entry->width * entry->height / 8) + 1;
	  break;
	case 4:
	  D("Allocating a 16-slot palette\n");
	  data->used_clrs = 16;
	  data->palette = (DATA32*) calloc(data->used_clrs, sizeof(DATA32));
	  ico->cp += ico_read_int32(ico->fp, data->palette, data->used_clrs);
	  xor_len = entry->width * entry->height / 2;
	  break;
	case 8:
	  D("Allocating a 256-slot palette\n");
	  data->used_clrs = 256;
	  data->palette = (DATA32*) calloc(data->used_clrs, sizeof(DATA32));
	  ico->cp += ico_read_int32(ico->fp, data->palette, data->used_clrs);
	  xor_len = entry->width * entry->height;
	  break;
	default:
	  D("No colormap, used_clrs = %i, bpp = %i\n", data->used_clrs, data->bpp);
	  /* Otherwise, no colormap is used, but RGB values. */
	  xor_len = entry->width * entry->height * (data->bpp/8);
	}
    }
  else
    {
      switch (data->bpp)
	{
	case 1:
	  D("Allocating a %i-slot palette for monochrome\n", data->used_clrs);
	  data->palette = (DATA32*) calloc(data->used_clrs, sizeof(DATA32));
	  ico->cp += ico_read_int32(ico->fp, data->palette, data->used_clrs);

	  if (((entry->width * entry->height) % 8) == 0)
	    xor_len = (entry->width * entry->height / 8);
	  else
	    xor_len = (entry->width * entry->height / 8) + 1;
	  break;
	case 4:
	  D("Allocating a %i-slot palette for 4 bpp\n", data->used_clrs);
	  data->palette = (DATA32*) calloc(data->used_clrs, sizeof(DATA32));
	  ico->cp += ico_read_int32(ico->fp, data->palette, data->used_clrs);
	  xor_len = entry->width * entry->height / 2;
	  break;
	case 8:
	  D("Allocating a %i-slot palette for 8 bpp\n", data->used_clrs);
	  data->palette = (DATA32*) calloc(data->used_clrs, sizeof(DATA32));
	  ico->cp += ico_read_int32(ico->fp, data->palette, data->used_clrs);
	  xor_len = entry->width * entry->height;
	  break;
	default:
	  D("No colormap, used_clrs = %i, bpp = %i\n", data->used_clrs, data->bpp);
	  /* Otherwise, no colormap is used, but RGB values. */
	  xor_len = entry->width * entry->height * (data->bpp/8);
	}
    }
  
  data->xor_map = (DATA8*) calloc(xor_len, sizeof(DATA8));
  ico->cp += ico_read_int8(ico->fp, data->xor_map, xor_len);

  /* Read in and_map. It's padded out to 32 bits: */
  if ((entry->width % 32) == 0)
    and_len = ((entry->width/32) * entry->height);
  else
    and_len = ((entry->width/32 + 1) * entry->height);
  
  data->and_map = (DATA8*) calloc(and_len, sizeof(DATA32));
  ico->cp += ico_read_int8(ico->fp, data->and_map, and_len * sizeof(DATA32));

  D("Entry width %i, height %i, bpp %i\n", entry->width, entry->height, data->bpp);
  D("Length of xor_map: %i\n", xor_len);
  D("Length of and_map: %i\n", and_len * sizeof(DATA32));
}


static void
ico_load (MsIcon *ico)
{
  int i;

  if (!ico)
    return;

  ico->cp += ico_read_int16(ico->fp, &ico->icon_count, 1);
  ico->icon_dir = (MsIconEntry*) calloc(ico->icon_count, sizeof(MsIconEntry));
  ico->icon_data = (MsIconData*) calloc(ico->icon_count, sizeof(MsIconData));

  D("%s: Microsoft icon file, containing %i icon(s)\n",
	 ico->filename, ico->icon_count);
  
  for (i = 0; i < ico->icon_count; i++)
    ico_read_entry(ico, &ico->icon_dir[i]);

  for (i = 0; i < ico->icon_count; i++)
    ico_read_data(ico, i);
}


static int
ico_get_bit_from_data(DATA8 *data, int line_width, int bit)
{
  int line;
  int width32;
  int offset;

  /* width per line in multiples of 32 bits */
  width32 = (line_width % 32 == 0 ? line_width/32 : line_width/32 + 1);

  line = bit / line_width;
  offset = bit % line_width;

  return (data[line * width32 * 4 + offset/8] & (1 << (7 - bit % 8)));
}


static int
ico_get_nibble_from_data(DATA8 *data, int nibble)
{
  int result = (data[nibble/2] & (0x0F << (4 * (1 - nibble % 2))));
  
  if (nibble % 2 == 0)
    result = result >> 4;
  
  return result;
}


static int
ico_to_imlib (MsIcon *ico, ImlibImage *im)
{
  int x, y, w, h;
  DATA8 *xor_map;
  DATA8 *and_map;
  DATA32 *palette;
  MsIconData *data;

  w = ico->icon_dir[0].width;
  h = ico->icon_dir[0].height;
  palette = ico->icon_data[0].palette;
  xor_map = ico->icon_data[0].xor_map;
  and_map = ico->icon_data[0].and_map;

  if (! (im->data = (DATA32 *) malloc(sizeof(DATA32) * w * h)))
    return 0;

  im->w = w; im->h = h;
  SET_FLAG(im->flags, F_HAS_ALPHA);
  data = &ico->icon_data[0];

  switch (data->bpp)
    {
    case 1:
      D("Rendering at 1 bpp\n");
      for (y = 0; y < h; y++)
	for (x = 0; x < w; x++)
	  {
	    DATA32 color = palette[ico_get_bit_from_data(xor_map, w, y * w + x)];
	    DATA32 *dest = &(im->data[(h-1-y) * w + x]);

	    R_VAL(dest) = R_VAL(&color);
	    G_VAL(dest) = G_VAL(&color);
	    B_VAL(dest) = B_VAL(&color);

	    if (ico_get_bit_from_data(and_map, w, y*w + x))
	      A_VAL(dest) = 0;
	    else
	      A_VAL(dest) = 255;	      
	  }
      break;						     

    case 4:
      D("Rendering at 4 bpp\n");
      for (y = 0; y < h; y++)
	for (x = 0; x < w; x++)
	  {
	    DATA32 color = palette[ico_get_nibble_from_data(xor_map, y * w + x)];
	    DATA32 *dest = &(im->data[(h-1-y) * w + x]);

	    R_VAL(dest) = R_VAL(&color);
	    G_VAL(dest) = G_VAL(&color);
	    B_VAL(dest) = B_VAL(&color);

	    if (ico_get_bit_from_data(and_map, w, y*w + x))
	      A_VAL(dest) = 0;
	    else
	      A_VAL(dest) = 255;	      

	  }
      break;

    case 8:
      D("Rendering at 8 bpp\n");
      for (y = 0; y < h; y++)
	for (x = 0; x < w; x++)
	  {
	    DATA32 color = palette[xor_map[y * w + x]];
	    DATA32 *dest = &(im->data[(h-1-y) * w + x]);

	    R_VAL(dest) = R_VAL(&color);
	    G_VAL(dest) = G_VAL(&color);
	    B_VAL(dest) = B_VAL(&color);

	    if (ico_get_bit_from_data(and_map, w, y*w + x))
	      A_VAL(dest) = 0;
	    else
	      A_VAL(dest) = 255;	      
	  }
      break;

    default:
      {
	int bytespp = data->bpp/8;

	D("Rendering at %i bpp\n", data->bpp);
	for (y = 0; y < h; y++)
	  for (x = 0; x < w; x++)
	    {
	      DATA32 *dest = &(im->data[(h-1-y) * w + x]);
	      
	      B_VAL(dest) = xor_map[(y * w + x) * bytespp];
	      G_VAL(dest) = xor_map[(y * w + x) * bytespp + 1];
	      R_VAL(dest) = xor_map[(y * w + x) * bytespp + 2];

	      if (data->bpp < 32)
		{
		  if (ico_get_bit_from_data(and_map, w, y*w + x))
		    A_VAL(dest) = 0;
		  else
		    A_VAL(dest) = 255;	      
		}
	      else
		{
		  A_VAL(dest) = xor_map[(y * w + x) * bytespp + 3];
		}
	    }
      }
    }

  return 1;
}


char 
load(ImlibImage *im, ImlibProgressFunction progress, char progress_granularity, char immediate_load)
{
  MsIcon *ico = NULL;

  /* if immediate_load is 1, then dont delay image laoding as below, or */
  /* already data in this image - dont load it again */
  if (im->data)
    return 0;

  /* set the format string member to the lower-case full extension */
  /* name for the format - so example names would be: */
  /* "png", "jpeg", "tiff", "ppm", "pgm", "pbm", "gif", "xpm" ... */

  if (!im->format)
    im->format = strdup("ico");

  if (im->loader || immediate_load || progress)
    {
      if (! (ico = ico_init((im->real_file))))
	return 0;

      ico_load (ico);

      if (! ico_to_imlib (ico, im))
	goto error;

      ico_cleanup (ico);
    }
  
  if (progress)
    {
      progress(im, 100, 0, 0, im->w, im->h);      
    }
  
  return 1;

 error:
  
  if (im->data)
    free(im->data);
  return 0;

  progress_granularity = 0;
}

/* fills the ImlibLoader struct with a strign array of format file */
/* extensions this loader can load. eg: */
/* loader->formats = { "jpeg", "jpg"}; */
/* giving permutations is a good idea. case sensitivity is irrelevant */
/* your laoder CAN load more than one format if it likes - like: */
/* loader->formats = { "gif", "png", "jpeg", "jpg"} */
/* if it can load those formats. */
void 
formats (ImlibLoader *l)
{  
   /* this is the only bit you have to change... */
   char *list_formats[] = 
     { "ico" };

   /* don't bother changing any of this - it just reads this in and sets */
   /* the struct values and makes copies */
     {
	int i;
	
	l->num_formats = (sizeof(list_formats) / sizeof (char *));
	l->formats = malloc(sizeof(char *) * l->num_formats);
	for (i = 0; i < l->num_formats; i++)
	   l->formats[i] = strdup(list_formats[i]);
     }
}

