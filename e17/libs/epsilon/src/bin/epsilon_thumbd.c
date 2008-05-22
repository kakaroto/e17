#include "epsilon_private.h"
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Ipc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <inttypes.h>
#include <netdb.h>
#include <unistd.h>

#include "Epsilon.h"

#define HAVE_NETINET_IN_H

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#elif WIN32
#include <winsock.h>
#endif

#define EPSILON_WORKERS 4
#define EPSILON_WORKER_RECONNECT_ATTEMPTS 5
#define EPSILON_WORKER_RECONNECT_INTERVAL 100000

/*
 * Connected client requesting thumbnailing.
 */
typedef struct _Epsilon_Client Epsilon_Client;
struct _Epsilon_Client
{
	unsigned int        id;
	Ecore_Ipc_Client    *client;
	Ecore_List          *thumbs;

	Epsilon_Ipc_End  ipcend;
};

/*
 * Track the thumbnail to be generated.
 */
/*
typedef struct _Epsilon_Entry Epsilon_Entry;
struct _Epsilon_Entry
{
	Epsilon_Client *client;
	Epsilon_Thumb_Size  size;
	char               *path;
};
*/

/*
 * Tracks worker threads for thumbnailing.
 */
typedef struct _Epsilon_Worker Epsilon_Worker;
struct _Epsilon_Worker
{
	Ecore_List *thumbs;
	pid_t child;
	double runtime;
	Ecore_Ipc_Server *server;
};


/*
 * Track responses to send to clients.
 */
typedef struct _Epsilon_Response Epsilon_Response;
struct _Epsilon_Response
{
	Epsilon_Client *client;
	Epsilon_Message *msg;
};

static unsigned int client_id = 1;
static int debug = 0;
static double idle_time = 0;
static int running_workers = 0;

static int worker_limit = EPSILON_WORKERS;

static Ecore_Event_Handler *client_add = NULL;
static Ecore_Event_Handler *client_del = NULL;
static Ecore_Event_Handler *client_data = NULL;

static Ecore_Event_Handler *worker_add = NULL;
static Ecore_Event_Handler *worker_del = NULL;
static Ecore_Event_Handler *worker_data = NULL;

static Ecore_List *queued_workers;
static Ecore_List *gworkers = NULL;

static Ecore_Ipc_Server *thumb_server = NULL;
static Ecore_Ipc_Server *thumbd_server = NULL;

static Ecore_List *response_queue = NULL;

static char *epsilond_socket_path(char *name);
/* static int epsilond_entry_free(Epsilon_Entry *thumb); */
static int epsilond_client_clean(Epsilon_Client *cl);
static int epsilond_worker_clean(Epsilon_Worker *worker);

int epsilond_cb_worker_add(void *data, int type, void *event);
int epsilond_cb_worker_del(void *data, int type, void *event);
int epsilond_cb_worker_data(void *data, int type, void *event);

void epsilond_init_thumbd_server(Ecore_List* workers) 
{
	char* buf;

	ecore_init();
	
	/*
	 * Retrieve the safe socket name for the server.
	 */
	buf = epsilond_socket_path(EPSILOND_SOCK);

	if (debug) printf("socket name %s\n", buf);
	free(buf);

	/*
	 * Setup the IPC server to handle completed notifications
	 */
	thumbd_server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, EPSILOND_SOCK, 0, NULL);
	if (thumb_server) ecore_ipc_server_client_limit_set(thumbd_server, 50, 0);

	/*
	 * Prepare the handlers for worker IPC events
	 */
	worker_add = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, epsilond_cb_worker_add, workers);
	worker_del = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, epsilond_cb_worker_del, workers);
	worker_data = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, epsilond_cb_worker_data, workers);	
}



void epsilond_shutdown_thumbd_server()
{
	if (debug) printf("Shutting down server (thumbd)\n");
	if (thumbd_server) {
		ecore_ipc_server_del(thumbd_server);
		thumbd_server = NULL;
	}
	
	ecore_event_handler_del(worker_add);
	ecore_event_handler_del(worker_del);
	ecore_event_handler_del(worker_data);
}



/*
 * Handle a client connect and add the new client to the list of clients.
 */
