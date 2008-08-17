#include "evfs.h"

static EvfsFilereference* empty_filereference = NULL;

int
evfs_file_path_compare(EvfsFilereference * file1, EvfsFilereference * file2)
{
   return strcasecmp(file1->path, file2->path);

}

Ecore_List *
evfs_file_list_sort(Ecore_List * file_list)
{

   Ecore_Sheap *heap;
   EvfsFilereference *f;

   /*
    * Push the data into a heap.
    */
   heap =
      ecore_sheap_new(ECORE_COMPARE_CB(evfs_file_path_compare),
                      ecore_list_count(file_list));
   while ((f = ecore_list_first_remove(file_list)))
     {
        ecore_sheap_insert(heap, f);
     }

   /*
    * Extract in sorted order.
    */
   while ((f = ecore_sheap_extract(heap)))
     {
        ecore_list_append(file_list, f);
     }

   ecore_list_first_goto(file_list);
   ecore_sheap_destroy(heap);

   return file_list;
}


int EvfsFilereference_equal_is(EvfsFilereference* file1, EvfsFilereference* file2)
{
	int equal = 1;

	if (strcmp(file1->path, file2->path) || strcmp(file1->plugin_uri, file2->plugin_uri))
		equal = 0;

	if (equal) {
		if ( (file1->parent && !file2->parent)  || (file2->parent && !file1->parent))
			equal = 0;
		else if (file1 ->parent && file2->parent)
			equal = EvfsFilereference_equal_is(file1->parent, file2->parent);
	}

	return equal;
}

EvfsFilereference* evfs_empty_file_get()
{
	if (!empty_filereference) {
		EvfsFilereference* ref = calloc(1,sizeof(EvfsFilereference));
		ref->path = "/";
		ref->plugin_uri = "";

		empty_filereference = ref;
	}


	return empty_filereference;
}

