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

#ifdef __EMX__
#include <strings.h>
#include <sys/select.h>
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_io.h>
#include <efsd_fam_r.h>
#include <efsd_fam.h>
#include <efsd_fileops.h>
#include <efsd_filetype.h>
#include <efsd_list.h>
#include <efsd_lock.h>
#include <efsd_macros.h>
#include <efsd_main.h>
#include <efsd_meta.h>
#include <efsd_misc.h>
#include <efsd_queue.h>
#include <efsd_types.h>
#include <efsd_statcache.h>

/* The connection to FAM */
FAMConnection        famcon;

/* File desciptors for connected clients */
int                  clientfd[EFSD_CLIENTS];

/* Command line options: */
char                 opt_foreground = FALSE;
char                 opt_careful    = FALSE;
char                 opt_debug      = FALSE;
char                 opt_nesting    = FALSE;

/* File descriptor for accepting new clients */
static int           listen_fd;

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
  int          client;
  char         threaded; /* whether the contained command should
			    be run in a thread or not */
}
EfsdCommandClientContainer;

/* Pthread wrappers to minimize #ifdef clutter */
static void   main_thread_launch(EfsdCommand *ecom, int client);
static void   main_serial_launch(EfsdCommand *ecom, int client);
static void   main_thread_detach(void);

static void  *main_handle_client_command(void *container);
static void   main_handle_listdir_options(char *filename, EfsdFamRequest *efr);
static void   main_handle_fam_events(void);
static void   main_handle_connections(void);
static void   main_cleanup_signal_callback(int signal);
static void   main_cleanup(void);
static void   main_initialize(void);
static void   main_daemonize(void);
static void   main_check_permissions(void);
static void   main_check_options(int argc, char**argv);


