 /* Team members of IMLIB2 plz have a look at first scratch of .gz & .bz2
    loader - I need it in my common work for lots of reasons, it is more
    or less UNIX ready, but I used OS/2 for testing, also I am not sure
    about temp directory for example gimp loader use tempnames for some
    reason (??) If you think this loader is not needed for Imlib2 feel 
    free to wipe it entirely. Also if it will be ok, wipe this comment off,
    and add to makefiles
    Stauff (pla@cland.ru)
 */
    
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <errno.h>
#include "image.h"

char load (ImlibImage *im, ImlibProgressFunction progress,
	   char progress_granularity, char immediate_load);
char save (ImlibImage *im, ImlibProgressFunction progress,
	   char progress_granularity);
void formats (ImlibLoader *l);

static int
exists(char *file)
{
   struct stat         st;
   
   if (stat(file, &st) < 0)
      return 0;
   return 1;
}

char * pure_filename(char *path_filename)
{
 char *ptr;
 
 ptr = strrchr(path_filename,'/');
#ifdef __EMX__
 /* Care about \\ */ 
#endif
 if (ptr) ptr++;
   else ptr = path_filename;
   
 return ptr;
}

char load (ImlibImage *im, ImlibProgressFunction progress,
	   char progress_granularity, char immediate_load)
{
   char                 file[4096], key[4096], *ptr;
   char 		str_gz[4096];
   ImlibLoader	        *sub_loader;   
   
   if (im->data)
      return 0;
   if (!im->file)
      return 0;
   strcpy(file, im->real_file);
   if (!exists(file))    
      return 0;
   ptr = strrchr(file,'.');
   if (!ptr) return 0;
   *ptr = 0;
   if (getenv("TMPDIR"))
      strcpy(key,getenv("TMPDIR"));
   else  
       { _getcwd2(&key[0],4096);   
#ifdef __EMX__       
         if (key[strlen(key)-1] == '/')  key[strlen(key)-1] = 0;
#endif         
       }  
   if ( (strlen(ptr+1)>=2) && (!strcmp(ptr+1,"gz")) )    
      sprintf(str_gz,"gzip -d %s -c > %s/%s", im->real_file, key, pure_filename(file));
   else 
   if ( (strlen(ptr+1)>=3) && (!strcmp(ptr+1,"bz2")) )      
      sprintf(str_gz,"bzip2 -d %s -c > %s/%s", im->real_file, key, pure_filename(file));
   else   
      return 0; /* Eeek why we are here? */

   if (system(str_gz)<0) return 0;
   free(im->real_file);
   sprintf(str_gz,"%s/%s", key, pure_filename(file));
   im->real_file = strdup(str_gz);
   im->format = strdup(++ptr);
   sub_loader = __imlib_FindBestLoaderForFile(im->real_file);
   if (sub_loader)
        sub_loader->load(im, progress, progress_granularity,1);
   else      
     {    unlink(im->real_file);                           
     	  return 0;
     }
   unlink(im->real_file);                           
   free(im->real_file);
   *--ptr = '.';   
   im->real_file=strdup(real_file);
   return 1;
}

char save (ImlibImage *im, ImlibProgressFunction progress,
	   char progress_granularity)
{
  /* TODO */
  return 0;
}
	   

void 
formats (ImlibLoader *l)
{  
   char *list_formats[] = 
     { "gz","bz2" };

     {
	int i;
	
	l->num_formats = (sizeof(list_formats) / sizeof (char *));
	l->formats = malloc(sizeof(char *) * l->num_formats);
	for (i = 0; i < l->num_formats; i++)
	   l->formats[i] = strdup(list_formats[i]);
     }
}

