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
#include <fam.h>

#ifdef __EMX__
#include <strings.h>
#include <sys/select.h>
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_io.h>
#include <efsd_fam.h>
#include <efsd_fileops.h>
#include <efsd_list.h>
#include <efsd_macros.h>
#include <efsd_main.h>
#include <efsd_misc.h>
#include <efsd_queue.h>
#include <efsd_types.h>
#include <efsd_misc.h>

/* The connection to FAM */
FAMConnection        famcon;

/* File desciptors for connected clients */
int                  clientfd[EFSD_CLIENTS];

/* File descriptor for accepting new clients */
static int           listen_fd;

/* For options: */
static int           opt_foreground = 0;
static int           opt_careful    = 0;

static void   efsd_connect_to_fam(void);
static int    efsd_handle_client_command(EfsdCommand *command, int sockfd);
static void   efsd_handle_listdir_options(char *filename, EfsdFamRequest *efr);
static void   efsd_handle_fam_events(void);
static void   efsd_handle_connections(void);
static void   efsd_cleanup_signal_callback(int signal);
static void   efsd_cleanup(void);
static void   efsd_daemonize(void);
static void   efsd_check_options(int argc, char**argv);


static void 
efsd_connect_to_fam(void)
{
  D_ENTER;

  if ((FAMOpen(&famcon)) < 0)
    {
      fprintf(stderr, "Can not connect to fam -- exiting.\n");
      exit(-1);
    }
  D_RETURN;
}


static int
efsd_handle_client_command(EfsdCommand *command, int client)
{
  int result = (-1);
  
  D_ENTER;

  switch (command->type)
    {
    case EFSD_CMD_REMOVE:
      D(("Handling REMOVE\n"));
      result = efsd_file_remove(command, client);
      break;
    case EFSD_CMD_MOVE:
      D(("Handling MOVE\n"));
      result = efsd_file_move(command, client);
      break;
    case EFSD_CMD_COPY:
      D(("Handling COPY\n"));
      result = efsd_file_copy(command, client);
      break;
    case EFSD_CMD_SYMLINK:
      D(("Handling SYMLINK\n"));
      result = efsd_file_symlink(command, client);
      break;
    case EFSD_CMD_LISTDIR:
      D(("Handling LISTDIR\n"));
      result = efsd_file_listdir(command, client);
      break;
    case EFSD_CMD_MAKEDIR:
      D(("Handling MAKEDIR\n"));
      result = efsd_file_makedir(command, client);
      break;
    case EFSD_CMD_CHMOD:
      D(("Handling CHMOD\n"));
      result = efsd_file_chmod(command, client);
      break;
    case EFSD_CMD_SETMETA:
      D(("Handling SETMETA\n"));
      result = efsd_file_set_metadata(command, client);
      break;
    case EFSD_CMD_GETMETA:
      D(("Handling GETMETA\n"));
      result = efsd_file_get_metadata(command, client);
      break;
    case EFSD_CMD_STARTMON:
      D(("Handling STARTMON\n"));
      result = efsd_file_start_monitor(command, client);
      break;
    case EFSD_CMD_STOPMON:
      D(("Handling STOPMON\n"));
      result = efsd_file_stop_monitor(command, client);
      break;
    case EFSD_CMD_STAT:
      D(("Handling STAT on %s\n", command->efsd_file_cmd.file));
      result = efsd_file_stat(command, client);
      break;
    case EFSD_CMD_READLINK:
      D(("Handling READLINK\n"));
      result = efsd_file_readlink(command, client);
      break;
    case EFSD_CMD_GETMIME:
      D(("Handling GETMIME\n"));
      result = efsd_file_getmime(command, client);
      break;
    case EFSD_CMD_CLOSE:
      D(("Handling CLOSE\n"));
      result = efsd_main_close_connection(client);
      break;
    default:
    }

  D_RETURN_(result);
}


static void
efsd_handle_listdir_options(char *filename, EfsdFamRequest *efr)
{
  EfsdCommand ec;
  int i;

  D_ENTER;

  ec.efsd_file_cmd.file = filename;
  ec.efsd_file_cmd.id = efr->id;

  for (i = 0; i < efr->num_options; i++)
    {
      switch (efr->options[i].type)
	{
	case EFSD_OP_LS_GET_STAT:
	  D(("Trying stat option on file-exists event on '%s'...\n", filename));
	  ec.type = EFSD_CMD_STAT;
	  if (efsd_file_stat(&ec, efr->client) < 0)
	    {
	      D(("Stat option for file-exists event failed -- queued.\n"));
	    }
	  else
	    {
	      D(("Succeeded.\n"));
	    }
	  break;
	case EFSD_OP_LS_GET_MIME:
	  D(("Trying getmime option on file-exists event on '%s'...\n", filename));
	  ec.type = EFSD_CMD_GETMIME;
	  if (efsd_file_getmime(&ec, efr->client) < 0)
	    {
	      D(("Get-mime option for file-exists event failed -- queued.\n"));
	    }
	  else
	    {
	      D(("Succeeded.\n"));
	    }
	  break;
	case EFSD_OP_LS_GET_META:
	  D(("Trying get-meta option on file-exists event ...\n"));
	  break;
	default:
	  D(("Nonsense listdir option (code %i)!\n", efr->options[i].type));
	}
    }

  D_RETURN;
}


