#ifndef __ENTROPY_GENERIC_H_
#define __ENTROPY_GENERIC_H_

#include "plugin_base.h"
#include "entropy_core.h"
#include "thumbnail_generic.h"


typedef struct entropy_generic_file entropy_generic_file;
struct entropy_generic_file {
        char path[255];
        char filename[255];
        char mime_type[40];
	char uri_base[15];
	
	char perms[10];
	char filetype;

	char* username;  /*Do we have a cached auth reference for this file/location? */
	char* password;

	entropy_thumbnail* thumbnail;

	struct entropy_generic_file* parent;

	char* md5; /*A reference to the md5sum made for this file*/
};

typedef struct entropy_file_listener entropy_file_listener;
struct entropy_file_listener {
	entropy_generic_file* file;
	int count;
};

typedef struct entropy_file_request entropy_file_request;
struct entropy_file_request {
	entropy_generic_file* file;
	entropy_generic_file* file2;
	entropy_core* core; /*A reference into the system core */
	void* requester; /*The object that requested this file/directory, for reference tracking purposes*/
	int file_type;
	int drill_down; /*Indicate if this request should drill down through the child's mime type
			  I.e. if the file's uri is 'posix', and the file's mime is 'application/x-tar,
			  produce a construct of the form posix://path/to/file#tar:/// ..etc */

	int set_parent;
};

typedef struct entropy_file_stat entropy_file_stat;
struct entropy_file_stat {
	entropy_generic_file* file;
	struct stat* stat_obj;
};

	

enum entropy_generic_file_type {
	FILE_ALL,
	FILE_UNKNOWN,
	FILE_STANDARD,
	FILE_FOLDER
};

#endif
