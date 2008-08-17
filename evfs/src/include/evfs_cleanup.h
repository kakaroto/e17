#ifndef __EVFS_CLEANUP_H_
#define __EVFS_CLEANUP_H_

typedef enum evfs_free_state
{
   EVFS_CLEANUP_FREE_COMMAND,
   EVFS_CLEANUP_PRESERVE_COMMAND
} evfs_free_state;

void evfs_cleanup_filereference(EvfsFilereference * ref);
void evfs_cleanup_command(evfs_command * command, int free_command);
void evfs_cleanup_file_command(evfs_command * command);
void evfs_cleanup_file_command_only(evfs_command* command);
void evfs_cleanup_file_monitor(evfs_file_monitor * mon);
void evfs_cleanup_event(EvfsEvent *);
void evfs_cleanup_file_uri_path(evfs_file_uri_path * path);
void evfs_cleanup_client(evfs_client * client);
void evfs_cleanup_vfolder_create(evfs_command* command);

#endif
