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
#ifndef __efsd_meta_monitor_h
#define __efsd_meta_monitor_h

#include <efsd.h>

typedef struct efsd_meta_monitor EfsdMetaMonitor;

/**
 * efsd_meta_monitor_init - Initializer function.
 *
 * This function sets up everything necessary to monitor
 * metadata entries.
 */
void efsd_meta_monitor_init(void);

/**
 * efsd_meta_monitor_add - Adds a client to a metadata monitor.
 * @cmd:        EfsdCommand that contains the request.
 * @client:     Client which requested the monitor.
 *
 * This function established metadata monitoring for a given
 * client. The file and key are stored in the commnd. If no
 * monitor for the file/key combination exists yet, it is
 * created, otherwise the client is simply added to the monitor.
 *
 * Returns TRUE when successful, FALSE otherwise.
 */
int  efsd_meta_monitor_add(EfsdCommand *cmd, Ecore_Ipc_Client* client);


/**
 * efsd_meta_monitor_del - Removes a client to a metadata monitor.
 * @cmd:        EfsdCommand that contains the request.
 * @client:     Client which requested the monitoring stop.
 *
 * This function stops metadata monitoring for a given
 * client and file/key. The file and key are stored in the commnd.
 * If no monitor for the file/key combination exists, the
 * function simply returns, otherwise the client is removed
 * from the monitor.
 *
 * Returns TRUE when successful, FALSE otherwise.
 */
int  efsd_meta_monitor_del(EfsdCommand *cmd, Ecore_Ipc_Client* client);

/**
 * efsd_meta_monitor_notify - Reports changed metadata.
 * @file:       The file whose metadata changed.
 * @key:        The metadata that changed.
 * @type:       The type of the modified data.
 * @data_len:   The length of the new metadata.
 * @data:       The new metadata.
 *
 * This function is called when a metadata entry changed,
 * and causes metadata change events to be sent to all
 * monitoring clients.
 */
void efsd_meta_monitor_notify(char *file, char *key, EfsdDatatype type,
			      int data_len, void *data);

/**
 * efsd_meta_monitor_cleanup_client - Remove all monitors for a client.
 * @client:       The client that no longer exists.
 *
 * This function is called when the connection to a client ends.
 * It cleans up all monitors that this client had still registered.
 */
void efsd_meta_monitor_cleanup_client(Ecore_Ipc_Client* client);

#endif
