/*

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
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <efsd_debug.h>
#include <efsd_io.h>

static int     read_data(int sockfd, void *dest, int size);
static int     read_int(int sockfd, int *dest);
static int     read_string(int sockfd, char **s);
static int     write_data(int sockfd, void *data, int size);
static int     write_int(int sockfd, int data);
static int     write_string(int sockfd, char *s);

static int     read_remove_cmd(int sockfd, EfsdCommand *cmd);
static int     read_move_cmd(int sockfd, EfsdCommand *cmd);
static int     read_listdir_cmd(int sockfd, EfsdCommand *cmd);
static int     read_makedir_cmd(int sockfd, EfsdCommand *cmd);
static int     read_chmod_cmd(int sockfd, EfsdCommand *cmd);
static int     read_set_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     read_get_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     read_start_monitor_cmd(int sockfd, EfsdCommand *cmd);
static int     read_stop_monitor_cmd(int sockfd, EfsdCommand *cmd);
static int     read_filechange_event(int sockfd, EfsdEvent *ee);
static int     read_reply_event(int sockfd, EfsdEvent *ee);

static int     write_remove_cmd(int sockfd, EfsdCommand *cmd);
static int     write_move_cmd(int sockfd, EfsdCommand *cmd);
static int     write_listdir_cmd(int sockfd, EfsdCommand *cmd);
static int     write_makedir_cmd(int sockfd, EfsdCommand *cmd);
static int     write_chmod_cmd(int sockfd, EfsdCommand *cmd);
static int     write_set_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     write_get_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     write_start_monitor_cmd(int sockfd, EfsdCommand *cmd);
static int     write_stop_monitor_cmd(int sockfd, EfsdCommand *cmd);
static int     write_filechange_event(int sockfd, EfsdEvent *ee);
static int     write_reply_event(int sockfd, EfsdEvent *ee);

/* Read data from the client -- adapted from
   the improved read() functions in the Stevens books.
*/
int 
read_data(int sockfd, void *dest, int size)
{
  int   num_left, num_read;
  char *ptr;

  if (sockfd < 0)
    return (-1);

  ptr = (char*)dest;
  num_left = size;
  num_read = 0;

  while (num_left)
    {
      num_read = read(sockfd, ptr, num_left);

      if (num_read < 0)
	return num_read;        /* Error occurred -- return error. */
      else if (num_read == 0)
	break;                  /* End of file */
      
      num_left -= num_read;
      ptr += num_read;
    }

  return (size - num_left);
}


static int     
read_int(int sockfd, int *dest)
{
  if (read_data(sockfd, dest, sizeof(int)) != sizeof(int))
    return (-1);

  return (0);
}


/* Reads a character string from the socket.
   It is assumed that the length of the string
   including the terminating 0 is sent in an
   integer before the string itself.
*/
static int     
read_string(int sockfd, char **s)
{
  int i;

  if (read_data(sockfd, &i, sizeof(int)) != sizeof(int))
    return (-1);

  *s = (char*)malloc(sizeof(char) * i);
  if (read_data(sockfd, *s, i) != i)
    return (-1);

  return (0);
}


static int     
write_data(int sockfd, void *data, int size)
{
  int result;

  if (sockfd < 0)
    return (-1);

  if ( (result = write(sockfd, data, size)) != size)
    {
      if (result < 0 && errno == EPIPE)
	{
	  D(("Broken pipe in write_data()\n"));
	}

      return (-1);
    }

  return (0);  
}


static int     
write_int(int sockfd, int data)
{
  return write_data(sockfd, &data, sizeof(int));
}


static int     
write_string(int sockfd, char *data)
{
  if (write_int(sockfd, strlen(data)+1) < 0)
    return (-1);

  return write_data(sockfd, data, strlen(data)+1);
}


static int     
read_remove_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_file_cmd.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(cmd->efsd_file_cmd.options)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_file_cmd.file)) < 0)
    return (-1);

  return (0);
}


static int     
read_move_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_2file_cmd.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(cmd->efsd_2file_cmd.options)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_2file_cmd.file1)) < 0)
    return (-1);
  
  if (read_string(sockfd, &(cmd->efsd_2file_cmd.file2)) < 0)
    return (-1);
  
  return (0);
}