int
epsilond_cb_client_add(void *data, int type, void *event)
{
	Epsilon_Client *cl;
	Ecore_Ipc_Event_Client_Add *e;
	Ecore_List *clients = data;

	e = event;
	type = 0;

	if (ecore_ipc_client_server_get(e->client) != thumb_server) return 1;

	if (debug) printf("!!! client %p connected to server!\n", e->client);
	cl = calloc(1, sizeof(Epsilon_Client));
	if (!cl) return 0;

	/*
	 * Add the client but defer list creation until necessary.
	 */
	cl->id = client_id++;
	cl->client = e->client;
	ecore_list_append(clients, cl);

	if (debug) printf("Added client %d\n", cl->id);

	return 1;
}

/*
 * Handle a worker connect
 */
int
epsilond_cb_worker_add(void *data, int type, void *event)
{
	Epsilon_Client *cl;
	Ecore_Ipc_Event_Client_Add *e;
	Ecore_List *workers = data;

	e = event;
	data = NULL;
	type = 0;

	if (ecore_ipc_client_server_get(e->client) != thumbd_server) return 1;

	if (debug) printf("?? Worker added to %ld ??\n", (long int)getpid());
	cl = calloc(1, sizeof(Epsilon_Client));
	if (!cl) return 0;

	/*
	 * Add the client but defer list creation until necessary.
	 */
	cl->id = client_id++;
	cl->client = e->client;
	ecore_list_append(workers, cl);

	if (debug) printf("Added worker client %d\n", cl->id);

	return 1;
}

int
epsilond_cb_client_del(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Del *e;
	Epsilon_Client *cl;
	Ecore_List *clients = data;

	e = event;
	type = 0;

	if (ecore_ipc_client_server_get(e->client) != thumb_server) return 1;

	if (debug) printf("!!! client %p disconnected from server!\n", e->client);

	/*
	 * Search for the disconnecting client
	 */
	ecore_list_first_goto(clients);
	while ((cl = ecore_list_current(clients))) {
		if (cl->client == e->client) {
			ecore_list_remove(clients);

			/*
			 *  Free thumbnail data associated with this client.
			 */
			epsilond_client_clean(cl);
			free(cl);

			break;
		}
		ecore_list_next(clients);
	}

	ecore_ipc_client_del(e->client);

	return 1;
}

int
epsilond_cb_worker_del(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Del *e;
	Epsilon_Client *cl;
	Ecore_List *workers = data;

	e = event;
	type = 0;

	if (ecore_ipc_client_server_get(e->client) != thumbd_server) return 1;

	if (debug) printf("!!! worker %p disconnected from server!\n", e->client);

	/*
	 * Search for the disconnecting client
	 */
	ecore_list_first_goto(workers);
	while ((cl = ecore_list_current(workers))) {
		if (cl->client == e->client) {
			ecore_list_remove(workers);

			/*
			 *  Free thumbnail data associated with this client.
			 */
			epsilond_client_clean(cl);
			free(cl);

			break;
		}
		ecore_list_next(workers);
	}

	ecore_ipc_client_del(e->client);
	running_workers--;

	return 1;
}

/*
int
epsilon_client_thumb_add(Epsilon_Client *cl, char *path)
{
	Epsilon_Entry *thumb;

	thumb = calloc(1, sizeof(Epsilon_Entry));
	if (thumb) {
		thumb->client = cl;
		thumb->path = strdup(path);
		thumb->size = strlen(path) + 1;

		if (!cl->thumbs)
			cl->thumbs = ecore_list_new();
		if (debug) printf("Queueing %s\n", thumb->path);
		ecore_list_append(cl->thumbs, thumb);
	}

	return 1;
}
*/

int
epsilond_cb_client_data(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Data *e;
	Epsilon_Client *cl;
	Ecore_List *clients = data;

	e = event;
	type = 0;

	if (ecore_ipc_client_server_get(e->client) != thumb_server)  {
		return 1;
	}

	if (debug) printf("!!! client %p sent data\n", e->client);

	/*
	 * Match the client sending the data.
	 */
	ecore_list_first_goto(clients);
	while ((cl = ecore_list_next(clients))) {
		if (cl->client == e->client)
			break;
	}

	if (cl) {
		Epsilon_Message *msg;
		Epsilon_Message *msg2;

		/*
		 * Process all messages in the data buffer for this client and
		 * queue them for processing in the next idle state.
		 */
		msg = e->data;
		msg2 = calloc(1,sizeof(Epsilon_Message)+msg->bufsize); 
		memcpy(msg2,msg,sizeof(Epsilon_Message)+msg->bufsize);
		
		if (debug) printf("** Received %d : %s **\n",msg->mid,  ((char *)msg + sizeof(Epsilon_Message)));
		if (!cl->thumbs)
			cl->thumbs = ecore_list_new();
		msg2->nid = cl->id;
		ecore_list_append(cl->thumbs, msg2);
	}
	else {
		if (debug) printf("ERROR: No matching client for data\n");
	}

	return 1;
}

