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
#include <strings.h>
#include <sys/select.h>
#define getcwd _getcwd2
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_io.h>
#include <efsd_macros.h>
#include <efsd_options.h>
#include <efsd_types.h>
#include <libefsd_misc.h>
#include <libefsd.h>

struct efsd_connection
{
  int        fd;
};

struct efsd_options
{
  int          num_options;
  int          num_used;

  EfsdOption  *ops;
};

static int       send_command(EfsdConnection *ec, EfsdCommand *com);
static EfsdCmdId get_next_id(void);
static EfsdCmdId file_cmd(EfsdConnection *ec, EfsdCommandType type, char *file,
			  int num_options, EfsdOption *ops);
static EfsdCmdId twofile_cmd(EfsdConnection *ec, EfsdCommandType type,
			     char *file1, char *file2,
			     int num_options, EfsdOption *ops);

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

  memset(&cmd, 0, sizeof(EfsdCommand));

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
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }

  efsd_cmd_cleanup(&cmd);
  D_RETURN_(cmd.efsd_file_cmd.id);
}


static EfsdCmdId 
twofile_cmd(EfsdConnection *ec, EfsdCommandType type,
	    char *file1, char *file2,
	    int num_options, EfsdOption *ops)
{
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !file1 || file1[0] == '\0' || !file2 || file2[0] == '\0')
    D_RETURN_(-1);

  memset(&cmd, 0, sizeof(EfsdCommand));

  cmd.type = type;
  cmd.efsd_2file_cmd.id = get_next_id();
  cmd.efsd_2file_cmd.file1 = get_full_path(file1);
  cmd.efsd_2file_cmd.file2 = get_full_path(file2);

  if ((num_options > 0) && (ops))
    {
      cmd.efsd_2file_cmd.num_options = num_options;
      cmd.efsd_2file_cmd.options = ops;
    }

  if (send_command(ec, &cmd) < 0)
    {
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }
  
  efsd_cmd_cleanup(&cmd);
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

  memset(&cli_sun, 0, sizeof(cli_sun));
  cli_sun.sun_family = AF_UNIX;
  strncpy(cli_sun.sun_path, misc_get_socket_file(), sizeof(cli_sun.sun_path));

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

  memset(&cmd, 0, sizeof(EfsdCommand));
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
efsd_remove(EfsdConnection *ec, char *filename,
	    EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_REMOVE, filename, ops->num_options, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_REMOVE, filename, 0, NULL);
  
  FREE(ops);

  D_RETURN_(result);
}


EfsdCmdId      
efsd_move(EfsdConnection *ec, char *from_file, char *to_file,
	  EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = twofile_cmd(ec, EFSD_CMD_MOVE, from_file, to_file, ops->num_options, ops->ops);
  else
    result = twofile_cmd(ec, EFSD_CMD_MOVE, from_file, to_file, 0, NULL);
  
  FREE(ops);

  D_RETURN_(result);
}


EfsdCmdId      
efsd_copy(EfsdConnection *ec, char *from_file, char *to_file,
	  EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = twofile_cmd(ec, EFSD_CMD_COPY, from_file, to_file, ops->num_options, ops->ops);
  else
    result = twofile_cmd(ec, EFSD_CMD_COPY, from_file, to_file, 0, NULL);
  
  FREE(ops);

  D_RETURN_(result);
}


EfsdCmdId      
efsd_symlink(EfsdConnection *ec, char *from_file, char *to_file)
{
  D_ENTER;
  D_RETURN_(twofile_cmd(ec, EFSD_CMD_SYMLINK, from_file, to_file,
			0, NULL));
}


