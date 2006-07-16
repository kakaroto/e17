#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "evfs.h"
#include "evfs_metadata.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <Eet.h>
#include <Evas.h>
#include <limits.h>

#define EVFS_METADATA_GROUP_LIST "/evfs/group/list"
#define EVFS_METADATA_BASE_DATA "block"

/*
 * TODO
 *
 * * Handle recursive (i.e. nested) files
 */

static int evfs_metadata_state = 0;

static Eet_Data_Descriptor* Evfs_Metadata_String_Edd;
static Eet_Data_Descriptor* Evfs_Metadata_Group_Edd;
static Eet_Data_Descriptor* Evfs_Metadata_File_Groups_Edd;
static Eet_Data_Descriptor* Evfs_Metadata_Root_Edd;

static evfs_metadata_root* metadata_root;

static char* homedir;
static char metadata_file[PATH_MAX];
static Eet_File* _evfs_metadata_eet;

Eet_Data_Descriptor* _evfs_metadata_edd_create(char* desc, int size) 
{
	Eet_Data_Descriptor* edd;
	
	edd= eet_data_descriptor_new(desc, size,
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

	return edd;
}

void evfs_metadata_debug_group_list_print()
{
	Evas_List* l;
	evfs_metadata_group_header* g;
	int ret;

	printf("Printing group list:\n");
	for (l = metadata_root->group_list; l; ) {
		g = l->data;
	

		printf("Name: %s, Desc: %s\n", g->name, g->description);
		
		l = l->next;
	}
	printf("Done..\n");
}

void evfs_metadata_debug_file_groups_print(evfs_metadata_file_groups* groups)
{
	Evas_List* l;
	evfs_metadata_group_header* g;
	int ret;

	printf("Printing group list:\n");
	for (l = groups->groups; l; ) {
		g = l->data;
		
		printf("Name: %s, Desc: %s\n", g->name, g->description);
		
		l = l->next;
	}
	printf("Done..\n");
}


void evfs_metadata_group_header_free(evfs_metadata_group_header* g)
{
	if (g->name) free(g->name);
	if (g->description) free(g->description);
	free(g);
}

void evfs_metadata_file_groups_free(evfs_metadata_file_groups* groups) {
	Evas_List* l;
	evfs_metadata_group_header* g;

	for (l = groups->groups; l; ) {
		g = l->data;
	
		evfs_metadata_group_header_free(g);
		
		l = l->next;
	}

}

int evfs_metadata_file_groups_group_check(evfs_metadata_file_groups* groups, char* group) {
	Evas_List* l;
	evfs_metadata_group_header* g;
	int ret = 0;

	for (l = groups->groups; l; ) {
		g = l->data;
	
		if (!strncmp(group, g->name, strlen(g->name))) {
			ret = 1;
			goto JUMP_OUT;
		}
			
		l = l->next;
	}	

	JUMP_OUT:
	return ret;
}

evfs_metadata_group_header*
evfs_metadata_group_header_new(char* name, char* desc) 
{
	evfs_metadata_group_header* group;

	group = calloc(1, sizeof(evfs_metadata_group_header));

	if (name) group->name = strdup(name);
	if (desc) group->description = strdup(desc);

	return group;
}

int evfs_metadata_group_header_exists(char* group)
{
	Evas_List* l;
	evfs_metadata_group_header* g;
	int ret;

	ret = 0;

	for (l = metadata_root->group_list; l; ) {
		g = l->data;
		
		if (!strcmp(g->name, group)) {
			ret = 1;
			goto GROUP_DONE;
		}
		
		l = l->next;
	}

	GROUP_DONE:
	return ret;
}

void evfs_metadata_initialise()
{
	struct stat config_dir_stat;
	Evas_List* group;
	char* data;
	int size;
	int ret;
	evfs_filereference* ref;
	evfs_metadata_file_groups* groups;
	char** ret_list;
	int i;
	
	if (!evfs_metadata_state) {
		evfs_metadata_state++;
	} else return;

	group = NULL;

	printf(". EVFS metadata initialise..\n");

	/*String edd*/
	Evfs_Metadata_String_Edd = _evfs_metadata_edd_create("evfs_metadata_string", sizeof(evfs_metadata_object));
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "description", description, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "key", key, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "value", value, EET_T_STRING);

	/*Group edd*/
	Evfs_Metadata_Group_Edd = _evfs_metadata_edd_create("evfs_metadata_group_header", sizeof(evfs_metadata_group_header));
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_Group_Edd, evfs_metadata_group_header,
                                 "description", description, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_Group_Edd, evfs_metadata_group_header,
                                 "name", name, EET_T_STRING);

	/*Metadata group root*/
	Evfs_Metadata_Root_Edd = _evfs_metadata_edd_create("evfs_metadata_root", sizeof(evfs_metadata_root));
	EET_DATA_DESCRIPTOR_ADD_LIST(Evfs_Metadata_Root_Edd, evfs_metadata_root,
                                 "group_list", group_list, Evfs_Metadata_Group_Edd);

	/*Metadata group root*/
	Evfs_Metadata_File_Groups_Edd = _evfs_metadata_edd_create("evfs_metadata_file_groups", sizeof(evfs_metadata_file_groups));
	EET_DATA_DESCRIPTOR_ADD_LIST(Evfs_Metadata_File_Groups_Edd, evfs_metadata_file_groups,
                                 "groups", groups, Evfs_Metadata_Group_Edd);

	homedir = strdup(getenv("HOME"));
	snprintf(metadata_file, PATH_MAX, "%s/.e/evfs", homedir);

	if (stat(metadata_file, &config_dir_stat)) {
		mkdir(metadata_file, 0700);
	}

	snprintf(metadata_file, PATH_MAX, "%s/.e/evfs/evfs_metadata.eet", homedir);

	if (stat(metadata_file, &config_dir_stat)) {
		printf("Making new metadata file..\n");
			
		/*Open/close the file*/
		_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_WRITE);

		metadata_root = calloc(1, sizeof(evfs_metadata_root));

		/*Create a starting 'group' list*/
		metadata_root->group_list = evas_list_append(metadata_root->group_list, 
				evfs_metadata_group_header_new("Pictures", "Pictures"));
		metadata_root->group_list = evas_list_append(metadata_root->group_list, 
				evfs_metadata_group_header_new("Video", "Video"));
		metadata_root->group_list = evas_list_append(metadata_root->group_list, 
				evfs_metadata_group_header_new("Audio", "Audio"));

		data = eet_data_descriptor_encode(Evfs_Metadata_Root_Edd, metadata_root, &size);
		ret = eet_write(_evfs_metadata_eet, EVFS_METADATA_GROUP_LIST, data, size, 0);

		free(data);

		eet_close(_evfs_metadata_eet);
		
	} else {
			printf("Loading pre-existing metadata root..\n");
		
			_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);	
			data = eet_read(_evfs_metadata_eet, EVFS_METADATA_GROUP_LIST, &size);
			if (data) {
				metadata_root = 
					eet_data_descriptor_decode(Evfs_Metadata_Root_Edd, data, size);
				free(data);

				printf("..Loaded group list..\n");

				evfs_metadata_debug_group_list_print();
			} else {
				printf("Error loading group list..\n");
			}
			eet_close(_evfs_metadata_eet);
	}

	/*ref = calloc(1, sizeof(evfs_filereference));
	ref->plugin_uri= strdup("file");
	ref->path = strdup("/home/chaos/respite3x3840.jpg");
	evfs_metadata_group_header_file_add(ref, "Pictures");

	printf("\n*****\nFile groups are now:\n");

	groups = evfs_metadata_file_groups_get(ref);
	evfs_metadata_debug_file_groups_print(groups);
	evfs_metadata_file_groups_free(groups);

	ret_list = evfs_metadata_file_group_list("Pictures", &size);
	for (i=0;i<size;i++) {
		printf("In group: %s\n", ret_list[i]);
	}*/
	


}

