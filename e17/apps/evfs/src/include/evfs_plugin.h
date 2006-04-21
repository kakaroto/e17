#ifndef __EVFS_PLUGIN_H_
#define __EVFS_PLUGIN_H_

#include "evfs_client.h"

typedef struct evfs_plugin_functions_meta
{
   Evas_List* (*evfs_file_meta_retrieve)(evfs_client* client, evfs_command* command);
} evfs_plugin_functions_meta;

typedef struct evfs_plugin_functions
{
   int (*evfs_client_disconnect) (evfs_client *);
   int (*evfs_file_remove) (char *path);
   int (*evfs_monitor_start) (evfs_client * client, evfs_command * command);
   int (*evfs_monitor_stop) (evfs_client * client, evfs_command * command);
   int (*evfs_file_rename) (evfs_client * client, evfs_command * command);
   int (*evfs_file_stat) (evfs_command * command, struct stat * file_stat, int);
   int (*evfs_file_lstat) (evfs_command * command, struct stat * file_stat,
                           int);
   void (*evfs_dir_list) (evfs_client * client, evfs_command * command,
                          Ecore_List ** directory_list);

   int (*evfs_file_open) (evfs_client * client, evfs_filereference * command);
   int (*evfs_file_seek) (evfs_filereference * command, long offset,
                          int whence);
   int (*evfs_file_read) (evfs_client * client, evfs_filereference * command,
                          char *bytes, long size);
   int (*evfs_file_write) (evfs_filereference * command, char *bytes,
                           long size);
   int (*evfs_file_close) (evfs_filereference * command);
   int (*evfs_file_create) (evfs_filereference * file);

   int (*evfs_file_mkdir) (evfs_filereference * file);

} evfs_plugin_functions;

typedef struct evfs_plugin
{
   void *dl_ref;

} evfs_plugin;
#define EVFS_PLUGIN(plugin) ((evfs_plugin*)plugin)

typedef struct evfs_plugin_file
{
	evfs_plugin base;
	
	char *uri;
	evfs_plugin_functions *functions;

} evfs_plugin_file;
#define EVFS_PLUGIN_FILE(plugin) ((evfs_plugin_file*)plugin)

typedef struct evfs_plugin_meta
{
	evfs_plugin base;
	
	char *uri;
	evfs_plugin_functions_meta *functions;

} evfs_plugin_meta;
#define EVFS_PLUGIN_META(plugin) ((evfs_plugin_meta*)plugin)



#endif
