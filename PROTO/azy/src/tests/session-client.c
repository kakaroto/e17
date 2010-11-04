/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

/*
 * Copyright 2006-2008 Ondrej Jirman <ondrej.jirman@zonio.net>
 *
 * This file is part of libxr.
 *
 * Libxr is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option) any
 * later version.
 *
 * Libxr is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libxr.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>

#include "TTest1.azy_client.h"
#include "TTest2.azy_client.h"

/* this function prints client error if any and resets error so that futher calls to client funcs work */

static char *uri;

static int
_check_err(azy_content *err)
{
   if (!err)
     return 1;

   if (!azy_content_error_is_set(err))
     return 0;

   printf("** ERROR **: %s\n", azy_content_error_message_get(err));
   azy_content_error_reset(err);
   return 1;
}

Eina_Bool
connect_call_disconnect(int id)
{
   azy_content *err = azy_content_new(NULL);

   /* create object for performing client connections */
   azy_client *conn = azy_client_new();

   if (!conn)
     return EINA_TRUE;

   /* connect to the servlet on the server specified by uri */

   if (!azy_client_open(conn, uri))
     {
        azy_client_free(conn);
        return EINA_TRUE;
     }

   const char *session_id = eina_stringshare_printf("%d", id);
   azy_client_net_header_set(conn, "X-SESSION-ID", session_id);
   azy_client_net_header_set(conn, "X-SESSION-USE", "1");
   eina_stringshare_del(session_id);

   azy_client_transport_set(conn, AZY_CALL_JSON_RPC);

   TTest2_auth(conn, "a", "b", err);
   _check_err(err);
   err = NULL;
   TTest2_getUsername(conn, err);
   _check_err(err);
   err = NULL;

   azy_client_free(conn);

   return EINA_FALSE;
}

int
main(int   ac,
     char *av[])
{
   Ecore_Thread *t[100];
   int i;

   azy_init();
   uri = ac == 2 ? av[1] : "https://localhost:4444/RPC2";

   for (i = 0; i < 100; i++)
     t[i] = ecore_thread_run((Ecore_Thread_Blocking_Cb)connect_call_disconnect, NULL, NULL, &i);

   ecore_main_loop_begin();

   azy_fini();

   return 0;
}

