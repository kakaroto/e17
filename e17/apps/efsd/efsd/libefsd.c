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
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#ifdef __EMX__
#include <strings.h>  /* eeek... OS/2 has bzero(...) there */
#include <sys/select.h>
#define getcwd _getcwd2
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_io.h>
#include <efsd_common.h>
#include <efsd_options.h>
#include <efsd_macros.h>
#include <libefsd.h>

struct efsd_connection
{
  int        fd;
};


static int       send_command(EfsdConnection *ec, EfsdCommand *com);
static EfsdCmdId get_next_id(void);
static EfsdCmdId file_cmd(EfsdConnection *ec, EfsdCommandType type, char *file,
			  int num_options, EfsdOption *ops);
static EfsdCmdId twofile_cmd(EfsdConnection *ec, EfsdCommandType type,
			     char *file1, char *file2);

static char*
get_full_path(char *file)
{
  char *result = NULL;

  D_ENTER;

  if (!file || !file[0])
    D_RETURN_(NULL);
  
#ifndef __EMX__
  if (file[0] == '/')
#else  
  if (_fnisabs(file))
#endif  
    D_RETURN_(strdup(file));

  result = getcwd(NULL, 0);
  result = realloc(result, strlen(result) + strlen(file) + 2);
  strcat(result, "/");
  strcat(result, file);

  D_RETURN_(result);
}


static int
send_command(EfsdConnection *ec, EfsdCommand *com)
{
  D_ENTER;

  if (!ec || !com)
    D_RETURN_(-1);

  if (efsd_io_write_command(ec->fd, com) < 0)
    {
      fprintf(stderr, "libefsd: couldn't write command.\n");
      D_RETURN_(-1);
    }
  
  D_RETURN_(0);
}


static EfsdCmdId 
get_next_id(void)
{
  static EfsdCmdId id_counter = 0;

  D_ENTER;
  D_RETURN_(++id_counter);
}


static EfsdCmdId 
file_cmd(EfsdConnection *ec, EfsdCommandType type, char *file,
	 int num_options, EfsdOption *ops)
{
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !file || file[0] == '\0')
    D_RETURN_(-1);

  bzero(&cmd, sizeof(EfsdCommand));

  cmd.type = type;
  cmd.efsd_file_cmd.id = get_next_id();
  cmd.efsd_file_cmd.file = get_full_path(file);

  if ((num_options > 0) && (ops))
    {
      cmd.efsd_file_cmd.num_options = num_options;
      cmd.efsd_file_cmd.options = ops;
    }

  if (send_command(ec, &cmd) < 0)
    {
      D_RETURN_(-1);
    }

  if ((num_options > 0) && (ops))
    {
      int i;

      for (i = 0; i < num_options; i++)
	efsd_option_cleanup(&ops[i]);

      FREE(ops);
    }

  D_RETURN_(cmd.efsd_file_cmd.id);
}


static EfsdCmdId 
twofile_cmd(EfsdConnection *ec, EfsdCommandType type, char *file1, char *file2)
{
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !file1 || file1[0] == '\0' || !file2 || file2[0] == '\0')
    D_RETURN_(-1);

  cmd.type = type;
  cmd.efsd_2file_cmd.id = get_next_id();
  cmd.efsd_2file_cmd.file1 = get_full_path(file1);
  cmd.efsd_2file_cmd.file2 = get_full_path(file2);
  
  D_RETURN_(cmd.efsd_2file_cmd.id);
}


/** API starts here
 */
extern void efsd_cleanup_event(EfsdEvent *ev);


EfsdConnection *
efsd_open(void)
{
  struct sockaddr_un    cli_sun;
  EfsdConnection       *ec;

  D_ENTER;

  ec = (EfsdConnection*)malloc(sizeof(EfsdConnection));
  if (!ec)
    D_RETURN_(NULL);

  if ( (ec->fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "libefsd: socket() error.\n"); D_RETURN_(NULL);
    }

  bzero(&cli_sun, sizeof(cli_sun));
  cli_sun.sun_family = AF_UNIX;
  strncpy(cli_sun.sun_path, efsd_common_get_socket_file(), sizeof(cli_sun.sun_path));

  if (connect(ec->fd, (struct sockaddr*)&cli_sun, sizeof(cli_sun)) < 0)
    {
      fprintf(stderr, "libefsd: connect() error.\n"); D_RETURN_(NULL);      
    }

  if (fcntl(ec->fd, F_SETFL, O_NONBLOCK) < 0)
    {
      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
      exit(-1);
    }

  D_RETURN_(ec);
}


