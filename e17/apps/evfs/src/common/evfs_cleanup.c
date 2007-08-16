#include "evfs.h"

void
evfs_cleanup_command(evfs_command * command, int free_command)
{

   switch (command->type)
     {
     case EVFS_CMD_STARTMON_FILE:
     default:
        evfs_cleanup_file_command(command);
        break;
     }

   if (free_command == EVFS_CLEANUP_FREE_COMMAND)
      free(command);
}

void 
evfs_cleanup_file_command_only(evfs_command* command)
{
	if (command->file_command->files)
		free(command->file_command->files);

	free(command);
}

void
evfs_cleanup_file_command(evfs_command * command)
{
   int i;
   Evas_List* l;

   if (command->file_command->files) {
	   for (l = command->file_command->files; l; ){
		evfs_cleanup_filereference((EvfsFilereference*)l->data);
		l=l->next;
	   }
	   evas_list_free(command->file_command->files); 
   }
}

void
evfs_cleanup_file_monitor(evfs_file_monitor * mon)
{
   if (mon->monitor_path)
      free(mon->monitor_path);
   free(mon);
}

/*----------------------------------*/
void
evfs_cleanup_filereference(EvfsFilereference * ref)
{
   if (ref->plugin_uri)
      free(ref->plugin_uri);
   if (ref->path)
      free(ref->path);
   if (ref->username)
      free(ref->username);
   if (ref->password)
      free(ref->password);

   /*Do we assume this is just a string?*/
   if (ref->attach)
      free(ref->attach);
   free(ref);

}

void
evfs_cleanup_monitor_event(EvfsEventFileMonitor *event)
{
   if (event->file)
	evfs_cleanup_filereference(event->file);
}

void
evfs_cleanup_file_list_event(EvfsEventDirList *event)
{
   EvfsFilereference *file;
   Evas_List* l;

   if (event->files)
     {
	     for (l = event->files;l;) {
		     evfs_cleanup_filereference(l->data);
		     l = l->next;
	     }
	     evas_list_free(event->files);
     }
}

void
evfs_cleanup_file_uri_path(evfs_file_uri_path * path)
{
   if (path)
     {
        if (path->files)
           free(path->files);
        free(path);
     }

}

void
evfs_cleanup_file_read_event(EvfsEventData* event)
{
   if (event->bytes)
      free(event->bytes);
}

void
evfs_cleanup_progress_event(EvfsEventProgress* event)
{
   if (event->from) evfs_cleanup_filereference(event->from);
   if (event->to) evfs_cleanup_filereference(event->to);
   
}

void
evfs_cleanup_operation_event(EvfsEventOperation * event)
{
   /*We don't want to free the operation - the command owns this,
    * unless we're a client*/
   if (evfs_object_client_is_get()) {
	   IF_FREE(event->operation->misc_str);
	   free(event->operation);
  }
}

void evfs_cleanup_metadata_object(EvfsMetaObject* obj)
{
	free(obj->key);
	free(obj->value);
	free(obj);
}

void
evfs_cleanup_metadata_event(EvfsEventMetadata* event)
{
	if (!evfs_object_client_is_get()) {
		Evas_List* l = event->meta_list;
		if (l) {
			for (; l ;) {
				evfs_cleanup_metadata_object(l->data);
				
				l = l->next;
			}
			evas_list_free(event->meta_list);
		}
	}
}

void
evfs_cleanup_metadata_groups_event(EvfsEventMetadataGroups* event) 
{
	Evas_List* l;

	EvfsMetadataGroup* g;
	for (l=event->string_list; l;) {
		g = l->data;
		evfs_metadata_group_header_free(g);
		l = l->next;
	}
	evas_list_free(event->string_list);
	
}

void
evfs_cleanup_event(EvfsEvent* event)
{
   /*evfs_cleanup_command(event->command, EVFS_CLEANUP_PRESERVE_COMMAND);*/

   switch (event->type)
     {
     case EVFS_EV_FILE_MONITOR:
        evfs_cleanup_monitor_event(event);
        break;
     case EVFS_EV_DIR_LIST:
        evfs_cleanup_file_list_event(event);
        break;
     case EVFS_EV_FILE_READ:
        evfs_cleanup_file_read_event(event);
        break;
     case EVFS_EV_FILE_PROGRESS:
        evfs_cleanup_progress_event(event);
        break;
     case EVFS_EV_OPERATION:
        evfs_cleanup_operation_event(event);
        break;
     case EVFS_EV_METADATA:
	evfs_cleanup_metadata_event(event);
	break;

     case EVFS_EV_METADATA_GROUPS:
	evfs_cleanup_metadata_groups_event(event);
	break;
     default:
	break;
     }

   free(event);

}