int
epsilond_cb_worker_data(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Data *e;
	Epsilon_Client *cl;
	Ecore_List *workers;

	e = event;
	workers = data;
	type = 0;

	if (ecore_ipc_client_server_get(e->client) != thumbd_server) return 1;

	if (debug) printf("?? Worker data ??\n");

	/*
	 * Match the client data.
	 */
	ecore_list_first_goto(workers);
	while ((cl = ecore_list_next(workers))) {
		if (cl->client == e->client)
			break;
	}

	if (cl) {
		Epsilon_Message *msg;
		msg = (Epsilon_Message*)e->data;

		if (debug) printf("Onsend to client -> response for %d\n", msg->mid);

		/*
		 * Process all messages in the data buffer for this client.
		 */
		Epsilon_Response *response;

		response = calloc(1, sizeof(Epsilon_Response));
		if (response) {
			response->client = cl;
			response->msg = calloc(1,sizeof(Epsilon_Message)+msg->bufsize);
			memcpy(response->msg, msg, sizeof(Epsilon_Message)+msg->bufsize);
			ecore_list_append(response_queue, response);
		}
	}
	else {
		if (debug) printf("ERROR: No matching client for data\n");
	}

	return 1;
}

static int
epsilond_client_dispatch(Epsilon_Client *cl)
{
	Epsilon_Worker *worker;
	int available = 0;

	/*
	 * Iterate over the thumbnails splitting them between threads.
	 */
	while (ecore_list_count(cl->thumbs) > 0) {
		available = ecore_list_count(queued_workers);

		while ((worker = ecore_list_current(queued_workers))) {

			/*
			 * Don't dispatch work to active workers.
			 */
			if (!worker->child && !ecore_list_count(worker->thumbs)) {
				Epsilon_Message *msg;

				/*
				 * Get the next message on the queue and break
				 * out of the inner loop if NULL.
				 */
				msg = ecore_list_first_remove(cl->thumbs);
				if (!msg)
					break;

				if (debug) printf("Dispatching message %d\n", msg->mid);
				ecore_list_append(worker->thumbs, msg);

				if (debug) printf("Assigning %s to worker %p\n", 
						((char *)msg + sizeof(Epsilon_Message)), worker);
				if (debug) printf("Dispatched message %d\n", msg->mid);

				ecore_list_next(queued_workers);
			}
			else {
				available--;
			}

			if (available < 1)
				break;
		}

		/*
		 * Only start over in the list if we reached the end.
		 */
		if (!worker)
			ecore_list_first_goto(queued_workers);

		if (!available)
			break;
	}

	return available;
}

static int
epsilond_worker_run(void *data)
{
	Epsilon_Worker *worker;
	Epsilon_Message *msg;

	worker = data;
	if (debug) printf("Running worker thread %p for %d thumbnails\n", worker,
			ecore_list_count(worker->thumbs));

	/* FIXME: Do we want the fast exit point here?
	if (!ecore_list_count(worker->thumbs))
		exit(0);
		*/

	while ((msg = ecore_list_first_remove(worker->thumbs))) {
		int status = 0;
		char *path;
		Epsilon *ep;

		path = ((char *)msg + sizeof(Epsilon_Message));
		ep = epsilon_new(path);
		epsilon_thumb_size(ep, msg->thumbsize);
		if (debug) printf("Thumbnailing %d: %s\n", msg->mid, path);

		/*
		 * Check for an existing thumbnail and generate one if it
		 * doesn't exist.
		 */
		if (epsilon_exists(ep) == EPSILON_FAIL) {
			if (epsilon_generate(ep) == EPSILON_FAIL) {
				if (debug) printf("Failed to thumbnail %s\n", path);
			}
			else {
				if (debug) printf("Thumbnail generated\n");
				status = 1;
			}
		}
		else {
			if (debug) printf("Cached thumbnail exists\n");
			status = 1;
		}

		if (debug) printf("Thumbnailing of %s complete\n", path);

		epsilon_free(ep);

		/*
		 * Drop the message contents, the message id is all
		 * the client needs to match the request.
		 */
		msg->bufsize = 0;

		/*
		 * Notify the daemon of the completed thumbnail.
		 */
		msg->status = status;
		if (debug) printf("Dispatching response for %d\n", msg->mid);
		if (ecore_ipc_server_send(worker->server, 1,1,1,1,1,msg,sizeof(Epsilon_Message)+msg->bufsize) < 0) {
			perror("write");
		}
		ecore_ipc_server_flush(worker->server);

		free(msg);
	}
	ecore_ipc_server_flush(worker->server);
	ecore_main_loop_quit();

	return 1;
}

