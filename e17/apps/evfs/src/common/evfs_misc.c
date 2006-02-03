#include "evfs.h"

int
evfs_file_path_compare(evfs_filereference * file1, evfs_filereference * file2)
{
   return strcasecmp(file1->path, file2->path);

}

Ecore_List *
evfs_file_list_sort(Ecore_List * file_list)
{

   Ecore_Sheap *heap;
   evfs_filereference *f;

   /*
    * Push the data into a heap.
    */
   heap =
      ecore_sheap_new(ECORE_COMPARE_CB(evfs_file_path_compare),
                      ecore_list_nodes(file_list));
   while ((f = ecore_list_remove_first(file_list)))
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

   ecore_list_goto_first(file_list);
   ecore_sheap_destroy(heap);

   return file_list;
}
