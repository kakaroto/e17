/* 

                          --- Efsd Demo ---


Copyright (C) 2000, 2001 Christian Kreibich <kreibich@aciri.org>.

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
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libefsd.h>

/* This one demonstrates how to read events
   from efsd in blocking mode, by calling
   efsd_next_event().
*/
void read_answers_blocking(EfsdConnection *ec);

/* This one shows how to get events in more
   complex applications, when multiple non-
   blocking file descriptors are selected()
   e.g. in an event queue.
*/
void read_answers_selecting(EfsdConnection *ec);

/* Finally handle_efsd_event() shows how to
   interpret the various events. Look at
   efsd.h for details on the various types.
*/
void handle_efsd_event(EfsdEvent *ee);


void read_answers_blocking(EfsdConnection *ec)
{
  EfsdEvent ee;

  printf("RUNNING IN BLOCKING MODE\n");

  while (efsd_next_event(ec, &ee) != -1)
    handle_efsd_event(&ee);
}


void read_answers_selecting(EfsdConnection *ec)
{
  EfsdEvent ee;
  fd_set    fdset;
  int       fdsize;
  int       fd;

  printf("RUNNING IN SELECTING MODE\n");

  fd = efsd_get_connection_fd(ec);

  for ( ; ; )
    {
      FD_ZERO(&fdset);
      fdsize = 0;

      /* in real life some other file descriptors would be added here .. */

      /* now add efsd fd */
      FD_SET(fd, &fdset);
      if (fd > fdsize)
	fdsize = fd;

      /* Wait for next event to happen ... */
      select(fdsize+1, &fdset, NULL, NULL, NULL);

      if (FD_ISSET(fd, &fdset))
	{
	  if (efsd_next_event(ec, &ee) != -1)
	    handle_efsd_event(&ee);
	  else
	    /* The following comment is grammatically 100 % correct
	       thanks to Andrew Shugg <andrew@neep.com.au> :P
	    */

	    /* We're wimps here and just exit. */
	    break;
	}
    }
}


void handle_efsd_event(EfsdEvent *ee)
{
  switch (ee->type)
    {
    case FILECHANGE:
      switch (ee->efsd_filechange_event.changecode)
	{
	case FAMChanged:
	  printf("Filechange event for cmd %i: %s changed.\n", 
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMDeleted:
	  printf("Filechange event for cmd %i: %s deleted.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMStartExecuting:
	  printf("Filechange event for cmd %i: %s started.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMStopExecuting:
	  printf("Filechange event for cmd %i: %s stopped.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMCreated:
	  printf("Filechange event for cmd %i: %s created.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMMoved:
	  printf("Filechange event for cmd %i: %s moved.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMAcknowledge:
	  printf("Filechange event for cmd %i: %s acked.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMExists:
	  printf("Filechange event for cmd %i: %s exists.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case FAMEndExist:
	  printf("Filechange event for cmd %i: %s end exists.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	default:
	  printf("Filechange event for cmd %i: %s WHAT THE HELL IS THAT?.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	}
      break;
    case REPLY:
      switch (ee->efsd_reply_event.command.type)
	{
	case REMOVE:
	  printf("Remove event %i\n -- removing %s\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.file);		 
	  break;
	case MOVE:
	  printf("Move event %i\n -- moving %s to %s\n",
		 ee->efsd_reply_event.command.efsd_2file_cmd.id,
		 ee->efsd_reply_event.command.efsd_2file_cmd.file1,		 
		 ee->efsd_reply_event.command.efsd_2file_cmd.file2);		 
	  break;
	case SYMLINK:
	  printf("Symlink event %i\n",
		 ee->efsd_reply_event.command.efsd_2file_cmd.id);
	  break;
	case LISTDIR:
	  printf("Listdir event %i\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case MAKEDIR:
	  printf("Mkdir event %i\n -- creating %s\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.file);
	  break;
	case CHMOD:
	  printf("Chmod event %i\n", 
		 ee->efsd_reply_event.command.efsd_chmod_cmd.id);
	  break;
	case SETMETA:
	  printf("Setmeta event %i\n", 
		 ee->efsd_reply_event.command.efsd_set_metadata_cmd.id);
	  break;
	case GETMETA:
	  printf("Getmeta event %i\n", 
		 ee->efsd_reply_event.command.efsd_get_metadata_cmd.id);
	  break;
	case STARTMON:
	  printf("Startmon event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case STOPMON:
	  printf("Stopmon event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case STAT:
	  printf("Stat event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case CLOSE:
	  printf("Close event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	default:
	  printf("UNKNOWN event\n");
	}

      if (ee->efsd_reply_event.status == SUCCESS)
	printf(" -- SUCCESS\n");
      else
	printf(" -- ERROR: %s\n", strerror(ee->efsd_reply_event.errorcode));
      
      break;
    default:
    }
  
  /* Cleanup memory allocated for this event */
  efsd_cleanup_event(ee);
}


void 
demo_sighandler(int signal)
{
  printf("Broken pipe caught.\n");
  exit(0);
}


int
main(int argc, char** argv)
{
  EfsdConnection     *ec;
  EfsdCmdId           id;
  pid_t               child;
  int                 blocking;

  /* Read command-line options. */
  if (argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")))
    {
      printf("USAGE: %s [-select]\n", argv[0]);
      exit(0);
    }

  signal(SIGPIPE, demo_sighandler);

  /* Command line option to set non-blocking mode. */
  if (argc > 1 && !strcmp(argv[1], "-select"))
    blocking = 0;
  else
    blocking = 1;
  
  /* Open connection to efsd. */
  if ( (ec = efsd_open()) == NULL)
    {
      /* Didn't work -- likely efsd is not running. */
      fprintf(stderr, "Oooops -- could not connect to efsd.\n");
      exit(-1);
    }
  
  /* Forked child reads results, while parent process launches queries. */
  if ( (child = fork()) == 0)
    {
      if (blocking)
	read_answers_blocking(ec);
      else
	read_answers_selecting(ec);
      exit(0);
    }

  /* Create a directory */
  {
    char s[4096];

    s[0] = '\0';
    strcat(s, getenv("HOME"));
    strcat(s, "//yep//efsd/can/create/dir/trees/////");
    id = efsd_makedir(ec, s);
  }

  sleep(2);

  /* Remove a file */
  id = efsd_remove(ec, "some-crappy-file-that-wont-exist");
  printf("Removing file, command ID %i\n", id);

  sleep(2);

  id = efsd_move(ec, "raster-is-flim.demo", "cK-is-flim.demo");
  printf("Moving file, command ID %i\n", id);

  /* List contents of a directory */
  id = efsd_listdir(ec, "/usr/local/enlightenment/bin");
  printf("Listing directory, command ID %i\n", id);

  sleep(2);

  /* Start monitoring home directory */
  id = efsd_start_monitor(ec, getenv("HOME"));
  printf("Starting monitor, command ID %i\n", id);

  /* Sleep a while -- you should see events if you
     for example touch files in you home during that
     time etc.

     Also try running multiple instances of this
     demo -- you should see the use counts for the
     monitor be in-/decremented.
  */
  sleep(2);

  /* Stop monitoring home directory */
  id = efsd_stop_monitor(ec, getenv("HOME"));
  printf("Stopping monitor, command ID %i\n", id);

  sleep(2);

  /* Close connection to efsd. */
  efsd_close(ec);
  kill(child, SIGKILL);

  return 0;
}
