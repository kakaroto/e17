
/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_ECORE
#include <Ecore.h>
#include <Ecore_Ipc.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <Edb.h>

#ifdef __EMX__
#include <strings.h>
#include <sys/select.h>
#define getcwd _getcwd2
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_io.h>
#include <efsd_fam_r.h>
#include <efsd_monitor.h>
#include <efsd_commands.h>
#include <efsd_filetype.h>
#include <efsd_list.h>
#include <efsd_lock.h>
#include <efsd_macros.h>
#include <efsd_main.h>
#include <efsd_meta.h>
#include <efsd_meta_monitor.h>
#include <efsd_misc.h>
#include <efsd_event_queue.h>
#include <efsd_types.h>
#include <efsd_statcache.h>

/* The connection to FAM */
FAMConnection        famcon;


/* Command line options: */
char                 opt_foreground = FALSE;
char                 opt_careful    = FALSE;
char                 opt_debug      = FALSE;
char                 opt_nesting    = FALSE;

EfsdQueue           *ev_q;

/* File desciptors for connected clients */
static int           clientfd[EFSD_CLIENTS];


/* File descriptor for accepting new clients */
static int           listen_fd;
static char          app_cmd[MAXPATHLEN];
static pid_t         app_pid;

#if HAVE_ECORE
Ecore_Hash* partial_command_hash; /*The has we use to keep references to commands we're in the process of deserializing*/
#endif

#if USE_THREADS

#define EFSD_MAX_THREADS 50

pthread_mutex_t threadcount_mutex = PTHREAD_MUTEX_INITIALIZER;
int             threadcount = 0;
#endif

/* We need a struct to hold all arguments
   needed when launching a thread, because
   we can only pass a void pointer:
*/
typedef struct efsd_command_client_container
{
  EfsdCommand *ecmd;

  #if HAVE_ECORE
  Ecore_Ipc_Client* client;
  #else
  int          client;
  #endif
  
  char         threaded; /* whether the contained command should
			    be run in a thread or not */
}
EfsdCommandClientContainer;

/* Pthread wrappers to minimize #ifdef clutter */
#if HAVE_ECORE
static void   main_thread_launch(EfsdCommand *ecom, Ecore_Ipc_Client* client);
#else
static void   main_thread_launch(EfsdCommand *ecom, int client);
#endif



static void   main_thread_detach(void);

static void  *main_handle_client_command(void *container);
static void  *main_fam_events_listener(void *dummy);
static void   main_handle_fam_events(void);
static void   main_handle_connections(void);
static void   main_crash_sighandler(int signal);
static void   main_cleanup_sighandler(int signal);
static void   main_reload_user_sighandler(int signal);
static void   main_reload_sys_sighandler(int signal);
static void   main_cleanup(void);
static void   main_initialize(char *appname);
static void   main_daemonize(void);
static void   main_check_permissions(void);
static void   main_check_options(int argc, char**argv);

int queue_process_idler(void*);

extern Ecore_List* process_queue;
extern pthread_mutex_t queue_mutex;
extern int msg_count=0;

static Ecore_Ipc_Client* cl;

/* *********************************** */
/* IPC Functions */
int queue_process_idler(void* data) {
	ecore_ipc_message* msg = NULL;

	
	if (msg_count > 0) {
		
	LOCK(&queue_mutex);

	ecore_list_goto_first(process_queue);
	msg = ecore_list_next(process_queue);

	if (msg) {
		ecore_ipc_client_send(msg->client, msg->major, msg->minor, msg->ref, msg->ref_to, msg->response,msg->data, msg->len);
		free(msg);

		ecore_list_remove_first(process_queue);
		msg_count--;
	}

	UNLOCK(&queue_mutex);

	}


	return 1;
}




int
ipc_client_add(void *data, int type, void *event)
{
      Ecore_Ipc_Event_Client_Add *e;
	                                                                                                            
      e = (Ecore_Ipc_Event_Client_Add *) event;
      printf("ERR: EFSD Client Connected!!!\n");
      return (1);
}

int
ipc_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
                                                                                                         
   e = (Ecore_Ipc_Event_Client_Del *) event;
    printf("ERR: Client Disconnected!!!\n");
   return (1);
}