static int     
read_listdir_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_file_cmd.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(cmd->efsd_file_cmd.options)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_file_cmd.file)) < 0)
    return (-1);

  return (0);
}


static int     
read_makedir_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_file_cmd.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(cmd->efsd_file_cmd.options)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_file_cmd.file)) < 0)
    return (-1);
  
  return (0);
}


static int     
read_chmod_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_chmod_cmd.id)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_chmod_cmd.file)) < 0)
    return (-1);
  
  if (read_data(sockfd, &(cmd->efsd_chmod_cmd.mode), sizeof(mode_t)) != sizeof(mode_t))
    return (-1);
  
  return (0);
}


static int     
read_set_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  int  i;

  if (read_int(sockfd, &(cmd->efsd_set_metadata_cmd.id)) < 0)
    return (-1);

  if (read_data(sockfd, &(cmd->efsd_set_metadata_cmd.datatype),
		sizeof(EfsdDatatype)) != sizeof(EfsdDatatype))
    return (-1);
  
  if (read_int(sockfd, &(cmd->efsd_set_metadata_cmd.data_len)) < 0)
    return (-1);
  
  i = cmd->efsd_set_metadata_cmd.data_len;
  cmd->efsd_set_metadata_cmd.data = malloc(i);
  if (read_data(sockfd, &(cmd->efsd_set_metadata_cmd.data), i) != i)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_set_metadata_cmd.key)) < 0)
    return (-1);
  
  if (read_string(sockfd, &(cmd->efsd_set_metadata_cmd.file)) < 0)
    return (-1);
   
  return (0);
}


static int     
read_get_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_get_metadata_cmd.id)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_get_metadata_cmd.key)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_get_metadata_cmd.file)) < 0)
    return (-1);
  
  return (0);
}


static int     
read_start_monitor_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_file_cmd.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(cmd->efsd_file_cmd.options)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_file_cmd.file)) < 0)
    return (-1);

  return (0);
}


static int     
read_stop_monitor_cmd(int sockfd, EfsdCommand *cmd)
{
  if (read_int(sockfd, &(cmd->efsd_file_cmd.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(cmd->efsd_file_cmd.options)) < 0)
    return (-1);

  if (read_string(sockfd, &(cmd->efsd_file_cmd.file)) < 0)
    return (-1);
  
  return (0);
}


static int     
read_filechange_event(int sockfd, EfsdEvent *ee)
{
  if (read_int(sockfd, &(ee->efsd_filechange_event.id)) < 0)
    return (-1);

  if (read_int(sockfd, &(ee->efsd_filechange_event.changecode)) < 0)
    return (-1);

  if (read_string(sockfd, &(ee->efsd_filechange_event.file)) < 0)
    return (-1);

  return (0);
}


static int     
read_reply_event(int sockfd, EfsdEvent *ee)
{
  if (efsd_read_command(sockfd, &(ee->efsd_reply_event.command)) < 0)
    return (-1);

  if (read_int(sockfd, (int*)&(ee->efsd_reply_event.status)) < 0)
    return (-1);

  if (read_int(sockfd, &(ee->efsd_reply_event.errorcode)) < 0)
    return (-1);

  if (read_int(sockfd, &(ee->efsd_reply_event.data_len)) < 0)
    return (-1);
  
  if (ee->efsd_reply_event.data_len > 0)
    {
      ee->efsd_reply_event.data = malloc(ee->efsd_reply_event.data_len);
      if (read_data(sockfd, &(ee->efsd_reply_event.data),
		    ee->efsd_reply_event.data_len) < 0)
	return (-1);
    }
  else
    {
      ee->efsd_reply_event.data_len = 0;
      ee->efsd_reply_event.data = NULL;
    }

  return (0);  
}


static int     
write_remove_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_file_cmd.id) < 0)
    return (-1);

  if (write_int(sockfd, cmd->efsd_file_cmd.options) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_file_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_move_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_2file_cmd.id) < 0)
    return (-1);
  
  if (write_int(sockfd, cmd->efsd_2file_cmd.options) < 0)
    return (-1);
  
  if (write_string(sockfd, cmd->efsd_2file_cmd.file1) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_2file_cmd.file2) < 0)
    return (-1);

  return (0);
}


