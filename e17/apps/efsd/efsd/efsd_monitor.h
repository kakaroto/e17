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
#include <efsd_list.h>


#define EFSD_CLIENT_INTERNAL -1

typedef struct efsd_monitor_request
{
  int                   client;
  EfsdCmdId             id;

  int                   num_options;
  EfsdOption           *options;
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

  /* Which clients monitor this file,
     and with what command id.
     list<EfsdFamRequest*>.
  */
  EfsdList             *clients;

  /* whether this monitor can safely
     be deleted, because there won't
     be any further FAM events */
  char                   is_finished;

  /* whether this monitor monitors an
     entire directory (checking the file
     won't do -- we could be monitoring
     only the directory file) */
  char                   is_dir;

  /* whether this monitor is registered
     as a "full" monitor or just as a
     temporary one. */
  
  char                   is_temporary;
}
EfsdMonitor;



void             efsd_monitor_init(void);
void             efsd_monitor_cleanup(void);

/* This one frees the monitor and removes it from
   the list of registered monitors.
*/
void             efsd_monitor_remove(EfsdMonitor *m);

/* High-level API for monitoring stuff -- refcounting
   & co are handled inside. Return >= 0 on success.
*/
EfsdMonitor     *efsd_monitor_start(EfsdCommand *com, int client, int dir_mode);
int              efsd_monitor_stop(EfsdCommand *cmd, int client);

/* For internal monitoring of files -- specify file name directly.
 */
EfsdMonitor     *efsd_monitor_start_internal(char *filename, int dir_mode);
int              efsd_monitor_stop_internal(char *filename);

/* Monitor filename briefly to get directory listing events.
 */
int              efsd_monitor_force_startstop(EfsdCommand *cmd, int client);

/* Returns value >0 when file is already monitored.
 */
EfsdMonitor     *efsd_monitored(char *filename);

/* Check for all monitors if they are requested by CLIENT
   and in that case release those requests.
*/
int              efsd_monitor_cleanup_client(int client);

#endif