static void 
efsd_handle_fam_events(void)
{
  FAMEvent    famev;
  EfsdEvent   ee;
  char        s[MAXPATHLEN];

  D_ENTER;

  while (FAMPending(&famcon) > 0)
    {
      bzero(&famev, sizeof(FAMEvent));

      if (FAMNextEvent(&famcon, &famev) < 0)
	{
	  FAMOpen(&famcon);
	  D_RETURN;
	}

      if (famev.filename)
	{
	  EfsdFamMonitor *m;
	  
	  m = (EfsdFamMonitor*)famev.userdata;

	  D(("Handling FAM event %i for file %s\n", famev.code, famev.filename));

	  bzero(&ee, sizeof(EfsdEvent));
	  ee.type = EFSD_EVENT_FILECHANGE;
	  ee.efsd_filechange_event.changetype = (EfsdFilechangeType)famev.code;
	  ee.efsd_filechange_event.file = strdup(famev.filename);
	  
	  switch(m->type)
	    {
	    case SIMPLE:
	      {
		/* This event is part of a directory listing ... */

		EfsdFamRequest  *efr;

		/* Do not report the metadata dir in normal dir
		   listings.
		*/
		if (strcmp(famev.filename, EFSD_META_DIR_NAME))
		  {
		    efr = (EfsdFamRequest*)(efsd_list_data(efsd_list_head(m->clients)));
		    
		    if (clientfd[efr->client] >= 0)
		      {
			ee.efsd_filechange_event.id = efr->id;
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
				efsd_handle_listdir_options(s, efr);
			      }
			    else
			      {
				efsd_handle_listdir_options(famev.filename, efr);
			      }
			  }
		      }
		  }    

		if (famev.code == FAMAcknowledge)
		  {
		    D(("got FAMEndExist for dir listing -- freeing monitor.\n"));
		    efsd_fam_free_monitor(m);
		  }
	      }
	      break;
	    case FULL:
	      {
		/* This is an event generated by an actual change */
		
		EfsdList      *cl;
			
		for (cl = efsd_list_head(m->clients); cl; cl = efsd_list_next(cl))
		  {
		    EfsdFamRequest *efr;
		    
		    efr = (EfsdFamRequest*) efsd_list_data(cl);
		    if (clientfd[efr->client] >= 0)
		      {
			ee.efsd_filechange_event.id = efr->id;
		    
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
				efsd_handle_listdir_options(s, efr);
			      }
			    else
			      {
				efsd_handle_listdir_options(famev.filename, efr);
			      }
			  }
		      }
		  }

		if (famev.code == FAMAcknowledge)
		  {
		    D(("got FAMEndExist for dir monitoring -- freeing monitor.\n"));
		    efsd_fam_remove_monitor(m);
		  }
	      }
	      break;
	    }
	  
	  efsd_event_cleanup(&ee);
	}
    }

  D_RETURN;
}


