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
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <efsd_debug.h>
#include <efsd_monitor.h>
#include <efsd_commands.h>
#include <efsd_fs.h>
#include <efsd_globals.h>
#include <efsd_io.h>
#include <efsd_macros.h>
#include <efsd_filetype.h>
#include <efsd_main.h>
#include <efsd_meta.h>
#include <efsd_meta_monitor.h>
#include <efsd_misc.h>
#include <efsd_event_queue.h>
#include <efsd_statcache.h>
#include <efsd_types.h>
#include <efsd_meta_monitor.h>


static void
errno_check(int line, char *function)
{
  if (errno == 0)
    {
      D("WARNING -- returning error with errno == 0 in line %i, %s\n",
	line, function);
      errno = EINVAL;
    }
}


#if HAVE_ECORE
static int
send_reply(EfsdCommand *cmd, int errorcode,
	   int data_len, void *data, Ecore_Ipc_Client* client)
#else
send_reply(EfsdCommand *cmd, int errorcode,
	   int data_len, void *data, int client)
#endif

{
  EfsdEvent  ee;
  
  int        sockfd;

  D_ENTER;

  #ifndef HAVE_ECORE
  sockfd = efsd_main_get_fd(client);
  #endif

  if (sockfd < 0)
    D_RETURN_(-1);

  memset(&ee, 0, sizeof(EfsdEvent));
  ee.type = EFSD_EVENT_REPLY;
  ee.efsd_reply_event.command = *cmd;
  ee.efsd_reply_event.errorcode = errorcode;
  ee.efsd_reply_event.data_len = data_len;

  /* This data chunk is supposed to be
     handled by the caller...
  */
  ee.efsd_reply_event.data = data;

  #if HAVE_ECORE
  printf("ERR: Writing event..\n");
  efsd_io_write_event(client, &ee);
  #else
  if (efsd_io_write_event(sockfd, &ee) < 0)
    {
      if (errno == EPIPE)
	efsd_main_close_connection(client);
      else
	efsd_event_queue_add_event(ev_q, client, &ee);

      D_RETURN_(-1);
    }
    #endif

  D_RETURN_(0);
}


