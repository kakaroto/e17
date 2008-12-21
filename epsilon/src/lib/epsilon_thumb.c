#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Ipc.h>
#include <Ecore_File.h>

#include "Epsilon_Request.h"
#include "epsilon_private.h"
#include "Epsilon.h"

#define EPSILON_SOCK "epsilon"

EAPI int EPSILON_EVENT_DONE = -1;

static int debug = 0;

static int epsilon_init_count = 0;
static unsigned long epsilon_mid = 0;

static Ecore_DList *epsilon_request_queue = NULL;

static Ecore_Ipc_Server *epsilon_server = NULL;

static Epsilon_Ipc_End buffer;

static Ecore_Event_Handler *epsilon_server_add = NULL;
static Ecore_Event_Handler *epsilon_server_del = NULL;
static Ecore_Event_Handler *epsilon_server_data = NULL;

static int epsilon_cb_server_add(void *data, int type, void *event);
static int epsilon_cb_server_del(void *data, int type, void *event);
static int epsilon_cb_server_data(void *data, int type, void *event);

/**
 * Initialize epsilon subsystem.
 */
int
epsilon_request_init(void)
{
	/*
	 * Init required subsystems.
	 */
        if (!eina_stringshare_init()) goto init_error;
	if (!ecore_init()) goto init_error;
	if (!ecore_ipc_init()) goto con_init_error;
	if (!epsilon_init()) goto init_error;

	/*
	 * Allocate a list for queueing requests.
	 */
	if (!epsilon_init_count) {
		epsilon_request_queue = ecore_dlist_new();
		if (!epsilon_request_queue) goto queue_error;

		/*
		 * Setup handlers for server events.
		 */
		epsilon_server_add = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, epsilon_cb_server_add, NULL);
		if (!epsilon_server_add) goto handler_add_error;

		epsilon_server_del = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, epsilon_cb_server_del, NULL);
		if (!epsilon_server_del) goto handler_del_error;

		epsilon_server_data = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, epsilon_cb_server_data, NULL);
		if (!epsilon_server_data) goto handler_data_error;

		/*
		 * Create an event id for completed thumbnails.
		 */
		EPSILON_EVENT_DONE = ecore_event_type_new();
		if (!EPSILON_EVENT_DONE) goto event_type_error;

		/*
		 * Init the receive buffer.
		 */
		memset(&buffer, 0, sizeof(Epsilon_Ipc_End));
	}

	return ++epsilon_init_count;

	/*
	 * Error handlers for the various error conditions, fall through to
	 * cleanup earlier work.
	 */
event_type_error:
	ecore_event_handler_del(epsilon_server_data);
handler_data_error:
	ecore_event_handler_del(epsilon_server_del);
handler_del_error:
	ecore_event_handler_del(epsilon_server_add);
handler_add_error:
	ecore_dlist_destroy(epsilon_request_queue);
queue_error:
	ecore_ipc_shutdown();
con_init_error:
	ecore_shutdown();
init_error:
	return 0;
};

/**
 * Shutdown and deallocate epsilon internal variables.
 */
int
epsilon_request_shutdown(void)
{
	if (--epsilon_init_count == 0) {
		ecore_event_handler_del(epsilon_server_data);
		ecore_event_handler_del(epsilon_server_del);
		ecore_event_handler_del(epsilon_server_add);
		ecore_dlist_destroy(epsilon_request_queue);
	}

	ecore_ipc_shutdown();
	ecore_shutdown();

	return epsilon_init_count;
};

static int
epsilon_cb_server_add(void *data, int type, void *event)
{
	data = NULL;
	type = 0;
	event = NULL;

	if (debug) printf("!! client connected to server !!\n");

	return 1;
}

static int
epsilon_cb_server_del(void *data, int type, void *event)
{
	epsilon_server = NULL;
	data = NULL;
	type = 0;
	event = NULL;
	if (debug) printf("!! client disconnected from server !!\n");
	return 1;
}

static void
epsilon_request_free(Epsilon_Request *thumb)
{
	eina_stringshare_del(thumb->path);
        eina_stringshare_del(thumb->dest);
	free(thumb);
}

static void
epsilon_event_free(void *data, void *ev)
{
	Epsilon_Request *thumb = ev;

	data = NULL;

	if (!thumb) {
		fprintf(stderr,"!!Warning!! NULL pointer (*ev) : epsilon_event_free(void *data, void *ev)\n");
		return;
	}
	epsilon_request_free(thumb);
}

static void
epsilon_event_inform_done(Epsilon_Request *thumb)
{
	if (!thumb)
		return;

	thumb->_event = ecore_event_add(EPSILON_EVENT_DONE, thumb,
					epsilon_event_free, NULL);
}

static int
epsilon_request_resolve_thumb_file(Epsilon_Request *thumb)
{
	Epsilon* tb;

	if (!thumb)
		return 0;

	/*
	 * Create a temp thumbnail struct to get the thumbnail
	 * path, don't actually generate the thumbnail here.
	 */
	tb = epsilon_new(thumb->path);
	epsilon_thumb_size(tb, thumb->size);
	if (epsilon_exists(tb) == EPSILON_OK)
          thumb->dest = eina_stringshare_add(epsilon_thumb_file_get(tb));
	epsilon_free(tb);

	return thumb->dest != NULL;
}