EfsdCmdId      
efsd_listdir(EfsdConnection *ec, char *dirname,
	     EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_LISTDIR, dirname, ops->num_options, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_LISTDIR, dirname, 0, NULL);
  
  FREE(ops);
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
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0')
    D_RETURN_(-1);

  memset(&cmd, 0, sizeof(EfsdCommand));
  cmd.type = EFSD_CMD_CHMOD;
  cmd.efsd_chmod_cmd.id = get_next_id();
  cmd.efsd_chmod_cmd.mode = mode;
  cmd.efsd_chmod_cmd.file = get_full_path(filename);

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
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0' ||
      !key || key[0] == '\0' || !data)
    D_RETURN_(-1);

  memset(&cmd, 0, sizeof(EfsdCommand));
  cmd.type = EFSD_CMD_SETMETA;
  cmd.efsd_set_metadata_cmd.id = get_next_id();
  cmd.efsd_set_metadata_cmd.datatype = datatype;
  cmd.efsd_set_metadata_cmd.data_len = data_len;
  cmd.efsd_set_metadata_cmd.data = data;
  cmd.efsd_set_metadata_cmd.key = strdup(key);
  cmd.efsd_set_metadata_cmd.file = get_full_path(filename);
  
  /*
    if (!efsd_misc_file_exists(cmd.efsd_set_metadata_cmd.file))
    {
      D(("File '%s' doesn't exist.\n",
	 cmd.efsd_set_metadata_cmd.file));
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }
  */
  
  if (send_command(ec, &cmd) < 0)
    {
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }

  efsd_cmd_cleanup(&cmd);
  D_RETURN_(cmd.efsd_set_metadata_cmd.id);
}


EfsdCmdId      
efsd_get_metadata(EfsdConnection *ec, char *key, char *filename,
		  EfsdDatatype datatype)
{
  EfsdCommand  cmd;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0'
      || !key || key[0] == '\0')
    D_RETURN_(-1);

  memset(&cmd, 0, sizeof(EfsdCommand));
  cmd.type = EFSD_CMD_GETMETA;
  cmd.efsd_get_metadata_cmd.id = get_next_id();
  cmd.efsd_get_metadata_cmd.datatype = datatype;
  cmd.efsd_get_metadata_cmd.key = strdup(key);
  cmd.efsd_get_metadata_cmd.file = get_full_path(filename);

  /*
  if (!efsd_misc_file_exists(cmd.efsd_get_metadata_cmd.file))
    {
      D(("File '%s' doesn't exist.\n",
	 cmd.efsd_set_metadata_cmd.file));
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }
  */

  if (send_command(ec, &cmd) < 0)
    {
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }

  efsd_cmd_cleanup(&cmd);
  D_RETURN_(cmd.efsd_get_metadata_cmd.id);
}


EfsdDatatype   
efsd_metadata_get_type(EfsdEvent *ee)
{
  D_ENTER;
  
  if ((ee->type != EFSD_EVENT_REPLY)         ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA))
    {
      D_RETURN_(0);
    }

  D_RETURN_(ee->efsd_reply_event.command.
	    efsd_get_metadata_cmd.datatype);
}


int            
efsd_metadata_get_int(EfsdEvent *ee, int *val)
{
  D_ENTER;
  
  if ((!val)                                 ||
      (ee->type != EFSD_EVENT_REPLY)         ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA)                     ||
      (ee->efsd_reply_event.command.
       efsd_get_metadata_cmd.datatype != EFSD_INT)
      )
    {
      D_RETURN_(0);
    }

  *val = *((int*)ee->efsd_reply_event.data);
  D_RETURN_(1);
}


int            
efsd_metadata_get_float(EfsdEvent *ee, float *val)
{
  D_ENTER;
  
  if ((!val)                                 ||
      (ee->type != EFSD_EVENT_REPLY)         ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA)                     ||
      (ee->efsd_reply_event.command.
       efsd_get_metadata_cmd.datatype != EFSD_FLOAT)
      )
    {
      D_RETURN_(0);
    }

  *val = *((float*)ee->efsd_reply_event.data);
  D_RETURN_(1);
}


char          *
efsd_metadata_get_str(EfsdEvent *ee)
{
  D_ENTER;
  
  if ((ee->type != EFSD_EVENT_REPLY)         ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA)                     ||
      (ee->efsd_reply_event.command.
       efsd_get_metadata_cmd.datatype != EFSD_STRING)
      )
    {
      D_RETURN_(NULL);
    }

  D_RETURN_((char*)ee->efsd_reply_event.data);
}


