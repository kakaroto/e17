/* 

                          --- Efsd Shell ---


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
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libefsd.h>
#include <fam.h>

void event_loop(EfsdConnection *ec);
void handle_efsd_event(EfsdEvent *ee);


void event_loop(EfsdConnection *ec)
{
  EfsdEvent ee;
  fd_set    fdset;
  int       fdsize;
  int       fd;

  fd = efsd_get_connection_fd(ec);

  for ( ; ; )
    {
      FD_ZERO(&fdset);
      fdsize = 0;

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
      if (ee->efsd_reply_event.status != SUCCESS)
	{
	  printf("failed: %s\n", strerror(ee->efsd_reply_event.errorcode));
	  break;
	}

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
	case EFSD_CMD_COPY:
	  printf("Copy event %i\n -- copying %s to %s\n",
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
	case EFSD_CMD_LSTAT:
	  {
	    struct stat *st;

	    st = (struct stat*) ee->efsd_reply_event.data;
	    
	    if (S_ISREG(st->st_mode))
	      printf("%s is a regular file, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);
	    
	    if (S_ISLNK(st->st_mode))
	      printf("%s is a symlink, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);
	    
	    if (S_ISDIR(st->st_mode))
	      printf("%s is a directory, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);

	    if (S_ISCHR(st->st_mode))
	      printf("%s is a character device, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);

	    if (S_ISBLK(st->st_mode))
	      printf("%s is a block device, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);

	    if (S_ISFIFO(st->st_mode))
	      printf("%s is a fifo, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);

	    if (S_ISSOCK(st->st_mode))
	      printf("%s is a socket file, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.file);

	    printf("dev %lu, ino %lu, size %lu.\n",
		   (long unsigned int)st->st_dev, st->st_ino, st->st_size);
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
	case EFSD_CMD_GETFILETYPE:
	  printf("Getfile event %i on %s\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.file);		 
	  if (ee->efsd_reply_event.status == SUCCESS)
	    {
	      printf("filetype is %s\n", (char*)ee->efsd_reply_event.data);
	    }
	  break;
	case EFSD_CMD_CLOSE:
	  printf("Close event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	default:
	  printf("UNKNOWN event\n");
	}
      
      break;
    default:
    }
  
  /* Cleanup memory allocated for this event */
  efsd_event_cleanup(ee);
}


void 
sighandler(int signal)
{
  printf("Broken pipe caught.\n");
  exit(0);
}


void
print_help(void)
{
  printf("efsdsh -- command line interface to efsd.\n\n"
	 "USAGE:\n\n"
	 "You can enter the following commands:\n\n"
	 "ls <file>                           Shows directory contents\n"
	 "startmon <file>                     Starts monitoring file or dir\n"
	 "stopmon <file>                      Stops monitoring file or dir\n"
	 "gettype <file>                      Returns file type of file\n"
	 "getstat <file>                      Returns result of stat on file.\n"
	 "getlstat <file>                     Returns result of lstat on file.\n"
	 "setmeta <file> <key> <type> <val>   Set metadata for file, where\n"
	 "                                    type can be int, float or str,\n"
	 "                                    key is the key for the item, and\n"
	 "                                    val is its value.\n"
	 "getmeta <file> <key> <type>         Returns metadata for given file\n"
	 "                                    and type.\n"        
	 "exit, quit                          Guess what.\n"
	 "ln <source> <target>                Symlink source to target\n"
	 "cp <source> <target>                Copy source to target\n"
	 "mv <source> <target>                Move source to target\n\n"
	 "rm <file>                           Remove file.\n\n"
	 "Options:\n"
	 "For ls and startmon:\n"
	 "-a                                  All files, also those starting\n"
         "                                    with a dot.\n"
	 "-s                                  Get stat as well.\n" 
	 "-t                                  Get file type as well.\n\n"
	 "For cp, mv, rm:\n"
	 "-f                                  Force. Ignore nonexistant files\n"
	 "                                    when removing, or existing ones\n"
	 "                                    when copying.\n"
	 "-r                                  Recursive mode.\n");
}


