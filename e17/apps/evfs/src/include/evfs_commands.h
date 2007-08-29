#ifndef __EVFS_COMMANDS_H_
#define __EVFS_COMMANDS_H_

long evfs_monitor_add(evfs_connection * conn, EvfsFilereference * ref);
long evfs_monitor_remove(evfs_connection * conn, EvfsFilereference * ref);
long evfs_client_file_remove(evfs_connection * conn, EvfsFilereference * ref);
long evfs_client_file_rename(evfs_connection * conn, EvfsFilereference * from,
                             EvfsFilereference * to);
long evfs_client_file_stat(evfs_connection * conn, EvfsFilereference * file);
long evfs_client_dir_list(evfs_connection * conn, EvfsFilereference * file, int options);
long evfs_client_file_open(evfs_connection * conn, EvfsFilereference * file);
long evfs_client_file_copy(evfs_connection * conn, EvfsFilereference * from,
                           EvfsFilereference * to);
long evfs_client_file_copy_multi(evfs_connection * conn, Ecore_List* files,
				EvfsFilereference* to);
long 
evfs_client_file_move_multi(evfs_connection * conn, Ecore_List* files,
				EvfsFilereference* to);
long evfs_client_file_trash_restore(evfs_connection * conn, Ecore_List* files);
long evfs_client_file_move(evfs_connection * conn, EvfsFilereference * from,
                           EvfsFilereference * to);
long evfs_client_file_read(evfs_connection * conn, EvfsFilereference * file,
                           int read_size);
long evfs_client_operation_respond(evfs_connection * conn, long opid,
                                   evfs_operation_response response);

long evfs_client_directory_create(evfs_connection * conn, EvfsFilereference * file);

long 
evfs_client_metadata_retrieve(evfs_connection * conn, EvfsFilereference* file );

long 
evfs_client_metadata_string_file_set(evfs_connection * conn, EvfsFilereference* file, char* key,char* value );

long 
evfs_client_metadata_string_file_get(evfs_connection * conn, EvfsFilereference* file, char* key );

long 
evfs_client_metadata_groups_get(evfs_connection * conn);

long 
evfs_client_metadata_group_file_add(evfs_connection * conn, EvfsFilereference* ref, char* group);

long 
evfs_client_metadata_group_file_remove(evfs_connection * conn, EvfsFilereference* ref, char* group);

long 
evfs_client_auth_send(evfs_connection* conn, EvfsFilereference* ref, char* user, char* password);

long
evfs_client_mime_request(evfs_connection* conn, EvfsFilereference* ref);

int evfs_vfolder_command_send(evfs_connection* conn, evfs_command* command);
void evfs_vfolder_command_entry_add(evfs_command* command, char type, char* name, char* value);
evfs_command* evfs_vfolder_create_command_new(char* name); 

long
evfs_client_meta_list_all(evfs_connection* conn);

#endif
