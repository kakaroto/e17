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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <efsd.h>
#include <efsd_debug.h>
#include <efsd_io.h>
#include <efsd_macros.h>
#include <efsd_options.h>
#include <efsd_types.h>
#include <efsd_queue.h>
#include <libefsd_misc.h>
#include <libefsd.h>

/* Enable those if you want call debugging output! */
char opt_nesting = FALSE;
char opt_debug   = FALSE;


struct efsd_connection
{
  int        fd;
  EfsdQueue *cmd_q;
};

struct efsd_options
{
  int          num_options;
  int          num_used;

  EfsdOption  *ops;
};


static int       send_command(EfsdConnection *ec, EfsdCommand *com);
static EfsdCmdId get_next_id(void);

static EfsdCmdId file_cmd(EfsdConnection *ec, EfsdCommandType type,
			  int num_files, char **files,
			  int num_options, EfsdOption *ops);

static EfsdCmdId set_metadata_internal(EfsdConnection *ec, char *key, char *filename,
				       EfsdDatatype datatype, int data_len, void *data);

static void      cmd_queue_add_command(EfsdConnection *ec, EfsdCommand *com);
static void      cmd_queue_process(EfsdConnection *ec);

static char*
get_full_path(char *file)
{
  char  cwd[MAXPATHLEN];
  char *result = NULL;
  int   len;

  D_ENTER;

  if (!file || !file[0])
    D_RETURN_(NULL);
  
#ifndef __EMX__
  if (file[0] == '/')
#else  
  if (_fnisabs(file))
#endif  
    D_RETURN_(strdup(file));

  /* Get current working directory, in the portable way.
     If it's now available, abort. */
  if (!getcwd(cwd, MAXPATHLEN))
    D_RETURN_(NULL);

  len = strlen(cwd) + strlen(file) + 2;

  result = malloc(len);
  snprintf(result, len, "%s/%s", cwd, file);
 
  D_RETURN_(result);
}


