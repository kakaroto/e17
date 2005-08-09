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
  int       fd;

  fd = efsd_get_connection_fd(ec);

  for ( ; ; )
    {
      FD_ZERO(&fdset);
      FD_SET(fd, &fdset);

      /* Wait for next event to happen ... */
      select(fd + 1, &fdset, NULL, NULL, NULL);

      if (FD_ISSET(fd, &fdset))
	{	  
	  while (efsd_events_pending(ec))
	    {
	      if (efsd_next_event(ec, &ee) >= 0)
		handle_efsd_event(&ee);
	      else
		exit(-1);
	    }
	}
    }
}


void handle_efsd_event(EfsdEvent *ee)
{
  int i;

  switch (ee->type)
    {
    case EFSD_EVENT_FILECHANGE:
      switch (ee->efsd_filechange_event.changetype)
	{
	case EFSD_FILE_CHANGED:
	  printf("Filechange event for cmd %i: %s changed.\n", 
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_DELETED:
	  printf("Filechange event for cmd %i: %s deleted.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_START_EXEC:
	  printf("Filechange event for cmd %i: %s started.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_STOP_EXEC:
	  printf("Filechange event for cmd %i: %s stopped.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_CREATED:
	  printf("Filechange event for cmd %i: %s created.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_MOVED:
	  printf("Filechange event for cmd %i: %s moved.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_ACKNOWLEDGE:
	  printf("Filechange event for cmd %i: %s acked.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_EXISTS:
	  printf("Filechange event for cmd %i: %s exists.\n",
		 ee->efsd_filechange_event.id,
		 ee->efsd_filechange_event.file);
	  break;
	case EFSD_FILE_END_EXISTS:
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
      if (ee->efsd_reply_event.errorcode != 0)
	{
	  printf("failed: %s\n", strerror(ee->efsd_reply_event.errorcode));
	  break;
	}

      switch (ee->efsd_reply_event.command.type)
	{
	case EFSD_CMD_REMOVE:
	  printf("Remove event %i\n -- removed ",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);

	  for (i = 0; i < ee->efsd_reply_event.command.efsd_file_cmd.num_files; i++)
	    printf("%s ", ee->efsd_reply_event.command.efsd_file_cmd.files[i]);
	  printf("\n");
	  break;
	case EFSD_CMD_MOVE:
	  printf("Move event %i\n -- moved ",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);

	  for (i = 0; i < ee->efsd_reply_event.command.efsd_file_cmd.num_files-1; i++)
	    printf("%s ", ee->efsd_reply_event.command.efsd_file_cmd.files[i]);
	  printf(" to %s.", ee->efsd_reply_event.command.efsd_file_cmd.files[ee->efsd_reply_event.command.efsd_file_cmd.num_files-1]);
	  break;
	case EFSD_CMD_COPY:
	  printf("Copy event %i\n -- copy ",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);

	  for (i = 0; i < ee->efsd_reply_event.command.efsd_file_cmd.num_files-1; i++)
	    printf("%s ", ee->efsd_reply_event.command.efsd_file_cmd.files[i]);
	  printf(" to %s.", ee->efsd_reply_event.command.efsd_file_cmd.files[ee->efsd_reply_event.command.efsd_file_cmd.num_files-1]);
	  break;
	case EFSD_CMD_SYMLINK:
	  printf("Symlink event %i\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_LISTDIR:
	  printf("Listdir event %i\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_MAKEDIR:
	  printf("Mkdir event %i\n -- created %s\n",
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.files[0]);
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

	  if (ee->efsd_reply_event.errorcode == 0)
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
		default: ;
		}
	    }
	  break;
	case EFSD_CMD_STARTMON_FILE:
	  printf("Startmon event for file %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STARTMON_DIR:
	  printf("Startmon event for dir %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STARTMON_META:
	  printf("Startmon event for metadata %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STOPMON_FILE:
	  printf("Stopmon event for file %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STOPMON_DIR:
	  printf("Stopmon event for dir %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STOPMON_META:
	  printf("Stopmon event for metadata %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  break;
	case EFSD_CMD_STAT:
	case EFSD_CMD_LSTAT:
	  {
	    struct stat *st;

	    st = (struct stat*) ee->efsd_reply_event.data;

	    if (S_ISREG(st->st_mode))
	      printf("%s is a regular file, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);
	    
	    if (S_ISLNK(st->st_mode))
	      printf("%s is a symlink, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);
	    
	    if (S_ISDIR(st->st_mode))
	      printf("%s is a directory, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);

	    if (S_ISCHR(st->st_mode))
	      printf("%s is a character device, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);

	    if (S_ISBLK(st->st_mode))
	      printf("%s is a block device, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);

	    if (S_ISFIFO(st->st_mode))
	      printf("%s is a fifo, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);

	    if (S_ISSOCK(st->st_mode))
	      printf("%s is a socket file, ",
		     ee->efsd_reply_event.command.efsd_file_cmd.files[0]);

	    printf("dev %lu, ino %lu, size %lu.\n",
		   (long unsigned int)st->st_dev, st->st_ino, st->st_size);
	  }
	  break;
	case EFSD_CMD_READLINK:
	  printf("Readlink event %i\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id);
	  if (ee->efsd_reply_event.errorcode == 0)
	    {
	      printf("target is %s\n", (char*)ee->efsd_reply_event.data);
	    }
	  break;
	case EFSD_CMD_GETFILETYPE:
	  printf("Gettype event %i on %s\n", 
		 ee->efsd_reply_event.command.efsd_file_cmd.id,
		 ee->efsd_reply_event.command.efsd_file_cmd.files[0]);		 
	  if (ee->efsd_reply_event.errorcode == 0)
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
    case EFSD_EVENT_METADATA_CHANGE:
      {
	printf("Metadata change event %i on %s, key %s\n",
	       ee->efsd_metachange_event.id,
	       ee->efsd_metachange_event.file,
	       ee->efsd_metachange_event.key);

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
	  default: ;
	  }
      }
    default: ;
    }
  
  /* Cleanup memory allocated for this event */
  
  efsd_event_cleanup(ee);
}


void 
sighandler(int signal)
{
  switch (signal)
    {
    case SIGPIPE:
      printf("Broken pipe caught.\n");
      break;
    default: ;
    }

  exit(-1);
}


void
print_help(void)
{
  printf("efsdsh -- command line interface to efsd.\n\n"
	 "USAGE:\n\n"
	 "You can enter the following commands:\n\n"
	 "ls <file>                           Shows directory contents\n"
	 "mon_file <file>                     Starts monitoring file\n"
	 "mon_dir <file>                      Starts monitoring dir\n"
	 "mon_meta <file> <key>               Starts monitoring metadata\n"
	 "stopmon_file <file>                 Stops monitoring file\n"
	 "stopmon_dir <file>                  Stops monitoring dir\n"
	 "stopmon_meta <file> <key>           Stops monitoring metadata\n"
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
	 "cp <source files> <target>          Copy sources to target\n"
	 "mv <source files> <target>          Move sources to target\n\n"
	 "rm <files>                          Remove files.\n\n"
	 "Options:\n"
	 "For ls and mon_XXX:\n"
	 "-a                                  All files, also those starting\n"
         "                                    with a dot.\n"
	 "-s                                  Get stat as well.\n" 
	 "-ls                                 Get lstat as well.\n" 
	 "-t                                  Get file type as well.\n"
	 "-sort                               Sort replies alphabetically.\n"
	 "-m <key> <type>                     Get metadata with given key and type\n"
	 "                                    as well.\n\n"
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

  printf("> ");
  fflush(stdout);

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
	      char **files = NULL;
	      int  num_files = 0;
	      EfsdOptions *ops = NULL;

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
		      num_files++;
		      files = realloc(files, sizeof(char*) * num_files);
		      files[num_files-1] = tok;
		    }
		}
		      
	      if (num_options > 0)
		{
		  ops = efsd_ops_create();
		  
		  if (force) efsd_ops_add(ops, efsd_op_force());
		  if (rec)   efsd_ops_add(ops, efsd_op_recursive());
		}
	      
	      if (move)
		{
		  if ((id = efsd_move(ec, num_files, files, ops)) < 0)
		    printf("Couldn't issue mv command.\n");
		}
	      else
		{
		  if ((id = efsd_copy(ec, num_files, files, ops)) < 0)
		    printf("Couldn't issue cp command.\n");
		}
	    }
	  else if (!strcmp(tok, "ls") || !strcmp(tok, "mon_dir") || !strcmp(tok, "mon_file"))
	    {
	      char mon_file = 0, mon_dir = 0,  show_all = 0, get_type = 0;
	      char get_stat = 0, get_lstat = 0, get_meta = 0, sort = 0;
	      char *meta_key = NULL, *meta_type = NULL;
	      EfsdDatatype meta_datatype = 0;

	      if (!strcmp(tok, "mon_file"))
		mon_file = 1;
	      else if (!strcmp(tok, "mon_dir"))
		mon_dir = 1;

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
		  else if (!strcmp(tok, "-ls"))
		    {
		      num_options++;
		      get_lstat = 1;
		    }
		  else if (!strcmp(tok, "-t"))
		    {
		      num_options++;
		      get_type = 1;
		    }
		  else if (!strcmp(tok, "-sort"))
		    {
		      num_options++;
		      sort = 1;
		    }
		  else if (!strcmp(tok, "-m"))
		    {
		      if ((meta_key = strtok(NULL, " \t\n")) &&
			  (meta_type = strtok(NULL, " \t\n")))
			{
			  get_meta = 1;
			  num_options++;
			  
			  if (!strcmp(meta_type, "int"))
			    meta_datatype = EFSD_INT;
			  else if (!strcmp(meta_type, "float"))
			    meta_datatype = EFSD_FLOAT;
			  else if (!strcmp(meta_type, "str"))
			    meta_datatype = EFSD_STRING;
			  else
			    {
			      get_meta = 0;
			      num_options--;
			      printf("Unknown data type.\n");
			    }			  
			}
		    }
		  else
		    {
		      EfsdOptions *ops = NULL;

		      if (num_options > 0)
			{
			  ops = efsd_ops_create();

			  if (show_all)  efsd_ops_add(ops, efsd_op_list_all());
			  if (get_stat)  efsd_ops_add(ops, efsd_op_get_stat());
			  if (get_lstat) efsd_ops_add(ops, efsd_op_get_lstat());
			  if (get_type)  efsd_ops_add(ops, efsd_op_get_filetype());
			  if (get_meta)  efsd_ops_add(ops, efsd_op_get_metadata(meta_key, meta_datatype));
			  if (sort)      efsd_ops_add(ops, efsd_op_sort());
			}

		      if (mon_dir)
			{
			  if ((id = efsd_start_monitor(ec, tok, ops, TRUE)) < 0)
			    printf("Couldn't issue mon_dir command.\n");
			}
		      else if (mon_file)
			{
			  if ((id = efsd_start_monitor(ec, tok, ops, FALSE)) < 0)
			    printf("Couldn't issue mon_file command.\n");
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
	      char **files = NULL;
	      int  num_files = 0;
	      EfsdOptions *ops = NULL;

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
		      num_files++;
		      files = realloc(files, sizeof(char*) * num_files);
		      files[num_files-1] = tok;
		    }
		}

	      if (num_options > 0)
		{
		  ops = efsd_ops_create();
		  
		  if (force) efsd_ops_add(ops, efsd_op_force());
		  if (rec)   efsd_ops_add(ops, efsd_op_recursive());
		}
	      
	      if ((id = efsd_remove(ec, num_files, files, ops)) < 0)
		printf("Couldn't issue rs command.\n");
	    }
	  else if (!strcmp(tok, "stopmon_dir"))
	    {
	      if ((tok = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_stop_monitor(ec, tok, TRUE)) < 0)
		    printf("Couldn't issue stopmon_dir command.\n");
		}
	    }
	  else if (!strcmp(tok, "stopmon_file"))
	    {
	      if ((tok = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_stop_monitor(ec, tok, FALSE)) < 0)
		    printf("Couldn't issue stopmon_file command.\n");
		}
	    }
	  else if (!strcmp(tok, "mon_meta"))
	    {
	      char *file;
	      char *key;

	      if ((file = strtok(NULL, " \t\n")) &&
		  (key = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_start_monitor_metadata(ec, file, key)) < 0)
		    printf("Couldn't issue mon_meta command.\n");		    
		}
	    }
	  else if (!strcmp(tok, "stopmon_meta"))
	    {
	      char *file;
	      char *key;

	      if ((file = strtok(NULL, " \t\n")) &&
		  (key = strtok(NULL, " \t\n")))
		{
		  if ((id = efsd_stop_monitor_metadata(ec, file, key)) < 0)
		    printf("Couldn't issue mon_meta command.\n");		    
		}
	    }
	  else
	    {
	      printf("Invalid command.\n");
	    }
	}

      printf("> ");
      fflush(stdout);
    }
}


int
main(int argc, char** argv)
{
  EfsdConnection     *ec;
  pid_t               child;

  signal(SIGPIPE, sighandler);
  signal(SIGCHLD, sighandler);

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

  argc = 0;
  argv = NULL;
}
