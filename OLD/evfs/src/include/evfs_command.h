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
   EVFS_CMD_METADATA_FILE_GET = 18,
   EVFS_CMD_METADATA_GROUPS_GET = 19,
   EVFS_CMD_METADATA_FILE_GROUP_ADD = 20,
   EVFS_CMD_METADATA_FILE_GROUP_REMOVE = 21,
   EVFS_CMD_TRASH_RESTORE = 22,
   EVFS_CMD_AUTH_RESPONSE = 23,
   EVFS_CMD_MIME_REQUEST = 24,
   EVFS_CMD_FILE_TAG_ADD = 25,
   EVFS_CMD_FILE_TAG_REMOVE = 26,
   EVFS_CMD_VFOLDER_CREATE = 27,
   EVFS_CMD_META_ALL_REQUEST = 28,
   EVFS_CMD_AUTH_EXCHANGE = 29
}
evfs_command_type;

typedef enum EvfsCommandOption {
	EVFS_COMMAND_OPTION_STAT = 1
} EvfsCommandOption;

typedef struct evfs_command_file
{
   evfs_command_type type;
   int extra;
   char* ref;
   char* ref2;

   Eina_List* files;
}
evfs_command_file;

typedef struct evfs_command
{
   evfs_command_type type;
   evfs_command_file* file_command;
   struct evfs_operation *op;
   Eina_List* entries;

   int options;
   long client_identifier;
}
evfs_command;

EvfsFilereference* evfs_command_first_file_get(evfs_command* command);
EvfsFilereference* evfs_command_second_file_get(evfs_command* command);
EvfsFilereference* evfs_command_nth_file_get(evfs_command* command, int n);
int evfs_command_file_count_get(evfs_command* command);
void evfs_command_localise(evfs_command* command);

#endif
