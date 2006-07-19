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
evfs_cleanup_file_command(evfs_command * command)
{
   int i;

   for (i = 0; i < command->file_command.num_files; i++)
     {
        free(command->file_command.files[i]->path);
        free(command->file_command.files[i]->plugin_uri);
        free(command->file_command.files[i]);
     }
   free(command->file_command.files);
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
evfs_cleanup_filereference(evfs_filereference * ref)
{
   if (ref->plugin_uri)
      free(ref->plugin_uri);
   if (ref->path)
      free(ref->path);
   if (ref->username)
      free(ref->username);
   if (ref->password)
      free(ref->password);
   free(ref);

}

void
evfs_cleanup_monitor_event(evfs_event * event)
{
   if (event->file_monitor.plugin)
      free(event->file_monitor.plugin);
   if (event->file_monitor.filename)
      free(event->file_monitor.filename);

}

void
evfs_cleanup_file_list_event(evfs_event * event)
{
   evfs_filereference *file;

   if (event->file_list.list)
     {
        ecore_list_goto_first(event->file_list.list);
        while ((file = ecore_list_remove_first(event->file_list.list)))
          {
             evfs_cleanup_filereference(file);
          }
        ecore_list_destroy(event->file_list.list);
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
evfs_cleanup_file_read_event(evfs_event * event)
{
   if (event->data.bytes)
      free(event->data.bytes);
}

void
evfs_cleanup_progress_event(evfs_event * event)
{
   free(event->progress->file_from);
   free(event->progress->file_to);
   free(event->progress);

   evfs_filereference *file;

   if (event->file_list.list)
     {
        ecore_list_goto_first(event->file_list.list);
        while ((file = ecore_list_remove_first(event->file_list.list)))
          {
             evfs_cleanup_filereference(file);
          }
        ecore_list_destroy(event->file_list.list);
     }
}

void
evfs_cleanup_operation_event(evfs_event * event)
{
   /*We don't want to free the operation - the command owns this,
    * unless we're a client*/
   if (evfs_object_client_is_get()) {
	   IF_FREE(event->op->misc_str);
	   free(event->op);
  }
}

void
evfs_cleanup_metadata_event(evfs_event* event)
{
	evfs_meta_obj* obj;
	if (!evfs_object_client_is_get()) {
		Evas_List* l = event->meta->meta_list;
		if (l) {
			for (; l ;) {
				obj = l->data;
		
				free(obj->key);
				free(obj->value);
				free(obj);

				l = l->next;
			}
			evas_list_free(event->meta->meta_list);
		}
	}

	if (evfs_object_client_is_get() && event->meta->meta_hash) {
		Ecore_List* keys;
		char* key;
		keys = ecore_hash_keys(event->meta->meta_hash);
		while ( (key = ecore_list_remove_first(keys))) {
			ecore_hash_remove(event->meta->meta_hash, key);
			free(key);
		}
		ecore_hash_destroy(event->meta->meta_hash);
		ecore_list_destroy(keys);
	}
	free(event->meta);	
}

evfs_cleanup_metadata_groups_event(evfs_event* event) 
{
	Evas_List* l;

	for (l=event->misc.string_list; l;) {
		free(l->data);

		l = l->next;
	}
	evas_list_free(event->misc.string_list);
}

void
evfs_cleanup_event(evfs_event * event)
{
   evfs_cleanup_command(&event->resp_command, EVFS_CLEANUP_PRESERVE_COMMAND);

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
     }

   free(event);

}
