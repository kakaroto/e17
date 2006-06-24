#ifndef __EVFS_COMMAND_H_
#define __EVFS_COMMAND_H_

#include "evfs_filereference.h"

/*Command structures*/
typedef enum evfs_command_type
{
   EVFS_CMD_STARTMON_FILE = 1,
   EVFS_CMD_STOPMON_FILE = 2,
   EVFS_CMD_FILE_COPY = 3,
   EVFS_CMD_FILE_MOVE = 4,
   EVFS_CMD_MOVE_FILE = 5,
   EVFS_CMD_RENAME_FILE = 6,
   EVFS_CMD_REMOVE_FILE = 7,
   EVFS_CMD_LIST_DIR = 8,
   EVFS_CMD_FILE_STAT = 9,
   EVFS_CMD_FILE_OPEN = 10,
   EVFS_CMD_FILE_READ = 11,
   EVFS_CMD_FILE_TEST = 12,
   EVFS_CMD_PING = 13,
   EVFS_CMD_OPERATION_RESPONSE = 14,
   EVFS_CMD_DIRECTORY_CREATE = 15,
   EVFS_CMD_METADATA_RETRIEVE = 16,
   EVFS_CMD_METADATA_FILE_SET = 17,
   EVFS_CMD_METADATA_FILE_GET = 18
}
evfs_command_type;

typedef struct evfs_command_file
{
   evfs_command_type type;
   int num_files;
   int extra;
   char* ref;
   char* ref2;

   evfs_filereference **files;
}
evfs_command_file;

typedef struct evfs_command
{
   evfs_command_type type;
   evfs_command_file file_command;
   struct evfs_operation *op;

   long client_identifier;
}
evfs_command;

#endif
