/*

Copyright (C) 2000, 2001 Christian Kreibich <kreibich@aciri.org>.

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
#ifndef efsd_fam_h
#define efsd_fam_h

#include <glib.h>
#include <fam.h>

#include <efsd.h>

typedef enum
{
  FULL,
  SIMPLE
}
EfsdFamMonType;

typedef struct efsd_fam_request
{
  int          client;
  EfsdCmdId    id;
}
EfsdFamRequest;


typedef struct efsd_fam_monitor
{
  EfsdFamMonType        type;

  /* filename that is monitored     */
  char                 *filename;

  /* the FAM request                */
  FAMRequest           *fam_req;

  /* use count for this monitor     */
  int                   use_count;

  /* Which clients monitor this file,
     and with what command id.
     list<EfsdFamRequest*>.
  */
  GList                *clients;
}
EfsdFamMonitor;



void             efsd_fam_init(void);
void             efsd_fam_cleanup(void);

/* Allocator and deallocator for a Monitor */
EfsdFamMonitor  *efsd_fam_new_monitor(EfsdCommand *com, int client,
				      EfsdFamMonType type);
void             efsd_fam_free_monitor(EfsdFamMonitor *m);


/* High-level API for monitoring stuff -- refcounting
   & co are handled inside. Return >= 0 on success.
*/
int              efsd_fam_start_monitor(EfsdCommand *cmd, int client);
int              efsd_fam_stop_monitor(EfsdCommand *cmd, int client);

/* Monitor filename briefly to get directory listing events.
 */
void             efsd_fam_force_startstop_monitor(EfsdCommand *cmd, int client);

/* Returns value >0 when file is already monitored.
 */
int              efsd_fam_is_monitored(char *filename);

/* Check for all monitors if they are requested by client i
   and in that case release those requests.
*/
int              efsd_fam_cleanup_client(int client);

/* Get a list of all clients that requested monitoring
   the given file. Not to be freed.
*/
GList           *efsd_fam_get_clients_for_event(FAMEvent *fe);

#endif
