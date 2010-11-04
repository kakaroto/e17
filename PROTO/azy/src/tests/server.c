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

#include "TTest1.azy_server.h"
#include "TTest2.azy_server.h"

int
main(void)
{
   eina_init();
   azy_init();
   Azy_Server_Module_Def *modules[3] = {
                                           __TTest1Module_def(),
                                           __TTest2Module_def(),
                                           NULL
                                        };

   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);

   azy_server_basic_run(4444, AZY_SERVER_TLS | AZY_SERVER_BROADCAST, "server.pem", modules);

   azy_shutdown();
   eina_shutdown();

   return 0;
}