int
ipc_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;
   EfsdCommand* ec;
                                                                                                         
   printf ("ERR: Got message!\n");
   if ((e = (Ecore_Ipc_Event_Client_Data *) event))
   {
	   ecore_ipc_message* msg= malloc(sizeof(ecore_ipc_message));
	   msg->major =e->major;
	   msg->minor = e->minor;
	   msg->ref = e->ref;
	   msg->ref_to = e->ref_to;
	   msg->response =e->response;
	   msg->data = e->data;
	   msg->len = e->size;

	   if (msg->major == 1) {


	   	ec = ecore_hash_get(partial_command_hash, e->client);
	   	if (!ec) {
	   		ec = NEW(EfsdCommand);
			ec->efsd_file_cmd.num_files = 0;	
			ec->efsd_file_cmd.num_options = 0;
			ecore_hash_set(partial_command_hash, e->client, ec);
	   	}

				   
	   	if (e->minor != 100) {
	   		deserialize_command(msg,ec);
	   	} else {
	   		printf("Command finished..processing..\n");
			ec = ecore_hash_get(partial_command_hash, e->client);
			main_thread_launch(ec, e->client);
	   	}
	   }
	   free(msg);
			   
			   
   }

   return 1;
}
                                                                                                         
                                                                                                         




/* *********************************** */


#if HAVE_ECORE
static void  
main_thread_launch(EfsdCommand *ecmd, Ecore_Ipc_Client* client)
#else
static void  
main_thread_launch(EfsdCommand *ecmd, int client)
#endif
	
{
  EfsdCommandClientContainer *container;

  container = NEW(EfsdCommandClientContainer);
  container->ecmd = ecmd;
  container->client = client;
  container->threaded = FALSE;

#if USE_THREADS

  pthread_mutex_lock(&threadcount_mutex);

  if (threadcount > EFSD_MAX_THREADS)
    container->threaded = FALSE;
  else
    threadcount++;
  
  pthread_mutex_unlock(&threadcount_mutex);


  if (container->threaded)
    {
      pthread_t thread;

      /*ecore_ipc_client_send(container->client, 1, 1, 0,0,0, NULL,0);*/
      if (pthread_create(&thread, NULL, main_handle_client_command, container) != 0)
	{
	  printf("No threads\n");
		
	  /* Couldn't create a thread -- run directly. */
	  container->threaded = FALSE;
	  main_handle_client_command(container);  
	}


    }
  else
    {
      main_handle_client_command(container);  
    }
#else
  /* If we don't have threads, just execute directly ... */
  main_handle_client_command(container);  
#endif
}

/*
static void   
main_serial_launch(EfsdCommand *ecmd, int client)
{
  EfsdCommandClientContainer *container;

  container = NEW(EfsdCommandClientContainer);
  container->ecmd = ecmd;
  container->client = client;
  container->threaded = FALSE;

  main_handle_client_command(container);  
}
*/

static void
main_thread_detach(void)
{
  D_ENTER;

#if USE_THREADS
  if (pthread_detach(pthread_self()) != 0)
    {
      fprintf(stderr, "Thread detach error -- exiting.\n");
      exit(-1);
    }
#endif

  D_RETURN;
}


static void *
main_handle_client_command(void *data)
{  
  EfsdCommandClientContainer *container = (EfsdCommandClientContainer *)data;
  EfsdCommand *command;
 
 
  
 #if HAVE_ECORE 
  Ecore_Ipc_Client* client;
 #else
  int client;
 #endif
 
  D_ENTER;


  if (container->threaded)
    main_thread_detach();

  command = container->ecmd;

  
  client = container->client;


  switch (command->type)
    {
    case EFSD_CMD_REMOVE:
      D("Handling REMOVE\n");
      efsd_command_remove(command, client);
      break;
    case EFSD_CMD_MOVE:
      D("Handling MOVE\n");
      efsd_command_move(command, client);
      break;
    case EFSD_CMD_COPY:
      D("Handling COPY\n");
      efsd_command_copy(command, client);
      break;
    case EFSD_CMD_SYMLINK:
      D("Handling SYMLINK\n");
      efsd_command_symlink(command, client);
      break;
    case EFSD_CMD_LISTDIR:
      D("Handling LISTDIR\n");
      efsd_command_listdir(command, client);
      break;
    case EFSD_CMD_MAKEDIR:
      D("Handling MAKEDIR\n");
      efsd_command_makedir(command, client);
      break;
    case EFSD_CMD_CHMOD:
      D("Handling CHMOD\n");
      efsd_command_chmod(command, client);
      break;
    case EFSD_CMD_SETMETA:
      D("Handling SETMETA\n");
      efsd_command_set_metadata(command, client);
      break;
    case EFSD_CMD_GETMETA:
      D("Handling GETMETA\n");
      efsd_command_get_metadata(command, client);
      break;
    case EFSD_CMD_STARTMON_FILE:
      D("Handling STARTMON_FILE\n");
      efsd_command_start_monitor(command, client, FALSE);
      break;
    case EFSD_CMD_STARTMON_DIR:
      D("Handling STARTMON_DIR\n");
      efsd_command_start_monitor(command, client, TRUE);
      break;
    case EFSD_CMD_STARTMON_META:
      D("Handling STARTMON_META\n");
      efsd_command_start_monitor_metadata(command, client);
      break;
    case EFSD_CMD_STOPMON_FILE:
      D("Handling STOPMON_FILE\n");
      efsd_command_stop_monitor(command, client, FALSE);
      break;
    case EFSD_CMD_STOPMON_DIR:
      D("Handling STOPMON_DIR\n");
      efsd_command_stop_monitor(command, client, TRUE);
      break;
    case EFSD_CMD_STOPMON_META:
      D("Handling STOPMON_META\n");
      efsd_command_stop_monitor_metadata(command, client);
      break;
    case EFSD_CMD_STAT:
      D("Handling STAT on %s\n", command->efsd_file_cmd.files[0]);
      efsd_command_stat(command, client, FALSE);
      break;
    case EFSD_CMD_LSTAT:
      D("Handling LSTAT on %s\n", command->efsd_file_cmd.files[0]);
      efsd_command_stat(command, client, TRUE);
      break;
    case EFSD_CMD_READLINK:
      D("Handling READLINK\n");
      efsd_command_readlink(command, client);
      break;
    case EFSD_CMD_GETFILETYPE:
      D("Handling GETFILETYPE\n");
      
      
      efsd_command_get_filetype(command, client);
      break;
    case EFSD_CMD_CLOSE:
      D("Handling CLOSE\n");
      efsd_main_close_connection(client);
      break;
    default:
      D("Handling INVALID command\n");
    }

  ecore_hash_remove(partial_command_hash, client);
  /*efsd_cmd_free(command);*/

#if USE_THREADS
  if (container->threaded)
    {
      pthread_mutex_lock(&threadcount_mutex);
      threadcount--;
      pthread_mutex_unlock(&threadcount_mutex);
    }
  D("Thread exits\n");
#endif

  FREE(container);

  D_RETURN_(NULL);
}


