/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifndef efsd_h
#define efsd_h

#include <sys/types.h>
#include <sys/stat.h>

#define EFSD_CLIENTS      100
#define IPC_TITLE "EFSD"

/* Miscellaneous, needed below. */

typedef int EfsdCmdId;

typedef enum efsd_filechange_type
{
  EFSD_FILE_CHANGED          = 1,
  EFSD_FILE_DELETED          = 2,
  EFSD_FILE_START_EXEC       = 3,
  EFSD_FILE_STOP_EXEC        = 4,
  EFSD_FILE_CREATED          = 5,
  EFSD_FILE_MOVED            = 6,
  EFSD_FILE_ACKNOWLEDGE      = 7,
  EFSD_FILE_EXISTS           = 8,
  EFSD_FILE_END_EXISTS       = 9,
  EFSD_FILE_METADATA_CHANGED = 10
}
EfsdFilechangeType;

typedef enum efsd_event_type
{
  EFSD_EVENT_FILECHANGE,
  EFSD_EVENT_METADATA_CHANGE,
  EFSD_EVENT_REPLY
}
EfsdEventType;

typedef enum efsd_command_type
{
  EFSD_CMD_REMOVE,
  EFSD_CMD_MOVE, 
  EFSD_CMD_COPY, 
  EFSD_CMD_SYMLINK,
  EFSD_CMD_LISTDIR, 
  EFSD_CMD_MAKEDIR, 
  EFSD_CMD_CHMOD,
  EFSD_CMD_SETMETA, 
  EFSD_CMD_GETMETA, 
  EFSD_CMD_STARTMON_FILE,
  EFSD_CMD_STARTMON_DIR,
  EFSD_CMD_STARTMON_META,
  EFSD_CMD_STOPMON_FILE, 
  EFSD_CMD_STOPMON_DIR, 
  EFSD_CMD_STOPMON_META,
  EFSD_CMD_STAT, 
  EFSD_CMD_LSTAT, 
  EFSD_CMD_READLINK, 
  EFSD_CMD_GETFILETYPE,
  EFSD_CMD_CLOSE
}
EfsdCommandType;

typedef enum efsd_datatype
{
  EFSD_INT    = 1,
  EFSD_FLOAT  = 2,
  EFSD_STRING = 3,
  EFSD_RAW    = 4
}
EfsdDatatype;

typedef enum efsd_option_type
{
  EFSD_OP_FORCE,
  EFSD_OP_RECURSIVE,
  EFSD_OP_ALL,
  EFSD_OP_GET_STAT,
  EFSD_OP_GET_LSTAT,
  EFSD_OP_GET_FILETYPE,
  EFSD_OP_GET_META,
  EFSD_OP_SORT
}
EfsdOptionType;

typedef struct efsd_option_getmeta
{
  EfsdOptionType      type;
  char               *key;
  EfsdDatatype        datatype;
}
EfsdOptionGetmeta;


/* Options -- only the getmeta option
   needs further parameters, all others
   are defined simply through their type.
*/
typedef union efsd_option
{
  EfsdOptionType      type;
  EfsdOptionGetmeta   efsd_op_getmeta;
}
EfsdOption;


/* Commands, sent from client to daemon. */

/* General datastructure for commands
   operating on an arbitrary number of
   files.
*/
typedef struct efsd_file_cmd
{
  EfsdCommandType     type;
  EfsdCmdId           id;
  int                 num_files;
  char              **files;
  int                 num_options;
  EfsdOption         *options;
}
EfsdFileCmd;

/* For chmodding files, contains new mode too. */
typedef struct efsd_chmod_cmd
{
  EfsdCommandType     type;
  EfsdCmdId           id;
  char               *file;
  mode_t              mode;
}
EfsdChmodCmd;

/* For setting metadata. */
typedef struct efsd_set_metadata_cmd
{
  EfsdCommandType     type;
  EfsdCmdId           id;
  EfsdDatatype        datatype;
  int                 data_len;
  void               *data;
  char               *key;
  char               *file;
}
EfsdSetMetadataCmd;

/* For getting metadata. */
typedef struct efsd_get_metadata_cmd
{
  EfsdCommandType     type;
  EfsdCmdId           id;
  EfsdDatatype        datatype;
  char               *key;
  char               *file;
}
EfsdGetMetadataCmd;

/* Empty -- just for closing a connection. */
typedef struct efsd_close_cmd
{
  EfsdCommandType     type;
}
EfsdCloseCmd;

/* X-Event-like union of commands. */
typedef union efsd_command
{
  EfsdCommandType     type;
  EfsdFileCmd         efsd_file_cmd;
  EfsdChmodCmd        efsd_chmod_cmd;
  EfsdGetMetadataCmd  efsd_get_metadata_cmd;
  EfsdSetMetadataCmd  efsd_set_metadata_cmd;
  EfsdCloseCmd        efsd_close_cmd;
}
EfsdCommand;


/* Events, sent from daemon to client. */

/* Filechange event.
*/
typedef struct efsd_filechange_event
{
  EfsdEventType       type;
  EfsdCmdId           id;
  EfsdFilechangeType  changetype;
  char               *file;
}
EfsdFileChangeEvent;


/* Metadata change event.
 */
typedef struct efsd_metachange_event
{
  EfsdEventType       type;
  EfsdCmdId           id;
  char               *key;
  char               *file;
  EfsdDatatype        datatype;
  int                 data_len;
  void               *data;
}
EfsdMetadataChangeEvent;


/* General reply to commands, contains
   entire command as well
*/
typedef struct efsd_reply_event
{
  /* Type of the event */
  EfsdEventType       type;      

  /* The original command, without options */
  EfsdCommand         command;   

  /* Errorcode -- if 0, things were successful. */
  int                 errorcode;

  /* Length of any returned data and data itself */
  int                 data_len;
  void               *data;
}
EfsdReplyEvent;


/* General event structure */
typedef union efsd_event
{
  EfsdEventType           type;
  EfsdFileChangeEvent     efsd_filechange_event;
  EfsdMetadataChangeEvent efsd_metachange_event;
  EfsdReplyEvent          efsd_reply_event;
}
EfsdEvent;


#endif
