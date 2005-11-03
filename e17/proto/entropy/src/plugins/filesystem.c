#include "plugin.h"
#include <stdio.h>
#include "plugin_base.h"
#include <Ecore.h>
#include "entropy.h"
#include "entropy_gui.h"
#include <evfs.h>
#include <sys/types.h>
#include <dirent.h>

#define NO_HIDDEN 1
#define URI_POSIX "posix"


/*static Ecore_Hash* file_interest_list;
static Ecore_Hash* folder_interest_list;*/

Ecore_Hash* folder_monitor_hash;
Ecore_Hash* stat_request_hash;
Ecore_Hash* evfs_dir_requests;
entropy_core* filesystem_core; /*A reference to the core*/


//TODO: In its current implementation - stats can only be tracked by one instance at a time.
//	I.e. if a request is made for a file by one instance, and another instance requests a stat on that file,
//	whilst we're waiting for a response on the first, we'll have a collision.  This will almost *never* happen, 
//	but it would be a good idea to fix.

evfs_connection* con;

void callback(evfs_event* data) {
	
	//printf("Callback hit..%d\n", data->type);

                                                                                                                                         
        switch (data->type) {
                        case EVFS_EV_SUB_MONITOR_NOTIFY: {
				/*Find a better way to do this, possibly modify evfs to pass a reference object*/
				Ecore_List* watchers;
				void* key;
				entropy_gui_event* gui_event;
								 
				char* folder = strdup((char*)data->file_monitor.filename);
				char* pos = rindex(folder, '/');
				*pos = '\0';
				
                                //printf("DEMO: Received a file monitor notification\n");
                                //printf("DEMO: For file: '%s'\n", (char*)data->file_monitor.filename);
				//printf("DEMO: Directory '%s\n", folder);
				//printf("DEMO: Plugin: '%s'\n", (char*)data->file_monitor.plugin);


				watchers = ecore_hash_keys(folder_monitor_hash);
				ecore_list_goto_first(watchers);
				while ( (key = ecore_list_next(watchers))) {
					//printf("'%s'  :  '%s'\n", ((evfs_file_uri_path*)ecore_hash_get(folder_monitor_hash, key))->files[0]->path, folder);
					
					if ( !strcmp( ((evfs_file_uri_path*)ecore_hash_get(folder_monitor_hash, key))->files[0]->path, folder)) {
						char* md5 = md5_entropy_path_file(folder, pos+1);
						entropy_file_listener* listener;

						
						entropy_generic_file* file;
						if ( !ecore_hash_get(((entropy_gui_component_instance*)key)->core->file_interest_list, md5) ) {
							file = entropy_generic_file_new();
							/*For now, just make an entropy_generic_file for this object*/

							strncpy(file->path, folder, 255);
							strncpy(file->filename, pos+1, strlen(pos+1)+1);
							strncpy(file->uri_base, data->file_monitor.plugin, strlen(data->file_monitor.plugin));
							file->md5 = strdup(md5);

							/*Register a new listener for this file*/
							listener = entropy_malloc(sizeof(entropy_file_listener));
							listener->file = file;
							listener->count = 0;
							entropy_core_file_cache_add(((entropy_gui_component_instance*)key)->core,md5, listener);

							entropy_free(folder);

						} else {
							listener = ecore_hash_get(((entropy_gui_component_instance*)key)->core->file_interest_list, md5);
							file=listener->file;
							entropy_free(md5); /*We don't need this one, we're returning an old ref*/
							entropy_free(folder);
						}

						
						//printf("Found a watcher of this dir\n");


						
						
						/*Create a GUI event to send to watchers*/
						gui_event = entropy_malloc(sizeof(entropy_gui_event));

						switch (data->file_monitor.fileev_type) {
								case EVFS_FILE_EV_CREATE: //printf("  Create event\n");
								  gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FILE_CREATE);
								  break;
								case EVFS_FILE_EV_CHANGE: //printf("  Change event\n");
								  gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FILE_CHANGE);
								  break;
								case EVFS_FILE_EV_REMOVE: //printf("  Change event\n");
								  gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FILE_REMOVE);
								  break;
								case EVFS_FILE_EV_REMOVE_DIRECTORY: //printf("  Change event\n");
								  gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FILE_REMOVE_DIRECTORY);
								  break;


								
						}	
						
						gui_event->data = file; 
						entropy_core_layout_notify_event(key, gui_event, ENTROPY_EVENT_LOCAL); 
					}
				}
			}	
                        break;


			case EVFS_EV_STAT: {

				entropy_gui_event* gui_event;
				entropy_gui_component_instance* instance;
				entropy_file_stat* file_stat;


				char* md5;
				char* folder = strdup((char*)data->resp_command.file_command.files[0]->path);
				char* pos = rindex(folder, '/');
				
				
				
				*pos = '\0';

				//printf("Folder: '%s'\nFilename: '%s'\n", folder, pos+1);
				md5 = md5_entropy_path_file(folder, pos+1);
				instance = ecore_hash_get(stat_request_hash, md5);

				//printf("Got an instance for this stat request: '%p'\n", instance);
				
			
				//printf("Received stat response at callback for file '%s'\n", data->resp_command.file_command.files[0]->path);

				/*Build a file<->stat structure to pass to requester*/
				file_stat = entropy_malloc(sizeof(entropy_file_stat));
				file_stat->stat_obj = entropy_malloc(sizeof(struct stat));
				memcpy(file_stat->stat_obj, &data->stat.stat_obj, sizeof(struct stat));

				/*Retrieve the file. This is bad - the file might not exist anymore! */
				file_stat->file = ((entropy_file_listener*)ecore_hash_get(((entropy_gui_component_instance*)instance)->core->file_interest_list, md5))->file;

				

				//printf("File got: md5 %s, pointer %p - '%s', '%s'\n", md5, file_stat->file, file_stat->file->path, file_stat->file->filename);

				/*Build up the gui_event wrapper*/
				gui_event = entropy_malloc(sizeof(entropy_gui_event)); 
				gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FILE_STAT_AVAILABLE);
				gui_event->data = file_stat;

				//printf("File stat at %p\n", file_stat);

				ecore_hash_remove(stat_request_hash, md5);
				entropy_free(folder);
				entropy_free(md5);

				/*Call the callback stuff*/
				entropy_core_layout_notify_event(instance, gui_event, ENTROPY_EVENT_LOCAL);

				/*Do some freeing*/
				entropy_free(file_stat);
				entropy_free(file_stat->stat_obj);
			

			}
			break;


			case EVFS_EV_DIR_LIST: {
				Ecore_List* file_list = ecore_list_new();	
				entropy_file_request* calling_request = NULL;
				entropy_generic_file* file;
				entropy_file_listener* listener;
				entropy_gui_event* gui_event;
				evfs_filereference* ref;
				char* filename;
				char* folder;
				char *pos;
				
				//printf("Received a directory listing..\nFiles:\n\n");
				//printf("Original uri: %s\n", data->resp_command.file_command.files[0]->path);

				if (!data->file_list.list) {
					//printf("No directory received - invalid uri?\n");
					ecore_list_destroy(file_list);
					return;
				} else {
					//printf("File list at %p\n", data->file_list.list);
				}

				ecore_list_goto_first(data->file_list.list);
				while (  (ref= ecore_list_next(data->file_list.list)) ) {

					
					printf("(%s) Received file type for file: %d\n", ref->path, ref->file_type);
				
					folder = strdup((char*)ref->path);
					pos = rindex(folder, '/');
					*pos = '\0';

					filename = strdup(pos+1);

					/*If we are the root dir (i.e. we only have one "/", replace it (so we can use it below),
					 * and assume this filename has length (BAD).  */
					if (!strlen(folder)) {
						*pos = '/';
						*(pos+1) = '\0';
					}

					printf("Calling folder is '%s'\n", folder);

					/*If the calling request is currently NULL, we must go to the hash to retrieve that caller*/
					if (!calling_request) {
						calling_request = ecore_hash_get(evfs_dir_requests, folder);
						//printf("Received a file from the hash, path is '%s'\n", calling_request->file->path);

						/*Append the original request as the first item in the list*/
						/*It's not nice having to do this, but we're kind of stuck.  Should be ok as long
						 * as it gets documented*/
						ecore_list_append(file_list, calling_request);

					}

					//printf("Folder name: '%s', filename '%s'\n", folder, pos+1);
				
					/*Look for an existing file we have cached*/
					char* md5 = md5_entropy_path_file(folder, filename);

					/*Now create, or grab, a file*/
					if ( !ecore_hash_get(filesystem_core->file_interest_list, md5) ) {

						file = entropy_generic_file_new();
						/*For now, just make an entropy_generic_file for this object*/
						strncpy(file->path, folder, 255);
						strncpy(file->filename, filename, strlen(filename)+1);
						file->md5 = strdup(md5);

						/*Set the file type, if evfs provided it*/
						if (ref->file_type == EVFS_FILE_DIRECTORY) {
							file->filetype = FILE_FOLDER;
							strcpy(file->mime_type, "file/folder");

							//printf("Marked this file as a directory\n");
						} else {
							//printf("Didn't mark this file, type %d\n",ref->file_type);
							file->filetype = FILE_STANDARD;
							bzero(file->mime_type, MIME_LENGTH);
						}

						if (calling_request && (calling_request->drill_down || calling_request->set_parent)) {
							printf("Calling request had a parent...\n");
							printf("File ('%s') parent's name is '%s'\n", file->filename, calling_request->reparent_file->filename);

							file->parent = calling_request->reparent_file;
						}

						/*Mark the file's uri FIXME do this properly*/
						strcpy(file->uri_base, "smb");
						

						/*Register a new listener for this file*/
						listener = entropy_malloc(sizeof(entropy_file_listener));
						listener->file = file;
						listener->count = 0;
						entropy_core_file_cache_add(filesystem_core,md5, listener);


					} else {
						listener = ecore_hash_get(filesystem_core->file_interest_list, md5);
						file=listener->file;
						entropy_free(md5); /*We don't need this one, we're returning an old ref*/
					}

					free(folder);
					free(filename);


					/*Add this file to our list*/
					ecore_list_append(file_list, file);
				}


				/*Create a GUI event to send to watchers*/
				gui_event = entropy_malloc(sizeof(entropy_gui_event));
				gui_event->event_type = entropy_core_gui_event_get(ENTROPY_GUI_EVENT_FOLDER_CHANGE_CONTENTS_EXTERNAL);
				gui_event->data = file_list; 
				entropy_core_layout_notify_event(calling_request->requester , gui_event, ENTROPY_EVENT_LOCAL); 

			
				//TODO - destroy all the shit we create here
			}

			break;
        }

}



