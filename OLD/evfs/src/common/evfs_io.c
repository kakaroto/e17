#include "evfs.h"
#include <Eet.h>
#include <Evas.h>

static int io_init = 0;
static int _evfs_object_client_is = 1;

static Eet_Data_Descriptor *_EvfsOperation_edd;
static Ecore_Hash* evfs_io_edd_hash;

/*Reorg*/
static Eet_Data_Descriptor *_EvfsCommandFile_edd;
static Eet_Data_Descriptor *_EvfsCommand_edd;

static Eet_Data_Descriptor *_EvfsEvent_edd;
static Eet_Data_Descriptor *_EvfsFilereference_edd;
static Eet_Data_Descriptor *_EvfsEventProgress_edd;
static Eet_Data_Descriptor *_EvfsEventDirList_edd;
static Eet_Data_Descriptor *_EvfsEventIdNotify_edd;
static Eet_Data_Descriptor *_EvfsEventData_edd;
static Eet_Data_Descriptor *_EvfsMetaObject_edd;
static Eet_Data_Descriptor *_EvfsEventMetadata_edd;
static Eet_Data_Descriptor *_EvfsEventStat_edd;
static Eet_Data_Descriptor *_EvfsMetadataGroup_edd;
static Eet_Data_Descriptor *_EvfsEventMetadataGroups_edd;
static Eet_Data_Descriptor *_EvfsOperation_edd;
static Eet_Data_Descriptor *_EvfsEventOperation_edd;
static Eet_Data_Descriptor *_EvfsEventAuthRequired_edd;
static Eet_Data_Descriptor *_EvfsEventOpen_edd;
static Eet_Data_Descriptor *_EvfsEventFileMonitor_edd;
static Eet_Data_Descriptor *_EvfsStat_edd;
static Eet_Data_Descriptor *_EvfsEventMime_edd;
static Eet_Data_Descriptor *_EvfsVfolderEntry_edd;
static Eet_Data_Descriptor *_EvfsEventMetaAll_edd;

#define _NEW_EDD(type) eet_data_descriptor_new(#type, sizeof(type), \
                              (void *(*)(void *))eina_list_next, \
                              (void *(*)(void *, void *))eina_list_append, \
                              (void *(*)(void *))eina_list_data_get, \
                              (void *(*)(void *))eina_list_free, \
                              (void (*) \
                               (void *, \
                                int (*)(void *, const char *, void *, void *), \
                                void *))evas_hash_foreach, (void *(*)(void *, \
                                                                      const char \
                                                                      *,  \
                                                                      void *)) \
                              evas_hash_add, (void (*)(void *))evas_hash_free)