int
epsilond_worker_fork(Epsilon_Worker *worker)
{
	int attempts = 0;

	/*
	 * Begin iteration of the thumb list.
	 */
	ecore_list_first_goto(worker->thumbs);

	worker->runtime = ecore_time_get();
	worker->child = fork();

	/*
	 * Prepare the worker thread to begin thumbnailing in
	 * it's idler.
	 */
	if (!worker->child) {
		ecore_event_handler_del(client_add);
		client_add = NULL;
		ecore_event_handler_del(client_del);
		client_del = NULL;
		ecore_event_handler_del(client_data);
		client_data = NULL;

		ecore_ipc_server_del(thumb_server);
		ecore_ipc_server_del(thumbd_server);

		thumb_server = NULL;
		thumbd_server = NULL;

		/*
		 * Connect back to the parent process.
		 */

		connect:
		worker->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EPSILOND_SOCK, 0, NULL);
		if (worker->server) {
			if (debug) printf("Connecting to %s\n", EPSILOND_SOCK);
		}
		else {
			if (attempts < EPSILON_WORKER_RECONNECT_ATTEMPTS) {
				attempts++;
				usleep(EPSILON_WORKER_RECONNECT_INTERVAL);
				goto connect;
			}
			if (debug) printf("Failed connection to %s\n", EPSILOND_SOCK);
			exit(1);
		}

		epsilond_worker_run(worker);
		return 0;
	}
	else {
		if (debug) printf("Created worker thread %ld\n",
				(long int)worker->child);
	}

	return 1;
}

int
epsilond_idle_enterer(void *data)
{
	int idle=0;
	Epsilon_Client *cl;
	Epsilon_Response *response;
	Epsilon_Worker *worker;
	Ecore_List *clients = data;

	if (debug) printf("Idle state entered\n");

	/*
	 * Send responses for completed thumbnails
	 */
	if (debug) printf("Preparing %d responses\n",
			ecore_list_count(response_queue));
	while ((response = ecore_list_first_remove(response_queue))) {
		Epsilon_Message *msg = response->msg;

		ecore_list_first_goto(clients);
		while ((cl = ecore_list_next(clients))) {
			if (cl->id == msg->nid)
				break;
		}

		if (cl && msg) {
			if (debug) printf("%ld sending response to %d (%d)\n",
					(long int)getpid(), cl->id, msg->mid);

			if (debug) printf("Client %p\n", cl->client);
			if (ecore_ipc_client_send(cl->client, 1,1,1,1,1,msg,sizeof(Epsilon_Message) + msg->bufsize) < 0)
				perror("write");
			free(msg);
		} else {
			if (debug) printf("Client: %p, Msg: %p\n",cl,msg);
		}
		free(response);
	}
	if (debug) printf("Finished responses\n");

	/*
	 * Collect completed worker threads.
	 */
	ecore_list_first_goto(queued_workers);
	while ((worker = ecore_list_next(queued_workers))) {
		int status = 0;

		if (worker->child) {
			if (debug) printf("Collecting child %ld\n", (long int)worker->child);

			/*
			 * Flag the worker available.
			 */
			if ((waitpid(worker->child, &status, WNOHANG) > 0) ||
				       errno == ECHILD)	{

				if (debug) printf("Collected child %ld status %d\n",
						(long int)worker->child,
						status);

				epsilond_worker_clean(worker);
			}
			else {
				if (debug) printf("Moving idle down to %d\n", idle);
				idle--;
			}

			if (debug) printf("Worker %d runtime: %f\n", worker->child, ecore_time_get() - worker->runtime);
		} else {
			if (debug) printf("Worker child pid not set, thumbs is: %d\n", ecore_list_count(worker->thumbs));
		}
	}

	/*
	 * Spread the client work across the various worker processes in the
	 * hope of spreading the workload evenly and avoid stalling any one
	 * particular client longer than others.
	 */
	ecore_list_first_goto(clients);
	while ((cl = ecore_list_next(clients))) {
		int available = ecore_list_count(queued_workers);

		if (cl->thumbs && ecore_list_count(cl->thumbs)) {
			available = epsilond_client_dispatch(cl);
		}

		/*
		 * No worker threads available, so quit trying to do work.
		 */
		if (available < 1) {
			if (debug) printf("No workers available\n");
			break;
		}

	}

	idle = ecore_list_count(queued_workers);

	if (debug) printf("Idle: %d\n", idle);

	/*
	 * Fork off worker threads to begin thumbnailing.
	 */
	ecore_list_first_goto(queued_workers);
	while ((worker = ecore_list_next(queued_workers))) {
		if (!worker->child) {
			if (ecore_list_count(worker->thumbs)) {
				idle--;

				running_workers++;
				
				if (!epsilond_worker_fork(worker)) {
					return 1;
				}
			} else {
				if (debug) printf ("No thumbs to process\n");
			}
		}
		else {
			idle--;
		}
	}
	if (debug) printf("Idle: %d\n", idle);

	/*
	 * FIXME: Detect idle time and exit after a specified interval
	 */
	if (idle == ecore_list_count(queued_workers)) {
		double now = ecore_time_get();
	       	if (!idle_time)
			idle_time = now;
		if (debug) printf("Idle time %g\n", now - idle_time);
	}
	else {
		idle_time = 0;
	}

	return 1;
}

