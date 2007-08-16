#ifndef _EVFS_METADATA_H_
#define _EVFS_METADATA_H_

struct _evfs_metadata_object {
	char* description;
	char* key;
	void* value;
};
typedef struct _evfs_metadata_object evfs_metadata_object;

typedef struct {
	char* name;
	char* description;
	char* visualhint;
} EvfsMetadataGroup;



struct _evfs_metadata_group {
	Evas_List* files;
};
typedef struct _evfs_metadata_group evfs_metadata_group;

struct _evfs_metadata_file_groups {
	        Evas_List* groups;
};
typedef struct _evfs_metadata_file_groups evfs_metadata_file_groups;

struct _evfs_metadata_root {
	Evas_List* group_list;
};
typedef struct _evfs_metadata_root evfs_metadata_root;


Eet_Data_Descriptor* _evfs_metadata_edd_create(char* desc, int size);
void evfs_metadata_debug_group_list_print();
int evfs_metadata_group_header_exists(char* group);
void evfs_metadata_group_header_free(EvfsMetadataGroup* g);
void evfs_metadata_file_groups_free(evfs_metadata_file_groups* groups);
void evfs_metadata_initialise(int);
void evfs_metadata_initialise_worker();
evfs_metadata_group* evfs_metadata_group_new(char* name, char* desc) ;
void evfs_metadata_file_set_key_value_string(EvfsFilereference* ref, char* key,
				char* value);
void evfs_metadata_group_header_file_add(EvfsFilereference* ref, char* group);
void evfs_metadata_group_header_file_remove(EvfsFilereference* ref, char* group);
evfs_metadata_file_groups* evfs_metadata_file_groups_get(EvfsFilereference* ref);
void evfs_metadata_debug_file_groups_print(evfs_metadata_file_groups* groups);
int evfs_metadata_file_groups_group_check(evfs_metadata_file_groups* groups, char* group);

Ecore_List* evfs_metadata_file_group_list(char* group);
Evas_List* evfs_metadata_groups_get();

int evfs_metadata_extract_init();
int evfs_metadata_extract_fork(EvfsFilereference* ref);

char* evfs_metadata_file_get_key_value_string(EvfsFilereference* ref, char* key);
void evfs_metadata_extract_queue(EvfsFilereference* ref);


#endif