#define _EVFS_EVENT_BASE_ADD(EVENTTYPE) \
 \
	EET_DATA_DESCRIPTOR_ADD_BASIC(_ ## EVENTTYPE ## _edd, EVENTTYPE, #EVENTTYPE "_base_type", base.type, EET_T_INT); \
	EET_DATA_DESCRIPTOR_ADD_BASIC(_ ## EVENTTYPE ## _edd, EVENTTYPE, #EVENTTYPE "_base_retCode", base.retCode, EET_T_INT); \
	EET_DATA_DESCRIPTOR_ADD_BASIC(_ ## EVENTTYPE ## _edd, EVENTTYPE, #EVENTTYPE "_base_suffix", base.suffix, EET_T_INT); \
	EET_DATA_DESCRIPTOR_ADD_SUB(_ ## EVENTTYPE ## _edd, EVENTTYPE, #EVENTTYPE "_base_command", base.command, _EvfsCommand_edd); 



ecore_ipc_message* evfs_io_event_construct (EvfsEvent* event);

/*Functions so we know what to clean in various objects client/server side*/
void evfs_object_server_is_set()
{
	_evfs_object_client_is = 0;
}

int evfs_object_client_is_get()
{
	return _evfs_object_client_is;
}
/*-----------------------*/

Eet_Data_Descriptor *
evfs_io_filereference_edd_get()
{
   return _EvfsFilereference_edd;
}

void 
evfs_io_event_edd_set(int type, Eet_Data_Descriptor* desc)
{
	ecore_hash_set(evfs_io_edd_hash, (int*)type,desc);
}

Eet_Data_Descriptor* evfs_io_event_edd_get(EvfsEvent* ev)
{
	return ecore_hash_get(evfs_io_edd_hash, (int*)ev->type);
}

Eet_Data_Descriptor* evfs_io_event_edd_get_type(int type)
{
	return ecore_hash_get(evfs_io_edd_hash, (int*)type);
}

int
evfs_io_initialise()
{
   if (io_init)
      return 1;

   io_init = 1;
   
   evfs_io_edd_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

   /*Stat*/
   _EvfsStat_edd = _NEW_EDD(EvfsStat);
   //EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_dev", st_dev, EET_T_INT);
   //EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_ino", st_ino, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_mode", st_mode, EET_T_INT);
   //EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_nlink", st_nlink, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_uid", st_uid, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_gid", st_gid, EET_T_INT);
   //EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_rdev", st_rdev, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_size", st_size, EET_T_LONG_LONG);
   //EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_blksize", st_blksize, EET_T_INT);
   //EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_blocks", st_blocks, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_atime", ist_atime, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_mtime", ist_mtime, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsStat_edd, EvfsStat, "stat_st_ctime", ist_ctime, EET_T_INT);
   

   /*File_reference eet */
   _EvfsFilereference_edd = _NEW_EDD(EvfsFilereference);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "file_type", file_type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "path", path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "plugin_uri", plugin_uri, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "username", username, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "password", password, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "attach", attach, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsFilereference_edd, EvfsFilereference,
                                 "fd", fd, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsFilereference_edd, EvfsFilereference, "parent", parent, 		   
		   _EvfsFilereference_edd);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsFilereference_edd, EvfsFilereference, "stat", stat, 
		   _EvfsStat_edd);

   /*Evfs_operation eet */
   _EvfsOperation_edd = _NEW_EDD(EvfsOperation);

   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation, "id", id,
                                 EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation,
		                                       "misc_str", misc_str, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation,
		                                       "ret_str_1", misc_str, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation,
		                                       "ret_str_2", misc_str, EET_T_STRING);
   
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation, "status",
                                 status, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation,
                                 "substatus", substatus, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsOperation_edd, evfs_operation,
                                 "response", response, EET_T_INT);

    /*EvfsVfolderEntry_edd*/
    _EvfsVfolderEntry_edd = _NEW_EDD(EvfsVfolderEntry);
    EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsVfolderEntry_edd, EvfsVfolderEntry, "type", type,
                               EET_T_CHAR);
    EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsVfolderEntry_edd, EvfsVfolderEntry, "name", name,
                                   EET_T_STRING);
    EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsVfolderEntry_edd, EvfsVfolderEntry, "value", value,
                                   EET_T_STRING);

				   


   /*Command edd*/
   _EvfsCommandFile_edd = _NEW_EDD(evfs_command_file);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommandFile_edd, evfs_command_file,
                   "type", type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommandFile_edd, evfs_command_file,
                   "extra", extra, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommandFile_edd, evfs_command_file,
                   "ref", ref, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommandFile_edd, evfs_command_file,
                   "ref2", ref2, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_LIST(_EvfsCommandFile_edd, evfs_command_file,
                   "files", files, _EvfsFilereference_edd);
   
   _EvfsCommand_edd = _NEW_EDD(evfs_command);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommand_edd, evfs_command,
                                 "EvfsCommand_type", type, EET_T_INT);   
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsCommand_edd, evfs_command,
                                 "EvfsCommand_filecommand", file_command, _EvfsCommandFile_edd);  
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsCommand_edd, evfs_command,
                                 "EvfsCommand_operation", op, _EvfsOperation_edd);   
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommand_edd, evfs_command,
                                 "EvfsCommand_id", client_identifier, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsCommand_edd, evfs_command,
                                 "EvfsCommand_options", options, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_LIST(_EvfsCommand_edd, evfs_command,
		   		 "EvfsCommand_entries", entries, _EvfsVfolderEntry_edd);

   /*Base event EDD*/
   _EvfsEvent_edd = _NEW_EDD(EvfsEvent);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEvent_edd, EvfsEvent,
                                 "EvfsEvent_type", type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEvent_edd, EvfsEvent,
                                 "EvfsEvent_retCode", retCode, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEvent_edd, EvfsEvent,
                                 "EvfsEvent_suffix", suffix, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEvent_edd, EvfsEvent,
		   		 "EvfsEvent_command", command, _EvfsCommand_edd);

   /*ProgressEventEdd*/
   _EvfsEventProgress_edd = _NEW_EDD(EvfsEventProgress);
   _EVFS_EVENT_BASE_ADD(EvfsEventProgress);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEventProgress_edd, EvfsEventProgress, "EvfsEventProgress_from", from, _EvfsFilereference_edd);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEventProgress_edd, EvfsEventProgress, "EvfsEventProgress_to", to, _EvfsFilereference_edd);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEventProgress_edd, EvfsEventProgress, "EvfsEventProgress_progressAmt", progressAmt, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEventProgress_edd, EvfsEventProgress, "EvfsEventProgress_type", type, EET_T_INT);
   evfs_io_event_edd_set(EVFS_EV_FILE_PROGRESS, _EvfsEventProgress_edd);

   /*DirListEdd*/
   _EvfsEventDirList_edd = _NEW_EDD(EvfsEventDirList);
   _EVFS_EVENT_BASE_ADD(EvfsEventDirList);
   EET_DATA_DESCRIPTOR_ADD_LIST(_EvfsEventDirList_edd, EvfsEventDirList, "EvfsEventDirList_files", files, _EvfsFilereference_edd);
   evfs_io_event_edd_set(EVFS_EV_DIR_LIST, _EvfsEventDirList_edd);

   /*IdNotifyEvent*/
   _EvfsEventIdNotify_edd = _NEW_EDD(EvfsEventIdNotify);
   _EVFS_EVENT_BASE_ADD(EvfsEventIdNotify);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEventIdNotify_edd, EvfsEventIdNotify, "EvfsEventIdNotify_id", id, EET_T_INT);
   evfs_io_event_edd_set(EVFS_EV_NOTIFY_ID, _EvfsEventIdNotify_edd);
  
   /*DataEvent*/
   /*The actual data is encoded in the suffix*/
   _EvfsEventData_edd = _NEW_EDD(EvfsEventData);
   _EVFS_EVENT_BASE_ADD(EvfsEventData);
   evfs_io_event_edd_set(EVFS_EV_FILE_READ, _EvfsEventData_edd);

   /*EvfsMetaDataEvent*/
   _EvfsMetaObject_edd = _NEW_EDD(EvfsMetaObject);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsMetaObject_edd, EvfsMetaObject, "EvfsMetaObject_key", key, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsMetaObject_edd, EvfsMetaObject, "EvfsMetaObject_value", value, EET_T_STRING);
   _EvfsEventMetadata_edd = _NEW_EDD(EvfsEventMetadata);
   _EVFS_EVENT_BASE_ADD(EvfsEventMetadata);
   EET_DATA_DESCRIPTOR_ADD_LIST(_EvfsEventMetadata_edd, EvfsEventMetadata, "EvfsEventMetadata_list", meta_list,
		   _EvfsMetaObject_edd	);
   evfs_io_event_edd_set(EVFS_EV_METADATA, _EvfsEventMetadata_edd);

   /*EvfsEventStat*/
   _EvfsEventStat_edd = _NEW_EDD(EvfsEventStat);
   _EVFS_EVENT_BASE_ADD(EvfsEventStat);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEventStat_edd, EvfsEventStat, "EvfsEventStat_file", file, _EvfsFilereference_edd);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEventStat_edd, EvfsEventStat, "EvfsEventStat_stat", stat, _EvfsStat_edd);
   evfs_io_event_edd_set(EVFS_EV_STAT, _EvfsEventStat_edd); 

   /*EvfsMetadataGroup*/
   _EvfsMetadataGroup_edd = _NEW_EDD(EvfsMetadataGroup);   
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsMetadataGroup_edd, EvfsMetadataGroup, "EvfsMetadataGroup_name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsMetadataGroup_edd, EvfsMetadataGroup, "EvfsMetadataGroup_desc", description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsMetadataGroup_edd, EvfsMetadataGroup, "EvfsMetadataGroup_visualhint", visualhint, EET_T_STRING);
   
   /*EvfsEventMetadataGroups*/
   _EvfsEventMetadataGroups_edd = _NEW_EDD(EvfsEventMetadataGroups); 
   _EVFS_EVENT_BASE_ADD(EvfsEventMetadataGroups);
   EET_DATA_DESCRIPTOR_ADD_LIST(_EvfsEventMetadataGroups_edd, EvfsEventMetadataGroups, "EvfsEventMetadataGroups_list", string_list,
		   _EvfsMetadataGroup_edd	);
   evfs_io_event_edd_set(EVFS_EV_METADATA_GROUPS, _EvfsEventMetadataGroups_edd);

   /*EvfsEventFileMonitor*/
   _EvfsEventFileMonitor_edd = _NEW_EDD(EvfsEventFileMonitor);
   _EVFS_EVENT_BASE_ADD(EvfsEventFileMonitor);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEventFileMonitor_edd, EvfsEventFileMonitor, "EvfsEventFileMonitor_type", type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEventFileMonitor_edd, EvfsEventFileMonitor, "EvfsEventFileMonitor_file", file, _EvfsFilereference_edd);
   evfs_io_event_edd_set(EVFS_EV_FILE_MONITOR, _EvfsEventFileMonitor_edd);
   
   /*EvfsEventAuthFailure*/
   _EvfsEventAuthRequired_edd = _NEW_EDD(EvfsEventAuthRequired);
   _EVFS_EVENT_BASE_ADD(EvfsEventAuthRequired);
   evfs_io_event_edd_set(EVFS_EV_AUTH_REQUIRED, _EvfsEventAuthRequired_edd);

   /*EvfsEventOpen*/
   _EvfsEventOpen_edd = _NEW_EDD(EvfsEventOpen);
   _EVFS_EVENT_BASE_ADD(EvfsEventOpen);
   evfs_io_event_edd_set(EVFS_EV_FILE_OPEN, _EvfsEventOpen_edd);

   /*EvfsEventOperation*/
   _EvfsEventOperation_edd = _NEW_EDD(EvfsEventOperation);
   _EVFS_EVENT_BASE_ADD(EvfsEventOperation);
   EET_DATA_DESCRIPTOR_ADD_SUB(_EvfsEventOperation_edd, EvfsEventOperation, "EvfsEventOperation_operation", operation, _EvfsOperation_edd);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEventOperation_edd, EvfsEventOperation, "EvfsEventOperation_misc", misc, EET_T_STRING);
   evfs_io_event_edd_set(EVFS_EV_OPERATION, _EvfsEventOperation_edd);

   _EvfsEventMime_edd = _NEW_EDD(EvfsEventMime);
   _EVFS_EVENT_BASE_ADD(EvfsEventMime);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_EvfsEventMime_edd, EvfsEventMime, "EvfsEventMime_mime", mime, EET_T_STRING);
   evfs_io_event_edd_set(EVFS_EV_MIME, _EvfsEventMime_edd);

   /*EvfsEventMetaAll*/
   _EvfsEventMetaAll_edd = _NEW_EDD(EvfsEventMetaAll);
   _EVFS_EVENT_BASE_ADD(EvfsEventMetaAll);
   EET_DATA_DESCRIPTOR_ADD_LIST(_EvfsEventMetaAll_edd, EvfsEventMetaAll, "EvfsEventMetaAll_meta", meta, _EvfsMetaObject_edd);
   evfs_io_event_edd_set(EVFS_EV_METAALL, _EvfsEventMetaAll_edd);

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