Evas_List* evfs_metadata_groups_get() {
	return metadata_root->group_list;	
}

void evfs_metadata_file_set_key_value_edd(evfs_filereference* ref, char* key, 
		void* value, Eet_Data_Descriptor* edd) 
{

}

char**
evfs_metadata_file_group_list(char* group, int* num) 
{
	char** ret_list;
	char group_base[PATH_MAX];
	int i;
	char* j;

	snprintf(group_base, sizeof(group_base), "/groups/%s:*", group);

	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);
	ret_list = eet_list(_evfs_metadata_eet, group_base, num);

	if (*num>0) {
		for(i=0;i<*num;i++) {
			j = index(ret_list[i], ':');
			if (j) ret_list[i] = j+1;
		}
	}

	return ret_list;
}

evfs_metadata_file_groups* evfs_metadata_file_groups_get(evfs_filereference* ref)
{
	evfs_metadata_file_groups* groups = NULL;

	char* data;
	int size;
	int ret = 0;
	char path[PATH_MAX];
	char* file_path;

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);

	snprintf(path, PATH_MAX, "/filedata/%s/groups", file_path);

	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);
	data = eet_read(_evfs_metadata_eet, path, &size);
	if (data) {
		groups = eet_data_descriptor_decode(Evfs_Metadata_File_Groups_Edd, data, size);
		free(data);
	}

	free(file_path);

	return groups;

}