void
efsd_main_handle_file_exists_options(char *filename, EfsdMonitorRequest *emr)
{
  EfsdCommand *ecmd;
  int i;

  D_ENTER;

  for (i = 0; i < emr->num_options; i++)
    {
      ecmd = efsd_cmd_new();

      switch (emr->options[i].type)
	{
	case EFSD_OP_GET_STAT:
	  D("Trying stat option on file-exists event on '%s'...\n", filename);
	  ecmd->type = EFSD_CMD_STAT;
	  ecmd->efsd_file_cmd.num_files = 1;
	  ecmd->efsd_file_cmd.files = malloc(sizeof(char*));
	  ecmd->efsd_file_cmd.files[0] = strdup(filename);
	  ecmd->efsd_file_cmd.id = emr->id;
	  
	  /* Launch thread -- the command is freed there. */
	  main_thread_launch(ecmd, emr->client);
	  break;
	case EFSD_OP_GET_LSTAT:
	  D("Trying lstat option on file-exists event on '%s'...\n", filename);
	  ecmd->type = EFSD_CMD_LSTAT;
	  ecmd->efsd_file_cmd.num_files = 1;
	  ecmd->efsd_file_cmd.files = malloc(sizeof(char*));
	  ecmd->efsd_file_cmd.files[0] = strdup(filename);
	  ecmd->efsd_file_cmd.id = emr->id;
	  
	  /* Launch thread -- the command is freed there. */
	  main_thread_launch(ecmd, emr->client);
	  break;
	case EFSD_OP_GET_FILETYPE:
	  D("Trying getfile option on file-exists event on '%s'...\n", filename);
	  ecmd->type = EFSD_CMD_GETFILETYPE;
	  ecmd->efsd_file_cmd.num_files = 1;
	  ecmd->efsd_file_cmd.files = malloc(sizeof(char*));
	  ecmd->efsd_file_cmd.files[0] = strdup(filename);
	  ecmd->efsd_file_cmd.id = emr->id;

	  /* Launch thread -- the command is freed there. */
	  main_thread_launch(ecmd, emr->client);
	  break;
	case EFSD_OP_GET_META:
	  {
	    EfsdOptionGetmeta *op = &(emr->options[i].efsd_op_getmeta);

	    D("Trying get-meta option on file-exists event ...\n");
	    ecmd->type = EFSD_CMD_GETMETA;
	    ecmd->efsd_get_metadata_cmd.file = strdup(filename);
	    ecmd->efsd_get_metadata_cmd.key = strdup(op->key);
	    ecmd->efsd_get_metadata_cmd.datatype = op->datatype;
	    ecmd->efsd_get_metadata_cmd.id = emr->id;

	    /* Launch thread -- the command is freed there. */
	    main_thread_launch(ecmd, emr->client);
	  }
	  break;
	case EFSD_OP_ALL:
	case EFSD_OP_SORT:
	  break;
	default:
	  D("Nonsense listdir option (code %i)!\n", emr->options[i].type);
	}
    }

  D_RETURN;
}