static int
epsilon_cb_server_data(void *data, int type, void *event)
{
        Ecore_Ipc_Event_Server_Data *e;
	Epsilon_Message *msg;
	int i = 0;

	data = NULL;
	type = 0;

	e = event;

	if (debug) printf("Receiving data\n");

	if (e->server != epsilon_server)
		return 1;

	if (!(e && e->data))
		return 1;

	msg = e->data;
	Epsilon_Request *thumb;

	if (debug) printf("Received %d response for %d\n", msg->status,msg->mid);

	/*
	 * Find the thumbnail request matching this message response.
	 */

	ecore_dlist_first_goto(epsilon_request_queue);
	while ((thumb = ecore_dlist_current(epsilon_request_queue))) {
		if (thumb->id == msg->mid) {
			if (debug) printf("Removing %d from queue\n", thumb->id);
			thumb->status = msg->status;
			ecore_dlist_remove(epsilon_request_queue);
			break;
		}
		ecore_dlist_next(epsilon_request_queue);
		if (debug) printf("Cycling %d times looking for %d, current is %d\n", i++, msg->mid, thumb->id);
	}

	if (thumb && thumb->status)
		epsilon_request_resolve_thumb_file(thumb);

	epsilon_event_inform_done(thumb);
	if (debug) printf("Jump out\n");

	return 1;
}

/**
 *
 */
static int
epsilon_client_connect(void)
{
	int retries = 0;

#define MAX_RETRY 5

	/*
	 * Use a socket with the hostname appended to help avoid potential
	 * conflicts in NFS systems.
	 */
	if (debug) printf("socket name %s\n", EPSILON_SOCK);

	/*
	 * Connect to an existing server instance if available.
	 */
	while (!epsilon_server && retries++ < MAX_RETRY) {
		epsilon_server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER,
						  EPSILON_SOCK, 0, NULL);
		if (!epsilon_server) {
			pid_t child;

			if ((child = fork()) == 0) {
				if (debug) printf("First child\n");
				if (fork() == 0) {
					if (debug) printf("Second child\n");
					// daemon(0, 0);
					execlp(PACKAGE_BIN_DIR "/epsilon_thumbd", NULL, NULL);
					perror("execlp");
					exit(0);
				}
				else {
					exit(0);
				}
			}

			usleep(100000 * retries);
		}
	}

	if (epsilon_server) {
		if (debug) printf("connect to ipc server: %p\n", epsilon_server);

	}


	return (epsilon_server ? 1 : 0);
}

/**
 * @param path Path to the original image that will be thumbnailed.
 * @param size Enum determining the scale of the thumbnail.
 * @param size Enum determining the format of the thumbnail.
 * @param data Data associated with this thumbnail for client use.
 * @brief Request a thumbnail to be generated asynchronously for the specified
 * @a path.
 */
Epsilon_Request *
epsilon_request_add_advanced(const char *path, Epsilon_Thumb_Size size,
			     Epsilon_Thumb_Format format, void *data)
{
	Epsilon_Request *thumb;

	if (!epsilon_server) {
		if (!epsilon_client_connect()) {
			fprintf(stderr, "Failed to connect to server\n");
			return NULL;
		}
	}

	thumb = calloc(1, sizeof(Epsilon_Request));
	if (!thumb)
          return NULL;

	if (!path) {
             free(thumb);
             return NULL;
	}
	thumb->path = eina_stringshare_add(path);
	thumb->size = size;
	thumb->data = data;
	thumb->format = format;
	if (epsilon_request_resolve_thumb_file(thumb)) {
		thumb->status = 1;
		epsilon_event_inform_done(thumb);
	} else {
		Epsilon_Message *msg;

		msg = epsilon_message_new(epsilon_mid++, path, 0);
		if (msg) {
			msg->thumbsize = size;
			msg->thumbformat = format;
			if (debug) printf("!! requesting thumbnail for %s (request %d)!!, %d\n", path, msg->mid, sizeof(Epsilon_Message)+msg->bufsize);
			if (ecore_ipc_server_send(epsilon_server, 1,1,1,1,1,msg,sizeof(Epsilon_Message)+msg->bufsize)) {
				thumb->id = msg->mid;
				ecore_dlist_append(epsilon_request_queue, thumb);
			}
			free(msg);
		} else {
			epsilon_request_free(thumb);
			return NULL;
		}
	}

	return thumb;
}

/**
 * @param path Path to the original image that will be thumbnailed.
 * @param size Enum determining the scale of the thumbnail.
 * @param data Data associated with this thumbnail for client use.
 * @brief Request a thumbnail to be generated asynchronously for the specified
 * @a path.
 */
Epsilon_Request *
epsilon_request_add(const char *path, Epsilon_Thumb_Size size, void *data)
{
	return epsilon_request_add_advanced(path, size, EPSILON_THUMB_FDO, data);
}

/**
 * @param thumb Thumbnail request to delete.
 * @brief Request a thumbnail request to be cancelled.
 */
void
epsilon_request_del(Epsilon_Request *thumb)
{
	Epsilon_Request *temp;

	if (!thumb)
		return;

	/* was nonexistent and so requested to server */
	if (thumb->id > 0) {
		/*
		 * Find the thumbnail request matching this message response and
		 * remove it, at this point we don't bother cancelling the outstanding
		 * request to the daemon.
		 */
		ecore_dlist_first_goto(epsilon_request_queue);
		while ((temp = ecore_dlist_current(epsilon_request_queue))) {
			if (temp->id == thumb->id) {
				ecore_dlist_remove(epsilon_request_queue);
				break;
			}
			ecore_dlist_next(epsilon_request_queue);
		}
	}

	if (thumb->_event)
		ecore_event_del(thumb->_event);
	else
		epsilon_request_free(thumb);
}
