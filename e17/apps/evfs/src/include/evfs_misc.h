#ifndef _EVFS_MISC_H_
#define _EVFS_MISC_H_

typedef struct evfs_command_client
{
   evfs_client *client;
   evfs_command *command;
} evfs_command_client;

/*-----------*/

/*This structure needs more development*/
typedef struct evfs_auth_cache
{
   char *path;
   char *username;
   char *password;
} evfs_auth_cache;

typedef struct evfs_file_monitor evfs_file_monitor;
struct evfs_file_monitor
{
   evfs_client *client;
   char *monitor_path;

   Ecore_File_Monitor *em;
};

Ecore_List *evfs_file_list_sort(Ecore_List * file_list);

void evfs_cleanup_client(evfs_client * client);
void evfs_disconnect(evfs_connection * connection);
evfs_connection *evfs_connect(void (*callback_func) (evfs_event *, void *),
                              void *obj);
evfs_file_uri_path *evfs_parse_uri(char *uri);
int evfs_handle_command(evfs_client * client, evfs_command * command);
void evfs_handle_monitor_start_command(evfs_client * client,
                                       evfs_command * command);
unsigned long evfs_server_get_next_id(evfs_server * serve);
char *evfs_filereference_to_string(evfs_filereference * ref);

int evfs_filereference_equal_is(evfs_filereference* file1, evfs_filereference* file2);

evfs_filereference* evfs_empty_file_get();

#endif
