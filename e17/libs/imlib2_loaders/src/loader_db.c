#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "image.h"

char load (ImlibImage *im, ImlibProgressFunction progress,
	   char progress_granularity, char immediate_load);
char save (ImlibImage *im, ImlibProgressFunction progress,
	   char progress_granularity);
void formats (ImlibLoader *l);

#define SWAP32(x) (x) = \
((((x) & 0x000000ff ) << 24) |\
 (((x) & 0x0000ff00 ) << 8) |\
 (((x) & 0x00ff0000 ) >> 8) |\
 (((x) & 0xff000000 ) >> 24))
#include <Edb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <zlib.h>

static int
permissions(char *file)
{
   struct stat         st;
   
   if (stat(file, &st) < 0)
      return 0;
   return st.st_mode;
}

static int
exists(char *file)
{
   struct stat         st;
   
   if (stat(file, &st) < 0)
      return 0;
   return 1;
}

static int
can_read(char *file)
{
#ifndef __EMX__
   if (!(permissions(file) & (S_IRUSR | S_IRGRP | S_IROTH)))
#else   
   if (!(permissions(file)))
#endif   
      return 0;
   return (1 + access(file, R_OK));
}

static int
can_write(char *file)
{
#ifndef __EMX__
   if (!(permissions(file) & (S_IWUSR | S_IWGRP | S_IWOTH)))
#else   
   if (!(permissions(file)))
#endif   
      return 0;
   return (1 + access(file, W_OK));
}

char 
load (ImlibImage *im, ImlibProgressFunction progress,
      char progress_granularity, char immediate_load)
{
   int                  w, h, alpha, compression, size;
   E_DB_File           *db;
   char                 file[4096], key[4096];
   DATA32              *ret;
   DATA32              *body;
   if (im->data)
      return 0;
   if ((!im->file) || (!im->real_file) || (!im->key))
      return 0;
   strcpy(file, im->real_file);
   strcpy(key, im->key);
   if (!can_read(file)) return 0;
   db = e_db_open_read(file);
   if (!db)
      return 0;
   ret = e_db_data_get(db, key, &size);
   if (!ret)
     {
	e_db_close(db);
	return 0;
     }
   /* header */
     {
	DATA32 header[8];
	
	if (size < 32)
	  {
	     free(ret);
	     e_db_close(db);
	     return 0;
	  }
	memcpy(header, ret, 32);
#ifdef WORDS_BIGENDIAN
	  {
	     int i;
	     for (i = 0; i < 8; i++)
		SWAP32(header[i]);
	  }
#endif
	if (header[0] != 0xac1dfeed)
	  {
	     free(ret);
	     e_db_close(db);
	     return 0;
	  }
	w = header[1];
	h = header[2];
	alpha = header[3];
	compression = header[4];
	if ((w > 8192) || (h > 8192))
	  {
	     free(ret);
	     e_db_close(db);
	     return 0;
	  }
	if ((compression == 0) && (size < ((w * h * 4) + 32)))
	  {
	     free(ret);
	     e_db_close(db);
	     return 0;
	  }
	im->w = w;
	im->h = h;
	if (!im->format)
	  {
	     if (alpha)
		SET_FLAG(im->flags, F_HAS_ALPHA);
	     else
		UNSET_FLAG(im->flags, F_HAS_ALPHA);
	     im->format = strdup("db");
	  }
     }
   if (((!im->data) && (im->loader)) || (immediate_load) || (progress))
     {
	DATA32 *ptr;
	int     y, pl = 0;
	char    pper = 0;

	body = &(ret[8]);
	/* must set the im->data member before callign progress function */
	if (!compression)
	  {
	     if (progress)
	       {
		  char per;
		  int l;
		  
		  ptr = im->data = malloc(w * h * sizeof(DATA32));
		  if (!im->data)
		    {
		       free(ret);
		       e_db_close(db);
		       return 0;
		    }
		  for (y = 0; y < h; y++)
		    {
#ifdef WORDS_BIGENDIAN
			 {
			    int x;
			    
			    memcpy(ptr, &(body[y * w]), im->w * sizeof(DATA32));
			    for (x = 0; x < im->w; x++)
			      SWAP32(ptr[x]);
			 }
#else
		       memcpy(ptr, &(body[y * w]), im->w * sizeof(DATA32));
#endif	     
		       ptr += im->w;
		       
		       per = (char)((100 * y) / im->h);
		       if (((per - pper) >= progress_granularity) ||
			   (y == (im->h - 1)))
			 {
			    l = y - pl;
			    if(!progress(im, per, 0, (y - l), im->w, l))
			      {
				 free(ret);
				 e_db_close(db);
				 return 2;
			      }
			    pper = per;
			    pl = y;
			 }
		    }
	       }
	     else
	       {
		  ptr = im->data = malloc(w * h * sizeof(DATA32));
		  if (!im->data)
		    {
		       free(ret);
		       e_db_close(db);
		       return 0;
		    }
#ifdef WORDS_BIGENDIAN
		    {
		       int x;
		       
		       memcpy(ptr, body, im->w * im->h * sizeof(DATA32));
		       for (x = 0; x < (im->w * im->h); x++)
			 SWAP32(ptr[x]);
		    }
#else
		  memcpy(ptr, body, im->w * im->h * sizeof(DATA32));
#endif	     
	       }
	  }
	else
	  {
	     uLongf dlen;
	     
	     dlen = w * h * sizeof(DATA32);
	     im->data = malloc(w * h * sizeof(DATA32));
	     if (!im->data)
	       {
		  free(ret);
		  e_db_close(db);
		  return 0;
	       }
	     uncompress((Bytef *)im->data, &dlen, (Bytef *)body, (uLongf)(size - 32));
#ifdef WORDS_BIGENDIAN
	       {
		  int x;
		  
		  for (x = 0; x < (im->w * im->h); x++)
		     SWAP32(im->data[x]);
	       }
#endif			
	     if (progress)
		progress(im, 100, 0, 0, im->w, im->h);	     
	  }
     }
   free(ret);
   e_db_close(db);
   return 1;
}

