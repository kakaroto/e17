#ifndef _EVFS_MISC_H_
#define _EVFS_MISC_H_

#ifndef IF_FREE
  #define IF_FREE(obj) if (obj) free(obj)
#endif

typedef struct evfs_command_client
{
   evfs_client *client;
   evfs_command *command;
} evfs_command_client;

/*-----------*/

typedef struct evfs_file_monitor evfs_file_monitor;
struct evfs_file_monitor
{
   evfs_client *client;
   char *monitor_path;

   Ecore_File_Monitor *em;
};

#define EVFS_VFOLDER_ENTRY_META 'M'
#define EVFS_VFOLDER_ENTRY_TAG 'T'
typedef struct {
	char type; /*M = meta, T = tag*/
	char* name;
	char* value;
} EvfsVfolderEntry;

Ecore_List *evfs_file_list_sort(Ecore_List * file_list);

void evfs_disconnect(evfs_connection * connection);
evfs_connection *evfs_connect(void (*callback_func) (EvfsEvent *, void *),
                              void *obj);

evfs_file_uri_path *evfs_parse_uri(char *uri);
EvfsFilereference * evfs_parse_uri_single(char *uri);

int evfs_handle_command(evfs_client * client, evfs_command * command);
void evfs_handle_monitor_start_command(evfs_client * client,
                                       evfs_command * command);
unsigned long evfs_server_get_next_id(evfs_server * serve);
char *EvfsFilereference_to_string(EvfsFilereference * ref);

int EvfsFilereference_equal_is(EvfsFilereference* file1, EvfsFilereference* file2);

EvfsFilereference* evfs_empty_file_get();

long libevfs_next_command_id_get();
evfs_command* evfs_client_command_new();

#endif
