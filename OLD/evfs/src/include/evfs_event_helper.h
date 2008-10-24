#ifndef __EVFS_EVENT_HELPER_
#define __EVFS_EVENT_HELPER_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "evfs.h"

void evfs_file_monitor_event_create(evfs_client * client, int type,
                                    const char *pathi, int filetype, const char *plugin);
void evfs_stat_event_create(evfs_client * client, evfs_command * command,
                            struct stat *stat_obj);
void evfs_list_dir_event_create(evfs_client * client, evfs_command * command,
                                Ecore_List * files);
void evfs_file_progress_event_create(evfs_client * client,
				     EvfsFilereference* prog_file1,
				     EvfsFilereference* prog_file2,
                                     evfs_command * root_command,
                                     double progress, EvfsEventProgressType type);
void evfs_open_event_create(evfs_client * client, evfs_command * command);
void evfs_read_event_create(evfs_client * client, evfs_command * command,
                            char *bytes, long size);
void evfs_operation_event_create(evfs_client * client, evfs_command * command,
                                 evfs_operation * op, char* misc);

void
evfs_meta_data_event_create(evfs_client* client,evfs_command* command, Eina_List* ret_list);

void evfs_group_list_event_create(evfs_client* client, evfs_command* command, Eina_List* group_list);

void
evfs_auth_failure_event_create(evfs_client * client, evfs_command * command);

void evfs_mime_event_create(evfs_client* client, evfs_command* command, const char* mime);

void evfs_metaall_event_create(evfs_client* client, evfs_command* command, Eina_List* e);

#endif
