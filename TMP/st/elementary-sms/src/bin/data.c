#include "common.h"

// this is a really simple data store for messages. it's a directory in ~/ with
// 1 message per file in .txt format

typedef struct _Callback Callback;

struct _Callback
{
   void (*func) (void *data, Data_Message *msg);
   void  *data;
   Eina_Bool delete_me : 1;
};

static const char *msgdir = NULL;
static Eina_List *messages = NULL;
static int add_walking = 0;
static int add_deletes = 0;
static Eina_List *add_callbacks = NULL;
static int del_walking = 0;
static int del_deletes = 0;
static Eina_List *del_callbacks = NULL;

static Callback *
_callback_new(void (*func) (void *data, Data_Message *msg), const void *data)
{
   Callback *cb;
   
   cb = calloc(1, sizeof(Callback));
   if (!cb) return NULL;
   cb->func = func;
   cb->data = (void *)data;
   return cb;
}

static Eina_List *
_callback_del(Eina_List *list, int *walking, int *deletes, void (*func) (void *data, Data_Message *msg), const void *data)
{
   Eina_List *l;
   
   for (l = list; l; l = l->next)
     {
        Callback *cb = l->data;
        if ((cb->func == func) && (cb->data == data))
          {
             if (*walking == 0)
               {
                  free(cb);
                  list = eina_list_remove_list(list, l);
               }
             else
               {
                  cb->delete_me = 1;
                  (*deletes)++;
               }
             return list;
          }
     }
   return list;
}

static Eina_List *
_callback_call(Eina_List *list, int *walking, int *deletes, Data_Message *msg)
{
   Eina_List *l, *pl;
   
   (*walking)++;
   for (l = list; l; l = l->next)
     {
        Callback *cb = l->data;
        if (!cb->delete_me)
          cb->func(cb->data, msg);
     }
   (*walking)--;
   if ((*walking == 0) && (*deletes > 0))
     {
        for (l = list; l;)
          {
             Callback *cb = l->data;
             pl = l;
             l = l->next;
             if (cb->delete_me)
               {
                  free(cb);
                  list = eina_list_remove_list(list, pl);
                  (*deletes)--;
               }
          }
     }
   return list;
}

void
data_init(void)
{
   char buf[PATH_MAX];
   char *home;
   
   home = getenv("HOME");
   if (!home) home = ".";
   snprintf(buf, sizeof(buf), "%s/Messages", home);
   msgdir = eina_stringshare_add(buf);
   ecore_file_mkpath(msgdir);
   snprintf(buf, sizeof(buf), "%s/Inbox", msgdir);
   ecore_file_mkpath(buf);
   snprintf(buf, sizeof(buf), "%s/Sent", msgdir);
   ecore_file_mkpath(buf);
   snprintf(buf, sizeof(buf), "%s/Trash", msgdir);
   ecore_file_mkpath(buf);
   // FIXME: start background scanner to scan Inbox, Sent and Trash
   // FIXME: should later optimise to scan in order of most recent first
}

void
data_shutdown(void)
{
   eina_stringshare_del(msgdir);
   msgdir = NULL;
   // FIXME: free messages, and callbacks
}

void
data_message_sent_store(Data_Message *reply_to, const char *to, const char *message)
{
   // FIXE: alloc msg, add to list and store on disk
}

void
data_message_add_callback_add(void (*func) (void *data, Data_Message *msg), const void *data)
{
   Callback *cb = _callback_new(func, data);
   if (!cb) return;
   add_callbacks = eina_list_append(add_callbacks, cb);
}

void
data_message_add_callback_del(void (*func) (void *data, Data_Message *msg), const void *data)
{
   add_callbacks = _callback_del(add_callbacks, &add_walking, &add_deletes, func, data);
}

void
data_message_del_callback_add(void (*func) (void *data, Data_Message *msg), const void *data)
{
   Callback *cb = _callback_new(func, data);
   if (!cb) return;
   del_callbacks = eina_list_append(del_callbacks, cb);
}

void
data_message_del_callback_del(void (*func) (void *data, Data_Message *msg), const void *data)
{
   del_callbacks = _callback_del(del_callbacks, &del_walking, &del_deletes, func, data);
}

const Eina_List *
data_message_all_list(void)
{
   return messages;
}
           
void
data_message_trash(Data_Message *msg)
{
   _callback_call(del_callbacks, &del_walking, &del_deletes, msg);
   msg->flags |= DATA_MESSAGE_TRASH;
   _callback_call(add_callbacks, &add_walking, &add_deletes, msg);
}

void
data_message_del(Data_Message *msg)
{
   _callback_call(del_callbacks, &del_walking, &del_deletes, msg);
   // FIXME: delete file
   messages = eina_list_remove(messages, msg);
   eina_stringshare_del(msg->from_to);
   eina_stringshare_del(msg->body);
   free(msg);
}