static void *
main_fam_events_listener(void *dummy)
{
  int             fdsize, n;
  fd_set          fdrset;

  D_ENTER;

  main_thread_detach();
  D("FAM event listener thread running.\n");

  for ( ; ; )
    {
      FD_ZERO(&fdrset);

      /* listen to FAM file descriptor */

      FD_SET(famcon.fd, &fdrset);
      fdsize = famcon.fd;

      /* Wait for next FAM event to happen: */
      while ((n = select(fdsize+1, &fdrset, NULL, NULL, NULL)) < 0)
	{
	  if (errno == EINTR)
	    {
	      D(__FUNCTION__ " select() interrupted\n");
	      FD_ZERO(&fdrset);
	      FD_SET(famcon.fd, &fdrset);
	      fdsize = famcon.fd;
	    }
	  else if (errno == EBADF)
	    {
	      D("Connection to FAM died -- probably we're shutting down?\n");
	      D_RETURN_(NULL);
	    }
	  else
	    {
	      fprintf(stderr, __FUNCTION__ ": select error -- exiting.\n");
	      perror("Mhmmm");
	      exit(-1);
	    }
	}

      if (FD_ISSET(famcon.fd, &fdrset))
	{
	  /* FAM reported something -- handle it. */
	  main_handle_fam_events();
	}      
    }

  /* Not reached. */
  D_RETURN_(NULL);
  dummy = NULL;
}


static void 
main_handle_fam_events(void)
{
  FAMEvent     famev;
  int          i;
  EfsdMonitor *m = NULL;
  EfsdList    *cl = NULL;
  char         famev_filename_canonical[MAXPATHLEN];
  
  D_ENTER;

  /* The mutex-protected versions of the FAM calls
     are not really needed here -- these are the
     reading calls, not writers.
  */
  
  while (FAMPending(&famcon) > 0)
    {
      memset(&famev, 0, sizeof(FAMEvent));
      
      if (FAMNextEvent(&famcon, &famev) < 0)
	{
	  FAMOpen(&famcon);
	  D_RETURN;
	}
      
      if (!famev.filename || famev.filename[0] == '\0')
	continue;
      
      m = (EfsdMonitor*)famev.userdata;
      
      D("Handling FAM event %i for file %s\n", famev.code, famev.filename);
      
      /* If the monitor is for a directory, and it's a file-exists
	 event, store the file in the monitor. If another client
	 requests monitoring this directory, we can then simply
	 return those files instead of needed a new monitor for
	 the file-exists events.
      */
      switch (famev.code)
	{
	case FAMExists:
	  m->is_receiving_exist_events = TRUE;
	  /* no break here */
	case FAMCreated:
	  if (m->is_dir)
	    efsd_dca_append(m->files, famev.filename);
	  break;
	case FAMDeleted:
	  if (m->is_dir)
	    efsd_dca_remove(m->files, famev.filename);
	  break;
	default:
	}

      for (cl = efsd_list_head(m->clients); cl; cl = efsd_list_next(cl))
	{
	  EfsdMonitorRequest *emr;
	  char                list_all_files = FALSE;
	  char                sort_files = FALSE;
	  
	  emr = (EfsdMonitorRequest*) efsd_list_data(cl);
	  
	  for (i = 0; i < emr->num_options; i++)
	    {
	      switch (emr->options[i].type)
		{
		case EFSD_OP_ALL:
		  list_all_files = TRUE;
		  break;
		case EFSD_OP_SORT:
		  sort_files = TRUE;
		  break;
		default:
		}
	    }

	  #if HAVE_ECORE
	  #else
	  if (clientfd[emr->client] < 0)
	    {
	      D("Warning -- client %i's file descriptor seems to be closed.\n", emr->client);
	      continue;
	    }
	  #endif

	  /* The famev.filename is not always
	     canonical. Let's build one that is.
	  */		  
	  if (famev.filename[0] != '/')
	    {
	      if (m->filename[strlen(m->filename) - 1] == '/')
		snprintf(famev_filename_canonical, MAXPATHLEN, "%s%s", m->filename, famev.filename);
	      else
		snprintf(famev_filename_canonical, MAXPATHLEN, "%s/%s", m->filename, famev.filename);
	    }
	  else
	    snprintf(famev_filename_canonical, MAXPATHLEN, "%s", famev.filename);
	  
	  if (famev.code == FAMDeleted)
	    {
	      /* A monitored file got deleted. Remove its metadata. */
	      D("remove event for stat cached file %s -- remove metadata.\n",
		famev_filename_canonical);
	      efsd_meta_remove_data(famev_filename_canonical);
	    }

	  if ((famev.code == FAMChanged) || (famev.code == FAMDeleted))
	    {
	      /* A monitored file changed or got deleted,
		 -- therefore, remove the file from the
		 stat cache. If the file isn't in the cache,
		 the calls simply return, doing nothing.
	      */
	      
	      D("change|remove event for stat cached file %s -- removing from cache.\n",
		famev_filename_canonical);
	      efsd_stat_remove(famev_filename_canonical, TRUE);
	    }

	  if (emr->client == EFSD_CLIENT_INTERNAL)
	    {
	      if ((famev.code == FAMChanged) || (famev.code == FAMCreated))
		{
		  D("File change event for stat cached file %s\n", famev.filename);
		  
		  /* Check our "special files": the filetype dbs,
		     and update them if there is a match.
		  */
		  
		  if (!strcmp(famev.filename, efsd_filetype_get_system_file()))
		    {
		      /* The system-wide magic file has changed. Wait a bit
			 to make sure the operation that modified the file
			 finished.
		      */
		      signal(SIGALRM, main_reload_sys_sighandler);
		      alarm(3);
		    }
		  else if (!strcmp(famev.filename, efsd_filetype_get_user_file()))
		    {
		      /* The user magic file has changed. Wait a bit
			 to make sure the operation that modified the file
			 finished.
		      */
		      signal(SIGALRM, main_reload_user_sighandler);
		      alarm(3);
		    }
		}
	    }
	  else		    
	    {
	      /* Here the file monitoring events that are delivered to
		 clients are handled. We have two different cases for
		 directory monitors -- if sorted file-exists events
		 have been requested, we need to wait for all exists
		 events first, otherwise, we can process them directly.
	      */
	      if (!m->is_dir || !m->is_sorted)
		{
		  /* Process directly. */

		  /* Let's look only at the files we wanted: */
		  if (list_all_files || !efsd_misc_file_is_dotfile(famev.filename))
		    {
		      printf("1\n");
		      efsd_monitor_send_filechange_event(m, emr, famev.code, famev.filename);
		    }		      			  
		}
	      else
		{
		  /* Wait end of FILE_EXISTS ... */
		  
		  if (famev.code == FAMEndExist)
		    {
		      char *filename;
		      int index = 0;
		      
		      /* ... now sort the files in the monitor, ... */
		      efsd_dca_sort(m->files);
		      
		      /* ... and handle each of them. */
		      while ((filename = efsd_dca_get(m->files, index)) != NULL)
			{
			  if ((list_all_files || !efsd_misc_file_is_dotfile(filename)))
			    {
			     printf("2\n");
			      efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_EXISTS, filename);
			    }
			  
			  index++;
			}
		      
		      /* Send FILE_END_EXISTS so that the client knows that the end is reached. */
			printf("3\n");
		      efsd_monitor_send_filechange_event(m, emr, EFSD_FILE_END_EXISTS, famev.filename);
		    }
		}
	    }
	  	  
	  if (famev.code == FAMAcknowledge)
	    {
	      efsd_monitor_remove(m);
	      break;
	    }
	}

      if (famev.code == FAMEndExist)
	{
	  m->is_receiving_exist_events = FALSE;
	  efsd_monitor_cleanup_requests(m);
	}	        
    }

  D_RETURN;
}


