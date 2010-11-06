/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
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

   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_INFO);

   azy_server_basic_run(4444, AZY_SERVER_TLS | AZY_SERVER_BROADCAST, "server.pem", modules);

   azy_shutdown();
   eina_shutdown();

   return 0;
}

