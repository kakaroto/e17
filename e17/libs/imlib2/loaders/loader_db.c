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
# define DB_DBM_HSEARCH    1
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <db.h>
#include <unistd.h>
#include <sys/stat.h>

static int
permissions(char *file)
{
   struct stat         st;
   
   if (!stat(file, &st) < 0)
      return 0;
   return st.st_mode;
}

static int
exists(char *file)
{
   struct stat         st;
   
   if (!stat(file, &st) < 0)
      return 0;
   return 1;
}

static int
can_read(char *file)
{
   if (!(permissions(file) & (S_IRUSR | S_IRGRP | S_IROTH)))
      return 0;
   return (1 + access(file, R_OK));
}

static int
can_write(char *file)
{
   if (!(permissions(file) & (S_IWUSR | S_IWGRP | S_IWOTH)))
      return 0;
   return (1 + access(file, W_OK));
}

char 
load (ImlibImage *im, ImlibProgressFunction progress,
      char progress_granularity, char immediate_load)
{
   int                  w, h, alpha;
   DBM                 *db;
   char                 file[4096], key[4096], *ptr;
   datum                dkey, ret;
   DATA32 *body;
      
   if (im->data)
      return 0;
   if (!im->file)
      return 0;
   strcpy(file, im->file);
   ptr = strrchr(file, ':');
   if (ptr) 
      {
	 int flen;
	 
	 *ptr = 0;
	 if (!can_read(file))
	    return 0;
	 flen = strlen(file);
	 strcpy(key, &(ptr[1]));
	 if ((flen > 3) &&
	     (file[flen - 3] == '.') &&
	     (file[flen - 2] == 'd') &&
	     (file[flen - 1] == 'b'))
	    file[flen - 3] = 0;
      }
   else
      return 0;
   db = dbm_open(file, O_RDONLY, 0664);
   if (!db)
     {
	int i;
	
	for (i = 0; i < 32; i++)
	  {
	     usleep((rand() % 0xff) * 1000);
	     db = dbm_open(file, O_RDONLY, 0664);
	     if (db)
		break;
	  }
	if (!db)
	   return 0;
     }
   
   dkey.dptr = key;
   dkey.dsize = strlen(key);
   ret = dbm_fetch(db, dkey);
   if (!ret.dptr)
     {
	dbm_close(db);
	return 0;
     }
   /* header */
     {
	int header[8];
	
	if (ret.dsize < 32)
	  {
	     dbm_close(db);
	     return 0;
	  }
	memcpy(header, ret.dptr, 32);
#ifdef WORDS_BIGENDIAN
	  {
	     int i;
	     for (i = 0; i < dat.dsize; i++)
		SWAP32(header[i]);
	  }
#endif
	body = &(header[8]);
	if (header[0] != (int)0xac1dfeed)
	  {
	     dbm_close(db);
	     return 0;
	  }
	w = header[1];
	h = header[2];
	alpha = header[3];
	if ((w > 8192) || (h > 8192))
	  {
	     dbm_close(db);
	     return 0;
	  }
	if (ret.dsize < ((w * h * 4) + 32))
	  {
	     dbm_close(db);
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

	/* must set the im->data member before callign progress function */
	ptr = im->data = malloc(w * h * sizeof(DATA32));
	if (!im->data)
	  {
	     dbm_close(db);
	     return 0;
	  }
	for (y = 0; y < h; y++)
	  {
#ifdef WORDS_BIGENDIAN
	       {
		  int x;
		  
		  memcpy(ptr, &(body[y * w]), im->w * 4);
		  for (x = 0; x < im->w; x++)
		     SWAP32(ptr[x]);
	       }
#else
	     memcpy(ptr, &(body[y * w]), im->w * 4);
#endif	     
	     ptr += im->w;
	     if (progress)
	       {
		  char per;
		  int l;
		  
		  per = (char)((100 * y) / im->h);
		  if (((per - pper) >= progress_granularity) ||
		      (y == (im->h - 1)))
		    {
		       l = y - pl;
                       if(!progress(im, per, 0, (y - l), im->w, l))
			 {
			    dbm_close(db);
			    return 2;
			 }
		       pper = per;
		       pl = y;
		    }
	       }
	  }	   
     }
   dbm_close(db);
   return 1;
}

char 
save (ImlibImage *im, ImlibProgressFunction progress,
      char progress_granularity)
{
   int                 alpha = 0;
   char                 file[4096], key[4096], *cp;
   int                 *header;
   datum                dkey, ret;
   DATA32             *buf;
   DBM                 *db;
   
   
   /* no image data? abort */
   if (!im->data)
      return 0;
   if (im->flags & F_HAS_ALPHA)
      alpha = 1;
   if (!im->file)
      return 0;
   strcpy(file, im->file);
   cp = strrchr(file, ':');
   if (cp)
     {
	int flen;
	
	*cp = 0;
	if (exists(file))
	  {
	     if (!can_write(file))
		return 0;
	     if (!can_read(file))
		return 0;
	  }
	flen = strlen(file);
	strcpy(key, &(cp[1]));
	if ((flen > 3) &&
	    (file[flen - 3] == '.') &&
	    (file[flen - 2] == 'd') &&
	    (file[flen - 1] == 'b'))
	   file[flen - 3] = 0;
     }
   else
      return 0;
   db = dbm_open(file, O_RDWR | O_CREAT, 0664);
   if (!db)
     {
	int i;
	
	for (i = 0; i < 32; i++)
	  {
	     usleep((rand() % 0xff) * 1000);
	     db = dbm_open(file, O_RDWR | O_CREAT, 0664);
	     if (db)
		break;
	  }
	if (!db)
	   return 0;
     }
   
   dkey.dptr = key;
   dkey.dsize = strlen(key);
   
   buf = (DATA32 *) malloc((im->w * im->h) + 32);   
   header = buf;
   header[0] = 0xac1dfeed;
   header[1] = im->w;
   header[2] = im->h;
   header[3] = alpha;
   header[4] = 0;
   memcpy(&(buf[8]), im->data, im->w * im->h * 4);
#ifdef WORDS_BIGENDIAN
   for (y = 0; y < (im->w * im->h) + 8; y++)
      SWAP32(buf[y]);
#endif
   ret.dsize = ((im->w * im->h) + 8) * 4;
   ret.dptr = buf;
   dbm_store(db, dkey, ret, DBM_REPLACE);
   free(buf);
   if (progress)
      progress(im, 100, 0, 0, im->w, im->h);
   /* finish off */
   dbm_close(db);
   return 1;
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

