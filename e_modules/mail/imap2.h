/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef IMAP_H
#define IMAP_H

#include <e.h>
#include "e_mod_main.h"

typedef struct _ImapServer ImapServer;
typedef struct _ImapClient ImapClient;

typedef enum 
{
   IMAP_STATE_DISCONNECTED,
     IMAP_STATE_CONNECTED,
     IMAP_STATE_AUTHENTICATED,
     IMAP_STATE_SELECTED
} ImapState;

struct _ImapClient
{
   Config_Box *config;

   Ecore_Con_Server *server;
   int               cmd;
   ImapState         state;
   int               idle;
   unsigned char     idling : 1;
 
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;

   void *data;
};

void _mail_imap_check_mail(void *data);
void _mail_imap_add_mailbox(void *data);
void _mail_imap_del_mailbox(void *data);
void _mail_imap_shutdown();

#endif

//imap.1and1.com
//m39988144-2/3
////testing/2
//
