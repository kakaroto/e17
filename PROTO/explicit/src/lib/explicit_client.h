#ifndef EXPLICIT_CLIENT_H_
# define EXPLICIT_CLIENT_H_

#include <Ecore.h>
#include <Ecore_Con.h>

#include "explicit_private.h"

#define EINA_MAGIC_EXPLICIT 0x70042CCE

/* Client side structure */
typedef struct _Explicit_Callback_Object Explicit_Callback_Object;
typedef struct _Explicit_Object Explicit_Object;
typedef struct _Explicit_Object_Request Explicit_Object_Request;
typedef struct _Explicit_Client_Request Explicit_Client_Request;

struct _Explicit
{
   EINA_MAGIC;

   const char *name;

   int next_transaction;

   struct {
      Ecore_Event_Handler *add;
      Ecore_Event_Handler *del;
      Ecore_Event_Handler *data;
   } handler;

   Eina_List *callbacks;
   Eina_Hash *requests_lookup;

   Ecore_Con_Server *server;
   Eet_Connection *conn;

   const char *remote;
   int port;

   Ecore_Idler *idler;

   Eina_Bool connected : 1;
};

struct _Explicit_Callback_Object
{
   Explicit_Callback_Type type;
   Explicit_Callback cb;
   const void *data;
};

struct _Explicit_Object
{
   EINA_MAGIC;

   Explicit *context;
   Evas_Object *object;

   Eina_List *object_requests;

   Eina_Bool single_request;
};

struct _Explicit_Object_Request
{
   Explicit_Object *obj;
   Explicit_Client_Request *client;

   const char *key;
};

struct _Explicit_Client_Request
{
   int id;

   const char *file;

   Eina_List *object_requests;

   Eina_Bool dying : 1;
};

#define EXLICIT_MAGIC_CHECK(Pointer, Value)		\
  if (!EINA_MAGIC_CHECK(Pointer, EINA_MAGIC_EXPLICIT))	\
    {							\
       EINA_MAGIC_FAIL(Pointer, EINA_MAGIC_EXPLICIT);	\
       return Value;					\
    }

/* We expect file to be a stringshare. */
Eina_Bool _explicit_request_file(Explicit *context, const char *file, Explicit_Object_Request *request_object);
Eina_Bool _explicit_request_cancel(Explicit_Object_Request *request);

Eina_Bool _explicit_call_event_cancel(Explicit *context, Evas_Object *obj, const char *file, const char *url, int status);
Eina_Bool _explicit_call_event_approval(Explicit *context, Evas_Object *obj, const char *file, const char *url, Explicit_Target *destination);
/* Before calling done, try if it's a correct file for that type of object. */
Eina_Bool _explicit_call_event_done(Explicit *context, Evas_Object *obj, const char *file, const char *key, size_t downloaded, size_t size, int status);
Eina_Bool _explicit_call_event_progress(Explicit *context, Evas_Object *obj, const char *url, const char *key, size_t downloaded, size_t size);

Eina_Bool _explicit_object_cancel(Explicit_Object_Request *request, int status);
Eina_Bool _explicit_object_done(Explicit_Object_Request *request, size_t downloaded, size_t size, int status);

Eina_Bool _explicit_object_downloaded(Explicit *context, int id, const char *file, size_t downloaded, size_t size, int status);

#endif
