/*An object created when an evfs command is being processed by a server
 * This object allows us to do magic like send-client-response-and-lock
 * (e.g., during a file copy, send an 'overwrite?' question to the client,
 * wait for a response, and then continue based on the response*/

#ifndef __EVFS_OPERATION_H_
#define __EVFS_OPERATION_H_

typedef enum evfs_operation_status
{
   EVFS_OPERATION_STATUS_USER_WAIT,
   EVFS_OPERATION_STATUS_ERROR,
   EVFS_OPERATION_STATUS_OVERRIDE,
   EVFS_OPERATION_STATUS_NORMAL,
   EVFS_OPERATION_STATUS_COMPLETED
} evfs_operation_status;

typedef enum evfs_operation_response
{
   EVFS_OPERATION_RESPONSE_AFFIRM=0,
   EVFS_OPERATION_RESPONSE_AFFIRM_ALL=1,
   EVFS_OPERATION_RESPONSE_NEGATE=2,
   EVFS_OPERATION_RESPONSE_NEGATE_ALL=3,
   EVFS_OPERATION_RESPONSE_ABORT=4
} evfs_operation_response;

typedef enum evfs_operation_substatus
{
   EVFS_OPERATION_SUBSTATUS_FILE_OVERWRITE
} evfs_operation_substatus;

typedef enum evfs_operation_type
{
	EVFS_OPERATION_TYPE_FILES=0,
} evfs_operation_type;

typedef enum evfs_operation_task_type
{
	EVFS_OPERATION_TASK_TYPE_FILE_COPY,
	EVFS_OPERATION_TASK_TYPE_FILE_REMOVE,
	EVFS_OPERATION_TASK_TYPE_MKDIR
} evfs_operation_task_type;

typedef enum evfs_operation_task_status
{
	EVFS_OPERATION_TASK_STATUS_PENDING,
	EVFS_OPERATION_TASK_STATUS_EXEC,
	EVFS_OPERATION_TASK_STATUS_COMMITTED
} evfs_operation_task_status;


#define EVFS_OPERATION(op) ((evfs_operation*) op)
typedef struct evfs_operation evfs_operation;
struct evfs_operation
{
   evfs_operation_type type;
   evfs_client* client;
   evfs_command* command;
   int processed_tasks;
	
   long id;                     /* A unique id for this op */
   char* misc_str;
   evfs_operation_status status;
   evfs_operation_substatus substatus;
   evfs_operation_response response;

   Ecore_List* sub_task;   /*The tasks that must be performed by this operation pre-completion*/
};



#define EVFS_OPERATION_FILES(op) ((evfs_operation_files*) op)
struct evfs_operation_files {
	evfs_operation op;

	uint64 total_bytes;
	long total_files;

	uint64 progress_bytes;
	long progress_files;
};
typedef struct evfs_operation_files evfs_operation_files;



#define EVFS_OPERATION_TASK(task) ((evfs_operation_task *) task)
struct evfs_operation_task {
	evfs_operation_task_type type;
	evfs_operation_task_status status;

	evfs_command* orig_exec_command;  /*The command that led to the creation of
					    this op*/
};
typedef struct evfs_operation_task evfs_operation_task;

#define EVFS_OPERATION_TASK_FILE_COPY(task) ((evfs_operation_task_file_copy *) task)
struct evfs_operation_task_file_copy {
	evfs_operation_task task;

	struct stat source_stat;
	struct stat dest_stat;

	evfs_filereference* file_from;
	evfs_filereference* file_to;

	uint64 next_byte;
	
};
typedef struct evfs_operation_task_file_copy evfs_operation_task_file_copy;

#define EVFS_OPERATION_TASK_FILE_REMOVE(task) ((evfs_operation_task_file_remove *) task)
struct evfs_operation_task_file_remove {
	evfs_operation_task task;

	struct stat file_stat;
	evfs_filereference* file;
};
typedef struct evfs_operation_task_file_remove evfs_operation_task_file_remove;


#define EVFS_OPERATION_TASK_MKDIR(task) ((evfs_operation_task_mkdir *) task)
struct evfs_operation_task_mkdir {
	evfs_operation_task task;

	evfs_filereference* file;
};
typedef struct evfs_operation_task_mkdir evfs_operation_task_mkdir;



void evfs_operation_initialise();
void evfs_operation_base_init(evfs_operation* op);
evfs_operation_files* evfs_operation_files_new(evfs_client* client, evfs_command* command);
void evfs_operation_destroy(evfs_operation * op);
evfs_operation *evfs_operation_get_by_id(long id);
void evfs_operation_status_set(evfs_operation * op, int status);
void evfs_operation_copy_task_add(evfs_operation* op, evfs_filereference* file_from, 
		evfs_filereference* file_to, struct stat from_stat, struct stat to_stat);
void evfs_operation_mkdir_task_add(evfs_operation* op, evfs_filereference* dir);
void evfs_operation_tasks_print(evfs_operation* op);
void evfs_operation_queue_pending_add(evfs_operation* op);
void evfs_operation_queue_run();
void evfs_operation_run_tasks(evfs_operation* op);

#endif
