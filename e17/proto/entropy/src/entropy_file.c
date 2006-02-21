#include "entropy.h"

entropy_generic_file* entropy_generic_file_new() {
	entropy_generic_file* file = entropy_malloc(sizeof(entropy_generic_file));
	file->thumbnail = NULL;
	file->username = NULL;
	file->password = NULL;
	file->parent = NULL;
	file->retrieved_stat = 0;
	bzero(file->mime_type, MIME_LENGTH);

	allocated_files++;
	print_allocation();

	return file;
}

void entropy_generic_file_destroy(entropy_generic_file* file) {
		if (file->thumbnail) {
			entropy_thumbnail_destroy(file->thumbnail);
		}

		/*We own the username/password strings if they exist - nuke 'em*/
		if (file->username) entropy_free(file->username);
		if (file->password) entropy_free(file->password);

		/*If the file has a parent, remove a reference, because we added one when we got here..*/
		if (file->parent) {
			//TODO fix this - we need to detatch this from threading issues*/
			//entropy_core_file_cache_remove_reference(entropy_core_get_core(), file->parent->md5);
		}

		if (file->md5) entropy_free(file->md5);


		entropy_free(file);

		allocated_files -= 1;
		print_allocation();
}

void entropy_generic_file_uri_set(entropy_generic_file* file) {
	if (file->uri == NULL) {
		file->uri =entropy_core_generic_file_uri_create(file,0);
		ecore_hash_set(entropy_core_get_core()->uri_reference_list, file->uri, file);

		printf("Set uri for %p to %s...\n", file, file->uri);
	}
}

/*Ecore_List* entropy_generic_file_list_sort(Ecore_List* file_list) {
	Ecore_List* new_list = ecore_list_new();
	entropy_generic_file* file_ins;
	entropy_generic_file* file;

	ecore_list_goto_first(file_list);
	while ( (file_ins = ecore_list_remove_first(file_list)) ) {
		int ins = 0;

		ecore_list_goto_first(new_list);
		while ( (file = ecore_list_current(new_list)) ) {
			if (strcmp(file_ins->filename, file->filename) < 0) {
				ecore_list_insert(new_list, file_ins);
				ins = 1;

				goto out;
			}

			ecore_list_next(new_list);
		}
		
		if (!ins) {
			ecore_list_append(new_list, file_ins);
		}


		

		out: ;

	}
	ecore_list_destroy(file_list);


	return new_list;
	
}*/


int entropy_generic_file_path_compare(entropy_generic_file* file1, entropy_generic_file* file2) {
	return strcasecmp(file1->filename, file2->filename);
	
}


Ecore_List* entropy_generic_file_list_sort(Ecore_List* file_list) {

   Ecore_Sheap        *heap;
   entropy_generic_file* f;
	
   /*
    * Push the data into a heap.
    */
   heap = ecore_sheap_new(ECORE_COMPARE_CB(entropy_generic_file_path_compare), ecore_list_nodes(file_list));
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