ecore_ipc_message* evfs_io_event_construct (EvfsEvent* event) {
	Eet_Data_Descriptor* edd;
	char* data;
	int size;
	int ssize=0;

	edd = evfs_io_event_edd_get(event);
	if (!edd) {
		fprintf(stderr, "evfs_io_event_construct: Could not find edd for event: %d\n", event->type);
	} else {
		data = eet_data_descriptor_encode(edd, event, &size);

		/*Semi-hack. If it's a "DATA" event, encode in the suffix*/
		if (event->type == EVFS_EV_FILE_READ) {
			event->suffix = size;
			free(data);
			data=eet_data_descriptor_encode(edd,event,&size);

			data = realloc(data, size + EVFS_EVENT_DATA(event)->size);
			memcpy(data+event->suffix, EVFS_EVENT_DATA(event)->bytes, EVFS_EVENT_DATA(event)->size);

			ssize = size;
			size += EVFS_EVENT_DATA(event)->size;
		}		

		return ecore_ipc_message_new(EVFS_EV_REPLY, event->type, 0,ssize,0,data,size);
	}

	return NULL;	
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
evfs_write_event(evfs_client * client, evfs_command * command,
                 EvfsEvent * event)
{
   event->command = command;

   ecore_ipc_message* msg = evfs_io_event_construct(event);
   if (msg) {
	   evfs_write_ecore_ipc_server_message(client->master,msg);
   } else {
	   printf("Could not find message to write\n");
   }
}

EvfsEvent*
evfs_read_event(ecore_ipc_message * msg)
{
   Eet_Data_Descriptor* edd;
   EvfsEvent* ev;
   
   /*Event type is stored in minor ref*/
   edd = evfs_io_event_edd_get_type(msg->minor);

   if (edd) {
	  int edd_data_size=msg->len;
	  if (msg->ref_to > 0) edd_data_size = msg->ref_to;

          ev = eet_data_descriptor_decode(edd, msg->data, edd_data_size);
	  if (ev->suffix) {
		  EVFS_EVENT_DATA(ev)->size = msg->len-ev->suffix;
		  EVFS_EVENT_DATA(ev)->bytes = calloc(EVFS_EVENT_DATA(ev)->size,1);		  

		  /*fprintf(stderr, "Copying %d bytes into place from offset %d\n",EVFS_EVENT_DATA(ev)->size, ev->suffix );*/
		  memcpy(EVFS_EVENT_DATA(ev)->bytes, msg->data+ev->suffix, EVFS_EVENT_DATA(ev)->size);
	  }
	  return ev;
   } else {
	   fprintf(stderr,"evfs_read_event: Could not find edd for incoming event: %d\n", msg->minor);
   }
	
   return NULL;
}

/*Writers*/
void
evfs_write_ecore_ipc_server_message(Ecore_Ipc_Server * server,
                                    ecore_ipc_message * msg)
{

   ecore_ipc_server_send(server, msg->major, msg->minor, msg->ref, msg->ref_to,
                         msg->response, msg->data, msg->len);
   /*printf("Sent %d, %d, %d, %d, %d, - data %p, size %d\n", msg->major, msg->minor, msg->ref, msg->ref_to,
                         msg->response, msg->data, msg->len);*/
   free(msg->data);
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
evfs_write_command(evfs_connection * conn, evfs_command * command)
{
   char* data;
   int size;

   data = eet_data_descriptor_encode(_EvfsCommand_edd, command, &size);
      evfs_write_ecore_ipc_server_message(conn->server,
                                       ecore_ipc_message_new(EVFS_COMMAND,
                                                             0,
                                                             0, 0, 0, data,
                                                             size));

}

/*----------------------------*/

/*Readers*/
evfs_command*
evfs_process_incoming_command(evfs_server * server, 
                              ecore_ipc_message * message)
{
	evfs_command* com;

	com = eet_data_descriptor_decode(_EvfsCommand_edd, message->data, message->len);

	/*Set local references to plugins, etc*/
	evfs_command_localise(com);

	return com;
}

/*----------------------------*/
