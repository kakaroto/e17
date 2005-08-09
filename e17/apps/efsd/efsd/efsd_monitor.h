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
#ifndef efsd_monitor_h
#define efsd_monitor_h

#include <fam.h>

#include <efsd.h>
#include <efsd_options.h>
#include <efsd_dynarray.h>
#include <efsd_list.h>
#include <efsd_lock.h>

#define EFSD_CLIENT_INTERNAL -1

typedef struct efsd_monitor_request
{
  Ecore_Ipc_Client*      client;
  EfsdCmdId             id;

  int                   num_options;
  EfsdOption           *options;

  /* whether or not this request is still valid. */
  char                  is_finished;
}
EfsdMonitorRequest;


typedef struct efsd_monitor
{
  /* filename that is monitored     */
  char                 *filename;

  /* the FAM request                */
  FAMRequest           *fam_req;

  /* Use counts for this monitor: the client_use_count
     counts 1 per client, the internal_use count is
     a special client that represents the efsd-internal
     monitoring requests.
  */
  int                   client_use_count;
  int                   internal_use_count;

#if USE_THREADS
  pthread_mutex_t       use_count_mutex;
#endif


  /* Which clients monitor this file,
     and with what command id.
     list<EfsdMonitorRequest*>.
  */
  EfsdList             *clients;

  /* The files monitored by this monitor: */
  EfsdDynCharArray     *files; 

  /* whether this monitor can safely
     be deleted, because there won't
     be any further FAM events */
  char                  is_finished;

  /* whether this monitor monitors an
     entire directory (checking the file
     won't do -- we could be monitoring
     only the directory file) */
  char                  is_dir;

  /* whether this monitor is registered
     as a "full" monitor or just as a
     temporary one. */
  
  char                  is_temporary;

  /* if this is a directory monitor,
     specify whether or not the files
     list is to be kept sorted.
  */

  char                  is_sorted;

  /* while efsd receives file-exists events,
     this flag is set to TRUE. It is needed to avoid races
     when a client requests stopping monitoring while events
     are still received.
  */

  char                  is_receiving_exist_events;
}
EfsdMonitor;



void             efsd_monitor_init(void);
void             efsd_monitor_cleanup(void);

EfsdMonitorRequest *efsd_monitor_request_new(Ecore_Ipc_Client* client, EfsdFileCmd *cmd);
void                efsd_monitor_request_free(EfsdMonitorRequest *emr);


/* This one frees the monitor and removes it from
   the list of registered monitors.
*/
void             efsd_monitor_remove(EfsdMonitor *m);

/* High-level API for monitoring stuff -- refcounting
   & co are handled inside. Return >= 0 on success.
*/
EfsdMonitor     *efsd_monitor_start(EfsdCommand *cmd, Ecore_Ipc_Client* client, int dir_mode, int sort);
int              efsd_monitor_stop(EfsdCommand *cmd, int client, int dir_mode);

/* For internal monitoring of files -- specify file name directly.
 */
EfsdMonitor     *efsd_monitor_start_internal(char *filename, int dir_mode);
int              efsd_monitor_stop_internal(char *filename, int dir_mode);

/* Looks up file monitor and returns it, NULL if not found.
 */
EfsdMonitor     *efsd_monitored(char *filename, int dir_mode);

/* Check for all monitors if they are requested by CLIENT
   and in that case release those requests.
*/
void             efsd_monitor_cleanup_client(int client);

/* Checks all monitoring requests for a particular monitor
   and removes those requests that are no longer valid.
*/
void             efsd_monitor_cleanup_requests(EfsdMonitor *m);

int              efsd_monitor_send_filechange_event(EfsdMonitor *m, EfsdMonitorRequest *emr,
						    EfsdFilechangeType type, char *filename);

void             efsd_monitor_send_filechange_events(EfsdMonitor *m, EfsdMonitorRequest *emr);

#endif
