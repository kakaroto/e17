#ifndef POP_H
#define POP_H

#include <e.h>
#include "e_mod_main.h"

typedef struct _PopClient PopClient;

typedef enum 
{
   POP_STATE_DISCONNECTED,
     POP_STATE_CONNECTED,
     POP_STATE_SERVER_READY,
     POP_STATE_USER_OK,
     POP_STATE_PASS_OK,
     POP_STATE_STATUS_OK
} PopState;

struct _PopClient 
{
   void *data;
   PopState state;
   Config_Box *config;
   Ecore_Con_Server *server;
};

void _mail_pop_check_mail(void *data, void *data2);

#endif
