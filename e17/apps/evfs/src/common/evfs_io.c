#include "evfs.h"
#include <Eet.h>
#include <Evas.h>

static int io_init = 0;
static Eet_Data_Descriptor *_evfs_filereference_edd;
static Eet_Data_Descriptor *_evfs_progress_event_edd;
static Eet_Data_Descriptor *_evfs_operation_edd;
static Eet_Data_Descriptor *_evfs_filemonitor_edd;

Eet_Data_Descriptor *
evfs_io_filereference_edd_get()
{
   return _evfs_filereference_edd;
}

int
evfs_io_initialise()
{
   if (io_init)
      return 1;

   io_init = 1;

   /*File_reference eet */
   _evfs_filereference_edd =
      eet_data_descriptor_new("evfs_filereference", sizeof(evfs_filereference),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filereference_edd, evfs_filereference,
                                 "file_type", file_type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filereference_edd, evfs_filereference,
                                 "path", path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filereference_edd, evfs_filereference,
                                 "plugin_uri", plugin_uri, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filereference_edd, evfs_filereference,
                                 "fd", fd, EET_T_INT);

   /*Progress event eet */
   _evfs_progress_event_edd =
      eet_data_descriptor_new("evfs_progress_event",
                              sizeof(evfs_event_progress),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_progress_event_edd, evfs_event_progress,
                                 "progress", file_progress, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_progress_event_edd, evfs_event_progress,
                                 "file_from", file_from, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_progress_event_edd, evfs_event_progress,
                                 "file_to", file_to, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_progress_event_edd, evfs_event_progress,
                                 "type", type, EET_T_INT);

   /*Evfs_operation eet */
   _evfs_operation_edd =
      eet_data_descriptor_new("evfs_operation", sizeof(evfs_operation),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_operation_edd, evfs_operation, "id", id,
                                 EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_operation_edd, evfs_operation,
		                                       "misc_str", misc_str, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_operation_edd, evfs_operation, "status",
                                 status, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_operation_edd, evfs_operation,
                                 "substatus", substatus, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_operation_edd, evfs_operation,
                                 "response", response, EET_T_INT);


   /*File monitor edd*/
   _evfs_filemonitor_edd =
      eet_data_descriptor_new("evfs_filemonitor", sizeof(evfs_event_file_monitor),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filemonitor_edd, evfs_event_file_monitor, "fileev_type", fileev_type,
                                 EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filemonitor_edd, evfs_event_file_monitor,
		                                       "plugin", plugin, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filemonitor_edd, evfs_event_file_monitor,
		                                       "filename", filename, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filemonitor_edd, evfs_event_file_monitor,
		                                       "filename_len", filename_len, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_evfs_filemonitor_edd, evfs_event_file_monitor,
		                                       "filetype", filetype, EET_T_INT);
   

   
   return 0;

}

ecore_ipc_message *
ecore_ipc_message_new(int major, int minor, int ref, int ref_to, int response,
                      void *data, int len)
{
   ecore_ipc_message *msg = malloc(sizeof(ecore_ipc_message));

   msg->major = major;
   msg->minor = minor;
   msg->ref = ref;
   msg->ref_to = ref_to;
   msg->response = response;
   msg->data = data;
   msg->len = len;
   msg->client = NULL;
   msg->server = NULL;
   msg->dest = 0;

   return msg;
}

void
evfs_event_client_id_notify(evfs_client * client)
{

   /*printf("Notifying client of id %ld\n", client->id); */
   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_NOTIFY_ID,
                                                             0, 0, 0, 0,
                                                             &client->id,
                                                             sizeof(long)));
}

void
evfs_write_event_file_monitor(evfs_client * client, evfs_event * event)
{
   int size_ret = 0;
   char *data;

	
   /*Write "From" file */
   data = eet_data_descriptor_encode(_evfs_filemonitor_edd, &event->file_monitor, &size_ret);

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_FILE_MONITOR,
                                                             client->id, 0, 0,
                                                             data, size_ret));

}

void
evfs_write_stat_event(evfs_client * client, evfs_event * event)
{
   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_STAT_SIZE,
                                                             client->id, 0, 0,
                                                             &event->stat.
                                                             stat_obj,
                                                             sizeof
                                                             (evfs_stat)));

}