char 
save (ImlibImage *im, ImlibProgressFunction progress,
      char progress_granularity)
{
   int                 alpha = 0;
   char                 file[4096], key[4096], *tmp;
   DATA32              *header;
   DATA32              *buf;
   E_DB_File           *db;
   int                  compression = 0, size = 0;
   DATA32              *ret;
   
   
   /* no image data? abort */
   if (!im->data)
      return 0;
   if (im->flags & F_HAS_ALPHA)
      alpha = 1;
   if ((!im->file) || (!im->real_file))
      return 0;
   strcpy(file, im->real_file);
   
   tmp = strrchr(file, ':');
   if(!tmp)
      return 0;
   *tmp++ = '\0';
   if(!*tmp)
      return 0;
   strcpy(key, tmp);
   
   if (exists(file))
     {
	if (!can_write(file)) return 0;
	if (!can_read(file)) return 0;
     }
   db = e_db_open(file);
   if (!db)
      return 0;
   
   /* account for space for compression */
   buf = (DATA32 *) malloc((((im->w * im->h * 101) / 100) + 3 + 8) * sizeof(DATA32));   
   header = buf;
   header[0] = 0xac1dfeed;
   header[1] = im->w;
   header[2] = im->h;
   header[3] = alpha;
     {
	ImlibImageTag      *tag;
	
	tag = __imlib_GetTag(im, "compression");
	if (!tag)
	   header[4] = 0;
	else
	  {
	     compression = tag->val;
	     if (compression < 0)
		compression = 0;
	     else if (compression > 9)
		compression = 9;
	     header[4] = compression;
	  }
     }
   if (compression > 0)
     {
	DATA32 *compressed;
	int retr;
	uLongf buflen;
	
	compressed = &(buf[8]);
	buflen = ((im->w * im->h * sizeof(DATA32) * 101) / 100) + 12;
#ifdef WORDS_BIGENDIAN
	  {
	     int i;
	     DATA32 *buf2;

	     for (i = 0; i < 8; i++)
		SWAP32(header[i]);
	     
	     buf2 = malloc((((im->w * im->h * 101) / 100) + 3) * sizeof(DATA32));
	     if (buf2)
	       {
		  int y;
		  
		  memcpy(buf2, im->data, im->w * im->h * sizeof(DATA32));
		  for (y = 0; y < (im->w * im->h) + 8; y++)
		     SWAP32(buf2[y]);
		  retr = compress2((Bytef *)compressed, &buflen, 
				   (Bytef *)buf2, 
				   (uLong)(im->w * im->h * sizeof(DATA32)), 
				   compression);
		  free(buf2);
	       }
	     else
		retr = Z_MEM_ERROR;
	  }
#else
	retr = compress2((Bytef *)compressed, &buflen, 
			 (Bytef *)im->data, 
			 (uLong)(im->w * im->h * sizeof(DATA32)), 
			 compression);
#endif
	if (retr != Z_OK)
	   compressed = 0;
	else
	  {
	     if (buflen >= (im->w * im->h * sizeof(DATA32)))
		compressed = 0;
	     else
		size = (8 * sizeof(DATA32)) + buflen;
	  }
     }
   else
     {
	memcpy(&(buf[8]), im->data, im->w * im->h * sizeof(DATA32));
	header[4] = compression;
#ifdef WORDS_BIGENDIAN
	  {
	     int y;
	     
	     for (y = 0; y < (im->w * im->h) + 8; y++)
		SWAP32(buf[y]);
	  }
#endif
	size = ((im->w * im->h) + 8) * sizeof(DATA32);
     }
   ret = buf;
   e_db_data_set(db, key, ret, size);
   free(buf);
   if (progress)
      progress(im, 100, 0, 0, im->w, im->h);
   /* finish off */
   e_db_close(db);
   return 1;
   progress_granularity = 0;
}

void 
formats (ImlibLoader *l)
{  
   char *list_formats[] = 
     { "db" };

     {
	int i;
	
	l->num_formats = (sizeof(list_formats) / sizeof (char *));
	l->formats = malloc(sizeof(char *) * l->num_formats);
	for (i = 0; i < l->num_formats; i++)
	   l->formats[i] = strdup(list_formats[i]);
     }
}

