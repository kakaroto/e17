#ifndef __ENTROPY_GENERIC_H_
#define __ENTROPY_GENERIC_H_

#include "entropy_core.h"
#include "thumbnail_generic.h"
#include "limits.h"

#define MIME_LENGTH 40
#define FILENAME_LENGTH 255

typedef struct entropy_generic_file entropy_generic_file; /*Deprecated*/
typedef struct entropy_generic_file Entropy_Generic_File; /*New*/
struct entropy_generic_file {
        char path[PATH_MAX];
        char filename[FILENAME_LENGTH];
        char mime_type[MIME_LENGTH];
	char uri_base[15];
	
	char filetype;

	char* username;  /*Do we have a cached auth reference for this file/location? */
	char* password;

	char* uri;

	entropy_thumbnail* thumbnail; /*NULL until created*/

	struct entropy_generic_file* parent;

	char retrieved_stat;
	struct stat properties;

	char* md5; /*A reference to the md5sum made for this file*/

	char* attach;
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
	entropy_generic_file* reparent_file;
	
	entropy_core* core; /*A reference into the system core */
	void* requester; /*The object that requested this file/directory, for reference tracking purposes*/
	int file_type;
	int drill_down; /*Indicate if this request should drill down through the child's mime type
			  I.e. if the file's uri is 'posix', and the file's mime is 'application/x-tar,
			  produce a construct of the form posix://path/to/file#tar:/// ..etc */

	int set_parent;
	int value;
};

typedef struct Entropy_Metadata_Object Entropy_Metadata_Object;
struct Entropy_Metadata_Object {
	const char* key;
	const char* value;
};


#define TYPE_CONTINUE 0
#define TYPE_END 1 
/*A temporary structure until I find a better way to do this*/
typedef struct entropy_file_progress {
	entropy_generic_file* file_from;
	entropy_generic_file* file_to;

	float progress;
	int type;
	long identifier;
} entropy_file_progress;

typedef struct entropy_file_operation {
	char* file;
	long id;
} entropy_file_operation;

typedef struct {
	char* filename;
} entropy_auth_request;

typedef struct entropy_file_stat entropy_file_stat;
struct entropy_file_stat {
	entropy_generic_file* file;
	struct stat* stat_obj;
};

typedef enum entropy_user_interaction_response {
	ENTROPY_USER_INTERACTION_RESPONSE_YES=0,
	ENTROPY_USER_INTERACTION_RESPONSE_YES_TO_ALL=1,
	ENTROPY_USER_INTERACTION_RESPONSE_NO=2,
	ENTROPY_USER_INTERACTION_RESPONSE_NO_TO_ALL=3,
	ENTROPY_USER_INTERACTION_RESPONSE_ABORT=4
} entropy_user_interaction_response;

typedef struct entropy_user_interaction entropy_user_interaction;
struct entropy_user_interaction {
	long id;
	entropy_user_interaction_response response;
};

	

enum entropy_generic_file_type {
	FILE_ALL,
	FILE_UNKNOWN,
	FILE_STANDARD,
	FILE_FOLDER
};

void entropy_generic_file_uri_set(entropy_generic_file* file);

#endif
