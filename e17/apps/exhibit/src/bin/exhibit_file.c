#include "exhibit.h"

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
