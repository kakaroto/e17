int evfs_uri_open(evfs_client * client, evfs_filereference * uri);
int evfs_uri_read(evfs_client * client, evfs_filereference * uri, char *bytes,
                  long size);
int evfs_uri_close(evfs_client * client, evfs_filereference * uri);

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
                           evfs_command * root_command);
void evfs_handle_file_open_command(evfs_client * client,
                                   evfs_command * command);
void evfs_handle_file_read_command(evfs_client * client,
                                   evfs_command * command);
void evfs_handle_ping_command(evfs_client * client, evfs_command * command);
void evfs_handle_operation_command(evfs_client * client,
                                   evfs_command * command);

void evfs_handle_directory_create_command(evfs_client * client, evfs_command * command);