static void 
main_handle_connections(void)
{
  int i;
	
  #if HAVE_ECORE 
  #else
  struct sockaddr_un serv_sun;
  int             num_read, fdsize, clilen,  n, can_accept, sock_fd;
  fd_set          fdrset;
  fd_set          fdwset;
  fd_set         *fdwset_ptr = NULL;
  #endif
  char            have_fam_thread = FALSE;
  struct timeval  tv;
  int             rebuild_fdset = FALSE;

  #if HAVE_ECORE
  Ecore_Ipc_Server *server = NULL;
  #endif

  D_ENTER;

  ev_q = efsd_queue_new();


  #ifndef HAVE_ECORE
  for (i = 0; i < EFSD_CLIENTS; i++)
    clientfd[i] = -1;
    
  if ( (listen_fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "Could not establish listening socket -- exiting.\n");
      exit(-1);
    }

  memset(&serv_sun, 0, sizeof(serv_sun));
  serv_sun.sun_family = AF_UNIX;
  strncpy(serv_sun.sun_path, efsd_misc_get_socket_file(), sizeof(serv_sun.sun_path));
  /*printf("Listening on %s\n", efsd_misc_get_socket_file());*/

  if (bind(listen_fd, (struct sockaddr *)&serv_sun, sizeof(serv_sun)) < 0)
    {
      perror("Error:");
      fprintf(stderr, "Could not bind socket -- exiting.\n");
      exit(-1);
    }

  if (listen(listen_fd, SOMAXCONN) < 0)
    {
      fprintf(stderr, "Could not listen on socket -- exiting.\n");
      exit(-1);
    }
  #else
  partial_command_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
  #endif

#if USE_THREADS
  {
    pthread_t thread;

    /* Launch a FAM-handling thread: */

    if (pthread_create(&thread, NULL, main_fam_events_listener, NULL) != 0)
      {
	D("WARNING -- Could not launch FAM-handler thread!\n");
      }
    else
      {
	have_fam_thread = TRUE;
      }
  }
#endif

  /* Handle connections: */

  #if HAVE_ECORE
  printf("ERR: Support using eCore/IPC!\n");

  /*Init the ipc server*/
  if (ecore_ipc_init() < 1)
                return (1);


  if ((server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL))) {
      ecore_ipc_server_del(server);
      printf ("ERR: Server already running...\n");
      return (1);
   } else {
      printf ("ERR: Server created..\n");

      server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);

      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
                 NULL);

      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
                 NULL);

       ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
                 NULL);


   }

  process_queue = ecore_list_new();

  ecore_idler_add(&queue_process_idler, NULL);

  ecore_main_loop_begin();

  
  #else
  
 
  for ( ; ; )
    {
      rebuild_fdset = FALSE;
      can_accept = FALSE;
      FD_ZERO(&fdrset);
      FD_ZERO(&fdwset);
      fdwset_ptr = NULL;
      fdsize = 0;

      /* listen to FAM file descriptor if necessary */

      if (!have_fam_thread)
	{
	  FD_SET(famcon.fd, &fdrset);
	  if (famcon.fd > fdsize)
	    fdsize = famcon.fd;
	}

      /* listen to connected clients */
      for (i = 0; i < EFSD_CLIENTS; i++)
	{
	  if (clientfd[i] >= 0)
	    {
	      FD_SET(clientfd[i], &fdrset);
	      if (clientfd[i] > fdsize)
		fdsize = clientfd[i];
	    }
	  else
	    can_accept = TRUE;
	}

      /* listen for new connections */
      if (can_accept)
	{
	  FD_SET(listen_fd, &fdrset);
	  if (listen_fd > fdsize)
	    fdsize = listen_fd;
	}

      if (!efsd_queue_empty(ev_q))
	{
	  efsd_event_queue_fill_fdset(ev_q, &fdwset, &fdsize);
	  fdwset_ptr = &fdwset;
	}

      /* Wait for next event to happen -- can be either an incoming
         command, or queued events can now be sent to formerly
         clogged clients ... no timeout.
      */
      tv.tv_sec  = 1;
      tv.tv_usec = 0;       

      while ((n = select(fdsize+1, &fdrset, fdwset_ptr, NULL, &tv)) < 0)
	{
	  if (errno == EINTR)
	    {
	      D(__FUNCTION__ " select() interrupted\n");

	      FD_ZERO(&fdrset);

	      if (fdwset_ptr)
		{
		  FD_ZERO(fdwset_ptr);
		}
	    }
	  else
	    {
	      D("Select error: %s.\n", strerror(errno));

	      /* FIXME -- if we get a bad descriptor here, we need to close
		 that connection properly and rebuild the fdset. */

	      rebuild_fdset = TRUE;
	    }
	  tv.tv_sec  = 1;
	  tv.tv_usec = 0;
	}
      
      /* If something went wrong with the select, start over. */
      if (rebuild_fdset)
	continue;

      /* if we timed out - ie 0 fd's available */
      if (n == 0)
	efsd_meta_idle();

      /* Check if anything is queued to be written
         to the writable fds ... */
      if (!efsd_queue_empty(ev_q))
	efsd_event_queue_process(ev_q, fdwset_ptr);
      
      if (!have_fam_thread && FD_ISSET(famcon.fd, &fdrset))
	{
	  /* FAM reported something -- handle it. */
	  main_handle_fam_events();
	}
      
      for (i = 0; i < EFSD_CLIENTS; i++)
	{
	  if (clientfd[i] >= 0 && FD_ISSET(clientfd[i], &fdrset))
	    {
	      /* A connected client sent something -- handle it. */

	      EfsdCommand  *ecmd;

	      ecmd = efsd_cmd_new();

	      if ( (num_read = efsd_io_read_command(clientfd[i], ecmd)) >= 0)
		{
		  /* Launch a new thread that handles the command --
		     the result is queued if the resulting event cannot
		     be sent ...
		  */
		  main_thread_launch(ecmd, i);
		}
	      else
		{
		  efsd_cmd_free(ecmd);
		  efsd_main_close_connection(i);
		}
	    }
	}

      if (FD_ISSET(listen_fd, &fdrset))
	{
	  struct sockaddr_un cli_sun;

	  /* There's a new client connecting -- register it. */
	  clilen = sizeof(cli_sun);
	  if ( (sock_fd = accept(listen_fd, (struct sockaddr *)&cli_sun, &clilen)) < 0)
	    {
	      /* FIXME -- Are Unix Domain Sockets susceptible to the same
		 race condition as in TCP/IP where accept() may block after
		 select() returned an fd as readable, when the client
		 immediately sent a RST? */

	      perror("Error:");
	      fprintf(stderr, "Could not accept connection -- exiting.\n");
	      exit(-1);
	    }

	  for (i = 0; i < EFSD_CLIENTS; i++)
	    {
	      if (clientfd[i] < 0)
		{
		  int flags;

		  D("New connection -- client %i.\n", i);	  
		  clientfd[i] = sock_fd;

		  if ( (flags = fcntl(sock_fd, F_GETFL, 0)) < 0)
		    {
		      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
		      exit(-1);
		    }

		  if (fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK) < 0)
		    {
		      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
		      exit(-1);
		    }

		  break;
		}
	    }
	}
    }

 #endif


  D_RETURN;
}


