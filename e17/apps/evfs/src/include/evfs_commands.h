#ifndef __EVFS_COMMANDS_H_
#define __EVFS_COMMANDS_H_

void evfs_monitor_add(evfs_connection* conn, evfs_filereference* ref);
void evfs_write_file_command(evfs_connection* conn, evfs_command* command);

#endif

