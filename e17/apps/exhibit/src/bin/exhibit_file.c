/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"
#include <Edje.h>

char *viewables[] =
{
   ".jpg",
   ".jpeg",
   ".png",
   ".gif",
   ".tiff",
   ".edj",
   NULL
};

int 
_ex_file_download_progress_cb(void *data, const char *file, long int dltotal, long int dlnow, long int ultotal, long int ulnow)
{
   D(("Download progress on %s\n", file));

   return TRUE;
}

void 
_ex_file_download_complete_cb(void *data, const char *file, int status)
{
   D(("Download of file %s is done\n", file));

}

int
_ex_file_is_viewable(char *file)
{
   char *ext;
   int i = 0;

   ext = strrchr(file, '.');
   if(!ext) return 0;

   for(i = 0; viewables[i]; i++)
     {
	if(!strcasecmp(ext, viewables[i]))
	  return 1;
     }

   return 0;
}

int
_ex_file_is_ebg(char *file)
{
   int          val;
   char        *ext;
   Evas_List   *groups, *l;
   
   ext = strrchr(file, '.');
   if (!ext) return 0;
   
   if (strcasecmp(ext, ".edj"))
     return 0;
   
   val = 0;
   groups = edje_file_collection_list(file);
   if (!groups)
     return 0;
   
   for (l = groups; l; l = l->next)
     {
	if (!strcmp(l->data, "desktop/background"))
	  {
	     val = 1;
	     break;
	  }
     }
   edje_file_collection_list_free(groups);
   return val;
}

int
_ex_file_is_jpg(char *file)
{
   char        *ext;

   ext = strrchr(file, '.');
   if (!ext) return 0;

   if (strcasecmp(ext, ".jpg") && strcasecmp(ext, ".jpeg"))
      return 0;

   return 1;
}

char *
_ex_file_strip_extention(char *path) {
   char *ptr;
   char *c;
   char *file;
   int i;
   
   i = 0;
   ptr = path;
   c = strrchr(ptr,'.');
   file = malloc(strlen(path));
   
   while(ptr != c) {
      file[i] = *ptr;
      ptr++;
      i++;
   }
   
   file[i] = '\0';
   return file;
}

const char *
_ex_file_get(char *path)
{
   if(!path[0]) return NULL;
   
   if(path[strlen(path) - 1] == '/' && strlen(path) >= 2)
     {
	char *ret;
	
	ret = &path[strlen(path) - 2];
	while(ret != path)
	  {
	     if(*ret == '/')
	       return ++ret;
	     --ret;
	  }
	return path;	
     }
   else return ecore_file_get_file(path);
}
