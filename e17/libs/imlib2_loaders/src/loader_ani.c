/*

Copyright (C) 2002 Christian Kreibich <cK@whoop.org>.

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
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"
#include "color_values.h"

/* #define ANI_DBG */

#ifdef ANI_DBG
#define D(fmt, args...) \
{ \
  printf("Imlib2 ANI loader: "); \
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

#ifdef WORDS_BIGENDIAN
#define ENDIAN_SWAP(x) (SWAP32(x))
#else
#define ENDIAN_SWAP(x) (x)
#endif


typedef struct _MsChunk
{
  struct _MsChunk     *next;
  DATA32       chunk_id;
  DATA32       chunk_size;  /* Size of this chunk, starting from */
  char         data;        /* the following byte. Thus chunk_size = full size - 8 */
} MsChunk;

typedef struct _MsAni
{
  char        *filename;
  FILE        *fp;
  DATA32       cp;

  DATA32       riff_id;     /* "RIFF" */
  DATA32       data_size;
  DATA32       chunk_id;    /* "ACON" */
  
  MsChunk     *chunks;
} MsAni;

static void        ani_cleanup (MsAni *ani);

char load(ImlibImage *im, ImlibProgressFunction progress,char progress_granularity, char immediate_load);
char save(ImlibImage *im, ImlibProgressFunction progress, char progress_granularity);
void formats(ImlibLoader *l);


static int
ani_read_int8 (FILE     *fp,
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


static int
ani_read_int32 (FILE     *fp,
		DATA32   *data,
		int       count)
{
  int i, total;

  total = count;
  if (count > 0)
    {
      ani_read_int8 (fp, (DATA8*) data, count * 4);
      for (i = 0; i < count; i++)
         data[i] = ENDIAN_SWAP(data[i]);
    }

  return total * 4;
}


static MsAni    *
ani_init (char *filename)
{
  MsAni *ani;

  if (! (ani = (MsAni*) calloc(1, sizeof(MsAni))))
    return NULL;
      
  if (! (ani->fp = fopen (filename, "r")))
    return NULL;

  ani->filename = filename;
  ani->cp += ani_read_int32(ani->fp, &ani->riff_id, 1);
  ani->cp += ani_read_int32(ani->fp, &ani->data_size, 1);
  ani->cp += ani_read_int32(ani->fp, &ani->chunk_id, 1);

  if (ani->riff_id != 0x46464952 || ani->chunk_id != 0x4E4F4341)
    {
      ani_cleanup(ani);
      return NULL;
    }

  return ani;
}


static void
ani_cleanup (MsAni *ani)
{
  MsChunk *c, *c_next;

  D("Failed to allocate ANI image. Cleaning up\n");

  if (!ani)
    return;

  if (ani->fp)
    fclose(ani->fp);

  for (c = ani->chunks; c; )
    {
      c_next = c->next;
      free(c);
      c = c_next;
    }

  free (ani);
}


static MsChunk*
ani_load_chunk(MsAni *ani)
{
  DATA32   chunk_id, chunk_size, dummy;
  MsChunk *chunk;

  if (ani->cp >= ani->data_size + 8)
    return NULL;

  ani->cp += ani_read_int32(ani->fp, &chunk_id, 1);

  while (chunk_id == 0x5453494C)
    {
      D("Skipping LIST chunk header ...\n");
      ani->cp += ani_read_int32(ani->fp, &dummy, 1);
      ani->cp += ani_read_int32(ani->fp, &dummy, 1);
      ani->cp += ani_read_int32(ani->fp, &chunk_id, 1);
    }

  ani->cp += ani_read_int32(ani->fp, &chunk_size, 1);

  /* Pad it up to word length */
  if (chunk_size % 2)
    chunk_size += (2 - (chunk_size % 2));

  chunk = (MsChunk*) calloc(1, sizeof(MsChunk*) + 2 * sizeof(DATA32) + chunk_size);

  if (!chunk)
    {
      D("Warning, failed to allocate ANI chunk of size %d\n", sizeof(MsChunk*)
		      + 2 * sizeof(DATA32) + chunk_size);
      return NULL;
    }

  chunk->chunk_id = chunk_id;
  chunk->chunk_size = chunk_size;

  chunk_id = ENDIAN_SWAP(chunk_id);

  D("Loaded chunk with ID '%c%c%c%c' and length %i\n",
    ((char*)&chunk_id)[0], ((char*)&chunk_id)[1],
    ((char*)&chunk_id)[2], ((char*)&chunk_id)[3], chunk_size);
  
  ani->cp += ani_read_int8(ani->fp, &chunk->data, chunk_size);
  
  return chunk;
}


static void
ani_load (MsAni *ani)
{
  MsChunk *last_chunk;
  MsChunk *chunk;

  if (!ani)
    return;

  ani->chunks = ani_load_chunk(ani);
  last_chunk = ani->chunks;
  if (!last_chunk)
    return;

  while ( (chunk = ani_load_chunk(ani)) != NULL)
    {
      last_chunk->next = chunk;
      last_chunk = chunk;
    }
}


static char *
ani_save_ico (MsChunk *chunk)
{
  char *temp;
  FILE *f;

  if ( (temp = tempnam(NULL, "ico_")) == NULL)
    return NULL;

  if ( (f = fopen(temp, "w+")) == NULL)
    {
      free(temp);
      return NULL;
    }

  fwrite(&chunk->data, chunk->chunk_size, 1, f);
  fclose(f);

  return temp;
}


char 
load(ImlibImage *im, ImlibProgressFunction progress, char progress_granularity, char immediate_load)
{
  MsAni *ani = NULL;
  MsChunk *chunk;

  /* if immediate_load is 1, then dont delay image laoding as below, or */
  /* already data in this image - dont load it again */
  if (im->data)
    return 0;

  /* set the format string member to the lower-case full extension */
  /* name for the format - so example names would be: */
  /* "png", "jpeg", "tiff", "ppm", "pgm", "pbm", "gif", "xpm" ... */

  if (!im->format)
    im->format = strdup("ani");

  if (im->loader || immediate_load || progress)
    {
      if (! (ani = ani_init((im->real_file))))
	return 0;

      ani_load (ani);

      for (chunk = ani->chunks; chunk; chunk = chunk->next)
	{
	  if (chunk->chunk_id == 0x6E6F6369)
	    {
	      ImlibLoadError err;
	      ImlibImage *temp_im;
	      char *filename;

	      if ( (filename = ani_save_ico(chunk)) == NULL)
		return 0;

	      temp_im = __imlib_LoadImage(filename, progress, progress_granularity,
					  immediate_load, 0, &err);

	      im->w = temp_im->w;
	      im->h = temp_im->h;
	      SET_FLAG(im->flags, F_HAS_ALPHA);

	      if (! (im->data = (DATA32 *) malloc(sizeof(DATA32) * im->w * im->h)))
		{
		  free(filename);
		  return 0;
		}

	      memcpy(im->data, temp_im->data, sizeof(DATA32) * im->w * im->h);
	      unlink(filename);
	      free(filename);
	      break;
	    }
	}

      ani_cleanup (ani);
    }
  
  if (progress)
    {
      progress(im, 100, 0, 0, im->w, im->h);      
    }
  
  return 1;

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
     { "ani" };

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

