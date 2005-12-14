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
