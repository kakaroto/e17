#include <evfs.h>

static int evfs_vfolder_init=0;
static Eet_Data_Descriptor *_evfs_vfolder_edd;

void evfs_vfolder_initialise() {
	if (evfs_vfolder_init)
		return;
	evfs_vfolder_init = 1;

	_evfs_vfolder_edd = eet_data_descriptor_new("evfs_vfolder", sizeof(evfs_filereference),
			      (void *(*) (void *))evas_list_next, 
			      (void *(*) (void *, void *))evas_list_append, 
			      (void *(*) (void *))evas_list_data, 
			      (void *(*) (void *))evas_list_free, 
			      (void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))evas_hash_foreach, 
			      (void *(*) (void *, const char *, void *))evas_hash_add, 
			      (void  (*) (void *))evas_hash_free);


	EET_DATA_DESCRIPTOR_ADD_LIST(_evfs_vfolder_edd, evfs_vfolder, "bases", bases,  evfs_io_filereference_edd_get() );
}


evfs_vfolder* evfs_vfolder_new() {
	return NEW(evfs_vfolder);
}

evfs_vfolder* evfs_vfolder_base_add(evfs_vfolder* folder, evfs_filereference* file) {
	evfs_vfolder* ret_folder = NULL;
	if (!folder) {
		/*No current folder, we need a new one*/
		ret_folder = evfs_vfolder_new();
	} else {
		ret_folder = folder;
	}



	return ret_folder;
}