static int
send_command(EfsdConnection *ec, EfsdCommand *com)
{
  D_ENTER;

  if (!ec || !com)
    D_RETURN_(-1);

  if (!efsd_queue_empty(ec->cmd_q))
    {
      cmd_queue_add_command(ec, com);
      cmd_queue_process(ec);
    }
  else
    {
      if (efsd_io_write_command(ec->fd, com) < 0)
	{
	  if (errno == EPIPE)
	    {
	      D_RETURN_(-1);	  
	    }
	  
	  cmd_queue_add_command(ec, com);
	}
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
file_cmd(EfsdConnection *ec, EfsdCommandType type,
	 int num_files, char **files,
	 int num_options, EfsdOption *ops)
{
  char         **full_files = NULL;
  int            i, used_files;
  EfsdCommand    cmd;
  EfsdCmdId      id;

  D_ENTER;

  if (!ec || !files)
    D_RETURN_(-1);

  memset(&cmd, 0, sizeof(EfsdCommand));

  cmd.type = type;
  cmd.efsd_file_cmd.id = get_next_id();

  full_files = malloc(sizeof(char*) * num_files);

  /* Hook in full paths of all given files, if possible. */
  for (i = 0, used_files = 0; i < num_files; i++)
    {
      full_files[used_files] = get_full_path(files[i]);
      if (full_files[used_files])
	used_files++;
    }

  if (used_files < num_files)
    {
      if (used_files == 0)
	{
	  /* Wow. Not a single file was available. Not much
	     sense in sending the command now. */
	  FREE(full_files);
	  D_RETURN_(-1);
	}

      full_files = realloc(full_files, sizeof(char*) * used_files);
      num_files = used_files;
    }

  cmd.efsd_file_cmd.num_files = num_files;
  cmd.efsd_file_cmd.files = full_files; 

  /* Hook in any options: */
  if ((num_options > 0) && (ops))
    {
      cmd.efsd_file_cmd.num_options = num_options;
      cmd.efsd_file_cmd.options = ops;
    }

  /* And send it! */
  if (send_command(ec, &cmd) < 0)
    {
      efsd_cmd_cleanup(&cmd);
      D_RETURN_(-1);
    }

  id = cmd.efsd_file_cmd.id;
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(id);
}


static EfsdCmdId      
set_metadata_internal(EfsdConnection *ec, char *key, char *filename,
		      EfsdDatatype datatype, int data_len, void *data)
{
  EfsdCommand  cmd;
  EfsdCmdId    id;

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

  if (!cmd.efsd_set_metadata_cmd.file)
    goto error_return;
  
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
    goto error_return;

  id = cmd.efsd_set_metadata_cmd.id;
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(id);

 error_return:
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(-1);
}


static void      
cmd_queue_add_command(EfsdConnection *ec, EfsdCommand *com)
{
  EfsdCommand *com_copy;

  D_ENTER;

  if (!ec || !com)
    D_RETURN;

  com_copy = NEW(EfsdCommand);
  efsd_cmd_duplicate(com, com_copy);
  efsd_queue_append_item(ec->cmd_q, com_copy);

  D_RETURN;
}


static void      
cmd_queue_process(EfsdConnection *ec)
{
  EfsdCommand *cmd;

  D_ENTER;

  if (!ec)
    D_RETURN;

  while (!efsd_queue_empty(ec->cmd_q))
    {
      cmd = (EfsdCommand*)efsd_queue_next_item(ec->cmd_q);
      if (efsd_io_write_command(ec->fd, cmd) < 0)
	{
	  fprintf(stderr, "libefsd: queue NOT empty -- %i!\n", efsd_queue_size(ec->cmd_q));
	  D_RETURN;
	}

      efsd_cmd_free(cmd);
      efsd_queue_remove_item(ec->cmd_q);
      fprintf(stderr, "libefsd: processed 1\n");
    }

  fprintf(stderr, "libefsd: queue empty! %i\n", efsd_queue_size(ec->cmd_q));

  D_RETURN;
}


/* Efsd API starts here --------------------------------------------- */


EfsdConnection * efsd_open(void)
{
  struct sockaddr_un    cli_sun;
  EfsdConnection       *ec;
  int                   flags;

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

  if ( (flags = fcntl(ec->fd, F_GETFL, 0)) < 0)
    {
      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
      exit(-1);
    }

  if (fcntl(ec->fd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
      exit(-1);
    }

  ec->cmd_q = efsd_queue_new();

  D_RETURN_(ec);
}


int efsd_get_connection_fd(EfsdConnection *ec)
{
  D_ENTER;

  if (!ec)
    D_RETURN_(-1);

  D_RETURN_(ec->fd);
}


int efsd_close(EfsdConnection *ec)
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

  efsd_queue_free(ec->cmd_q, (EfsdFunc)efsd_cmd_free);

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
  
  if (select(ec->fd + 1, &fdset, NULL, NULL, &tv) < 0)
    D_RETURN_(-1);
  
  D_RETURN_(FD_ISSET(ec->fd, &fdset));
}


int           
efsd_next_event(EfsdConnection *ec, EfsdEvent *ev)
{
  int result;

  D_ENTER;

  if (!ec || !ev || ec->fd < 0)
    D_RETURN_(-1);

  result = efsd_io_read_event(ec->fd, ev);
  D_RETURN_(result);
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
    D_RETURN_(-1);

  D_RETURN_(efsd_io_read_event(ec->fd, ev));
}


int            
efsd_commands_pending(EfsdConnection *ec)
{
  D_ENTER;

  if (!ec)
    D_RETURN_(-1);

  D_RETURN_(!efsd_queue_empty(ec->cmd_q));
}


int            
efsd_flush(EfsdConnection *ec)
{
  D_ENTER;

  if (!ec)
    D_RETURN_(-1);

  cmd_queue_process(ec);

  D_RETURN_(efsd_queue_empty(ec->cmd_q));
}



EfsdCmdId      
efsd_remove(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_REMOVE, num_files, files, ops->num_used, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_REMOVE, num_files, files, 0, NULL);

  FREE(ops);
  
  D_RETURN_(result);
}


EfsdCmdId      
efsd_move(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_MOVE, num_files, files, ops->num_used, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_MOVE, num_files, files, 0, NULL);

  FREE(ops);
  
  D_RETURN_(result);
}


EfsdCmdId      
efsd_copy(EfsdConnection *ec, int num_files, char **files, EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_COPY, num_files, files, ops->num_used, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_COPY, num_files, files, 0, NULL);

  FREE(ops);
  
  D_RETURN_(result);
}


EfsdCmdId      
efsd_symlink(EfsdConnection *ec, char *from_file, char *to_file)
{
  char *files[2];
  int   result;

  D_ENTER;

  files[0] = from_file; files[1] = to_file;
  result = file_cmd(ec, EFSD_CMD_SYMLINK, 2, files, 0, NULL);

  D_RETURN_(result);
}


EfsdCmdId      
efsd_listdir(EfsdConnection *ec, char *dirname,
	     EfsdOptions *ops)
{
  int result = 0;

  D_ENTER;

  if (ops)
    result = file_cmd(ec, EFSD_CMD_LISTDIR, 1, &dirname, ops->num_used, ops->ops);
  else
    result = file_cmd(ec, EFSD_CMD_LISTDIR, 1, &dirname, 0, NULL);

  FREE(ops);
  
  D_RETURN_(result);
}


EfsdCmdId      
efsd_makedir(EfsdConnection *ec, char *dirname)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_MAKEDIR, 1, &dirname, 0, NULL));
}