static void   
main_thread_launch(EfsdCommand *ecmd, int client)
{
  EfsdCommandClientContainer *container;

  container = NEW(EfsdCommandClientContainer);
  container->ecmd = ecmd;
  container->client = client;
  container->threaded = TRUE;

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

      if (pthread_create(&thread, NULL, main_handle_client_command, container) != 0)
	{
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


static void
main_thread_detach(void)
{
  D_ENTER;

#if USE_THREADS
  if (pthread_detach(pthread_self()) != 0)
    {
      printf("Thread detach error -- exiting.\n");
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
  int client;
 
  D_ENTER;

  if (container->threaded)
    main_thread_detach();

  command = container->ecmd;
  client = container->client;

  switch (command->type)
    {
    case EFSD_CMD_REMOVE:
      D(("Handling REMOVE\n"));
      efsd_file_remove(command, client);
      break;
    case EFSD_CMD_MOVE:
      D(("Handling MOVE\n"));
      efsd_file_move(command, client);
      break;
    case EFSD_CMD_COPY:
      D(("Handling COPY\n"));
      efsd_file_copy(command, client);
      break;
    case EFSD_CMD_SYMLINK:
      D(("Handling SYMLINK\n"));
      efsd_file_symlink(command, client);
      break;
    case EFSD_CMD_LISTDIR:
      D(("Handling LISTDIR\n"));
      efsd_file_listdir(command, client);
      break;
    case EFSD_CMD_MAKEDIR:
      D(("Handling MAKEDIR\n"));
      efsd_file_makedir(command, client);
      break;
    case EFSD_CMD_CHMOD:
      D(("Handling CHMOD\n"));
      efsd_file_chmod(command, client);
      break;
    case EFSD_CMD_SETMETA:
      D(("Handling SETMETA\n"));
      efsd_file_set_metadata(command, client);
      break;
    case EFSD_CMD_GETMETA:
      D(("Handling GETMETA\n"));
      efsd_file_get_metadata(command, client);
      break;
    case EFSD_CMD_STARTMON:
      D(("Handling STARTMON\n"));
      efsd_file_start_monitor(command, client);
      break;
    case EFSD_CMD_STOPMON:
      D(("Handling STOPMON\n"));
      efsd_file_stop_monitor(command, client);
      break;
    case EFSD_CMD_STAT:
      D(("Handling STAT on %s\n", command->efsd_file_cmd.file));
      efsd_file_stat(command, client, FALSE);
      break;
    case EFSD_CMD_LSTAT:
      D(("Handling LSTAT on %s\n", command->efsd_file_cmd.file));
      efsd_file_stat(command, client, TRUE);
      break;
    case EFSD_CMD_READLINK:
      D(("Handling READLINK\n"));
      efsd_file_readlink(command, client);
      break;
    case EFSD_CMD_GETFILETYPE:
      D(("Handling GETFILETYPE\n"));
      efsd_file_getfiletype(command, client);
      break;
    case EFSD_CMD_CLOSE:
      D(("Handling CLOSE\n"));
      efsd_main_close_connection(client);
      break;
    default:
      D(("Handling INVALID command\n"));
    }

  efsd_cmd_free(command);

#if USE_THREADS
  if (container->threaded)
    {
      pthread_mutex_lock(&threadcount_mutex);
      threadcount--;
      pthread_mutex_unlock(&threadcount_mutex);
    }
  D(("Thread exits\n"));
#endif

  FREE(container);

  D_RETURN_(NULL);
}


static void
main_handle_listdir_options(char *filename, EfsdFamRequest *efr)
{
  EfsdCommand *ecmd;
  int i;

  D_ENTER;

  for (i = 0; i < efr->num_options; i++)
    {
      ecmd = efsd_cmd_new();

      switch (efr->options[i].type)
	{
	case EFSD_OP_GET_STAT:
	  D(("Trying stat option on file-exists event on '%s'...\n", filename));
	  ecmd->type = EFSD_CMD_STAT;
	  ecmd->efsd_file_cmd.file = strdup(filename);
	  ecmd->efsd_file_cmd.id = efr->id;
	  
	  /* Launch thread -- the command is freed there. */
	  main_thread_launch(ecmd, efr->client);
	  break;
	case EFSD_OP_GET_LSTAT:
	  D(("Trying lstat option on file-exists event on '%s'...\n", filename));
	  ecmd->type = EFSD_CMD_LSTAT;
	  ecmd->efsd_file_cmd.file = strdup(filename);
	  ecmd->efsd_file_cmd.id = efr->id;
	  
	  /* Launch thread -- the command is freed there. */
	  main_thread_launch(ecmd, efr->client);
	  break;
	case EFSD_OP_GET_FILETYPE:
	  D(("Trying getfile option on file-exists event on '%s'...\n", filename));
	  ecmd->type = EFSD_CMD_GETFILETYPE;
	  ecmd->efsd_file_cmd.file = strdup(filename);
	  ecmd->efsd_file_cmd.id = efr->id;

	  /* Launch thread -- the command is freed there. */
	  main_thread_launch(ecmd, efr->client);
	  break;
	case EFSD_OP_GET_META:
	  {
	    EfsdOptionGetmeta *op = &(efr->options[i].efsd_op_getmeta);

	    D(("Trying get-meta option on file-exists event ...\n"));
	    ecmd->type = EFSD_CMD_GETMETA;
	    ecmd->efsd_get_metadata_cmd.file = strdup(filename);
	    ecmd->efsd_get_metadata_cmd.key = strdup(op->key);
	    ecmd->efsd_get_metadata_cmd.datatype = op->datatype;
	    ecmd->efsd_get_metadata_cmd.id = efr->id;

	    /* Launch thread -- the command is freed there. */
	    main_thread_launch(ecmd, efr->client);
	  }
	  break;
	case EFSD_OP_ALL:
	  break;
	default:
	  D(("Nonsense listdir option (code %i)!\n", efr->options[i].type));
	}
    }

  D_RETURN;
}


static void 
main_handle_fam_events(void)
{
  FAMEvent    famev;
  EfsdEvent   ee;
  char        s[MAXPATHLEN];
  int         i;

  D_ENTER;

  while (FAMPending_r(&famcon) > 0)
    {
      memset(&famev, 0, sizeof(FAMEvent));

      if (FAMNextEvent_r(&famcon, &famev) < 0)
	{
	  FAMOpen_r(&famcon);
	  D_RETURN;
	}

      if (famev.filename)
	{
	  EfsdFamMonitor *m;
	  EfsdList       *cl;
	  
	  m = (EfsdFamMonitor*)famev.userdata;

	  /* D(("Handling FAM event %i for file %s\n", famev.code, famev.filename)); */

	  memset(&ee, 0, sizeof(EfsdEvent));
	  ee.type = EFSD_EVENT_FILECHANGE;
	  ee.efsd_filechange_event.changetype = (EfsdFilechangeType)famev.code;
	  ee.efsd_filechange_event.file = strdup(famev.filename);

	  for (cl = efsd_list_head(m->clients); cl; cl = efsd_list_next(cl))
	    {
	      EfsdFamRequest *efr;
	      char            list_all_files = FALSE;
		    
	      efr = (EfsdFamRequest*) efsd_list_data(cl);

	      for (i = 0; i < efr->num_options; i++)
		{
		  if (efr->options[i].type == EFSD_OP_ALL)
		    {
		      list_all_files = TRUE;
		      break;
		    }
		}

	      switch (efr->type)
		{
		case EFSD_FAM_MONITOR_NORMAL:

		  if (clientfd[efr->client] >= 0)
		    {
		      ee.efsd_filechange_event.id = efr->id;
		      
		      if (list_all_files || !efsd_misc_file_is_dotfile(famev.filename))
			{
			  D(("Writing FAM event %i to client %i\n",
			     famev.code, efr->client));
			  if (efsd_io_write_event(clientfd[efr->client], &ee) < 0)
			    {
			      if (errno == EPIPE)
				{
				  D(("Client %i died -- closing connection.\n", efr->client));
				  efsd_main_close_connection(efr->client);
				}
			      else
				{
				  efsd_queue_add_event(clientfd[efr->client], &ee);
				  D(("write() error when writing FAM event.\n"));
				}
			    }
			  
			  if (famev.code == FAMExists)
			    {
			      if (famev.filename[0] != '/')
				{
				  snprintf(s, MAXPATHLEN, "%s/%s", m->filename, famev.filename); 
				  main_handle_listdir_options(s, efr);
				}
			      else
				{
				  main_handle_listdir_options(famev.filename, efr);
				}
			    }
			}		      
		    }
		  break;
		case EFSD_FAM_MONITOR_INTERNAL:
		  if (famev.code == FAMChanged)
		    {
		      D(("File change event for stat cached file %s\n", famev.filename));
		      efsd_stat_update(famev.filename);
		    }
		  else if (famev.code == FAMDeleted)
		    {
		      D(("File remove event for stat cached file %s\n", famev.filename));
		      efsd_stat_remove(famev.filename);
		    }
		  break;
		default:
		  D(("Unknown EfsdFamMonitor type!\n"));
		}
	    }

	  if (famev.code == FAMAcknowledge)
	    efsd_fam_remove_monitor(m);

	  efsd_event_cleanup(&ee);
	}
    }

  D_RETURN;
}


static void 
main_handle_connections(void)
{
  struct sockaddr_un serv_sun, cli_sun;
  int             num_read, fdsize, clilen, i, n, can_accept, sock_fd;
  fd_set          fdrset;
  fd_set          fdwset;
  fd_set         *fdwset_ptr = NULL;

  D_ENTER;

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

  if (bind(listen_fd, (struct sockaddr *)&serv_sun, sizeof(serv_sun)) < 0)
    {
      fprintf(stderr, "Could not bind socket -- exiting.\n");
      exit(-1);
    }

  if (listen(listen_fd, SOMAXCONN) < 0)
    {
      fprintf(stderr, "Could not listen on socket -- exiting.\n");
      exit(-1);
    }

  /* handle connections: */

  for ( ; ; )
    {
      can_accept = 0;
      FD_ZERO(&fdrset);
      FD_ZERO(&fdwset);
      fdwset_ptr = NULL;
      fdsize = 0;

      /* listen to FAM file descriptor */
      FD_SET(famcon.fd, &fdrset);
      if (famcon.fd > fdsize)
	fdsize = famcon.fd;

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
	    can_accept = 1;
	}

      /* listen for new connections */
      if (can_accept)
	{
	  FD_SET(listen_fd, &fdrset);
	  if (listen_fd > fdsize)
	    fdsize = listen_fd;
	}

      if (!efsd_queue_empty())
	{
	  efsd_queue_fill_fdset(&fdwset, &fdsize);
	  fdwset_ptr = &fdwset;
	}

      /* Wait for next event to happen -- can be either an incoming
         command, or queued events can now be sent to formerly
         clogged clients ... no timeout.
      */
      while ((n = select(fdsize+1, &fdrset, fdwset_ptr, NULL, NULL)) < 0)
	{
	  if (errno == EINTR)
	    {
	      D(("read_data select() interrupted\n"));

	      FD_ZERO(&fdrset);

	      if (fdwset_ptr)
		{
		  FD_ZERO(fdwset_ptr);
		}
	    }
	  else
	    {
	      fprintf(stderr, "Select error -- exiting.\n");
	      exit(-1);
	    }
	}

      /* Check if anything is queued to be written
         to the writable fds ... */
      if (!efsd_queue_empty())
	{
	  efsd_queue_process(fdwset_ptr);
	}
      
      if (FD_ISSET(famcon.fd, &fdrset))
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
	      D(("Reading command ...\n"));

	      if ( (num_read = efsd_io_read_command(clientfd[i], ecmd)) >= 0)
		{
		  /* Launch a new thread that handles the command --
		     the result is queued if the resulting event cannot
		     be sent ...
		  */
		  D(("Received command type %i\n", ecmd->type));

		  main_thread_launch(ecmd, i);
		}
	      else
		{
		  efsd_main_close_connection(i);
		  efsd_cmd_free(ecmd);
		}
	      D(("Done.\n"));
	    }
	}

      if (FD_ISSET(listen_fd, &fdrset))
	{
	  /* There's a new client connecting -- register it. */
	  clilen = sizeof(cli_sun);
	  if ( (sock_fd = accept(listen_fd, (struct sockaddr *)&cli_sun, &clilen)) < 0)
	    {
	      perror("Error:");
	      fprintf(stderr, "Could not accept connection -- exiting.\n");
	      exit(-1);
	    }

	  for (i = 0; i < EFSD_CLIENTS; i++)
	    {
	      if (clientfd[i] < 0)
		{
		  D(("New connection -- client %i.\n", i));	  
		  clientfd[i] = sock_fd;

		  if (fcntl(sock_fd, F_SETFL, O_NONBLOCK) < 0)
		    {
		      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
		      exit(-1);
		    }

		  break;
		}
	    }
	}
    }
  D_RETURN;
}


