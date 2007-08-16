#ifndef __EVFS_IO_H_
#define __EVFS_IO_H_

#include <Eet.h>
#include <Evas.h>

#define EVFS_MESSAGE_CLIENTID 1001
#define EVFS_MESSAGE_KILL 1002

int evfs_io_initialise();

typedef struct ecore_ipc_message ecore_ipc_message;
struct ecore_ipc_message
{

   int major;
   int minor;
   int ref;
   int ref_to;
   int response;
   void *data;
   int len;

   Ecore_Ipc_Client *client;
   Ecore_Ipc_Client *server;
   int dest;                    /*1 = client, 2=server */
};

typedef enum EVFS_IO_TYPE
{
   EVFS_COMMAND,
   EVFS_EVENT
} EVFS_IO_TYPE;

/*typedef enum EVFS_IO_PART_TYPE {
	EVFS_COMMAND_TYPE,
	EVFS_FILE_REFERENCE,
	EVFS_COMMAND_END
} EVFS_IO_PART_TYPE;*/

int evfs_object_client_is_get();
void evfs_object_server_is_set();

Eet_Data_Descriptor *evfs_io_filereference_edd_get();

ecore_ipc_message *ecore_ipc_message_new(int major, int minor, int ref,
                                         int ref_to, int response, void *data,
                                         int len);
evfs_command* evfs_process_incoming_command(evfs_server * server, ecore_ipc_message * message);
void evfs_write_command_end(evfs_connection * conn);
void evfs_write_command(evfs_connection * conn, evfs_command * command);
void evfs_write_command_client(evfs_client * client, evfs_command * command);
void evfs_write_file_command(evfs_connection * conn, evfs_command * command);
void evfs_write_file_command_client(evfs_client * client,
                                    evfs_command * command);
void evfs_event_client_id_notify(evfs_client * client);
void evfs_write_file_command(evfs_connection * conn, evfs_command * command);
void evfs_write_ecore_ipc_client_message(Ecore_Ipc_Client * client,
                                         ecore_ipc_message * msg);
void evfs_write_ecore_ipc_server_message(Ecore_Ipc_Server * server,
                                         ecore_ipc_message * msg);
void evfs_write_event(evfs_client * client, evfs_command * command,
                      EvfsEvent * event);
EvfsEvent* evfs_read_event(ecore_ipc_message * msg);

void
evfs_write_command_end(evfs_connection * conn);
void
evfs_write_command_end_client(evfs_client * client);



#endif
