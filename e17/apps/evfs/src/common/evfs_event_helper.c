#include "evfs.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void
evfs_file_monitor_event_create(evfs_client * client, int type, const char *path,
                               const char *plugin)
{
   /*Create a reply event for a file mon event, send it , destroy event */

   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_FILE_MONITOR;
   event->file_monitor.filename = strdup(path);
   event->file_monitor.fileev_type = type;
   event->file_monitor.plugin = strdup(plugin);
   event->file_monitor.filename_len = strlen(path) + 1;

   evfs_write_event(client, NULL, event);

   /*Now destroy */
   evfs_cleanup_event(event);
   /*No need to free data -it's probably a const raised by the ecore - it will free it. */

}

void
evfs_stat_event_create(evfs_client * client, evfs_command * command,
                       struct stat *stat_obj)
{
   /*Create a reply event for a file mon event, send it , destroy event */

   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_STAT;

   //memcpy(&event->stat.stat_obj, stat_obj, sizeof(struct stat));
   event->stat.stat_obj.st_uid = stat_obj->st_uid;
   event->stat.stat_obj.st_gid = stat_obj->st_gid;
   event->stat.stat_obj.st_uid = stat_obj->st_uid;
   event->stat.stat_obj.st_size = stat_obj->st_size;
   event->stat.stat_obj.ist_atime = stat_obj->st_atime;
   event->stat.stat_obj.ist_mtime = stat_obj->st_mtime;
   event->stat.stat_obj.ist_ctime = stat_obj->st_ctime;

   evfs_write_event(client, command, event);

   /*Now destroy */
   evfs_cleanup_event(event);

}

void
evfs_list_dir_event_create(evfs_client * client, evfs_command * command,
                           Ecore_List * files)
{
   /*Create a reply event for a file mon event, send it , destroy event */

   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_DIR_LIST;
   event->file_list.list = files;
   evfs_write_event(client, command, event);

   /*Now destroy */
   evfs_cleanup_event(event);

}

void
evfs_file_progress_event_create(evfs_client * client,
                                evfs_command * event_command,
                                evfs_command * root_command, double progress,
                                evfs_progress_type type)
{
   /*Create a reply event for a file mon event, send it , destroy event */

   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_FILE_PROGRESS;
   event->progress = NEW(evfs_event_progress);
   event->progress->file_progress = progress;
   event->progress->type = type;

   event->file_list.list = ecore_list_new();
   ecore_list_append(event->file_list.list,
                     event_command->file_command.files[0]);
   ecore_list_append(event->file_list.list,
                     event_command->file_command.files[1]);

   evfs_write_event(client, root_command, event);

   /*Now destroy */
   evfs_cleanup_event(event);
}

void
evfs_open_event_create(evfs_client * client, evfs_command * command)
{
   /*Create a reply event for a file mon event, send it , destroy event */

   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_FILE_OPEN;
   evfs_write_event(client, command, event);

   /*Now destroy */
   evfs_cleanup_event(event);
}

void
evfs_read_event_create(evfs_client * client, evfs_command * command,
                       char *bytes, long size)
{
   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_FILE_READ;
   event->data.size = size;
   event->data.bytes = bytes;
   evfs_write_event(client, command, event);

   /*Destroy */
   evfs_cleanup_event(event);
}

void
evfs_operation_event_create(evfs_client * client, evfs_command * command,
                            evfs_operation * op, char* misc)
{
   evfs_event *event = NEW(evfs_event);

   event->type = EVFS_EV_OPERATION;
   event->op = op;
   event->op->misc_str = misc;
   evfs_write_event(client, command, event);

   /*Destroy */
   evfs_cleanup_event(event);
}