int            
efsd_get_connection_fd(EfsdConnection *ec)
{
  D_ENTER;

  if (!ec)
    D_RETURN_(-1);

  D_RETURN_(ec->fd);
}


int
efsd_close(EfsdConnection *ec)
{
  EfsdCommand cmd;

  D_ENTER;

  if (!ec)
    D_RETURN_(-1);

  cmd.type = EFSD_CMD_CLOSE;
  if (send_command(ec, &cmd) < 0)
    {
      D_RETURN_(-1);
    }

  close(ec->fd);
  free(ec);
  D_RETURN_(0);
}


int            
efsd_events_pending(EfsdConnection *ec)
{
  fd_set fdset;
  struct timeval tv;
  
  D_ENTER;

  if (!ec || ec->fd < 0)
    D_RETURN_(-1);
  
  FD_ZERO(&fdset);
  FD_SET(ec->fd, &fdset);
  
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select(ec->fd + 1, &fdset, NULL, NULL, &tv);
  
  D_RETURN_(FD_ISSET(ec->fd, &fdset));
}


int            
efsd_ready(EfsdConnection *ec)
{
  fd_set fdset;
  struct timeval tv;
  
  D_ENTER;

  if (!ec || ec->fd < 0)
    D_RETURN_(-1);
  
  FD_ZERO(&fdset);
  FD_SET(ec->fd, &fdset);
  
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  select(ec->fd + 1, NULL, &fdset, NULL, &tv);
  
  D_RETURN_(FD_ISSET(ec->fd, &fdset));
}


int           
efsd_next_event(EfsdConnection *ec, EfsdEvent *ev)
{
  D_ENTER;

  if (!ec || !ev || ec->fd < 0)
    D_RETURN_(-1);

  D_RETURN_(efsd_io_read_event(ec->fd, ev));
}


int           
efsd_wait_event(EfsdConnection *ec, EfsdEvent *ev)
{
  int result;
  fd_set    fdset;

  D_ENTER;

  if (!ec || !ev || ec->fd < 0)
    D_RETURN_(-1);

  FD_ZERO(&fdset);
  FD_SET(ec->fd, &fdset);
  if ((result = select(ec->fd+1, &fdset, NULL, NULL, NULL)) < 0)
    {
      D_RETURN_(-1);
    }

  D_RETURN_(efsd_io_read_event(ec->fd, ev));
}


EfsdCmdId      
efsd_remove(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_REMOVE, filename, 0, NULL));
}


EfsdCmdId      
efsd_move(EfsdConnection *ec, char *from_file, char *to_file)
{
  D_ENTER;
  D_RETURN_(twofile_cmd(ec, EFSD_CMD_MOVE, from_file, to_file));
}


EfsdCmdId      
efsd_copy(EfsdConnection *ec, char *from_file, char *to_file)
{
  D_ENTER;
  D_RETURN_(twofile_cmd(ec, EFSD_CMD_COPY, from_file, to_file));
}


EfsdCmdId      
efsd_symlink(EfsdConnection *ec, char *from_file, char *to_file)
{
  D_ENTER;
  D_RETURN_(twofile_cmd(ec, EFSD_CMD_SYMLINK, from_file, to_file));
}