/*Internal functions*/
void filesystem_monitor_directory(void* requester, evfs_file_uri_path* folder) {
	ecore_hash_set(folder_monitor_hash, requester, folder);
}

void filesystem_demonitor_directory(void* requester) {
	/*We assume we only monitor one dir at a time, per requester*/
	evfs_file_uri_path* dir = ecore_hash_get(folder_monitor_hash, requester);
	if (dir) {
		//fprintf(stderr, "Demonitoring '%s'\n", dir->files[0]->path);
		evfs_monitor_remove(con, dir->files[0]);
	} else {
		//fprintf(stderr, "This requester has no monitored directories\n");
	}
}
/*---------------------------*/



char* entropy_plugin_identify() {
	return (char*)"Simple file backend";	
}

int entropy_plugin_type_get() {
	return ENTROPY_PLUGIN_BACKEND_FILE;
}

void entropy_plugin_init(entropy_core* core) {
	/*file_interest_list = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	folder_interest_list = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);*/
	folder_monitor_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	stat_request_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	evfs_dir_requests = ecore_hash_new(ecore_str_hash, ecore_str_compare);	

	con = evfs_connect(&callback);

	filesystem_core = core;
}

void entropy_plugin_destroy(void* obj) {
}

Ecore_List* structurelist_get(char* base) {
	struct dirent* de;
	struct stat st;
	DIR* dir;
	entropy_generic_file* ef;
	Ecore_List* el = ecore_list_new();
	char full_name[1024];

	dir = opendir(base);
	while ( (de = readdir(dir)) ) {
			snprintf(full_name, 1024, "%s/%s", base, de->d_name);
			stat(full_name, &st);	
		
			if (S_ISDIR(st.st_mode) && strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
				//printf ("%s is a directory\n", de->d_name);
				
				ef = entropy_generic_file_new();

				/*TODO don't use this - make an internal representation,
				 * otherwise not generic */

				ef->filetype = DT_DIR; 
				strcpy(ef->filename, de->d_name);
				strcpy(ef->path, base);
				ecore_list_append(el, ef);
			}
	}

	return el;	
	
}