static void
main_hard_exit(void)
{
#if defined(SIGKILL)
  kill(app_pid, SIGKILL);
  abort();
#else
  kill(app_pid, 9);
  abort();
#endif
}


static void
main_stacktrace(void)
{
  char cmd[256];
  struct stat st;

  D_ENTER;

  /* This code taken from Eterm with permission
   * of the author. Cheers Michael :)
   */

#ifdef HAVE_U_STACK_TRACE
  U_STACK_TRACE();
  D_RETURN;
#elif defined(GDB)
  if (((stat(GDB_CMD_FILE, &st)) != 0) || (!S_ISREG(st.st_mode))) {
    D_RETURN;
  }
  snprintf(cmd, sizeof(cmd), GDB " -x " GDB_CMD_FILE " %s %d", app_cmd, getpid());
#elif defined(PSTACK)
  snprintf(cmd, sizeof(cmd), PSTACK " %d", getpid());
#elif defined(DBX)
#  ifdef _AIX
  snprintf(cmd, sizeof(cmd), "/bin/echo 'where\ndetach' | " DBX " -a %d", getpid());
#  elif defined(__sgi)
  snprintf(cmd, sizeof(cmd), "/bin/echo 'where\ndetach' | " DBX " -p %d", getpid());
#  else
  snprintf(cmd, sizeof(cmd), "/bin/echo 'where\ndetach' | " DBX " %s %d", app_cmd, getpid());
#  endif
#else
  fprintf(stdout, "Your system does not support any of the backtrace methods used.  Exiting.\n");
  main_hard_exit();
  D_RETURN;
#endif

  printf("Launching %s\n", cmd);
  system(cmd);
  main_hard_exit();

  D_RETURN;
}



