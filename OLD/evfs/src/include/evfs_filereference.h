#ifndef __EVFS_FILE_H_
#define __EVFS_FILE_H_

#include "evfs_plugin.h"

typedef enum evfs_file_type evfs_file_type;
enum evfs_file_type
{
   EVFS_FILE_NORMAL = 1,
   EVFS_FILE_DIRECTORY = 2,
   EVFS_FILE_LINK = 3,
   EVFS_FILE_SMB_WORKGROUP = 4,
   EVFS_FILE_SMB_SERVER = 5,
   EVFS_FILE_SMB_FILE_SHARE = 6,
   EVFS_FILE_SMB_PRINTER_SHARE = 7,
   EVFS_FILE_SMB_COMMS_SHARE = 8,
   EVFS_FILE_SMB_IPC_SHARE = 9,
};

typedef enum evfs_uri_token_type
{
   EVFS_URI_TOKEN_KEYWORD,
   EVFS_URI_TOKEN_OPERATOR,
   EVFS_URI_TOKEN_STRING
} evfs_uri_token_type;

typedef struct evfs_uri_token
{
   evfs_uri_token_type type;
   char *token_s;
} evfs_uri_token;

typedef struct {
   int st_mode;
   int st_uid;
   int st_gid;
   uint64 st_size;
   int ist_atime;
   int ist_mtime;
   int ist_ctime;
} EvfsStat;


typedef struct EvfsFilereference
{
   char *plugin_uri;
   struct evfs_plugin *plugin;

   struct EvfsFilereference *parent;
   struct evfs_server *server;  /*The server that spawned/owns this fileref, if any */

   evfs_file_type file_type;
   char *path;

   char *username;              /*The username/password pair (if any) required to hit this file */
   char *password;

   char* attach; 		/*Miscellaneous attached data to this file reference*/

   int fd;                      /*The file descriptor (if any) */
   void *fd_p;

   EvfsStat* stat;
} EvfsFilereference;

typedef struct evfs_file_uri_path evfs_file_uri_path;
struct evfs_file_uri_path
{
   int num_files;
   EvfsFilereference **files;
};

EvfsFilereference* evfs_filereference_new(char* plugin, char* path, int filetype);
EvfsFilereference* evfs_filereference_local_new(char* path);
void evfs_filereference_stat_to_system(EvfsFilereference* file, struct stat* st);
void evfs_stat_system_to_evfs_filereference(EvfsFilereference* file, struct stat* st);

#endif
