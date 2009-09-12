#include "epsilon_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int mid = 1;

Epsilon_Message *
epsilon_message_new(int nid, const char *path, int status)
{
	int size, pathlen;
	Epsilon_Message *msg;

	size = sizeof(Epsilon_Message);
	
	if (path)
		pathlen = strlen(path) + 1;
	else
		pathlen = 0;

	size += pathlen;

	msg = malloc(size);
	if (msg) {
		msg->head = EPSILON_MAJOR;
		msg->status = status;
		msg->nid = nid;
		msg->mid = mid++;
		msg->bufsize = size - sizeof(Epsilon_Message);

		if (path) {
			void *body;
			body = ((char *)msg) + sizeof(Epsilon_Message);
			memcpy(body, path, pathlen);
		}
	}

	return msg;
}

int
epsilon_ipc_client_send(Ecore_Con_Client *cl, Epsilon_Message *msg)
{
	return ecore_con_client_send(cl, msg,
			sizeof(Epsilon_Message) + msg->bufsize);
}

int
epsilon_ipc_server_send(Ecore_Con_Server *srv, Epsilon_Message *msg)
{
	return ecore_con_server_send(srv, msg,
			sizeof(Epsilon_Message) + msg->bufsize);
}

/*
 * Pop data off the buffer.
 */
Epsilon_Message *
epsilon_ipc_consume(Epsilon_Ipc_End *end)
{
	int remains;
	Epsilon_Message *msg = NULL;

	remains = end->offset - (int)sizeof(Epsilon_Message);
	if (remains >= 0) {
		int size;
		Epsilon_Message *msg2;

		msg = (Epsilon_Message *)end->buffer;

		/* Calculate the data remaining in the buffer */
		remains = (int)end->offset - (int)sizeof(Epsilon_Message);

		/* Verify we have all the data for this message. */
		if ((int)msg->bufsize > remains)
			return NULL;

		remains -= (int)msg->bufsize;
		msg = (Epsilon_Message *)end->buffer;

		/*
		 * Copy the incoming thumbnail message.
		 */
		size = sizeof(Epsilon_Message) + msg->bufsize;
		msg2 = malloc(size);
		if (msg2)
			memcpy(msg2, msg, size);

		/*
		 * Shift the contents of the buffer for further processing.
		 */
		if (remains > 0) {
			memmove(msg, ((char *)msg + size), remains);
		}
		else {
			free(end->buffer);
			end->buffer = NULL;
			end->bufsize = 0;
		}
		end->offset = remains;

		msg = msg2;
	}

	return msg;
}

void
epsilon_ipc_push(Epsilon_Ipc_End *end, char *data, int size)
{
	/*
	 * Create the buffer if it doesn't exist or reallocate
	 * the buffer if its not large enough.
	 */
	if (!end->buffer) {
		end->buffer = malloc(size);
		if (!end->buffer) return;

		end->bufsize = size;
		end->offset = 0;
	}
	else if (end->bufsize - end->offset < size) {
		end->bufsize = size + end->offset;
		end->buffer = realloc(end->buffer, end->bufsize);
	}

	/*
	 * Copy the new data to the client buffer.
	 */
	memcpy(end->buffer + end->offset, data, size);
	end->offset += size;
}
