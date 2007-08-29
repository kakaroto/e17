#ifndef _EVFS_SERVER_HANDLE_H_
#define _EVFS_SERVER_HANDLE_H_

int evfs_uri_open(evfs_client * client, EvfsFilereference * uri);
int evfs_uri_read(evfs_client * client, EvfsFilereference * uri, char *bytes,
                  long size);
int evfs_uri_close(evfs_client * client, EvfsFilereference * uri);

void evfs_handle_monitor_start_command(evfs_client * client,
                                       evfs_command * command);
void evfs_handle_monitor_stop_command(evfs_client * client,
                                      evfs_command * command);
void evfs_handle_file_remove_command(evfs_client * client,
                                     evfs_command * command, evfs_command* root_command);
void evfs_handle_file_rename_command(evfs_client * client,
                                     evfs_command * command);
void evfs_handle_file_stat_command(evfs_client * client,
                                   evfs_command * command);
void evfs_handle_dir_list_command(evfs_client * client, evfs_command * command);
void evfs_handle_file_copy(evfs_client * client, evfs_command * command,
                           evfs_command * root_command, int move);
void evfs_handle_file_open_command(evfs_client * client,
                                   evfs_command * command);
void evfs_handle_file_read_command(evfs_client * client,
                                   evfs_command * command);
void evfs_handle_ping_command(evfs_client * client, evfs_command * command);
void evfs_handle_operation_command(evfs_client * client,
                                   evfs_command * command);

void evfs_handle_directory_create_command(evfs_client * client, evfs_command * command);

void evfs_handle_metadata_command(evfs_client* client, evfs_command* command);

void evfs_handle_metadata_string_file_set_command(evfs_client* client, 
				evfs_command* command, char* key, char* value);

void evfs_handle_metadata_string_file_get_command(evfs_client* client, 
				evfs_command* command, char* key) ;

void evfs_handle_metadata_groups_request_command(evfs_client* client, evfs_command* command);

void evfs_handle_metadata_file_group_add(evfs_client* client, evfs_command* command);
void evfs_handle_metadata_file_group_remove(evfs_client* client, evfs_command* command);

void evfs_handle_trash_restore_command(evfs_client * client,
                                   evfs_command * command);

void evfs_handle_auth_respond_command(evfs_client* client, evfs_command* command);
void evfs_handle_mime_request(evfs_client* client, evfs_command* command);

void evfs_handle_vfolder_create(evfs_client* client, evfs_command* command);

void evfs_handle_meta_all_request(evfs_client* client, evfs_command* command);
#endif