int
epsilond_init()
{
	int wi;
	char *buf;
	Ecore_List *clients = NULL;

	epsilon_init();

	/*
	 * Create the accounting data for the clients and thumbnail queue.
	 */
	clients = ecore_list_new();
	gworkers = ecore_list_new();
	response_queue = ecore_list_new();
	queued_workers = ecore_list_new();

	/*
	 * Retrieve the safe socket name for the clients.
	 */
	buf = epsilond_socket_path(EPSILON_SOCK);

	if (debug) printf("socket name %s\n", buf);

	/*
	 * Setup the IPC server to handle requests
	 */
	thumb_server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, EPSILON_SOCK, 0, NULL);
        if (thumb_server) ecore_ipc_server_client_limit_set(thumb_server, 50, 0);

	free(buf);

	/*
	 * Prepare the handlers for client interaction.
	 */
	client_add = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, epsilond_cb_client_add, clients);
	client_del = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, epsilond_cb_client_del, clients);
	client_data = ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, epsilond_cb_client_data, clients);

	epsilond_init_thumbd_server(gworkers);

	/*
	 * Initialize the worker threads
	 */
	for (wi = 0; wi < worker_limit; wi++) {
		Epsilon_Worker *worker;

		worker = calloc(1, sizeof(Epsilon_Worker));
		worker->thumbs = ecore_list_new();
		ecore_list_append(queued_workers, worker);
	}

	ecore_idle_enterer_add(epsilond_idle_enterer, clients);

	return 1;
}

static char *
epsilond_socket_path(char *name)
{
	char *buf;
	char sockname[MAXHOSTNAMELEN];

	/*
	 * Use a socket with the hostname appended to help avoid potential
	 * conflicts in shared filesystems.
	 */
	gethostname(sockname, MAXHOSTNAMELEN);
	buf = malloc((MAXHOSTNAMELEN + strlen(EPSILON_SOCK) + 1));
	if (buf)
		snprintf(buf, MAXHOSTNAMELEN + strlen(EPSILON_SOCK),
				"%s-%s", name, sockname);

	return buf;
}

/*
static int
epsilond_entry_free(Epsilon_Entry *thumb)
{
	free(thumb->path);
	free(thumb);
	return 1;
}
*/

static int
epsilond_client_clean(Epsilon_Client *cl)
{
	if (cl->thumbs) {
		Epsilon_Message *msg;

		while ((msg = ecore_list_first_remove(cl->thumbs))) {
			free(msg);
		}
		ecore_list_destroy(cl->thumbs);
		cl->thumbs = NULL;
	}

	return 1;
}

static int
epsilond_worker_clean(Epsilon_Worker *worker)
{
	Epsilon_Message *msg;
	worker->child = 0;
	while ((msg = ecore_list_first_remove(worker->thumbs))) 
		free(msg);
	return 1;
}

int main(int argc, const char **argv)
{

	/*
	 * Initialize ecore internals.
	 */
	ecore_init();
	ecore_ipc_init();
	ecore_app_args_set(argc, argv);

	/*
	 * Prepare the app for the main loop.
	 */
	epsilond_init();

	/*
	 * Begin processing requests.
	 */
	ecore_main_loop_begin();

	/*
	 * Request processing complete, cleanup.
	 */
	ecore_ipc_shutdown();
	ecore_shutdown();

	if (debug) printf("Process %ld shut down\n", (long int)getpid());

	return 0;
}
