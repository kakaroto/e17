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

static int evfs_metadata_state = 0;
static Eet_Data_Descriptor* Evfs_Metadata_String_Edd;
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

void evfs_metadata_initialise()
{
	struct stat config_dir_stat;
	
	if (!evfs_metadata_state) {
		evfs_metadata_state++;
	} else return;

	printf(". EVFS metadata initialise..\n");

	homedir = strdup(getenv("HOME"));
	snprintf(metadata_file, PATH_MAX, "%s/.e/evfs", homedir);

	if (stat(metadata_file, &config_dir_stat)) {
		mkdir(metadata_file, 0700);

		snprintf(metadata_file, PATH_MAX, "%s/.e/evfs/evfs_metadata.eet", homedir);

		/*Open/close the file*/
		_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_WRITE);
		eet_close(_evfs_metadata_eet);
	} else {
		snprintf(metadata_file, PATH_MAX, "%s/.e/evfs/evfs_metadata.eet", homedir);
	}




	/*String edd*/
	Evfs_Metadata_String_Edd = _evfs_metadata_edd_create("evfs_metadata_string", sizeof(evfs_metadata_object));
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "description", description, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "key", key, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(Evfs_Metadata_String_Edd, evfs_metadata_object,
                                 "value", value, EET_T_STRING);
}

void evfs_metadata_file_set_key_value_edd(evfs_filereference* ref, char* key, 
		void* value, Eet_Data_Descriptor* edd) 
{

}

void evfs_metadata_file_set_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
	evfs_metadata_object obj;
	char path[PATH_MAX];
	char* data;
	int size;
	int ret;

	snprintf(path, PATH_MAX, "/%s/%s/string/%s", ref->plugin_uri, ref->path, key);
	ret = 0;

	obj.description = "string";
	obj.key = key;
	obj.value = (char*)value;

	data = eet_data_descriptor_encode(Evfs_Metadata_String_Edd, &obj, &size);
	
	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_WRITE);
	if (data) {
		ret = eet_write(_evfs_metadata_eet, path, data, size, 0);
	}
	if (ret) {
		printf("Wrote %s for %s\n", value, path);
	}
	eet_close(_evfs_metadata_eet);
	free(data);
}

void evfs_metadata_file_get_key_value_string(evfs_filereference* ref, char* key,
		char* value) 
{
	evfs_metadata_object* obj;
	char path[PATH_MAX];
	char* data;
	int size;
	int ret;

	snprintf(path, PATH_MAX, "/%s/%s/string/%s", ref->plugin_uri, ref->path, key);
	ret = 0;

	_evfs_metadata_eet = eet_open(metadata_file, EET_FILE_MODE_READ);
	if (_evfs_metadata_eet) {
		data = eet_read(_evfs_metadata_eet, path, &size);
		if (data) {
			obj = eet_data_descriptor_decode(Evfs_Metadata_String_Edd, data, size);
		}
		eet_close(_evfs_metadata_eet);

		if (obj) {
			printf ("Got %s -> %s\n", obj->key, obj->value);
			free(data);
			free(obj);
		}
	}	
}