void evfs_metadata_group_header_file_add(evfs_filereference* ref, char* group)
{
	evfs_metadata_file_groups* groups;
	char path[PATH_MAX];
	char* file_path;
	char* data;
	int size;
	int ret = 0;
	evfs_metadata_group_header* header;
	

	/*First make sure this group exists*/

	if (evfs_metadata_group_header_exists(group)) {
		printf("Group exists - proceed\n");
	} else {
		printf("Alert - group not found\n");
		return;
	}

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);
	printf("File path is: %s\n", file_path);


	/*Add to file groups*/
	snprintf(path, PATH_MAX, "/filedata/%s/groups", file_path);

	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);
	data = eet_read(_evfs_metadata_eet, path, &size);
	eet_close(_evfs_metadata_eet);
	
	if (data) {
		printf("Found group data for file in eet..\n");
		
		groups = eet_data_descriptor_decode(Evfs_Metadata_File_Groups_Edd, data, size);
		free(data);
	} else {
		printf("File is not a member of a group - making new group collection..\n");
		
		groups = calloc(1, sizeof(evfs_metadata_file_groups));	
	}

	/*Check if we're already in group*/
	if (!evfs_metadata_file_groups_group_check(groups, group)) {
		_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ_WRITE);

		/*Add this to file groups*/
		header = evfs_metadata_group_header_new(group, NULL);
		groups->groups = evas_list_append(groups->groups, header);

		/*Now write back to eet*/
		data = eet_data_descriptor_encode(Evfs_Metadata_File_Groups_Edd, groups, &size);
		
		if (data) {
			ret = eet_write(_evfs_metadata_eet, path, data, size, 0);
		}
		if (ret) {
			//printf("Wrote %d for %s\n", size, path);
		}
		free(data);

		
		/*Add to the group itself*/
		snprintf(path, sizeof(path), "/groups/%s:%s", group, file_path);
		eet_write(_evfs_metadata_eet, path, EVFS_METADATA_BASE_DATA, strlen(EVFS_METADATA_BASE_DATA), 0);

		eet_close(_evfs_metadata_eet);
	} else {
		printf("File aready in group %s!\n", group);
	}
	/*Free groups*/
	evfs_metadata_file_groups_free(groups);
	
	free(file_path);
	
}



void evfs_metadata_file_set_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
	evfs_metadata_object obj;
	char path[PATH_MAX];
	char* file_path;
	char* data;
	int size;
	int ret;

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);

	snprintf(path, PATH_MAX, "/filedata/%s/custommeta/string/%s", file_path, key);
	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ_WRITE);
	ret = 0;

	obj.description = "string";
	obj.key = key;
	obj.value = (char*)value;

	data = eet_data_descriptor_encode(Evfs_Metadata_String_Edd, &obj, &size);
	
	if (data) {
		ret = eet_write(_evfs_metadata_eet, path, data, size, 0);
	}
	if (ret) {
		printf("Wrote %s for %s\n", value, path);
	}
	free(data);
	free(file_path);

	eet_close(_evfs_metadata_eet);
}

void evfs_metadata_file_get_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
	evfs_metadata_object* obj = NULL;
	char path[PATH_MAX];
	char* data;
	char* file_path;
	int size;
	int ret;

	/*Build a path*/
	file_path = evfs_filereference_to_string(ref);

	snprintf(path, PATH_MAX, "/filedata/%s/custommeta/string/%s", file_path, key);
	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);
	ret = 0;

	if (_evfs_metadata_eet) {
		data = eet_read(_evfs_metadata_eet, path, &size);
		if (data) {
			obj = eet_data_descriptor_decode(Evfs_Metadata_String_Edd, data, size);
		}

		if (obj) {
			printf ("Got %s -> %s\n", obj->key, (char*)obj->value);
			free(data);
			free(obj);
		}
	}	
	eet_close(_evfs_metadata_eet);

	free(file_path);
}
