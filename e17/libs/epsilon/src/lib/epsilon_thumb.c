#include "Epsilon_Request.h"
#include "epsilon_private.h"
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Con.h>
#include <Ecore_File.h>
#include <Epsilon.h>

#define EPSILON_SOCK "epsilon"

EAPI int EPSILON_EVENT_DONE;

static int debug = 0;

static int epsilon_init_count = 0;
static int epsilon_mid = 0;

static Ecore_List *epsilon_request_queue = NULL;

static Ecore_Con_Server *epsilon_server = NULL;

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
epsilon_thumb_init()
{
	/*
	 * Init required subsystems.
	 */
	if (!ecore_init()) goto init_error;
	if (!ecore_con_init()) goto con_init_error;

	/*
	 * Allocate a list for queueing requests.
	 */
	if (!epsilon_init_count) {
		epsilon_request_queue = ecore_list_new();
		if (!epsilon_request_queue) goto queue_error;

		/*
		 * Setup handlers for server events.
		 */
		epsilon_server_add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, epsilon_cb_server_add, NULL);
		if (!epsilon_server_add) goto handler_add_error;

		epsilon_server_del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, epsilon_cb_server_del, NULL);
		if (!epsilon_server_del) goto handler_del_error;

		epsilon_server_data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, epsilon_cb_server_data, NULL);
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

	/*
	 * FIXME: This function should not be void, so we can detect failure
	 * and cleanup.
	 */
	epsilon_init();

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
	ecore_list_destroy(epsilon_request_queue);
queue_error:
	ecore_con_shutdown();
con_init_error:
	ecore_shutdown();
init_error:
	return 0;
};

/**
 * Shutdown and deallocate epsilon internal variables.
 */
int
epsilon_shutdown()
{
	if (--epsilon_init_count == 0) {
		ecore_event_handler_del(epsilon_server_data);
		ecore_event_handler_del(epsilon_server_del);
		ecore_event_handler_del(epsilon_server_add);
		ecore_list_destroy(epsilon_request_queue);
	}

	ecore_con_shutdown();
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
epsilon_event_free(void *data, void *ev)
{
	Epsilon_Request *thumb = ev;

	data = NULL;

	if (thumb->path)
		free(thumb->path);
	if (thumb->dest)
		free(thumb->dest);
	free(thumb);

	return;
}

static int
epsilon_cb_server_data(void *data, int type, void *event)
{
        Ecore_Con_Event_Server_Data *e;
	Epsilon_Message *msg;

	data = NULL;
	type = 0;

	e = event;

	if (debug) printf("Receiving data\n");

	if (e->server != epsilon_server)
		return 1;

	epsilon_ipc_push(&buffer, e->data, e->size);
	while ((msg = epsilon_ipc_consume(&buffer))) {
		Epsilon_Request *thumb;

		if (debug) printf("Received %d response for %s\n", msg->status,
				((char *)msg) + sizeof(Epsilon_Message));

		/*
		 * Find the thumbnail request matching this message response.
		 */
		ecore_dlist_goto_first(epsilon_request_queue);
		while ((thumb = ecore_dlist_current(epsilon_request_queue))) {
			if (thumb->id == msg->mid) {
				ecore_dlist_remove(epsilon_request_queue);
				break;
			}
			ecore_dlist_next(epsilon_request_queue);
		}

		/*If the thumb dest is not set, but the generation was successful,
		 * try to get the dest now */
		if ( (!thumb->dest) && thumb->path && !thumb->status ) {
			Epsilon* tb;
			
			/*
			 * Create a temp thumbnail struct to get the thumbnail
			 * path, don't actually generate the thumbnail here.
			 */
			tb = epsilon_new(thumb->path);
			epsilon_exists(tb);
			thumb->dest = (char *)epsilon_thumb_file_get(tb);
			if (thumb->dest)
				thumb->dest = strdup(thumb->dest);
			epsilon_free(tb);
		}

		ecore_event_add(EPSILON_EVENT_DONE, thumb, epsilon_event_free, NULL);
	}

	return 1;
}

/**
 *
 */
static int
epsilon_client_connect()
{
	int retries = 0;
	char *buf;
	char sockname[MAXHOSTNAMELEN];

#define MAX_RETRY 5

	/*
	 * Use a socket with the hostname appended to help avoid potential
	 * conflicts in NFS systems.
	 */
	gethostname(sockname, MAXHOSTNAMELEN);
	buf = malloc((MAXHOSTNAMELEN + strlen(EPSILON_SOCK) + 1));
	snprintf(buf, MAXHOSTNAMELEN + strlen(EPSILON_SOCK), "%s-%s",
			EPSILON_SOCK, sockname);
	if (debug) printf("socket name %s\n", buf);

	/*
	 * Connect to an existing server instance if available.
	 */
	while (!epsilon_server && retries++ < MAX_RETRY) {
		epsilon_server = ecore_con_server_connect(ECORE_CON_LOCAL_USER,
						  buf, 0, NULL);
		if (!epsilon_server) {
			pid_t child;

			if ((child = fork()) == 0) {
				if (debug) printf("First child\n");
				if (fork() == 0) {
					if (debug) printf("Second child\n");
					// daemon(0, 0);
					execlp(PACKAGE_BIN_DIR "/epsilon_thumbd", NULL);
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

	free(buf);

	if (epsilon_server) {
		if (debug) printf("connect to ipc server: %p\n", epsilon_server);

	}


	return (epsilon_server ? 1 : 0);
}

/**
 * @param path Path to the original image that will be thumbnailed.
 * @param dst Destination for generated thumbnail, NULL causes auto-generation.
 * @param size Enum determining the scale of the thumbnail.
 * @param data Data associated with this thumbnail for client use.
 * @brief Request a thumbnail to be generated asynchronously for the specified
 * @a path.
 */
Epsilon_Request *
epsilon_add(char *path, char *dst, int size, void *data)
{
	Epsilon_Request *thumb;
	Epsilon_Message *msg;

	if (!epsilon_server) {
		if (!epsilon_client_connect()) {
			if (debug) printf("Failed to connect to server\n");
			exit(0);
		}
	}

	thumb = calloc(1, sizeof(Epsilon_Request));
	if (thumb) {
		thumb->path = strdup(path);
		if (dst)
			thumb->dest = strdup(dst);
		else {
			Epsilon *tb;

			/*
			 * Create a temp thumbnail struct to get the thumbnail
			 * path, don't actually generate the thumbnail here.
			 */
			tb = epsilon_new(path);
			epsilon_exists(tb);
			epsilon_thumb_size(tb, size);
			thumb->dest = (char *)epsilon_thumb_file_get(tb);
			if (thumb->dest)
				thumb->dest = strdup(thumb->dest);
			epsilon_free(tb);
		}
		thumb->data = data;

		msg = epsilon_message_new(epsilon_mid++, (char *)path, dst, 0);
		if (msg) {
			msg->thumbsize = size;
			if (debug) printf("!! requesting thumbnail for %s !!\n", path);
			if (epsilon_ipc_server_send(epsilon_server, msg)) {
				thumb->id = msg->mid;
				ecore_dlist_append(epsilon_request_queue, thumb);
			}
		}
		else {
			free(thumb);
			thumb = NULL;
		}
	}

	return thumb;
}