static void 
efsd_handle_connections(void)
{
  struct sockaddr_un serv_sun, cli_sun;
  int             num_read, fdsize, clilen, i, n, can_accept, sock_fd;
  EfsdCommand     ecmd;
  fd_set          fdset;
  struct timeval  tv;

  D_ENTER;

  for (i = 0; i < EFSD_CLIENTS; i++)
    clientfd[i] = -1;
    
  if ( (listen_fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "Could not establish listening socket -- exiting.\n");
      exit(-1);
    }

  bzero(&serv_sun, sizeof(serv_sun));
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
      tv.tv_sec  = 1;
      tv.tv_usec = 0;

      can_accept = 0;
      FD_ZERO(&fdset);
      fdsize = 0;

      /* listen to FAM file descriptor */
      FD_SET(famcon.fd, &fdset);
      if (famcon.fd > fdsize)
	fdsize = famcon.fd;

      /* listen to connected clients */
      for (i = 0; i < EFSD_CLIENTS; i++)
	{
	  if (clientfd[i] >= 0)
	    {
	      FD_SET(clientfd[i], &fdset);
	      if (clientfd[i] > fdsize)
		fdsize = clientfd[i];
	    }
	  else
	    can_accept = 1;
	}

      /* listen for new connections */
      if (can_accept)
	{
	  FD_SET(listen_fd, &fdset);
	  if (listen_fd > fdsize)
	    fdsize = listen_fd;
	}

      /* Wait for next event to happen ... */
      while ((n = select(fdsize+1, &fdset, NULL, NULL, &tv)) < 0)
	{
	  if (errno == EINTR)
	    {
	      D(("read_data select() interrupted\n"));
	      tv.tv_sec  = 1;
	      tv.tv_usec = 0;
	      FD_ZERO(&fdset);
	    }
	  else
	    {
	      fprintf(stderr, "Select error -- exiting.\n");
	      exit(-1);
	    }
	}

      /* Check if anything is queued to be written ... */
      if (!efsd_queue_empty())
	{
	  efsd_queue_process();
	}
      
      if (FD_ISSET(famcon.fd, &fdset))
	{
	  /* FAM reported something -- handle it. */
	  efsd_handle_fam_events();
	}
      
      for (i = 0; i < EFSD_CLIENTS; i++)
	{
	  if (clientfd[i] >= 0 && FD_ISSET(clientfd[i], &fdset))
	    {
	      /* A connected client sent something -- handle it. */
	      bzero(&ecmd, sizeof(EfsdCommand));
	      D(("Reading command ...\n"));
	      if ( (num_read = efsd_io_read_command(clientfd[i], &ecmd)) >= 0)
		{
		  /* handle command -- it is queued if the
		     resulting event cannot be sent ...
		  */
		  if (efsd_handle_client_command(&ecmd, i) < 0)
		    {
		      D(("Failed to write command, command queued.\n"));
		    }
		}
	      else
		{
		  efsd_main_close_connection(i);
		}
	      D(("Done.\n"));
	      efsd_cmd_cleanup(&ecmd);
	    }
	}

      if (FD_ISSET(listen_fd, &fdset))
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
efsd_cleanup_signal_callback(int signal)
{
  D_ENTER;
  D(("Received sig %i -- cleanup.\n", signal));
  efsd_cleanup();
  D_RETURN;
}

static void 
efsd_cleanup(void)
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
  FAMClose(&famcon);
  efsd_fam_cleanup();
  exit(0);

  D_RETURN;
}


static void 
efsd_initialize(void)
{
  D_ENTER;

#ifndef __EMX__
  if (geteuid() == 0)
    {
      fprintf(stderr, "Efsd is not meant to be run by root -- at least not yet :)\n");
      exit(-1);
    }
#endif  

   /* lots of paranoia - clean up dead socket on exit no matter what */
   /* only case it doesnt work: SIGKILL (kill -9) */
  signal(SIGABRT,   efsd_cleanup_signal_callback);
  signal(SIGALRM,   efsd_cleanup_signal_callback);
  signal(SIGBUS,    efsd_cleanup_signal_callback);
#ifdef SIGEMT
  signal(SIGEMT, efsd_cleanup_signal_callback);
#endif
  signal(SIGFPE,    efsd_cleanup_signal_callback);
  signal(SIGHUP,    efsd_cleanup_signal_callback);
  signal(SIGILL,    efsd_cleanup_signal_callback);
  signal(SIGINT,    efsd_cleanup_signal_callback);
  signal(SIGQUIT,   efsd_cleanup_signal_callback);
  //signal(SIGSEGV,   efsd_cleanup_signal_callback);
  signal(SIGSYS,    efsd_cleanup_signal_callback);
  signal(SIGTERM,   efsd_cleanup_signal_callback);
  signal(SIGTRAP,   efsd_cleanup_signal_callback);
  signal(SIGUSR1,   efsd_cleanup_signal_callback);
  signal(SIGUSR2,   efsd_cleanup_signal_callback);
#ifndef __EMX__
  signal(SIGIO,     efsd_cleanup_signal_callback);
  signal(SIGIOT,    efsd_cleanup_signal_callback);
#ifdef SIGSTKFLT
  signal(SIGSTKFLT, efsd_cleanup_signal_callback);
#endif
  signal(SIGVTALRM, efsd_cleanup_signal_callback);
  signal(SIGXCPU,   efsd_cleanup_signal_callback);
  signal(SIGXFSZ,   efsd_cleanup_signal_callback);
#endif
  signal(SIGPIPE, SIG_IGN);

  atexit(efsd_misc_remove_socket_file);

  efsd_misc_check_dir();

  D_RETURN;
}


static void
efsd_daemonize(void)
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
efsd_check_options(int argc, char**argv)
{
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
		 "\n",
		 argv[0]);
	  exit(0);
	}
      else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--foreground"))
	{
	  opt_foreground = 1;
	}
      else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--careful"))
	{
	  opt_careful = 1;
	}
    }

  if (efsd_misc_file_exists(efsd_misc_get_socket_file()))
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
  efsd_check_options(argc, argv);

  efsd_daemonize();
  efsd_initialize();
  efsd_fam_init();
  efsd_connect_to_fam();
  efsd_handle_connections();

  D_RETURN_(0);
}