void
evfs_write_list_event(evfs_client * client, evfs_event * event)
{
   evfs_filereference *ref;
   char *data;
   int size_ret = 0;

   //char block[1024]; /*Maybe too small, make this dynamic*/

   ecore_list_goto_first(event->file_list.list);
   while ((ref = ecore_list_next(event->file_list.list)))
     {
        /*memcpy(block, &ref->file_type, sizeof(evfs_file_type));
         * memcpy(block+sizeof(evfs_file_type), ref->path, strlen(ref->path)+1); */

        //printf("Encoding filename: %d %s '%s'\n", ref->file_type, ref->plugin_uri, ref->path);        
        data =
           eet_data_descriptor_encode(_evfs_filereference_edd, ref, &size_ret);

        /*printf ("Writing filename '%s' with filetype %d\n", ref->path, ref->file_type); */
        evfs_write_ecore_ipc_client_message(client->client,
                                            ecore_ipc_message_new(EVFS_EV_REPLY,
                                                                  EVFS_EV_PART_FILE_REFERENCE,
                                                                  client->id, 0,
                                                                  0, data,
                                                                  size_ret));

        free(data);

     }

}

void
evfs_write_file_read_event(evfs_client * client, evfs_event * event)
{
   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_DATA,
                                                             client->id, 0, 0,
                                                             event->data.bytes,
                                                             event->data.size));
}

void
evfs_write_progress_event(evfs_client * client, evfs_command * command,
                          evfs_event * event)
{
   int size_ret = 0;
   evfs_filereference *ref;
   char *data;

   if (event->progress->type == EVFS_PROGRESS_TYPE_DONE)
     {
        printf("Sendign Done!\n");
     }

   data =
      eet_data_descriptor_encode(_evfs_progress_event_edd, event->progress,
                                 &size_ret);

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_PROGRESS,
                                                             client->id, 0, 0,
                                                             data, size_ret));

   free(data);

   /*Write "From" file */
   ref = ecore_list_remove_first(event->file_list.list);
   data = eet_data_descriptor_encode(_evfs_filereference_edd, ref, &size_ret);

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_FILE_REFERENCE,
                                                             client->id, 0, 0,
                                                             data, size_ret));
   free(data);

   /*Write "to" file */
   ref = ecore_list_remove_first(event->file_list.list);
   data = eet_data_descriptor_encode(_evfs_filereference_edd, ref, &size_ret);

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_FILE_REFERENCE,
                                                             client->id, 0, 0,
                                                             data, size_ret));
   free(data);

}

void
evfs_write_operation_event(evfs_client * client, evfs_event * event)
{
   int size_ret = 0;

   char *data =
      eet_data_descriptor_encode(_evfs_operation_edd, event->op, &size_ret);

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_OPERATION,
                                                             client->id, 0, 0,
                                                             data, size_ret));

   free(data);
}

void
evfs_write_event(evfs_client * client, evfs_command * command,
                 evfs_event * event)
{
   //printf("Sending event type '%d'\n", event->type);
   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_TYPE,
                                                             client->id, 0, 0,
                                                             &event->type,
                                                             sizeof
                                                             (evfs_eventtype)));

   /*Now write the source command, if any */
   if (command)
     {
        evfs_write_command_client(client, command);
     }

   switch (event->type)
     {
     case EVFS_EV_FILE_MONITOR:
        evfs_write_event_file_monitor(client, event);
        break;
     case EVFS_EV_STAT:
        evfs_write_stat_event(client, event);
        break;
     case EVFS_EV_DIR_LIST:
        evfs_write_list_event(client, event);
        break;
     case EVFS_EV_FILE_PROGRESS:
        evfs_write_progress_event(client, command, event);
        break;

     case EVFS_EV_FILE_OPEN:
        printf("Open event send\n");
        break;                  /*File open has no additional info - fd is in filereference */

     case EVFS_EV_FILE_READ:
        evfs_write_file_read_event(client, event);
        break;

     case EVFS_EV_OPERATION:
        evfs_write_operation_event(client, event);
        break;

     default:
        printf("Event type not handled in switch\n");
        break;
     }

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_EV_REPLY,
                                                             EVFS_EV_PART_END,
                                                             client->id, 0, 0,
                                                             NULL, 0));

}

