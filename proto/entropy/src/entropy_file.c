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

	return file;
}

void entropy_generic_file_destroy(entropy_generic_file* file) {
		/*printf("Cleaning '%s/%s' => %p (%p)\n", file->path, file->filename, file, file->mime_type);*/
		if (file->thumbnail) {
			entropy_thumbnail_destroy(file->thumbnail);
		}

		/*We own the username/password strings if they exist - nuke 'em*/
		if (file->username) entropy_free(file->username);
		if (file->password) entropy_free(file->password);

		/*If the file has a parent, remove a reference, because we added one when we got here..*/
		if (file->parent) {
			//TODO fix this - we need to detatch this from threading issues*/
			entropy_core_file_cache_remove_reference(file->parent->md5);
		}

		if (file->md5) entropy_free(file->md5);
		if (file->attach) entropy_free(file->attach);


		entropy_free(file);

		allocated_files -= 1;
}

void entropy_generic_file_uri_set(entropy_generic_file* file) {
	if (file->uri == NULL) {
		file->uri =entropy_core_generic_file_uri_create(file,0);
		ecore_hash_set(entropy_core_get_core()->uri_reference_list, file->uri, file);

	}
}

int entropy_generic_file_path_compare(entropy_generic_file* file1, entropy_generic_file* file2) {
	int val;

   if (file1 && file2) {
	 val = strcasecmp(file1->filename, file2->filename);
	 
	 if ( (!(file1->filetype == FILE_FOLDER)) && file2->filetype == FILE_FOLDER)
		 return 1;
	 else if  ( (!(file2->filetype == FILE_FOLDER)) && file1->filetype == FILE_FOLDER)
		 return -1;
	 else 
		 return val;
   } else {
	   printf("Could not locate file!\n");
	   return 0;
   }
}


Ecore_List* entropy_generic_file_list_sort(Ecore_List* file_list) {

   Ecore_Sheap        *heap;
   entropy_generic_file* f;
	
   /*
    * Push the data into a heap.
    */
   heap = ecore_sheap_new(ECORE_COMPARE_CB(entropy_generic_file_path_compare), ecore_list_count(file_list));
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



char* entropy_generic_file_display_permissions_parse(entropy_generic_file* file)
{
	char* ret;
	
	if (!file || !file->retrieved_stat) return NULL;

	ret = entropy_malloc(sizeof(char)*10);

	/*D bit*/
	if (file->properties.st_mode & S_IFDIR)
		ret[0] = 'd';
	else
		ret[0] = ' ';

	/*Owner */
	if (file->properties.st_mode & S_IRUSR)
		ret[1] = 'r';
	else
		ret[1] = ' ';

	if (file->properties.st_mode & S_IWUSR)
		ret[2] = 'w';
	else
		ret[2] = ' ';

	if (file->properties.st_mode & S_IXUSR)
		ret[3] = 'x';
	else
		ret[3] = ' ';
	/*----------*/

	/*Group */
	if (file->properties.st_mode & S_IRGRP)
		ret[4] = 'r';
	else
		ret[4] = ' ';

	if (file->properties.st_mode & S_IWGRP)
		ret[5] = 'w';
	else
		ret[5] = ' ';

	if (file->properties.st_mode & S_IXGRP)
		ret[6] = 'x';
	else
		ret[6] = ' ';
	/*----------*/

	/*Other */
	if (file->properties.st_mode & S_IROTH)
		ret[7] = 'r';
	else
		ret[7] = ' ';

	if (file->properties.st_mode & S_IWOTH)
		ret[8] = 'w';
	else
		ret[8] = ' ';

	if (file->properties.st_mode & S_IXOTH)
		ret[9] = 'x';
	else
		ret[9] = ' ';
	/*----------*/


	return ret;
}