int 
efsd_command_remove(EfsdCommand *cmd, int client)
{
  int i, options = 0;

  D_ENTER;

  /* OR together the various possible options: */
  
  for (i = 0; i < cmd->efsd_file_cmd.num_options; i++)
    {
      switch (cmd->efsd_file_cmd.options[i].type)
	{
	case EFSD_OP_FORCE:
	  options |= EFSD_FS_OP_FORCE;
	  break;
	case EFSD_OP_RECURSIVE:
	  options |= EFSD_FS_OP_RECURSIVE;
	  break;
	default:
	  D("Warning -- useless remove option %i\n",
	     cmd->efsd_file_cmd.options[i].type);
	}
    }

  /* ... and now go through the files and remove: */

  for (i = 0; i < cmd->efsd_file_cmd.num_files; i++)
    {
      if (!efsd_fs_rm(cmd->efsd_file_cmd.files[i], options))
	{
	  errno_check(__LINE__, __FUNCTION__);
	  D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
	}
    }
  
  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_move(EfsdCommand *cmd, int client)
{
  int i, options = 0;

  D_ENTER;

  for (i = 0; i < cmd->efsd_file_cmd.num_options; i++)
    {
      switch (cmd->efsd_file_cmd.options[i].type)
	{
	case EFSD_OP_FORCE:
	  options |= EFSD_FS_OP_FORCE;
	  break;
	default:
	  D("Warning -- useless copy option %i\n",
	     cmd->efsd_file_cmd.options[i].type);
	}
    }

  if (!efsd_fs_mv(cmd->efsd_file_cmd.num_files,
		  cmd->efsd_file_cmd.files,
		  options))
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }
  
  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_copy(EfsdCommand *cmd, int client)
{
  int i, options = 0;

  D_ENTER;

  for (i = 0; i < cmd->efsd_file_cmd.num_options; i++)
    {
      switch (cmd->efsd_file_cmd.options[i].type)
	{
	case EFSD_OP_FORCE:
	  options |= EFSD_FS_OP_FORCE;
	  break;
	case EFSD_OP_RECURSIVE:
	  options |= EFSD_FS_OP_RECURSIVE;
	  break;
	default:
	  D("Warning -- useless copy option %i\n",
	     cmd->efsd_file_cmd.options[i].type);
	}
    }

  if (!efsd_fs_cp(cmd->efsd_file_cmd.num_files,
		  cmd->efsd_file_cmd.files,
		  options))
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }
  
  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_symlink(EfsdCommand *cmd, int client)
{
  D_ENTER;

  if (symlink(cmd->efsd_file_cmd.files[0], cmd->efsd_file_cmd.files[1]) < 0)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_listdir(EfsdCommand *cmd, int client)
{
  int i, do_sort = FALSE;

  D_ENTER;

  for (i = 0; i < cmd->efsd_file_cmd.num_options; i++)
    {
      if (cmd->efsd_file_cmd.options[i].type == EFSD_OP_SORT)
	{
	  do_sort = TRUE;
	  break;
	}
    }

  if (efsd_monitor_start(cmd, client, TRUE, do_sort) != NULL &&
      efsd_monitor_stop(cmd, client, TRUE) >= 0)
    {      
      D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
    }

  errno_check(__LINE__, __FUNCTION__);
  D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
}


int 
efsd_command_makedir(EfsdCommand *cmd, int client)
{
  char **path_dirs;
  int    num_dirs, cur_dir, i;
  int    success = 1;

  D_ENTER;

  path_dirs = efsd_misc_get_path_dirs(cmd->efsd_file_cmd.files[0], &num_dirs);
  
  /* This is a full path, see libefsd.c */
  chdir("/");

  for (cur_dir = 0; cur_dir < num_dirs; cur_dir++)
    {
      if ( (mkdir(path_dirs[cur_dir], mode_755) < 0) &&
	   ! (errno == EEXIST && efsd_misc_file_is_dir(path_dirs[cur_dir])))
	{
	  success = 0;
	  break;
	}      
      chdir(path_dirs[cur_dir]);
    }
  
  for (i = 0; i < num_dirs; i++)
    free(path_dirs[i]);
  free(path_dirs);
  
  /* XXX this does not clean up if we had partial success ... */
  if (!success)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_chmod(EfsdCommand *cmd, int client)
{
  D_ENTER;

  if (chmod(cmd->efsd_chmod_cmd.file, cmd->efsd_chmod_cmd.mode) < 0)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_set_metadata(EfsdCommand *cmd, int client)
{
  D_ENTER;
  
  if (efsd_meta_set(cmd) < 0)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }
  
  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_get_metadata(EfsdCommand *cmd, int client)
{
  void *data;
  int   data_len, result;

  D_ENTER;
  
  if ( (data = efsd_meta_get(cmd, &data_len)) == NULL)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }
  
  result = send_reply(cmd, 0, data_len, data, client);
  FREE(data);

  D_RETURN_(result);
}


int 
efsd_command_start_monitor(EfsdCommand *cmd, Ecore_Ipc_Client* client, int dir_mode)
{
  int i, do_sort = FALSE;
  
  D_ENTER;

  D("Creating monitor command..\n");

  for (i = 0; i < cmd->efsd_file_cmd.num_options; i++)
    {
      if (cmd->efsd_file_cmd.options[i].type == EFSD_OP_SORT)
	{
	  do_sort = TRUE;
	  break;
	}
    }

  printf("Starting monitoring on %p\n", client);
  if (efsd_monitor_start(cmd, client, dir_mode, do_sort) == NULL)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));  
}


int 
efsd_command_stop_monitor(EfsdCommand *cmd, int client, int dir_mode)
{
  D_ENTER;

  if (efsd_monitor_stop(cmd, client, dir_mode) < 0)
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int  
efsd_command_start_monitor_metadata(EfsdCommand *cmd, int client)
{
  D_ENTER;

  if (!efsd_meta_monitor_add(cmd, client))
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int  
efsd_command_stop_monitor_metadata(EfsdCommand *cmd, int client)
{
  D_ENTER;

  if (!efsd_meta_monitor_del(cmd, client))
    {
      errno_check(__LINE__, __FUNCTION__);
      D_RETURN_(send_reply(cmd, errno, 0, NULL, client));
    }

  D_RETURN_(send_reply(cmd, 0, 0, NULL, client));
}


int 
efsd_command_stat(EfsdCommand *cmd, int client, char use_lstat)
{
  struct stat    st;
  int            result;

  D_ENTER;

  if (use_lstat)
    {
      if (efsd_lstat(cmd->efsd_file_cmd.files[0], &st))
	{
	  D("lstat suceeded, sending struct...\n");
	  result = send_reply(cmd, 0, sizeof(struct stat), &st, client);
	}
      else
	{
	  D("lstat failed, sending FAILURE.\n");
	  errno_check(__LINE__, __FUNCTION__);
	  result = send_reply(cmd, errno, 0, NULL, client);
	}
    }
  else
    {
      if (efsd_stat(cmd->efsd_file_cmd.files[0], &st))
	{
	  D("stat suceeded, sending struct...\n");
	  result = send_reply(cmd, 0, sizeof(struct stat), &st, client);
	}
      else
	{
	  D("stat failed, sending FAILURE.\n");
	  errno_check(__LINE__, __FUNCTION__);
	  result = send_reply(cmd, errno, 0, NULL, client);
	}
    }

  D_RETURN_(result);
}


int  
efsd_command_readlink(EfsdCommand *cmd, int client)
{
  char           s[MAXPATHLEN];
  int            result, n;

  D_ENTER;

  if ((n = readlink(cmd->efsd_file_cmd.files[0], s, MAXPATHLEN)) >= 0)
    {
      s[n] = '\0';
      result = send_reply(cmd, 0, n, s, client);
    }
  else
    {
      errno_check(__LINE__, __FUNCTION__);
      result = send_reply(cmd, errno, 0, NULL, client);
    }

  D_RETURN_(result);
}


#if HAVE_ECORE
int  
efsd_command_get_filetype(EfsdCommand *cmd, Ecore_Ipc_Client* client)
#else
int  
efsd_command_get_filetype(EfsdCommand *cmd, int client)
#endif
	
{
  char  type[MAXPATHLEN];
  int   result;

  D_ENTER;

  /*printf ("ERR: Running get filetype on %s\n",cmd->efsd_file_cmd.files[0] );*/

  if (efsd_filetype_get(cmd->efsd_file_cmd.files[0], type, MAXPATHLEN))
    {
      D("FILE lookup succeded: %s\n", type);
      /*printf("ERR: Type is %s\n", type);*/
      result = send_reply(cmd, 0, strlen(type)+1, type, client);
    }
  else
    {
      D("FILE lookup failed -- sending FAILURE.\n");
      errno_check(__LINE__, __FUNCTION__);
      result = send_reply(cmd, errno, 0, NULL, client);
    }

  D_RETURN_(result);
}
