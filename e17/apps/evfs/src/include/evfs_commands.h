#ifndef __EVFS_COMMANDS_H_
#define __EVFS_COMMANDS_H_

long evfs_monitor_add(evfs_connection * conn, evfs_filereference * ref);
long evfs_monitor_remove(evfs_connection * conn, evfs_filereference * ref);
long evfs_client_file_remove(evfs_connection * conn, evfs_filereference * ref);
long evfs_client_file_rename(evfs_connection * conn, evfs_filereference * from,
                             evfs_filereference * to);
long evfs_client_file_stat(evfs_connection * conn, evfs_filereference * file);
long evfs_client_dir_list(evfs_connection * conn, evfs_filereference * file);
long evfs_client_file_open(evfs_connection * conn, evfs_filereference * file);
long evfs_client_file_copy(evfs_connection * conn, evfs_filereference * from,
                           evfs_filereference * to);
long evfs_client_file_move(evfs_connection * conn, evfs_filereference * from,
                           evfs_filereference * to);
long evfs_client_file_read(evfs_connection * conn, evfs_filereference * file,
                           int read_size);
long evfs_client_operation_respond(evfs_connection * conn, long opid,
                                   evfs_operation_response response);

long evfs_client_directory_create(evfs_connection * conn, evfs_filereference * file);

long 
evfs_client_metadata_retrieve(evfs_connection * conn, evfs_filereference* file );

long 
evfs_client_metadata_string_file_set(evfs_connection * conn, evfs_filereference* file, char* key,char* value );

long 
evfs_client_metadata_string_file_get(evfs_connection * conn, evfs_filereference* file, char* key );

long 
evfs_client_metadata_groups_get(evfs_connection * conn);

#endif
