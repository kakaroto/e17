/*An object created when an evfs command is being processed by a server
 * This object allows us to do magic like send-client-response-and-lock
 * (e.g., during a file copy, send an 'overwrite?' question to the client,
 * wait for a response, and then continue based on the response*/

#ifndef __EVFS_OPERATION_H_
#define __EVFS_OPERATION_H_

typedef enum evfs_operation_status {
	EVFS_OPERATION_STATUS_USER_WAIT,
	EVFS_OPERATION_STATUS_ERROR,
	EVFS_OPERATION_STATUS_OVERRIDE,
	EVFS_OPERATION_STATUS_NORMAL
} evfs_operation_status;

typedef enum evfs_operation_response {
	EVFS_OPERATION_RESPONSE_AFFIRM,
	EVFS_OPERATION_RESPONSE_NEGATE
} evfs_operation_response;

typedef enum evfs_operation_substatus {
	EVFS_OPERATION_SUBSTATUS_FILE_OVERWRITE
} evfs_operation_substatus;

typedef struct evfs_operation evfs_operation;
struct evfs_operation {
	long id; /* A unique id for this op*/
	evfs_operation_status status;
	evfs_operation_substatus substatus;
	evfs_operation_response response;
};

void evfs_operation_initialise();
evfs_operation* evfs_operation_new();
void evfs_operation_destroy(evfs_operation* op);
evfs_operation* evfs_operation_get_by_id(long id);
void evfs_operation_status_set(evfs_operation* op, int status);
void evfs_operation_user_dispatch(evfs_client* client, evfs_command* command, evfs_operation* op);

#endif
