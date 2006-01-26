#ifndef __EVFS_H_
#define __EVFS_H_

#define _GNU_SOURCE
#include "evfs_macros.h"
#include "evfs_debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <Ecore_File.h>
#include <pthread.h>

#include <stdlib.h>
#include <string.h>


#define EVFS_IPC_TITLE "evfs_fs"
#define MAXPATHLEN 512
#define FALSE 0
#define TRUE 1
#define URI_SEP "#"
#define EVFS_META_DIR_NAME  ".e_meta"
#define COPY_BLOCKSIZE 4096
#define MAX_CLIENT 9999999

#define EVFS_FUNCTION_MONITOR_START "evfs_monitor_start"
#define EVFS_FUNCTION_MONITOR_STOP "evfs_monitor_stop"
#define EVFS_FUNCTION_DIRECTORY_LIST "evfs_directory_list"
#define EVFS_FUNCTION_FILE_COPY "evfs_file_copy"
#define EVFS_FUNCTION_FILE_MOVE "evfs_file_move"
#define EVFS_FUNCTION_FILE_STAT_GET "evfs_file_stat_get"


#include "evfs_plugin.h"


typedef enum
{
  EVFS_FS_OP_FORCE      = 1,
  EVFS_FS_OP_RECURSIVE  = 2
}
EfsdFsOps;

#include "evfs_event.h"
#include "evfs_server.h"
#include "evfs_misc.h"
#include "evfs_commands.h"
#include "evfs_cleanup.h"
#include "evfs_io.h"
#include "evfs_new.h"
#include "evfs_event_helper.h"
#include "evfs_server_handle.h"
#include "evfs_common.h"
#include "evfs_vfolder.h"
#include "evfs_operation.h"

/**
 * ATTRIBUTE_UNUSED:
 *
 * This macro is used to flag unused function parameters to GCC
 */
#ifdef __GNUC__
#ifdef HAVE_ANSIDECL_H
#include <ansidecl.h>
#endif
#ifndef __UNUSED__
#define __UNUSED__ __attribute__((unused))
#endif
#else
#define __UNUSED__
#endif




#endif