struct stat* filestat_get(entropy_file_request* request) {
	evfs_file_uri_path* path;
	char pathi[1024];
	

	//printf("Getting a stat from evfs...\n");

	snprintf(pathi,1024,"%s://%s/%s", request->file->uri_base, request->file->path, request->file->filename);

	path = evfs_parse_uri(pathi);
	char* md5 = md5_entropy_path_file(request->file->path, request->file->filename);
	ecore_hash_set(stat_request_hash, md5, request->requester);
	evfs_client_file_stat(con, path->files[0]);
	
	
	return NULL;
}



Ecore_List* filelist_get(entropy_file_request* request) {
	struct dirent* de;
	struct stat st;
	DIR* dir;
	entropy_generic_file* ef;
	Ecore_List* el = ecore_list_new();

	char dire[255];
	char full_name[1024];
	char posix_name[1024];
	char *md5;
	int filetype = -1;
	evfs_file_uri_path* dir_path;
	entropy_file_listener* listener;

	if ( (!strcmp(request->file->uri_base, "posix")) && !request->drill_down && !request->file->parent) {
		/*If either the path, or the filename, is the root dir, we don't need another slash*/
		if (strcmp(request->file->filename, "/") && strcmp(request->file->path, "/")) {		
			snprintf(dire, 255, "%s/%s", request->file->path, request->file->filename);
		} else {
			snprintf(dire, 255, "%s%s", request->file->path, request->file->filename);
		}
	

		//printf("Opening dir: %s\n", dire);


		if (access(dire, X_OK)) {
			return NULL;
		}

		snprintf(posix_name, 1024, "posix://%s", dire);
		dir_path = evfs_parse_uri(posix_name);

		/*First demonitor this reqeuestor's last directory, if any*/
		filesystem_demonitor_directory(request->requester);
	

		/*Start monitoring this directory*/
		//printf("Monitoring '%s'\n", dir_path->files[0]->path);
		evfs_monitor_add(con, dir_path->files[0]);
		filesystem_monitor_directory(request->requester, dir_path);

		dir = opendir(dire);
		while ( (de = readdir(dir)) ) {
			if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..") && (!NO_HIDDEN || de->d_name[0] != '.')) {
                                snprintf(full_name, 1024, "%s/%s", dire, de->d_name);
                                stat(full_name, &st);
				if (S_ISDIR(st.st_mode)) {
					filetype = FILE_FOLDER;
					/*printf ("This is a folder\n");*/
				} else {
					/*printf( "Not a folder\n");*/
					filetype = -1;
				}

				if (request->file_type == FILE_ALL || filetype == request->file_type) {	
					/*printf ("Adding..\n");*/
					
					md5 = md5_entropy_path_file(dire, de->d_name);				

					/*printf("Looking for key %s\n", md5);	*/
					if ( !ecore_hash_get(request->core->file_interest_list, md5) ) {			
						/*printf ("File not found in hash, making new instance\n");*/
					
						ef = entropy_generic_file_new();

						strcpy(ef->uri_base, URI_POSIX); /*We're a fallback posix handler, so set to posix*/

						/*TODO don't use this - make an internal representation, otherwise not generic */
						ef->filetype = (filetype ? filetype : de->d_type); 
						ef->md5 = md5;
							
						strcpy(ef->filename, de->d_name);
						strcpy(ef->path, dire);
	
						ecore_list_append(el, ef);

						/*Register a new listener for this file*/
						listener = entropy_malloc(sizeof(entropy_file_listener));
						listener->file = ef;
						listener->count = 0;

						entropy_core_file_cache_add(request->core,md5, listener);
					} else {
						listener = ecore_hash_get(request->core->file_interest_list, md5);
						ecore_list_append(el, listener->file);
						entropy_free(md5); /*We don't need this one, we're returning an old ref*/
					}
				} else {
					/*printf ("Not addiong...\n");*/
				}
				//
				
			}
		}
		closedir(dir);

		return entropy_generic_file_list_sort(el);
	} else { /*Not a posix call for a dir list - don't use our local optim function */
		entropy_file_request* new_request; /*We need to make a copy of the request object
						     because the original will be destroyed*/

		entropy_generic_file* source_file;
		char uri[512];
		evfs_file_uri_path* path;
		
		//printf("*** Requested a '%s' dir list, calling evfs\n", request->file->uri_base);
		//
		//
		/*If the file/location we are requesting has a 'parent' (i.e. it's inside another object),
		 * we have to grab the parent, not the file itself, as the base*/
		if (request->file->parent) {
			source_file = request->file->parent;
		} else
			source_file = request->file;
		//

		/*First build uri..*/

		/*Do we have login information*/
		/*TODO - wrap this up in some kind of entropy_generic_file_to_evfs_uri function*/
		if (!source_file->username) {
			snprintf(uri, 512, "%s://%s/%s",  source_file->uri_base, source_file->path, source_file->filename);
		} else {
			snprintf(uri, 512, "%s://%s:%s@%s/%s",  source_file->uri_base, 
					source_file->username, source_file->password, 
					source_file->path, source_file->filename);
		}

		/*If it's a drill down request, or the file has a parent */
		if (request->drill_down || request->file->parent) {
			char* uri_retrieve;
			char uri_build[255];
			
			/*The file is a drill down request*/

			
			printf("EVFS says that this file descends through '%s'\n", uri);
			
			/*If we're a 'drill-down', we're at the root - so request the root*/
			if (request->drill_down) {
				uri_retrieve = entropy_core_descent_for_mime_get(request->core,request->file->mime_type);
				snprintf(uri_build, 255, "#%s:///", uri_retrieve);
				printf("URI build says: '%s'\n", uri_build);
				strcat(uri, uri_build); 
			} else if (request->file->parent) {
				printf("Retrieving mime-descend from parent...'%s' for file with name '%s'\n", request->file->parent->mime_type, request->file->parent->filename);
				uri_retrieve = entropy_core_descent_for_mime_get(request->core,request->file->parent->mime_type);

				/*Special case handler for the root dir - FIXME*/
				printf("Path: '%s', filename '%s'\n", request->file->path, request->file->filename);
				if (!strcmp(request->file->path,"/")) {
					snprintf(uri_build, 255, "#%s://%s%s", uri_retrieve, request->file->path, request->file->filename);
				} else {
					snprintf(uri_build, 255, "#%s://%s/%s", uri_retrieve, request->file->path, request->file->filename);
				}

				strcat(uri, uri_build);
			}

			

		}
		
		printf("URI: %s\n", uri);

		path = evfs_parse_uri(uri);
		evfs_client_dir_list(con, path->files[0]);


		new_request = entropy_malloc(sizeof(entropy_file_request));

		/*If this request/file has a parent, the new file listing's parent will be 
		 * the same file - not the request file */
		
		new_request->file = request->file;
		
		if (request->file->parent || request->drill_down) {
			new_request->reparent_file = source_file;
			new_request->set_parent = 1;
		}
			
		
		
		
		new_request->core = request->core;
		new_request->drill_down = request->drill_down;
		new_request->requester = request->requester;
		new_request->file_type = request->file_type;
		if (request->drill_down) {
			ecore_hash_set(evfs_dir_requests, "/", new_request);
		} else {
			ecore_hash_set(evfs_dir_requests, path->files[0]->path, new_request);
		}
		
		return NULL;
	}

	
		
}

void entropy_plugin_backend_file_deregister_file_list() {

}



void entropy_filesystem_file_copy(entropy_generic_file* file, char* path_to) {
	evfs_file_uri_path* uri_path_from;
	evfs_file_uri_path* uri_path_to;

	char uri_from[512];
	char uri_to[512];
	
	

	snprintf(uri_from, 512, "%s://%s/%s", file->uri_base, file->path, file->filename);
	snprintf(uri_to, 512, "%s/%s", path_to, file->filename);

	//printf ("Copying file %s to %s\n", uri_from, uri_to);

	

	uri_path_from = evfs_parse_uri(uri_from);
	uri_path_to = evfs_parse_uri(uri_to);

	evfs_client_file_copy(con, uri_path_from->files[0], uri_path_to->files[0]);
}

