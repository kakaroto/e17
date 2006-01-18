/*An object created when an evfs command is being processed by a server
 * This object allows us to do magic like send-client-response-and-lock
 * (e.g., during a file copy, send an 'overwrite?' question to the client,
 * wait for a response, and then continue based on the response*/

#ifndef __EVFS_OPERATION_H_
#define __EVDS_OPERATION_H_

typedef struct evfs_operation evfs_operation;
struct evfs_operation {
	int id; /* A unique id for this op*/
};




#endif