static void
main_crash_sighandler(int sig)
{
  D_ENTER;

  fprintf(stdout, "Efsd [%d] received signal %i, attempting stack trace.\n",
	  getpid(), sig);

  signal(sig, SIG_DFL);
  main_stacktrace();
  exit(sig);

  D_RETURN;
}


static void
main_cleanup_sighandler(int sig)
{
  D_ENTER;

  D("Received sig %i -- cleanup.\n", sig);
  main_cleanup();
  
  D_RETURN;
}


static void
main_reload_user_sighandler(int sig)
{
  D_ENTER;
  
  D("Received sig %i -- reloading user settings.\n", sig);
  efsd_filetype_update_user_settings();
  signal(SIGALRM, SIG_IGN);
  
  D_RETURN;
}


static void
main_reload_sys_sighandler(int sig)
{
  D_ENTER;
  
  D("Received sig %i -- reloading system settings.\n", sig);
  efsd_filetype_update_system_settings();
  signal(SIGALRM, SIG_IGN);
  
  D_RETURN;
}


static void 
main_cleanup(void)
{
  static int done = FALSE;
  int i;
  
  D_ENTER;

  /* Fast exit if we've been here before ... */
  if (done)
    D_RETURN;

  done = TRUE;

  for (i = 0; i < EFSD_CLIENTS; i++)
    {
      if (clientfd[i] >= 0)
	{
	  D("Cleaning up client %i, fd %i\n", i, clientfd[i]);
	  close(clientfd[i]);
	  clientfd[i] = -1;
	}
    }
  
  close(listen_fd);
  efsd_misc_remove_socket_file();
  efsd_monitor_cleanup();
  efsd_stat_cleanup();
  efsd_meta_cleanup();
  efsd_filetype_cleanup();
  e_db_flush();

  /* I don't think I need to free the event queue here */

  D("Bye bye.\n");

  exit(0);
}