int
evfs_read_event(evfs_event * event, ecore_ipc_message * msg)
{

   switch (msg->minor)
     {
     case EVFS_EV_PART_TYPE:

        memcpy(&event->type, msg->data, sizeof(evfs_eventtype));
        break;
     case EVFS_EV_PART_FILE_MONITOR: {
	evfs_event_file_monitor* fmev =
		eet_data_descriptor_decode(_evfs_filemonitor_edd, msg->data,
                             msg->len); 				     
        memcpy(&event->file_monitor, fmev,
               sizeof(evfs_event_file_monitor));

	free(fmev);
     }
     break;

     case EVFS_EV_PART_STAT_SIZE:

        memcpy(&event->stat.stat_obj, msg->data, sizeof(evfs_stat));

        break;

     case EVFS_EV_PART_PROGRESS:
        {
           evfs_event_progress *pg =
              eet_data_descriptor_decode(_evfs_progress_event_edd, msg->data,
                                         msg->len);
           event->progress = pg;

        }
        break;

     case EVFS_EV_PART_OPERATION:
        {
           evfs_operation *op =
              eet_data_descriptor_decode(_evfs_operation_edd, msg->data,
                                         msg->len);
           event->op = op;

        }
        break;

     case EVFS_EV_PART_DATA:
        {
           event->data.size = msg->len;
           event->data.bytes = malloc(msg->len);
           memcpy(event->data.bytes, msg->data, msg->len);

        }
        break;

     case EVFS_EV_PART_FILE_REFERENCE:
        {

           evfs_filereference *ref;

           if (!event->file_list.list)
             {
                event->file_list.list = ecore_list_new();
                //printf("Created new ecore list at %p\n", event->file_list.list);
             }

           ref =
              eet_data_descriptor_decode(_evfs_filereference_edd, msg->data,
                                         msg->len);

           if (ref)
             {
                ecore_list_append(event->file_list.list, ref);
             }
           else
             {
                printf("Error decoding eet!\n");
             }
        }
        break;

        /*The pieces of the incoming command */
     case EVFS_COMMAND_TYPE:
     case EVFS_COMMAND_EXTRA:
     case EVFS_FILE_REFERENCE:
     case EVFS_FILE_REFERENCE_FD:
     case EVFS_COMMAND_END:
        evfs_process_incoming_command(NULL, &event->resp_command, msg);
        break;

     case EVFS_EV_PART_END:
        //printf("Created new ecore list at %p\n", event->file_list.list);
        return TRUE;
        break;
     default:
        printf("Unknown event part received! - %d\n", msg->minor);
        break;
     }

   return FALSE;

}

/*Writers*/
void
evfs_write_ecore_ipc_server_message(Ecore_Ipc_Server * server,
                                    ecore_ipc_message * msg)
{

   ecore_ipc_server_send(server, msg->major, msg->minor, msg->ref, msg->ref_to,
                         msg->response, msg->data, msg->len);
   free(msg);

}

void
evfs_write_ecore_ipc_client_message(Ecore_Ipc_Client * client,
                                    ecore_ipc_message * msg)
{

   ecore_ipc_client_send(client, msg->major, msg->minor, msg->ref, msg->ref_to,
                         msg->response, msg->data, msg->len);
   free(msg);
}

/*------------------------------------------------------------------------*/
//Some ugly duplication here - maybe we should consider reworking this so it can
//be generic
//
void
evfs_write_operation_command(evfs_connection * conn, evfs_command * command)
{
   int size_ret = 0;

   char *data =
      eet_data_descriptor_encode(_evfs_operation_edd, command->op, &size_ret);

   evfs_write_ecore_ipc_server_message(conn->server,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_PART_OPERATION,
                                                             0, 0, 0, data,
                                                             size_ret));
   free(data);
}

void
evfs_write_command(evfs_connection * conn, evfs_command * command)
{

   /*Write the command type structure */
   evfs_write_ecore_ipc_server_message(conn->server,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_TYPE,
                                                             0, 0, 0,
                                                             &command->type,
                                                             sizeof
                                                             (evfs_command_type)));

   evfs_write_ecore_ipc_server_message(conn->server,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_EXTRA,
                                                             0, 0, 0,
                                                             &command->
                                                             file_command.extra,
                                                             sizeof(int)));

   switch (command->type)
     {
     case EVFS_CMD_STOPMON_FILE:
     case EVFS_CMD_STARTMON_FILE:
     case EVFS_CMD_REMOVE_FILE:
     case EVFS_CMD_RENAME_FILE:
     case EVFS_CMD_FILE_STAT:
     case EVFS_CMD_LIST_DIR:
     case EVFS_CMD_FILE_TEST:
     case EVFS_CMD_FILE_COPY:
     case EVFS_CMD_FILE_OPEN:
     case EVFS_CMD_FILE_READ:
     case EVFS_CMD_DIRECTORY_CREATE:
     case EVFS_CMD_PING:
        evfs_write_file_command(conn, command);
        break;
     case EVFS_CMD_OPERATION_RESPONSE:
        evfs_write_operation_command(conn, command);
        break;
     default:
        printf("Command type not handled in switch\n");
        break;
     }

   /*Send a final */
   evfs_write_command_end(conn);

}

