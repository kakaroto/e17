#ifndef EVOAK_H
#define EVOAK_H

#include "config.h"

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Fb.h>
#include <Ecore_Con.h>
#include <Ecore_Ipc.h>
#include <Ecore_Job.h>
#include <Eet.h>
#include <Edje.h>
#include <Embryo.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <fnmatch.h>
#include <locale.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "evoak_protocol.h"

typedef struct _Evoak_Hash Evoak_Hash;
typedef struct _Evoak_Hash_Node Evoak_Hash_Node;

typedef struct _Evoak_Server Evoak_Server;
typedef struct _Evoak_Client Evoak_Client;
typedef struct _Evoak_Object Evoak_Object;
typedef struct _Evoak_Edje_Callback Evoak_Edje_Callback;

struct _Evoak_Hash
{
   Evas_List *buckets[64];
};

struct _Evoak_Hash_Node
{
   int id;
   void *data;
};

struct _Evoak_Server
{
   double       start_time;
   Ecore_Evas  *ecore_evas;
   Evas        *evas;
   Evas_List   *clients;
};

struct _Evoak_Client
{
   Evoak_Server *server;
   
   Ecore_Ipc_Client *client;
   
   char         *client_name;
   char         *client_class;
   int           request_id;

   Evoak_Hash    hash;
   
   Evas_List    *freeze_q;
   Evas_List    *focused;

   unsigned char authenticated :1;
   unsigned char frozen :1;
};

struct _Evoak_Object
{
   Evoak_Client  *ec;
   Evas_Object   *object;
   int            id;
   int            mask1;
   int            mask2;
   
   struct {
      int x, y, w, h;
   } rel_context;
   
   struct {
      struct {
	 int w, h;
      } min, max;
   } hint;
   
   Evas_List *edje_callbacks;
   
   unsigned char  focused : 1;
};

struct _Evoak_Edje_Callback
{
   int           id;
   Evoak_Object *eo;
};

#endif