static void 
main_cleanup_signal_callback(int signal)
{
  D_ENTER;
  D(("Received sig %i -- cleanup.\n", signal));
  main_cleanup();
  D_RETURN;
}

static void 
main_cleanup(void)
{
  int i;
  
  D_ENTER;

  for (i = 0; i < EFSD_CLIENTS; i++)
    {
      if (clientfd[i] >= 0)
	{
	  D(("Cleaning up client %i, fd %i\n", i, clientfd[i]));
	  close(clientfd[i]);
	}
    }
  
  close(listen_fd);
  efsd_misc_remove_socket_file();
  efsd_fam_cleanup();
  efsd_stat_cleanup();
  efsd_meta_cleanup();
  efsd_filetype_cleanup();
  D(("Bye bye.\n"));

  exit(0);
}


static void 
main_initialize(void)
{
  D_ENTER;

   /* lots of paranoia - clean up dead socket on exit no matter what */
   /* only case it doesnt work: SIGKILL (kill -9) */
  signal(SIGABRT,   main_cleanup_signal_callback);
  signal(SIGALRM,   main_cleanup_signal_callback);
  signal(SIGBUS,    main_cleanup_signal_callback);
#ifdef SIGEMT
  signal(SIGEMT, main_cleanup_signal_callback);
#endif
  signal(SIGFPE,    main_cleanup_signal_callback);
  signal(SIGHUP,    main_cleanup_signal_callback);
  signal(SIGILL,    main_cleanup_signal_callback);
  signal(SIGINT,    main_cleanup_signal_callback);
  signal(SIGQUIT,   main_cleanup_signal_callback);
  /* signal(SIGSEGV,   main_cleanup_signal_callback); */
  signal(SIGSYS,    main_cleanup_signal_callback);
  signal(SIGTERM,   main_cleanup_signal_callback);
  signal(SIGTRAP,   main_cleanup_signal_callback);
  signal(SIGUSR1,   main_cleanup_signal_callback);
  signal(SIGUSR2,   main_cleanup_signal_callback);
#ifndef __EMX__
  signal(SIGIO,     main_cleanup_signal_callback);
  signal(SIGIOT,    main_cleanup_signal_callback);
#ifdef SIGSTKFLT
  signal(SIGSTKFLT, main_cleanup_signal_callback);
#endif
  signal(SIGVTALRM, main_cleanup_signal_callback);
  signal(SIGXCPU,   main_cleanup_signal_callback);
  signal(SIGXFSZ,   main_cleanup_signal_callback);
#endif
  signal(SIGPIPE, SIG_IGN);

  atexit(efsd_misc_remove_socket_file);

  efsd_misc_create_efsd_dir();

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
  D_RETURN;
}


int
efsd_main_close_connection(int client)
{
  D_ENTER;
  D(("Closing connection %i\n", client));

  if (clientfd[client] < 0)
    {
      D(("Connection already closed ???\n"));
      D_RETURN_(-1);
    }

  efsd_fam_cleanup_client(client);
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

  efsd_fam_init();
  efsd_stat_init();
  efsd_meta_init();
  efsd_filetype_init();
  main_initialize();
  main_daemonize();
  main_handle_connections();

  D_RETURN_(0);
}
