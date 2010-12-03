/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#include "T_Test1.azy_server.h"
#include "T_Test2.azy_server.h"
#ifdef HAVE_MYSQL
#include "T_SQL.azy_server.h"
#endif

int
main(void)
{
   eina_init();
   azy_init();
   Azy_Server_Module_Def *modules[] = {
                                           __TTest1Module_def(),
                                           __TTest2Module_def(),
#ifdef HAVE_MYSQL
                                           __TSQLModule_def(),
#endif
                                           NULL
                                        };

   eina_log_domain_level_set("azy", EINA_LOG_LEVEL_ERR);

//   azy_server_basic_run(4444, AZY_SERVER_TLS | AZY_SERVER_BROADCAST, "server.pem", modules);
   azy_server_basic_run(4444, AZY_SERVER_BROADCAST, NULL, modules);

   azy_shutdown();
   eina_shutdown();

   return 0;
}