static void 
main_initialize(char *appname)
{
  D_ENTER;

   /* lots of paranoia - clean up dead socket on exit no matter what */
   /* only case it doesnt work: SIGKILL (kill -9) */
  signal(SIGALRM,   SIG_IGN);
  signal(SIGBUS,    main_cleanup_sighandler);
#ifdef SIGEMT
  signal(SIGEMT,    main_cleanup_sighandler);
#endif
  signal(SIGHUP,    main_cleanup_sighandler);
  signal(SIGINT,    main_cleanup_sighandler);
  signal(SIGTERM,   main_cleanup_sighandler);
  signal(SIGTRAP,   main_cleanup_sighandler);
  signal(SIGUSR1,   SIG_IGN);
  signal(SIGUSR2,   SIG_IGN);
#ifndef __EMX__
  signal(SIGIO,     main_cleanup_sighandler);
  signal(SIGIOT,    main_cleanup_sighandler);
#ifdef SIGSTKFLT
  signal(SIGSTKFLT, main_cleanup_sighandler);
#endif
  signal(SIGVTALRM, main_cleanup_sighandler);
  signal(SIGXCPU,   main_cleanup_sighandler);
  signal(SIGXFSZ,   main_cleanup_sighandler);
#endif
  signal(SIGPIPE,   SIG_IGN);

  signal(SIGQUIT,   main_crash_sighandler);
  /*  signal(SIGSEGV,   main_crash_sighandler); */
  signal(SIGFPE,    main_crash_sighandler);
  signal(SIGILL,    main_crash_sighandler);
#ifdef SIGSYS
  signal(SIGSYS,    main_crash_sighandler);
#endif
  atexit(efsd_misc_remove_socket_file);
  efsd_misc_create_user_dir();
  getcwd(app_cmd, MAXPATHLEN);
  sprintf(app_cmd + strlen(app_cmd), "/%s", appname);

  D_RETURN;
}


static void
main_daemonize(void)
{
  pid_t   pid;

  D_ENTER;
  
  if (!opt_foreground)
    {
      if ( (pid = fork()) < 0)
	{
	  fprintf(stderr, "Fork error -- exiting.\n");
	  exit(-1);
	}
      else if (pid != 0)
	{
	  /* parent exits */
	  exit(0);
	}
    }

#ifndef __EMX__
  setsid();
#endif
  chdir("/");
  umask(077);
  app_pid = getpid();

  D_RETURN;
}


static void   
main_check_permissions(void)
{
#ifndef __EMX__
  if (geteuid() == 0)
    {
      fprintf(stderr, "Efsd is not meant to be run by root -- at least not yet :)\n");
      exit(-1);
    }
#endif  
}


static void 
main_check_options(int argc, char**argv)
{
  struct stat st;
  int i;

  D_ENTER;

  for (i = 1; i < argc; i++)
    {
      if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")
	  || !strcmp(argv[i], "-?"))
	{
	  printf("Efsd -- the Enlightenment File System Daemon.\n"
		 "USAGE: %s [OPTIONS]\n"
		 "\n"
		 "  -f, --foreground      do not fork into background on startup.\n"
		 "  -c, --careful         don't start if socket file exists.\n"
#ifdef DEBUG
		 "  -d, --debug           show debugging output.\n"
#endif
#ifdef DEBUG_NEST
		 "  -t, --tree            show call tree.\n"
#endif
		 "\n",
		 argv[0]);
	  exit(0);
	}
      else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--foreground"))
	{
	  opt_foreground = TRUE;
	}
      else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--careful"))
	{
	  opt_careful = TRUE;
	}
#ifdef DEBUG
      else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--debug"))
	{
	  opt_debug = TRUE;
	}
#endif
#ifdef DEBUG_NEST
      else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--tree"))
	{
	  opt_nesting = TRUE;
	}
#endif
    }

  #ifndef HAVE_ECORE
  if (stat(efsd_misc_get_socket_file(), &st) >= 0)
    {
      if (opt_careful)
	{      
	  fprintf(stderr, "Efsd appears to be already running -- exiting.\n");
	  exit(-1);
	}

      if (unlink(efsd_misc_get_socket_file()) < 0)
	{
	  fprintf(stderr, "Socket file exists and cannot be removed -- exiting.\n");
	  exit(-1);
	}	  
    }
  #endif
  
  D_RETURN;
}


int
efsd_main_get_fd(int client)
{
  if (client < 0 || client >= EFSD_CLIENTS)
    D_RETURN_(-1);

  D_RETURN_(clientfd[client]);
}


int
efsd_main_close_connection(int client)
{
  D_ENTER;

  if (clientfd[client] < 0)
    {
      D_RETURN_(-1);
    }

  D("Closing connection %i\n", client);
  efsd_monitor_cleanup_client(client);
  efsd_meta_monitor_cleanup_client(client);
  close(clientfd[client]);
  clientfd[client] = -1;

  D_RETURN_(0);
}


int 
main(int argc, char **argv)
{
	
  D_ENTER;

  
  main_check_permissions();
  main_check_options(argc, argv);

  efsd_monitor_init();
  efsd_stat_init();
  efsd_meta_init();
  efsd_meta_monitor_init();
  efsd_filetype_init();

  main_initialize(argv[0]);
  main_daemonize();
  main_handle_connections();

  D_RETURN_(0);
}
