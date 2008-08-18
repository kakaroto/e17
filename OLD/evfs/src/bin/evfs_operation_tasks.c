#include "evfs.h"

/*FIXME - this should be dynamic, in proportion to the number
 * of items on the queue.  If queue is empty, we can iterate till done*/
#define TASK_COPY_ITERATIONS 3

int evfs_operation_tasks_file_copy_run(evfs_operation* op,
		evfs_operation_task_file_copy* copy)
{
	char bytes[COPY_BLOCKSIZE];
	int iter=0;
	int total = 0;

	if (!copy->file_from->plugin)
		copy->file_from->plugin = evfs_get_plugin_for_uri(evfs_server_get(),
                              copy->file_from->plugin_uri);

	if (!copy->file_to->plugin)
		copy->file_to->plugin = evfs_get_plugin_for_uri(evfs_server_get(),
                              copy->file_to->plugin_uri);


	/*Check if we're trying to overwrite*/
	if (!(op->status == EVFS_OPERATION_STATUS_OVERRIDE_YES)) {
		
	   if (!(EVFS_OPERATION_TASK(copy)->status == EVFS_OPERATION_TASK_STATUS_EXEC_CONT) &&
	   !(EVFS_OPERATION_TASK(copy)->status == EVFS_OPERATION_TASK_STATUS_CANCEL)) 	{

		if (copy->dest_stat_response != EVFS_ERROR) {
			if (!(op->status == EVFS_OPERATION_STATUS_OVERRIDE_NO)) {
				/*Looks like we're overwriting*/

				evfs_operation_status_set(op, EVFS_OPERATION_STATUS_USER_WAIT);
		 		evfs_operation_wait_type_set(op, EVFS_OPERATION_WAIT_TYPE_FILE_OVERWRITE);
				evfs_operation_user_dispatch(op->client, op->command, op, copy->file_to->path);

				return 0;
			} else {
				/*Overide == no to all - skip*/
				EVFS_OPERATION_TASK(copy)->status = EVFS_OPERATION_TASK_STATUS_CANCEL;
			}
		}
	   }
	}

	/*Check for cancel...*/
	if (EVFS_OPERATION_TASK(copy)->status == EVFS_OPERATION_TASK_STATUS_CANCEL) {
		printf("**** Cancelling copy via user request\n");
		
		return copy->source_stat.st_size;
	}

	if (copy->file_from->fd == 0 && copy->file_from->fd_p == NULL) {
		/*printf("Opening source file...\n");*/
		int fd =(*EVFS_PLUGIN_FILE(copy->file_from->plugin)->functions->evfs_file_open) (op->client, copy->file_from);
		if (!fd) {
			EVFS_OPERATION_TASK(copy)->status = EVFS_OPERATION_TASK_STATUS_ERROR;
			goto ERROR;
		}
	}

	if (copy->file_to->fd == 0 && copy->file_to->fd_p == NULL) {
	     /*printf("Creating destination file..\n");*/
	     if (*EVFS_PLUGIN_FILE(copy->file_to->plugin)->functions->evfs_file_notify_create)
   	         (*EVFS_PLUGIN_FILE(copy->file_to->plugin)->functions->evfs_file_notify_create) (copy->file_from);
             (*EVFS_PLUGIN_FILE(copy->file_to->plugin)->functions->evfs_file_create) (copy->file_to);
	}

	/*printf ("next_byte:size  -> %lld:%lld\n", copy->next_byte, copy->source_stat.st_size);	*/
	if (copy->file_to->fd > 0) {
	   while (iter < TASK_COPY_ITERATIONS && 
		copy->next_byte < copy->source_stat.st_size) {
		int b_read = 0, b_write= 0;

		uint64 read_write_bytes = (copy->source_stat.st_size >
	                      copy->next_byte + COPY_BLOCKSIZE) ? COPY_BLOCKSIZE : 
			(copy->source_stat.st_size - copy->next_byte);


		b_read = (*EVFS_PLUGIN_FILE(copy->file_from->plugin)->functions->evfs_file_read) (op->client,
                                                           copy->file_from, bytes,
                                                           read_write_bytes);


		if (b_read > 0) {
			b_write = (*EVFS_PLUGIN_FILE(copy->file_to->plugin)->functions->evfs_file_write) (copy->file_to,
                                                                     bytes,
                                                                     b_read);

			
			copy->next_byte += b_write;
			total += b_write;
			/*printf("Counter increment to: %lld\n", copy->next_byte);*/
		}

		/*FIXME - do error handling here somehow*/
		if (b_read == 0) {
			printf("File copy error - read 0 bytes\n");
			copy->next_byte = copy->source_stat.st_size;

			EVFS_OPERATION_TASK(copy)->status = EVFS_OPERATION_TASK_STATUS_ERROR;
			goto ERROR;
		}


		iter++;
	  }
	} else {
		printf("Destination File create error\n");
		copy->next_byte = copy->source_stat.st_size;	
		EVFS_OPERATION_TASK(copy)->status = EVFS_OPERATION_TASK_STATUS_ERROR;
		goto ERROR;
	}

	/*printf("Ending task, continuing operation...\n");*/

	/*Check if it's time to end..*/
	if (copy->next_byte == copy->source_stat.st_size) {
	        (*EVFS_PLUGIN_FILE(copy->file_from->plugin)->functions->evfs_file_close) (copy->file_from);
        	(*EVFS_PLUGIN_FILE(copy->file_to->plugin)->functions->evfs_file_close) (copy->file_to);
		
		EVFS_OPERATION_TASK(copy)->status = EVFS_OPERATION_TASK_STATUS_COMMITTED;
	}

	ERROR:
	return total;
	
}

void evfs_operation_tasks_mkdir_run(evfs_operation* op, evfs_operation_task_mkdir* task)
{
	int ret=0;

	if (!task->file->plugin)
		task->file->plugin = evfs_get_plugin_for_uri(evfs_server_get(),
                              task->file->plugin_uri);

        if (task->from && *EVFS_PLUGIN_FILE(task->file->plugin)->functions->evfs_file_notify_create)
  	      (*EVFS_PLUGIN_FILE(task->file->plugin)->functions->evfs_file_notify_create) (task->from);
	
	ret = (*EVFS_PLUGIN_FILE(task->file->plugin)->functions->evfs_file_mkdir) (task->file);

	/*TODO - handle 'fail' state*/
	EVFS_OPERATION_TASK(task)->status = EVFS_OPERATION_TASK_STATUS_COMMITTED;
}

uint64 evfs_operation_tasks_file_remove_run(evfs_operation* op, evfs_operation_task_file_remove* task)
{
	int ret=0;

	if (!task->file->plugin)
		task->file->plugin = evfs_get_plugin_for_uri(evfs_server_get(),
                              task->file->plugin_uri);

	ret = (*EVFS_PLUGIN_FILE(task->file->plugin)->functions->evfs_file_remove) (task->file->path);

	/*TODO - handle 'fail' state*/
	EVFS_OPERATION_TASK(task)->status = EVFS_OPERATION_TASK_STATUS_COMMITTED;

	return task->file_stat.st_size;
}