EfsdCmdId      
efsd_listdir(EfsdConnection *ec, char *dirname,
	     int num_options, ...)
{
  va_list ap;
  int i, j, result;
  EfsdOption *op;
  EfsdOption *ops = NULL;

  D_ENTER;

  va_start (ap, num_options);
  ops = (EfsdOption *) malloc(sizeof(EfsdOption) * num_options);

  if (!ops)
    D_RETURN_(-1);

  for (i = 0, j = 0; i < num_options; i++)
    {
      op = va_arg(ap, EfsdOption*);

      /* sanity check -- pass only options that make sense. */
      if ((op->type == EFSD_OP_LS_GET_STAT) ||
	  (op->type == EFSD_OP_LS_GET_MIME) ||
	  (op->type == EFSD_OP_LS_GET_META))
	{
	  ops = realloc(ops, sizeof(EfsdOption) * ++j);
	  ops[j-1] = *op; 
	}
      else
	{
	  efsd_option_cleanup(op);
	}

      /* Yes. This does not clean up any strings etc pointed
	 to -- but that gets cleaned up in file_cmd().
      */
      FREE(op);
    }

  result = file_cmd(ec, EFSD_CMD_LISTDIR, dirname, num_options, ops);
  va_end (ap);

  D_RETURN_(result);
}


EfsdCmdId      
efsd_makedir(EfsdConnection *ec, char *dirname)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_MAKEDIR, dirname, 0, NULL));
}


EfsdCmdId      
efsd_chmod(EfsdConnection *ec, char *filename,  mode_t mode)
{
  char        *f;
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0')
    D_RETURN_(-1);

  cmd.type = EFSD_CMD_CHMOD;
  cmd.efsd_chmod_cmd.id = get_next_id();
  cmd.efsd_chmod_cmd.mode = mode;
  f = get_full_path(filename);
  cmd.efsd_chmod_cmd.file = strdup(f);
  free(f);

  if (send_command(ec, &cmd) < 0)
    {
      D_RETURN_(-1);
    }
  D_RETURN_(cmd.efsd_chmod_cmd.id);
}


EfsdCmdId      
efsd_set_metadata(EfsdConnection *ec, char *key, char *filename,
		     EfsdDatatype datatype, int data_len, void *data)
{
  char        *f;
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0' ||
      !key || key[0] == '\0' || !data)
    D_RETURN_(-1);

  cmd.type = EFSD_CMD_SETMETA;
  cmd.efsd_set_metadata_cmd.id = get_next_id();
  cmd.efsd_set_metadata_cmd.datatype = datatype;
  cmd.efsd_set_metadata_cmd.data_len = data_len;
  cmd.efsd_set_metadata_cmd.data = data;
  cmd.efsd_set_metadata_cmd.key = strdup(key);
  f = get_full_path(filename);
  cmd.efsd_set_metadata_cmd.file = strdup(f);
  free(f);
  
  if (send_command(ec, &cmd) < 0)
    {
      D_RETURN_(-1);
    }
  D_RETURN_(cmd.efsd_set_metadata_cmd.id);
}


EfsdCmdId      
efsd_get_metadata(EfsdConnection *ec, char *key, char *filename)
{
  char        *f;
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0'
      || !key || key[0] == '\0')
    D_RETURN_(-1);

  cmd.type = EFSD_CMD_GETMETA;
  cmd.efsd_get_metadata_cmd.id = get_next_id();
  cmd.efsd_get_metadata_cmd.key = strdup(key);
  f = get_full_path(filename);
  cmd.efsd_get_metadata_cmd.file = strdup(f);
  free(f);
  
  if (send_command(ec, &cmd) < 0)
    {
      D_RETURN_(-1);
    }
  D_RETURN_(cmd.efsd_get_metadata_cmd.id);
}


EfsdCmdId      
efsd_start_monitor(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_STARTMON, filename, 0, NULL));
}


EfsdCmdId      
efsd_stop_monitor(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_STOPMON, filename, 0, NULL));
}


EfsdCmdId      
efsd_stat(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_STAT, filename, 0, NULL));
}


EfsdCmdId      
efsd_readlink(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_READLINK, filename, 0, NULL));
}


EfsdCmdId      
efsd_get_mimetype(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_GETMIME, filename, 0, NULL));
}


EfsdOption    *
efsd_op_ls_get_stat(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_ls_get_stat());
}


EfsdOption    *
efsd_op_ls_get_metadata(char *key, EfsdDatatype type)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_ls_get_metadata(key, type));
}


EfsdOption    *
efsd_op_ls_get_mimetype(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_ls_get_mimetype());
}


EfsdOption    *
efsd_op_fs_force(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_fs_force());
}


EfsdOption    *
efsd_op_fs_recursive(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_fs_recursive());
}
