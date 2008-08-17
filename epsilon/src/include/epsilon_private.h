#include <Ecore.h>
#include <Ecore_Con.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <netdb.h>
#include <unistd.h>

#define EPSILON_SOCK "epsilon"
#define EPSILOND_SOCK "epsilond"
#define EPSILON_MAJOR 0xec02e75b
#define EPSILOND_MAJOR 0xec02e75d

/*
 * Communication messages passed around.
 */
typedef struct _Epsilon_Message Epsilon_Message;
struct _Epsilon_Message
{
	unsigned int head;
	unsigned int nid;
	unsigned int mid;
	unsigned int status;
	unsigned int thumbsize;
	unsigned int bufsize;
};

typedef struct Epsilon_Ipc_End Epsilon_Ipc_End;
struct Epsilon_Ipc_End
{
	char    *buffer;
	int      bufsize;
	int      offset;
};

Epsilon_Message *epsilon_message_new(int clientid, const char *path, int status);

int epsilon_ipc_client_send(Ecore_Con_Client *cl, Epsilon_Message *msg);
int epsilon_ipc_server_send(Ecore_Con_Server *cl, Epsilon_Message *msg);

Epsilon_Message *epsilon_ipc_consume(Epsilon_Ipc_End *end);

void epsilon_ipc_push(Epsilon_Ipc_End *end, char *data, int size);