static int     
write_listdir_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_file_cmd.id) < 0)
    return (-1);

  if (write_int(sockfd, cmd->efsd_file_cmd.options) < 0)
    return (-1);
  
  if (write_string(sockfd, cmd->efsd_file_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_makedir_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_file_cmd.id) < 0)
    return (-1);

  if (write_int(sockfd, cmd->efsd_file_cmd.options) < 0)
    return (-1);
  
  if (write_string(sockfd, cmd->efsd_file_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_chmod_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_chmod_cmd.id) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_chmod_cmd.file) < 0)
    return (-1);
  
  if (write_data(sockfd, &(cmd->efsd_chmod_cmd.mode),
		 sizeof(cmd->efsd_chmod_cmd.mode)) < 0)
    return (-1);

  return (0);
}
  

static int     
write_set_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_set_metadata_cmd.id) < 0)
    return (-1);
  
  if (write_data(sockfd, &(cmd->efsd_set_metadata_cmd.datatype),
		 sizeof(EfsdDatatype)) < 0)
    return (-1);
  
  if (write_int(sockfd, cmd->efsd_set_metadata_cmd.data_len) < 0)
    return (-1);
  
  if (write_data(sockfd, cmd->efsd_set_metadata_cmd.data,
		 cmd->efsd_set_metadata_cmd.data_len) < 0)
    return (-1);
  
  if (write_string(sockfd, cmd->efsd_set_metadata_cmd.key) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_set_metadata_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_get_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_get_metadata_cmd.id) < 0)
    return (-1);
    
  if (write_string(sockfd, cmd->efsd_get_metadata_cmd.key) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_get_metadata_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_start_monitor_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_file_cmd.id) < 0)
    return (-1);

  if (write_int(sockfd, cmd->efsd_file_cmd.options) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_file_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_stop_monitor_cmd(int sockfd, EfsdCommand *cmd)
{
  if (write_int(sockfd, cmd->efsd_file_cmd.id) < 0)
    return (-1);

  if (write_int(sockfd, cmd->efsd_file_cmd.options) < 0)
    return (-1);

  if (write_string(sockfd, cmd->efsd_file_cmd.file) < 0)
    return (-1);

  return (0);
}


static int     
write_filechange_event(int sockfd, EfsdEvent *ee)
{
  if (write_int(sockfd, ee->efsd_filechange_event.id) < 0)
    return (-1);

  if (write_int(sockfd, ee->efsd_filechange_event.changecode) < 0)
    return (-1);

  if (write_string(sockfd, ee->efsd_filechange_event.file) < 0)
    return (-1);

  return (0);
}


static int     
write_reply_event(int sockfd, EfsdEvent *ee)
{
  if (efsd_write_command(sockfd, &(ee->efsd_reply_event.command)) < 0)
    return (-1);

  if (write_int(sockfd, ee->efsd_reply_event.status) < 0)
    return (-1);

  if (write_int(sockfd, ee->efsd_reply_event.errorcode) < 0)
    return (-1);

  if (write_int(sockfd, ee->efsd_reply_event.data_len) < 0)
    return (-1);

  if (write_data(sockfd, ee->efsd_reply_event.data,
		 ee->efsd_reply_event.data_len) < 0)
    return (-1);

  return (0);
}


/* Non-static stuff below: */

int      
efsd_write_command(int sockfd, EfsdCommand *cmd)
{
  int result = (-1);

  if (!cmd)
    return (-1);  

  if (write_int(sockfd, cmd->type) < 0)
    return (-1);
  
  switch (cmd->type)
    {
    case REMOVE:
      result = write_remove_cmd(sockfd, cmd);
      break;
    case MOVE:
      result = write_move_cmd(sockfd, cmd);
      break;
    case LISTDIR:
      result = write_listdir_cmd(sockfd, cmd);
      break;
    case MAKEDIR:
      result = write_makedir_cmd(sockfd, cmd);
      break;
    case CHMOD:
      result = write_chmod_cmd(sockfd, cmd);
      break;
    case SETMETA:
      result = write_set_metadata_cmd(sockfd, cmd);
      break;
    case GETMETA:
      result = write_get_metadata_cmd(sockfd, cmd); 
      break;
    case STARTMON:
      result = write_start_monitor_cmd(sockfd, cmd);   
      break;
    case STOPMON:
      result = write_stop_monitor_cmd(sockfd, cmd);    
      break;
    case CLOSE:
      result = 0;
      break;
    default:
    }

  if (result < 0)
    fprintf(stderr, "error writing command.\n");

  return result;
}


int      
efsd_read_command(int sockfd, EfsdCommand *cmd)
{
  int result = -1;

  if (!cmd)
    return (-1);  

  if (read_int(sockfd, (int*)&(cmd->type)) >= 0)
    {
      switch (cmd->type)
	{
	case REMOVE:
	  result = read_remove_cmd(sockfd, cmd);
	  break;
	case MOVE:
	  result = read_move_cmd(sockfd, cmd);
	  break;
	case LISTDIR:
	  result = read_listdir_cmd(sockfd, cmd);
	  break;
	case MAKEDIR:
	  result = read_makedir_cmd(sockfd, cmd);
	  break;
	case CHMOD:
	  result = read_chmod_cmd(sockfd, cmd);
	  break;
	case SETMETA:
	  result = read_set_metadata_cmd(sockfd, cmd);
	  break;
	case GETMETA:
	  result = read_get_metadata_cmd(sockfd, cmd); 
	  break;
	case STARTMON:
	  result = read_start_monitor_cmd(sockfd, cmd);   
	  break;
	case STOPMON:
	  result = read_stop_monitor_cmd(sockfd, cmd);    
	  break;
	case CLOSE:
	  result = 0;
	  break;
	default:
	}
    }

  
  return result;
}


int      
efsd_write_event(int sockfd, EfsdEvent *ee)
{
  int result = -1;

  if (!ee)
    return (-1);

  if (write_int(sockfd, ee->type) < 0)
    return (-1);

  switch (ee->type)
    {
    case FILECHANGE:
      result = write_filechange_event(sockfd, ee);    
      break;
    case REPLY:
      result = write_reply_event(sockfd, ee);    
      break;
    default:
    }

  if (result < 0)
    fprintf(stderr, "error writing event.\n");

  return result;
}


int      
efsd_read_event(int sockfd, EfsdEvent *ee)
{
  int result = -1;

  if (!ee)
    return (-1);

  if (read_int(sockfd, (int*)&(ee->type)) >= 0)
    {
      switch (ee->type)
	{
	case FILECHANGE:
	  result = read_filechange_event(sockfd, ee);    
	  break;
	case REPLY:
	  result = read_reply_event(sockfd, ee);    
	  break;
	default:
	}
    }

  return result;
}


void     
efsd_cleanup_command(EfsdCommand *ecom)
{
  if (!ecom)
    return;

  switch (ecom->type)
    {
    case REMOVE:
    case LISTDIR:
    case MAKEDIR:
    case CHMOD:
    case STARTMON:
    case STOPMON:
    case STAT:
      if (ecom->efsd_file_cmd.file)
	free(ecom->efsd_file_cmd.file);
      break;
    case MOVE:
    case SYMLINK:
      if (ecom->efsd_2file_cmd.file2)
	free(ecom->efsd_2file_cmd.file1);
      if (ecom->efsd_2file_cmd.file2)
	free(ecom->efsd_2file_cmd.file2);
      break;
    case SETMETA:
      if (ecom->efsd_set_metadata_cmd.data)
	free(ecom->efsd_set_metadata_cmd.data);
      if (ecom->efsd_set_metadata_cmd.key)
	free(ecom->efsd_set_metadata_cmd.key);
      if (ecom->efsd_set_metadata_cmd.file)
	free(ecom->efsd_set_metadata_cmd.file);
      break;
    case GETMETA:
      if (ecom->efsd_get_metadata_cmd.key)
	free(ecom->efsd_get_metadata_cmd.key);
      if (ecom->efsd_get_metadata_cmd.file)
	free(ecom->efsd_get_metadata_cmd.file);
      break;
    case CLOSE:
      break;
    default:
    }
}


void     
efsd_cleanup_event(EfsdEvent *ev)
{
  if (!ev)
    return;
  
  switch (ev->type)
    {
    case REPLY:
      if (ev->efsd_reply_event.data)
	free(ev->efsd_reply_event.data);
      break;
    case FILECHANGE:
      if (ev->efsd_filechange_event.file)
	free(ev->efsd_filechange_event.file);
      break;
    default:
    }     
}