void
evfs_write_command_client(evfs_client * client, evfs_command * command)
{
   switch (command->type)
     {
     case EVFS_CMD_STOPMON_FILE:
     case EVFS_CMD_STARTMON_FILE:
     case EVFS_CMD_REMOVE_FILE:
     case EVFS_CMD_RENAME_FILE:
     case EVFS_CMD_FILE_STAT:
     case EVFS_CMD_LIST_DIR:
     case EVFS_CMD_FILE_TEST:
     case EVFS_CMD_FILE_COPY:
     case EVFS_CMD_FILE_OPEN:
     case EVFS_CMD_FILE_READ:
     case EVFS_CMD_DIRECTORY_CREATE:
     case EVFS_CMD_PING:
        evfs_write_file_command_client(client, command);
        break;
     default:
        printf("Command type not handled in switch (client) : %d\n", command->type);
        break;

     }
}

void
evfs_write_command_end(evfs_connection * conn)
{
   evfs_write_ecore_ipc_server_message(conn->server,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_END,
                                                             0, 0, 0, NULL, 0));
}

void
evfs_write_command_end_client(evfs_client * client)
{
   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_END,
                                                             client->id, 0, 0,
                                                             NULL, 0));
}

void
evfs_write_file_command(evfs_connection * conn, evfs_command * command)
{
   int i;
   char uri[1024];
   char *part;
   int it;
   Ecore_List *uri_part;

   bzero(uri, 1024);

   /*Write the files */
   /*Send them de-parsed to save time */
   for (i = 0; i < command->file_command.num_files; i++)
     {
        evfs_filereference *ref = command->file_command.files[i];

        uri_part = ecore_dlist_new();

        it = 0;
        do
          {
             if (it)
               {
                  ref = ref->parent;
               }

             if (ref->username)
               {
                  snprintf(uri, 1024, "%s://%s:%s@%s",
                           ref->plugin_uri,
                           ref->username, ref->password, ref->path);
               }
             else
               {
                  snprintf(uri, 1024, "%s://%s", ref->plugin_uri, ref->path);
               }
             ecore_dlist_append(uri_part, strdup(uri));
             //printf("Appended URI '%s'\n", uri);

             it++;
          }
        while (ref->parent);

        it = 0;
        bzero(uri, 1024);

        ecore_dlist_goto_last(uri_part);
        while ((part = ecore_dlist_previous(uri_part)))
          {
             if (it)
                strcat(uri, "#");
             strcat(uri, part);

             free(part);
             it++;
          }
        ecore_dlist_destroy(uri_part);

        //printf("RE-Parsed URI: '%s'\n", uri);

        evfs_write_ecore_ipc_server_message(conn->server,
                                            ecore_ipc_message_new(EVFS_COMMAND,
                                                                  EVFS_FILE_REFERENCE,
                                                                  0, 0, 0, uri,
                                                                  sizeof(uri)));

        evfs_write_ecore_ipc_server_message(conn->server,
                                            ecore_ipc_message_new(EVFS_COMMAND,
                                                                  EVFS_FILE_REFERENCE_FD,
                                                                  0, 0, 0,
                                                                  &ref->fd,
                                                                  sizeof(int)));

     }

}