EfsdCmdId      
efsd_chmod(EfsdConnection *ec, char *filename,  mode_t mode)
{
  EfsdCommand  cmd;
  EfsdCmdId    id;

  D_ENTER;

  if (!ec || !filename || filename[0] == '\0')
    D_RETURN_(-1);

  memset(&cmd, 0, sizeof(EfsdCommand));
  cmd.type = EFSD_CMD_CHMOD;
  cmd.efsd_chmod_cmd.id = get_next_id();
  cmd.efsd_chmod_cmd.mode = mode;
  cmd.efsd_chmod_cmd.file = get_full_path(filename);

  if (!cmd.efsd_chmod_cmd.file)
    goto error_return;

  if (send_command(ec, &cmd) < 0)
    goto error_return;

  id = cmd.efsd_chmod_cmd.id;
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(id);

 error_return:
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(-1);
}


EfsdCmdId
efsd_set_metadata_raw(EfsdConnection *ec, char *key, char *filename,
		      EfsdDatatype datatype, int datalength, void *data)
{
  EfsdCmdId id;

  D_ENTER;

  id = set_metadata_internal(ec, key, filename, datatype,
			     datalength, data);

  D_RETURN_(id);
}


EfsdCmdId      
efsd_set_metadata_int(EfsdConnection *ec, char *key,
		      char *filename, int val)
{
  EfsdCmdId id;

  D_ENTER;

  id = set_metadata_internal(ec, key, filename, EFSD_INT,
			     sizeof(int), &val);

  D_RETURN_(id);
}


EfsdCmdId      
efsd_set_metadata_float(EfsdConnection *ec, char *key,
			char *filename, float val)
{
  EfsdCmdId id;

  D_ENTER;

  id = set_metadata_internal(ec, key, filename, EFSD_FLOAT,
			     sizeof(float), &val);

  D_RETURN_(id);
}


EfsdCmdId      
efsd_set_metadata_str(EfsdConnection *ec, char *key,
		      char *filename, char *val)
{
  EfsdCmdId id;

  D_ENTER;

  id = set_metadata_internal(ec, key, filename, EFSD_STRING,
			     strlen(val) + 1, val);

  D_RETURN_(id);
}


EfsdCmdId      
efsd_get_metadata(EfsdConnection *ec, char *key, char *filename,
		  EfsdDatatype datatype)
{
  EfsdCommand  cmd;
  EfsdCmdId    id;
   
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

  if (!cmd.efsd_get_metadata_cmd.file)
    goto error_return;

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
    goto error_return;

  id = cmd.efsd_get_metadata_cmd.id;
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(id);

 error_return:
  efsd_cmd_cleanup(&cmd);
  D_RETURN_(-1);
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

  D_RETURN_(strdup((char*)ee->efsd_reply_event.data));
}


void          *
efsd_metadata_get_raw(EfsdEvent *ee, int *data_len)
{
  void *result;

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
  
  if ((result = malloc(ee->efsd_reply_event.data_len)) == NULL)
    {
      D_RETURN_(NULL);
    }

  memcpy(result, ee->efsd_reply_event.data, ee->efsd_reply_event.data_len);

  D_RETURN_(result);
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
efsd_start_monitor(EfsdConnection *ec, char *filename, EfsdOptions *ops, int dir_mode)

{
  struct stat     st;
  EfsdCmdId       result = 0;
  EfsdCommandType type;

  D_ENTER;

  if (lstat(filename, &st) < 0)
    D_RETURN_(-1);

  if (!S_ISDIR(st.st_mode))
    D_RETURN_(-1);

  if (dir_mode)
    type = EFSD_CMD_STARTMON_DIR;
  else
    type = EFSD_CMD_STARTMON_FILE;

  if (ops)
    result = file_cmd(ec, type, 1, &filename, ops->num_used, ops->ops);
  else
    result = file_cmd(ec, type, 1, &filename, 0, NULL);
  
  FREE(ops);

  D_RETURN_(result);
}


EfsdCmdId      
efsd_stop_monitor(EfsdConnection *ec, char *filename, int dir_mode)
{
  EfsdCommandType type;

  D_ENTER;

  if (dir_mode)
    type = EFSD_CMD_STOPMON_DIR;
  else
    type = EFSD_CMD_STOPMON_FILE;

  D_RETURN_(file_cmd(ec, type, 1, &filename, 0, NULL));
}


EfsdCmdId      
efsd_stat(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_STAT, 1, &filename, 0, NULL));
}


EfsdCmdId      
efsd_lstat(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_LSTAT, 1, &filename, 0, NULL));
}


EfsdCmdId      
efsd_readlink(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_READLINK, 1, &filename, 0, NULL));
}