void          *
efsd_metadata_get_raw(EfsdEvent *ee, int *data_len)
{
  D_ENTER;
  
  if ((ee->type != EFSD_EVENT_REPLY)         ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA)                     ||
      (ee->efsd_reply_event.command.
       efsd_get_metadata_cmd.datatype != EFSD_FLOAT)
      )
    {
      D_RETURN_(NULL);
    }
  
  if (data_len)
    {
      *data_len = ee->efsd_reply_event.data_len;
    }
  
  D_RETURN_(ee->efsd_reply_event.data);
}


char          *
efsd_metadata_get_key(EfsdEvent *ee)
{
  D_ENTER;
  
  if ((ee->type != EFSD_EVENT_REPLY) ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA))
    {
      D_RETURN_(NULL);
    }
  
  D_RETURN_(ee->efsd_reply_event.command.efsd_get_metadata_cmd.key);
}


char          *
efsd_metadata_get_file(EfsdEvent *ee)
{
  D_ENTER;
  
  if ((ee->type != EFSD_EVENT_REPLY) ||
      (ee->efsd_reply_event.command.type !=
       EFSD_CMD_GETMETA))
    {
      D_RETURN_(NULL);
    }
  
  D_RETURN_(ee->efsd_reply_event.command.efsd_get_metadata_cmd.file);
}


EfsdCmdId      
efsd_start_monitor(EfsdConnection *ec, char *filename, EfsdOptions *ops)

{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_STARTMON, filename, ops->num_options, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_STARTMON, filename, 0, NULL);
  
  FREE(ops);
  D_RETURN_(result);
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
efsd_lstat(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_LSTAT, filename, 0, NULL));
}


EfsdCmdId      
efsd_readlink(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_READLINK, filename, 0, NULL));
}


EfsdCmdId      
efsd_get_filetype(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_GETFILETYPE, filename, 0, NULL));
}


EfsdOptions  *
efsd_ops(int num_options, ...)
{
  int i;
  va_list ap;
  EfsdOption  *op;
  EfsdOptions *ops;

  D_ENTER;

  va_start (ap, num_options);

  ops = efsd_ops_create(num_options);
  if (!ops)
    D_RETURN_(NULL);

  for (i = 0; i < num_options; i++)
    {
      op = va_arg(ap, EfsdOption*);
      efsd_ops_add(ops, op);
    }

  va_end(ap);

  D_RETURN_(ops);
}


EfsdOptions  *
efsd_ops_create(int num_options)
{
  EfsdOptions *ops;

  D_ENTER;

  ops = NEW(EfsdOptions);
  if (!ops)
    D_RETURN_(NULL);

  ops->num_options = num_options;
  ops->num_used = 0;
  ops->ops = calloc(num_options, sizeof(EfsdOption));
  if (!ops->ops)
    {
      FREE(ops);
      D_RETURN_(NULL);
    }

  D_RETURN_(ops);
}


void          
efsd_ops_add(EfsdOptions *ops, EfsdOption *op)
{
  D_ENTER;

  if (!ops || !op)
    D_RETURN;

  if (ops->num_used < ops->num_options)
    {
      ops->ops[ops->num_used] = *op;
      ops->num_used++;
    }
  else
    {
      efsd_option_cleanup(op);
    }

  FREE(op);

  D_RETURN;
}


EfsdOption    *
efsd_op_get_stat(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_get_stat());
}


EfsdOption    *
efsd_op_get_metadata(char *key, EfsdDatatype type)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_get_metadata(key, type));
}


EfsdOption    *
efsd_op_get_filetype(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_get_filetype());
}


EfsdOption    *
efsd_op_force(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_force());
}


EfsdOption    *
efsd_op_recursive(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_recursive());
}


EfsdOption    *
efsd_op_all(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_all());
}
