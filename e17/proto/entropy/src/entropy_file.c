#include "entropy.h"

entropy_generic_file* entropy_generic_file_new() {
	entropy_generic_file* file = entropy_malloc(sizeof(entropy_generic_file));
	file->thumbnail = NULL;
	file->username = NULL;
	file->password = NULL;
	file->parent = NULL;
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


		entropy_free(file);

		allocated_files -= 1;
		print_allocation();
}

Ecore_List* entropy_generic_file_list_sort(Ecore_List* file_list) {
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
	
}
