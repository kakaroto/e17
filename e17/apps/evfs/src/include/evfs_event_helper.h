#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void evfs_file_monitor_event_create(evfs_client* client, int type, const char* path);
void evfs_stat_event_create(evfs_client* client, struct stat* stat_obj);