void
evfs_write_file_command_client(evfs_client * client, evfs_command * command)
{
   int i;
   char uri[1024];
   char *part;
   int it;
   Ecore_List *uri_part;

   bzero(uri, 1024);

   /*Write the command type structure */
   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_TYPE,
                                                             client->id, 0, 0,
                                                             &command->type,
                                                             sizeof
                                                             (evfs_command_type)));

   evfs_write_ecore_ipc_client_message(client->client,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             EVFS_COMMAND_EXTRA,
                                                             client->id, 0, 0,
                                                             &command->
                                                             file_command.extra,
                                                             sizeof(int)));

   /*Write the files */
   /*Send them de-parsed to save time */
   for (i = 0; i < command->file_command.num_files; i++)
     {
        evfs_filereference *ref = command->file_command.files[i];

        uri_part = ecore_dlist_new();

        it = 0;
        do
          {
             if (it)
               {
                  ref = ref->parent;
               }

             if (ref->username)
               {
                  snprintf(uri, 1024, "%s://%s:%s@%s",
                           ref->plugin_uri,
                           ref->username, ref->password, ref->path);
               }
             else
               {
                  snprintf(uri, 1024, "%s://%s", ref->plugin_uri, ref->path);
               }
             ecore_dlist_append(uri_part, strdup(uri));
             //printf("Appended URI '%s'\n", uri);

             it++;
          }
        while (ref->parent);

        it = 0;
        bzero(uri, 1024);

        ecore_dlist_goto_last(uri_part);
        while ((part = ecore_dlist_previous(uri_part)))
          {
             if (it)
                strcat(uri, "#");
             strcat(uri, part);

             free(part);
             it++;
          }
        ecore_dlist_destroy(uri_part);

        evfs_write_ecore_ipc_client_message(client->client,
                                            ecore_ipc_message_new(EVFS_COMMAND,
                                                                  EVFS_FILE_REFERENCE,
                                                                  client->id, 0,
                                                                  0, uri,
                                                                  sizeof(uri) +
                                                                  1));

        evfs_write_ecore_ipc_client_message(client->client,
                                            ecore_ipc_message_new(EVFS_COMMAND,
                                                                  EVFS_FILE_REFERENCE_FD,
                                                                  client->id, 0,
                                                                  0, &ref->fd,
                                                                  sizeof(int)));

     }

   /*Send a final */
   evfs_write_command_end_client(client);

}

/*----------------------------*/

/*Readers*/
int
evfs_process_incoming_command(evfs_server * server, evfs_command * command,
                              ecore_ipc_message * message)
{
   evfs_filereference *ref;

   switch (message->minor)
     {
     case EVFS_COMMAND_TYPE:
        memcpy(&command->type, message->data, sizeof(evfs_command_type));
        break;

     case EVFS_COMMAND_EXTRA:
        memcpy(&command->file_command.extra, message->data, sizeof(int));
        break;
     case EVFS_FILE_REFERENCE:
        {
           //printf("Parsing URI: '%s'\n", message->data);                   
           evfs_file_uri_path *path = evfs_parse_uri(message->data);

           if (command->file_command.num_files == 0)
             {

                /*If we have a server ref, assign this ref to the files, so they
                 * know where they came from.  We'd do this in evfs_parse_uri,
                 * but that func can also be called from the client*/
                if (server)
                  {
                     ref = path->files[0];
                     do
                       {
                          ref->server = server;
                       }
                     while ((ref = ref->parent));
                  }

                command->file_command.num_files = 1;
                command->file_command.files =
                   malloc(sizeof(evfs_filereference *));
                command->file_command.files[0] = path->files[0];

             }
           else
             {

                //printf("we already have %d files\n", command->file_command.num_files);
                /*TODO Handle multiple files */

                command->file_command.files =
                   realloc(command->file_command.files,
                           sizeof(evfs_filereference *) *
                           (command->file_command.num_files + 1));
                command->file_command.files[command->file_command.num_files] =
                   path->files[0];
                command->file_command.num_files++;
             }

           evfs_cleanup_file_uri_path(path);

        }
        break;

     case EVFS_FILE_REFERENCE_USERNAME:
        if (command->file_command.num_files)
          {
             command->file_command.files[command->file_command.num_files -
                                         1]->username = strdup(message->data);
             printf("Received username: '%s'\n",
                    command->file_command.files[command->file_command.
                                                num_files - 1]->username);
          }
        else
          {
             printf("BAD: Received a username before a filerefereence!\n");
          }
        break;

     case EVFS_FILE_REFERENCE_PASSWORD:
        if (command->file_command.num_files)
          {
             command->file_command.files[command->file_command.num_files -
                                         1]->password = strdup(message->data);
             printf("Received password: '%s'\n",
                    command->file_command.files[command->file_command.
                                                num_files - 1]->password);
          }
        else
          {
             printf("BAD: Received a password before a filerefereence!\n");
          }
        break;

     case EVFS_FILE_REFERENCE_FD:
        if (command->file_command.num_files)
          {
             command->file_command.files[command->file_command.num_files -
                                         1]->fd = *(int *)message->data;
          }
        else
          {
             printf("BAD: Received an FD before a filerefereence!\n");
          }
        break;

     case EVFS_COMMAND_PART_OPERATION:
        {
           evfs_operation *op =
              eet_data_descriptor_decode(_evfs_operation_edd, message->data,
                                         message->len);
           command->op = op;
        }
        break;

     case EVFS_COMMAND_END:
        /*TODO cleanp ref event */

        return TRUE;
        break;

     default:
        printf("Unknown incoming command part\n");
        break;
     }

   return FALSE;
}

/*----------------------------*/
