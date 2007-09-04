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

#include <Evas.h>

#include <stdlib.h>
#include <string.h>

#define EVFS_IPC_TITLE "evfs_fs"
#define EVFS_WOR_TITLE "evfs_worker_daemon"

#define MAXPATHLEN 512
#define FALSE 0
#define TRUE 1
#define URI_SEP "#"
#define EVFS_META_DIR_NAME  ".e_meta"
#define COPY_BLOCKSIZE 16384
#define MAX_CLIENT 9999999

#define EVFS_FUNCTION_MONITOR_START "evfs_monitor_start"
#define EVFS_FUNCTION_MONITOR_STOP "evfs_monitor_stop"
#define EVFS_FUNCTION_DIRECTORY_LIST "evfs_directory_list"
#define EVFS_FUNCTION_FILE_COPY "evfs_file_copy"
#define EVFS_FUNCTION_FILE_MOVE "evfs_file_move"
#define EVFS_FUNCTION_FILE_STAT_GET "evfs_file_stat_get"

#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#  define GNUC_EXTENSION __extension__
#else
#  define GNUC_EXTENSION
#endif

GNUC_EXTENSION typedef signed long long int64;
GNUC_EXTENSION typedef unsigned long long uint64;


#include "evfs_plugin.h"

typedef enum
{
   EVFS_SUCCESS,
   EVFS_ERROR
} evfs_status;

typedef enum
{
   EVFS_FS_OP_FORCE = 1,
   EVFS_FS_OP_RECURSIVE = 2
}
EfsdFsOps;

/*----------------------------------------------------------------*/
#define INT64_CONSTANT(val)	(GNUC_EXTENSION (val##LL))

#define INT32_TO_BE(val)	((unsigned int) ( \
    (((unsigned int) (val) & (unsigned int) 0x000000ffU) << 24) | \
    (((unsigned int) (val) & (unsigned int) 0x0000ff00U) <<  8) | \
    (((unsigned int) (val) & (unsigned int) 0x00ff0000U) >>  8) | \
    (((unsigned int) (val) & (unsigned int) 0xff000000U) >> 24)))

#define UINT64_TO_BE(val)	((uint64) ( \
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x00000000000000ffU)) << 56) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x000000000000ff00U)) << 40) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x0000000000ff0000U)) << 24) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x00000000ff000000U)) <<  8) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x000000ff00000000U)) >>  8) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x0000ff0000000000U)) >> 24) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0x00ff000000000000U)) >> 40) |	\
      (((uint64) (val) &						\
	(uint64) INT64_CONSTANT (0xff00000000000000U)) >> 56)))

#include "evfs_auth.h"
#include "evfs_event.h"
#include "evfs_server.h"
#include "evfs_misc.h"
#include "evfs_cleanup.h"
#include "evfs_io.h"
#include "evfs_new.h"
#include "evfs_server_handle.h"
#include "evfs_common.h"
#include "evfs_vfolder.h"
#include "evfs_operation.h"
#include "evfs_operation_tasks.h"
#include "evfs_commands.h"
#include "evfs_event_helper.h"
#include "evfs_metadata.h"
#include "evfs_trash.h"
#include "evfs_filereference.h"

void evfs_operation_user_dispatch(evfs_client * client, evfs_command * command,
                                  evfs_operation * op, char* misc);
evfs_server* evfs_server_get();
evfs_server* evfs_server_new();

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
