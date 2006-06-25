#ifndef _EVFS_METADATA_H_
#define _EVFS_METADATA_H_

struct _evfs_metadata_object {
	char* description;
	char* key;
	void* value;
};

typedef struct _evfs_metadata_object evfs_metadata_object;

void evfs_metadata_initialise();

void evfs_metadata_file_set_key_value_string(evfs_filereference* ref, char* key,
				char* value);


#endif
