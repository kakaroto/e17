void evfs_handle_monitor_start_command(evfs_client* client, evfs_command* command);
void evfs_handle_monitor_stop_command(evfs_client* client, evfs_command* command);
void evfs_handle_file_remove_command(evfs_client* client, evfs_command* command);
void evfs_handle_file_rename_command(evfs_client* client, evfs_command* command);
void evfs_handle_file_stat_command(evfs_client* client, evfs_command* command);
void evfs_handle_dir_list_command(evfs_client* client, evfs_command* command);
