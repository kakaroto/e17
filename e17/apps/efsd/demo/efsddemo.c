/* 

                          --- Efsd Demo ---


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
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libefsd.h>
#include <fam.h>

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

  while (efsd_wait_event(ec, &ee) >= 0)
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
	  
	  while (efsd_events_pending(ec))
	    {
	      if (efsd_next_event(ec, &ee) >= 0)
		handle_efsd_event(&ee);
	    }
	}
    }
}


void handle_efsd_event(EfsdEvent *ee)
{
  switch (ee->type)
    {
    case EFSD_EVENT_FILECHANGE:
      switch (ee->efsd_filechange_event.changetype)
	{
	case EFSD_CHANGE_CHANGED:
	  printf("Filechange event for cmd %i: %s changed.\n", 
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_DELETED:
	  printf("Filechange event for cmd %i: %s deleted.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_START_EXEC:
	  printf("Filechange event for cmd %i: %s started.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_STOP_EXEC:
	  printf("Filechange event for cmd %i: %s stopped.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_CREATED:
	  printf("Filechange event for cmd %i: %s created.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_MOVED:
	  printf("Filechange event for cmd %i: %s moved.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_ACKNOWLEDGE:
	  printf("Filechange event for cmd %i: %s acked.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_EXISTS:
	  printf("Filechange event for cmd %i: %s exists.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_CHANGE_END_EXISTS:
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
    case EFSD_EVENT_REPLY:
      switch (ee->efsd_reply_event.command.type)
	{
	case EFSD_CMD_REMOVE:
	  printf("Remove event %i\n -- removing %s\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.file);		 
	  break;
	case EFSD_CMD_MOVE:
	  printf("Move event %i\n -- moving %s to %s\n",
		 ee->efsd_reply_event.command.efsd_2file_cmd.id,
		 ee->efsd_reply_event.command.efsd_2file_cmd.file1,		 
		 ee->efsd_reply_event.command.efsd_2file_cmd.file2);		 
	  break;
	case EFSD_CMD_SYMLINK:
	  printf("Symlink event %i\n",
		 ee->efsd_reply_event.command.efsd_2file_cmd.id);
	  break;
	case EFSD_CMD_LISTDIR:
	  printf("Listdir event %i\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_MAKEDIR:
	  printf("Mkdir event %i\n -- creating %s\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.file);
	  break;
	case EFSD_CMD_CHMOD:
	  printf("Chmod event %i\n", 
		 ee->efsd_reply_event.command.efsd_chmod_cmd.id);
	  break;
	case EFSD_CMD_SETMETA:
	  printf("Setmeta event %i\n", 
		 ee->efsd_reply_event.command.efsd_set_metadata_cmd.id);
	  break;
	case EFSD_CMD_GETMETA:
	  printf("Getmeta event %i\n", 
		 ee->efsd_reply_event.command.efsd_get_metadata_cmd.id);

	  if (ee->efsd_reply_event.status == SUCCESS)
	    {
	      switch (efsd_metadata_get_type(ee))
		{
		case EFSD_INT:
		  {
		    int val;

		    efsd_metadata_get_int(ee, &val);
		    printf("File: %s, key: %s --> val: %i\n",
			   efsd_metadata_get_file(ee),
			   efsd_metadata_get_key(ee),
			   val);
		  }
		  break;
		case EFSD_FLOAT:
		  {
		    float val;

		    efsd_metadata_get_float(ee, &val);
		    printf("File: %s, key: %s --> val: %f\n",
			   efsd_metadata_get_file(ee),
			   efsd_metadata_get_key(ee),
			   val);
		  }
		  break;
		case EFSD_STRING:
		  printf("File: %s, key: %s --> val: %s\n",
			 efsd_metadata_get_file(ee),
			 efsd_metadata_get_key(ee),
			 efsd_metadata_get_str(ee));
		  break;
		default:
		}
	    }
	  break;
	case EFSD_CMD_STARTMON:
	  printf("Startmon event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STOPMON:
	  printf("Stopmon event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STAT:
	  {
	    struct stat *st;

	    printf("Stat event %i stating file %s\n", 
		   ee->efsd_reply_event.command.efsd_file_cmd.id,
		   ee->efsd_reply_event.command.efsd_file_cmd.file);
		   

	    st = (struct stat*) ee->efsd_reply_event.data;
	    
	    if (S_ISREG(st->st_mode))
		printf("%s is a regular file.\n",
		       ee->efsd_reply_event.command.efsd_file_cmd.file);

	    if (S_ISLNK(st->st_mode))
		printf("%s is a symlink.\n",
		       ee->efsd_reply_event.command.efsd_file_cmd.file);

	    if (S_ISDIR(st->st_mode))
	      printf("%s is a directory.\n",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);

	  }
	  break;
	case EFSD_CMD_READLINK:
	  printf("Readlink event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  if (ee->efsd_reply_event.status == SUCCESS)
	    {
	      printf("target is %s\n", (char*)ee->efsd_reply_event.data);
	    }
	  break;
	case EFSD_CMD_GETMIME:
	  printf("Getmime event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  if (ee->efsd_reply_event.status == SUCCESS)
	    {
	      printf("mimetype is %s\n", (char*)ee->efsd_reply_event.data);
	    }
	  break;
	case EFSD_CMD_CLOSE:
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
  efsd_event_cleanup(ee);
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
  int                 blocking, i;

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

  /* MIME type tests */
  for (i = (blocking ? 1 : 2); i < argc; i++)
    {
      if ((id = efsd_get_mimetype(ec, argv[i])) >= 0)
	printf("Requesting mimetype for '%s'.\n", argv[i]);
      else
	printf("Couldn't issue getmime command.\n");      
    }

  sleep(2);

  /* Set some metadata */

  {
    int   x = 123;
    char *icon = "/path/to/some/icon.png";

    if ((id = efsd_set_metadata(ec, "/x", "efsddemo.c", EFSD_INT,
				sizeof(int), &x)) >= 0)
      printf("Setting metadata, command ID %i\n", id);
    else
      printf("Couldn't issue setmetadata command.\n");

    if ((id = efsd_set_metadata(ec, "/icon/normal", "/bin/ls", EFSD_STRING,
				strlen(icon)+1, icon)) >= 0)
      printf("Setting metadata, command ID %i\n", id);
    else
      printf("Couldn't issue setmetadata command.\n");
  }
  

  sleep(2);

  /* Get some metadata */
  if ((id = efsd_get_metadata(ec, "/x", "efsddemo.c", EFSD_INT)) >= 0)
    printf("Getting metadata, command ID %i\n", id);
  else
    printf("Couldn't issue getmetadata command.\n");

  if ((id = efsd_get_metadata(ec, "/icon/normal", "/bin/ls", EFSD_STRING)) >= 0)
    printf("Getting metadata, command ID %i\n", id);
  else
    printf("Couldn't issue getmetadata command.\n");

  sleep(2);

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

  if ((id = efsd_remove(ec, "some-crappy-file-that-wont-exist")) >= 0)
    printf("Removing file, command ID %i\n", id);
  else
    printf("Couldn't issue rm command.\n");

  sleep(2);


  if ((id = efsd_move(ec, "raster-is-flim.demo", "cK-is-flim.demo")) >= 0)
    printf("Moving file, command ID %i\n", id);
  else
    printf("Couldn't issue mv command.\n");

  sleep(2);

  /* List contents of a directory */
  if ((id = efsd_listdir(ec, "/usr/local/devel/sourceforge/e17/apps/efsd/demo", 2,
			 efsd_op_ls_get_stat(),
			 efsd_op_ls_get_mimetype())) >= 0)
    printf("Listing directory, command ID %i\n", id);
  else
    printf("Couldn't issue ls command.\n");
  
  sleep(2);

  /* Stat a file */
  if ((id = efsd_stat(ec, "/bin/")) >= 0)
    printf("Stat()ing file, command ID %i\n", id);
  else
    printf("Couldn't issue stat command.\n");

  sleep(2);

  /* Readlink a file */
  if ((id = efsd_readlink(ec, "horms-is-flim.demo")) >= 0)
    printf("Readlink file, command ID %i\n", id);
  else
    printf("Couldn't issue readlink command.\n");

  sleep(2);

    /*id = efsd_start_monitor(ec, getenv("HOME")); */

  /* Start monitoring home directory */

  if ((id = efsd_start_monitor(ec, "/dev")) >= 0)
    printf("Starting monitor, command ID %i\n", id);
  else
    printf("Couldn't issue startmon command.\n");

  /* Sleep a while -- you should see events if you
     for example touch files in you home during that
     time etc.

     Also try running multiple instances of this
     demo -- you should see the use counts for the
     monitor be in-/decremented.
  */

  sleep(10);

  /* Stop monitoring home directory */

  if ((id = efsd_stop_monitor(ec, "/dev")) >= 0)
    printf("Stopping monitor, command ID %i\n", id);
  else
    printf("Couldn't issue stopmon command.\n");

  sleep(10);

  /* Close connection to efsd. */
  if (efsd_close(ec) >= 0)
    printf ("Connection closed successfully.\n");

  kill(child, SIGKILL);

  return 0;
}
