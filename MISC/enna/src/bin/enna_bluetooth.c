/*
 * enna_bluetooth.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_bluetooth.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_bluetooth.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "enna.h"

#if WITH_BLUETOOTH_SUPPORT

#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <pthread.h>
#include "enna_util.h"
#include "enna_config.h"
#include "enna_bluetooth.h"
#include <gconf/gconf-client.h>

static int          enna_bluetooth_scan(void *data);

static GConfClient *client;
static int          fd_read;
static int          fd_write;
static Ecore_Fd_Handler *fd_handler;
static pthread_t    thread;

EAPI void
enna_bluetooth_init()
{
   gconf_init(0, NULL, NULL);

   ecore_timer_add(30.0, enna_bluetooth_scan, NULL);
   g_type_init();
   client = gconf_client_get_default();
   gconf_client_add_dir(client,
			"/system/pulseaudio/modules",
			GCONF_CLIENT_PRELOAD_NONE, NULL);
   enna_bluetooth_scan(NULL);

}

static void        *
_bluetooth_scan_thread(void *data)
{
   void               *buf[1];
   int                 dev_id;
   int                 max_rsp;
   int                 num_rsp;
   int                 length;
   int                 flags;
   int                 sock;
   inquiry_info       *info = NULL;
   char                addr[19] = { 0 };
   char                name[248] = { 0 };
   int                 i;
   gboolean            found = FALSE;

   dev_id = hci_get_route(NULL);
   sock = hci_open_dev(dev_id);
   if (dev_id < 0 || sock < 0)
     {
	dbg("Error : Unable to open Socket\n");
	return NULL;
     }

   length = 8;			/* 10.24 seconds */
   max_rsp = 2;
   flags = IREQ_CACHE_FLUSH;
   info = (inquiry_info *) malloc(max_rsp * sizeof(inquiry_info));

   num_rsp = hci_inquiry(dev_id, length, max_rsp, NULL, &info, flags);
   if (num_rsp < 0)
      dbg("Error : hci_inquiry\n");

   for (i = 0; i < num_rsp; i++)
     {
	ba2str(&(info + i)->bdaddr, addr);
	memset(name, 0, sizeof(name));
	if (hci_read_remote_name
	    (sock, &(info + i)->bdaddr, sizeof(name), name, 0) < 0)
	   strcpy(name, "[unknown]");
	dbg("%s %s\n", addr, name);
	if (!strcmp
	    (addr,
	     enna_config_get_conf_value_or_default("bluetooth", "device_addr",
						   "")))
	  {
	     dbg("device found : %s %s!!!\n", name, addr);
	     found = TRUE;
	     break;
	  }
     }

   free(info);
   close(sock);

   if (found)
      buf[0] = "found";
   else
      buf[0] = "not found";
   write(fd_write, buf, sizeof(buf));

   return NULL;
}

static int
_pipe_read_cb(void *data, Ecore_Fd_Handler * fdh)
{
   int                 fd;
   void               *buf[1];
   int                 len;
   char               *found_str;
   int                 found = 0;

   pthread_join(thread, NULL);
   fd = ecore_main_fd_handler_fd_get(fdh);
   while ((len = read(fd, buf, sizeof(buf))) > 0)
     {
	if (len == sizeof(buf))
	  {
	     found_str = strdup(buf[0]);
	     printf("Trouvé ? : %s\n", found_str);
	     if (!strcmp("found", found_str))
		found = 1;
	     else
		found = 0;
	  }
     }

   gconf_client_set_bool(client, "/system/pulseaudio/modules/rtp-recv/enabled",
			 found, NULL);

   return 0;
}

static int
enna_bluetooth_scan(void *data)
{

   int                 fds[2];

   if (pipe(fds) == 0)
     {
	fd_read = fds[0];
	fd_write = fds[1];
	fcntl(fd_read, F_SETFL, O_NONBLOCK);
	fd_handler = ecore_main_fd_handler_add(fd_read,
					       ECORE_FD_READ,
					       _pipe_read_cb, NULL, NULL, NULL);
	ecore_main_fd_handler_active_set(fd_handler, ECORE_FD_READ);
	pthread_create(&thread, NULL, _bluetooth_scan_thread, NULL);
	return 1;
     }
   return 1;
}

#endif