EfsdCmdId      
efsd_get_filetype(EfsdConnection *ec, char *filename)
{
  D_ENTER;
  D_RETURN_(file_cmd(ec, EFSD_CMD_GETFILETYPE, 1, &filename, 0, NULL));
}


static EfsdOptions *
ops_create(int num_options)
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



EfsdOptions  *
efsd_ops(int num_options, ...)
{
  int i;
  va_list ap;
  EfsdOption  *op;
  EfsdOptions *ops;

  D_ENTER;

  va_start (ap, num_options);

  ops = ops_create(num_options);
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
efsd_ops_create(void)
{
  EfsdOptions *ops;

  D_ENTER;
  ops = ops_create(5);  /* Good guess for initial value */
  D_RETURN_(ops);
}


void          
efsd_ops_add(EfsdOptions *ops, EfsdOption *op)
{
  D_ENTER;

  if (!ops || !op)
    D_RETURN;

  if (ops->num_used == ops->num_options)
    {
      /* Double the number of slots if space isn't sufficient. */
      ops->ops = realloc(ops->ops, 2 * ops->num_options * sizeof(EfsdOption));
      ops->num_options *= 2;
    }

  ops->ops[ops->num_used] = *op;
  ops->num_used++;
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
efsd_op_get_lstat(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_get_lstat());
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
efsd_op_list_all(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_list_all());
}


EfsdOption    *
efsd_op_sort(void)
{
  D_ENTER;
  D_RETURN_(efsd_option_new_sort());
}


char          *
efsd_reply_filename(EfsdEvent *ee)
{
  D_ENTER;

  if (!ee)
    D_RETURN_(NULL);

  if (ee->type == EFSD_EVENT_FILECHANGE)
    D_RETURN_(ee->efsd_filechange_event.file);

  switch (ee->efsd_reply_event.command.type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_MOVE:
    case EFSD_CMD_COPY:
    case EFSD_CMD_SYMLINK:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_STARTMON_FILE:
    case EFSD_CMD_STARTMON_DIR:
    case EFSD_CMD_STOPMON_FILE:
    case EFSD_CMD_STOPMON_DIR:
    case EFSD_CMD_STAT:
    case EFSD_CMD_LSTAT:
    case EFSD_CMD_READLINK:
    case EFSD_CMD_GETFILETYPE:      
      D_RETURN_(ee->efsd_reply_event.command.efsd_file_cmd.files[0]);
      break;
    case EFSD_CMD_CHMOD:
      D_RETURN_(ee->efsd_reply_event.command.efsd_chmod_cmd.file);
      break;
    case EFSD_CMD_SETMETA:
      D_RETURN_(ee->efsd_reply_event.command.efsd_set_metadata_cmd.file);
      break;
    case EFSD_CMD_GETMETA:
      D_RETURN_(ee->efsd_reply_event.command.efsd_get_metadata_cmd.file);
      break;
    case EFSD_CMD_CLOSE:
    default:
    }

  D_RETURN_(NULL);
}


EfsdCmdId      
efsd_reply_id(EfsdEvent *ee)
{
  D_ENTER;

  if (!ee)
    D_RETURN_(-1);

  if (ee->type == EFSD_EVENT_FILECHANGE)
    D_RETURN_(ee->efsd_filechange_event.id);

  switch (ee->efsd_reply_event.command.type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_MOVE:
    case EFSD_CMD_COPY:
    case EFSD_CMD_SYMLINK:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_STARTMON_FILE:
    case EFSD_CMD_STARTMON_DIR:
    case EFSD_CMD_STOPMON_FILE:
    case EFSD_CMD_STOPMON_DIR:
    case EFSD_CMD_STAT:
    case EFSD_CMD_LSTAT:
    case EFSD_CMD_READLINK:
    case EFSD_CMD_GETFILETYPE:      
      D_RETURN_(ee->efsd_reply_event.command.efsd_file_cmd.id);
      break;
    case EFSD_CMD_CHMOD:
      D_RETURN_(ee->efsd_reply_event.command.efsd_chmod_cmd.id);
      break;
    case EFSD_CMD_SETMETA:
      D_RETURN_(ee->efsd_reply_event.command.efsd_set_metadata_cmd.id);
      break;
    case EFSD_CMD_GETMETA:
      D_RETURN_(ee->efsd_reply_event.command.efsd_get_metadata_cmd.id);
      break;
    case EFSD_CMD_CLOSE:
    default:
    }

  D_RETURN_(-1);
}


void          *
efsd_reply_data(EfsdEvent *ee)
{
  D_ENTER;

  if (!ee)
    D_RETURN_(NULL);
  
  if (ee->type != EFSD_EVENT_REPLY)
    D_RETURN_(NULL);

  D_RETURN_(ee->efsd_reply_event.data);
}