void 
command_line(EfsdConnection *ec)
{
  EfsdCmdId  id;
  char       s[4096];
  char      *tok;
  int        num_options;

  while (fgets(s, 4096, stdin))
    {
      num_options = 0;

      if ((tok = strtok(s, " \t\n")))
	{
	  if (!strcmp(tok, "exit") || !strcmp(tok, "quit"))
	    return;
	  else if (!strcmp(tok, "help"))
	    print_help();
	  else if (!strcmp(tok, "gettype"))
	    {
	      if ((tok = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_get_filetype(ec, tok)) < 0)
		    printf("Couldn't issue gettype command.\n");
		}
	    }
	  else if (!strcmp(tok, "getstat"))
	    {
	      if ((tok = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_stat(ec, tok)) < 0)
		    printf("Couldn't issue stat command.\n");
		}
	    }
	  else if (!strcmp(tok, "getlstat"))
	    {
	      if ((tok = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_lstat(ec, tok)) < 0)
		    printf("Couldn't issue lstat command.\n");
		}
	    }
	  else if (!strcmp(tok, "setmeta"))
	    {
	      char *file, *key, *type, *val;
	      void *data;
	      int   len;
	      EfsdDatatype datatype;

	      if ((file = strtok(NULL, " \t\n")) &&
		  (key = strtok(NULL, " \t\n")) &&
		  (type = strtok(NULL, " \t\n")) &&
		  (val = strtok(NULL, " \t\n")))
		{
		  if (!strcmp(type, "int"))
		    {
		      datatype = EFSD_INT;
		      len = sizeof(int);
		      data = malloc(len);		      
		      sscanf(val, "%i", (int*)data);
		    }
		  else if (!strcmp(type, "float"))
		    {
		      datatype = EFSD_FLOAT;
		      len = sizeof(float);
		      data = malloc(len);
		      sscanf(val, "%f", (float*)data);
		    }
		  else if (!strcmp(type, "str"))
		    {
		      datatype = EFSD_STRING;
		      len = strlen(val) + 1;
		      data = val;
		    }
		  else
		    {
		      printf("Unknown data type.\n");
		      continue;
		    }
		  
		  if ((id = efsd_set_metadata_raw(ec, key, file,
						  datatype, len, data)) < 0)
		    printf("Couldn't issue setmeta command.\n");
		}
	    }
	  else if (!strcmp(tok, "getmeta"))
	    {
	      char *file, *key, *type;
	      EfsdDatatype datatype;

	      if ((file = strtok(NULL, " \t\n")) &&
		  (key = strtok(NULL, " \t\n")) &&
		  (type = strtok(NULL, " \t\n")))
		{
		  if (!strcmp(type, "int"))
		    datatype = EFSD_INT;
		  else if (!strcmp(type, "float"))
		    datatype = EFSD_FLOAT;
		  else if (!strcmp(type, "str"))
		    datatype = EFSD_STRING;
		  else
		    {
		      printf("Unknown data type.\n");
		      continue;
		    }

		  if ((id = efsd_get_metadata(ec, key, file, datatype)) < 0)
		    printf("Couldn't issue getmeta command.\n");
		}
	    }
	  else if (!strcmp(tok, "ln"))
	    {
	      char *file1, *file2;
	      
	      if ((file1 = strtok(NULL, " \t\n")) &&
		  (file2 = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_symlink(ec, file1, file2)) < 0)
		    printf("Couldn't issue ln command.\n");
		}
	    }
	  else if (!strcmp(tok, "cp") || !strcmp(tok, "mv"))
	    {
	      char force = 0;
	      char rec = 0;
	      char move = 0;

	      if (!strcmp(tok, "mv"))
		move = 1;

	      while ((tok = strtok(NULL, " \t\n")))
		{
		  if (!strcmp(tok, "-f"))
		    {
		      num_options++;
		      force = 1;
		    }
		  else if (!strcmp(tok, "-r"))
		    {
		      num_options++;
		      rec = 1;
		    }
		  else
		    {
		      char *target;
		      EfsdOptions *ops = NULL;
		      
		      if (num_options > 0)
			{
			  ops = efsd_ops_create(num_options);
			  
			  if (force) efsd_ops_add(ops, efsd_op_force());
			  if (rec)   efsd_ops_add(ops, efsd_op_recursive());
			}
		      
		      if ((target = strtok(NULL, " \t\n")))
			{
			  if (move)
			    {
			      if ((id = efsd_move(ec, tok, target, ops)) < 0)
				printf("Couldn't issue mv command.\n");
			    }
			  else
			    {
			      if ((id = efsd_copy(ec, tok, target, ops)) < 0)
				printf("Couldn't issue cp command.\n");
			    }
			}
		    }
		}
	    }
	  else if (!strcmp(tok, "ls") || !strcmp(tok, "startmon"))
	    {
	      char startmon = 0;
	      char show_all = 0;
	      char get_stat = 0;
	      char get_type = 0;

	      if (!strcmp(tok, "startmon"))
		startmon = 1;

	      while ((tok = strtok(NULL, " \t\n")))
		{
		  if (!strcmp(tok, "-a"))
		    {
		      num_options++;
		      show_all = 1;
		    }
		  else if (!strcmp(tok, "-s"))
		    {
		      num_options++;
		      get_stat = 1;
		    }
		  else if (!strcmp(tok, "-t"))
		    {
		      num_options++;
		      get_type = 1;
		    }
		  else
		    {
		      EfsdOptions *ops = NULL;

		      if (num_options > 0)
			{
			  ops = efsd_ops_create(num_options);

			  if (show_all) efsd_ops_add(ops, efsd_op_all());
			  if (get_stat) efsd_ops_add(ops, efsd_op_get_stat());
			  if (get_type) efsd_ops_add(ops, efsd_op_get_filetype());
			}

		      if (startmon)
			{
			  if ((id = efsd_start_monitor(ec, tok, ops)) < 0)
			    printf("Couldn't issue startmon command.\n");
			}
		      else
			{
			  if ((id = efsd_listdir(ec, tok, ops)) < 0)
			    printf("Couldn't issue ls command.\n");
			}
		    }
		}
	    }
	  else if (!strcmp(tok, "rm"))
	    {
	      char force = 0;
	      char rec   = 0;

	      while ((tok = strtok(NULL, " \t\n")))
		{
		  if (!strcmp(tok, "-f"))
		    {
		      num_options++;
		      force = 1;
		    }
		  else if (!strcmp(tok, "-r"))
		    {
		      num_options++;
		      rec = 1;
		    }
		  else
		    {
		      EfsdOptions *ops = NULL;

		      if (num_options > 0)
			{
			  ops = efsd_ops_create(num_options);

			  if (force) efsd_ops_add(ops, efsd_op_force());
			  if (rec)   efsd_ops_add(ops, efsd_op_recursive());
			}

		      if ((id = efsd_remove(ec, tok, ops)) < 0)
			printf("Couldn't issue rs command.\n");
		    }
		}
	    }
	  else if (!strcmp(tok, "stopmon"))
	    {
	      if ((tok = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_stop_monitor(ec, tok)) < 0)
		    printf("Couldn't issue stopmon command.\n");
		}
	    }
	}
    }
}


int
main(int argc, char** argv)
{
  EfsdConnection     *ec;
  pid_t               child;

  signal(SIGPIPE, sighandler);

  /* Open connection to efsd. */
  if ( (ec = efsd_open()) == NULL)
    {
      /* Didn't work -- likely efsd is not running. */
      fprintf(stderr, "Oooops -- could not connect to efsd.\n");
      exit(-1);
    }
  
  /* Forked child reads results, while parent process
     launches queries and handles command line. */
  if ( (child = fork()) == 0)
    {
      event_loop(ec);
      exit(0);
    }

  /* Parent goes interactive: */
  command_line(ec);

  /* Close connection to efsd. */
  if (efsd_close(ec) >= 0)
    printf ("Connection closed successfully.\n");

  kill(child, SIGKILL);

  return 0;
}
