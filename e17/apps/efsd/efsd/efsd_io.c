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
#include <sys/time.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_io.h>


static int     read_data(int sockfd, void *dest, int size);
static int     read_int(int sockfd, int *dest);
static int     read_string(int sockfd, char **s);
static int     write_data(int sockfd, void *data, int size);
static int     write_int(int sockfd, int data);
static int     write_string(int sockfd, char *s);

static int     read_file_cmd(int sockfd, EfsdCommand *cmd);
static int     read_2file_cmd(int sockfd, EfsdCommand *cmd);
static int     read_chmod_cmd(int sockfd, EfsdCommand *cmd);
static int     read_set_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     read_get_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     read_filechange_event(int sockfd, EfsdEvent *ee);
static int     read_reply_event(int sockfd, EfsdEvent *ee);

static int     write_file_cmd(int sockfd, EfsdCommand *cmd);
static int     write_2file_cmd(int sockfd, EfsdCommand *cmd);
static int     write_chmod_cmd(int sockfd, EfsdCommand *cmd);
static int     write_set_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     write_get_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     write_filechange_event(int sockfd, EfsdEvent *ee);
static int     write_reply_event(int sockfd, EfsdEvent *ee);

int 
read_data(int sockfd, void *dest, int size)
{
  int             num_left, num_read;
  fd_set          fdset;
  char           *ptr;
  struct timeval  tv;

  D_ENTER;

  if (sockfd < 0)
    D_RETURN_(-1);

  ptr = (char*)dest;
  num_left = size;
  num_read = 0;
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  FD_ZERO(&fdset);

  while (num_left)
    {
      FD_SET(sockfd, &fdset);

      while (select(sockfd + 1, &fdset, NULL, NULL, &tv) < 0)
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

      if ((num_read = read(sockfd, ptr, num_left)) < 0)
	{
	  perror("Read error:");
	  D_RETURN_(-1);        /* Error occurred -- return error. */
	}
      else if (num_read == 0)
	break;                  /* End of file */
      
      num_left -= num_read;
      ptr += num_read;
    }
  
  D_RETURN_(size - num_left);
}


static int     
read_int(int sockfd, int *dest)
{
  int count = 0;
   
  D_ENTER;

  if ((count = read_data(sockfd, dest, sizeof(int))) != sizeof(int))
    D_RETURN_(-1);

  D_RETURN_(count);
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
  int count = 0, count2 = 0;

  D_ENTER;

  if ((count = read_data(sockfd, &i, sizeof(int))) != sizeof(int))
    D_RETURN_(-1);

  *s = (char*)malloc(sizeof(char) * i);
  if ((count2 = read_data(sockfd, *s, i)) != i)
    D_RETURN_(-1);

  D_RETURN_(count + count2);
}


static int     
write_data(int sockfd, void *data, int size)
{
  int             num_left, num_written;
  fd_set          fdset;
  char           *ptr;
  struct timeval  tv;

  D_ENTER;

  if (sockfd < 0)
    D_RETURN_(-1);

  ptr = (char*)data;
  num_left = size;
  num_written = 0;
  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  FD_ZERO(&fdset);

  while (num_left)
    {
      FD_SET(sockfd, &fdset);

      while (select(sockfd + 1, NULL, &fdset, NULL, &tv) < 0)
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

      if ((num_written = write(sockfd, ptr, num_left)) < 0)
	{
	  if (errno == EPIPE)
	    {
	      D(("Broken pipe in write_data()\n"));
	      D_RETURN_(-1);
	    }
	  else		
	    {
	      perror("Write error:");
	      D_RETURN_(-1);
	    }
	}
      
      num_left -= num_written;
      ptr += num_written;
    }
  
  D_RETURN_(size - num_left);  
}


static int     
write_int(int sockfd, int data)
{
  D_ENTER;
  D_RETURN_(write_data(sockfd, &data, sizeof(int)));
}


static int     
write_string(int sockfd, char *data)
{
  D_ENTER;

  if (write_int(sockfd, strlen(data)+1) < 0)
    D_RETURN_(-1);

  D_RETURN_(write_data(sockfd, data, strlen(data)+1));
}


static int     
read_file_cmd(int sockfd, EfsdCommand *cmd)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(cmd->efsd_file_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;
   
  if ((count = read_int(sockfd, &(cmd->efsd_file_cmd.options))) < 0)
    D_RETURN_(-1);
  count2 += count;
   
  if ((count = read_string(sockfd, &(cmd->efsd_file_cmd.file))) < 0)
    D_RETURN_(-1);
  count2 += count;

  D_RETURN_(count2);
}


static int     
read_2file_cmd(int sockfd, EfsdCommand *cmd)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(cmd->efsd_2file_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_int(sockfd, &(cmd->efsd_2file_cmd.options))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(cmd->efsd_2file_cmd.file1))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_string(sockfd, &(cmd->efsd_2file_cmd.file2))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  D_RETURN_(count2);
}


static int     
read_chmod_cmd(int sockfd, EfsdCommand *cmd)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(cmd->efsd_chmod_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_string(sockfd, &(cmd->efsd_chmod_cmd.file))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_data(sockfd, &(cmd->efsd_chmod_cmd.mode), sizeof(mode_t))) != sizeof(mode_t))
    D_RETURN_(-1);
  count2 += count;
  
  D_RETURN_(count2);
}


static int     
read_set_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  int  i;
  int count = 0, count2;

  D_ENTER;

  if ((count = read_int(sockfd, &(cmd->efsd_set_metadata_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_data(sockfd, &(cmd->efsd_set_metadata_cmd.datatype),
		sizeof(EfsdDatatype))) != sizeof(EfsdDatatype))
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_int(sockfd, &(cmd->efsd_set_metadata_cmd.data_len))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  i = cmd->efsd_set_metadata_cmd.data_len;
  cmd->efsd_set_metadata_cmd.data = malloc(i);
  if ((count = read_data(sockfd, &(cmd->efsd_set_metadata_cmd.data), i)) != i)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(cmd->efsd_set_metadata_cmd.key))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_string(sockfd, &(cmd->efsd_set_metadata_cmd.file))) < 0)
    D_RETURN_(-1);
  count2 += count;
   
  D_RETURN_(count2);
}


static int     
read_get_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(cmd->efsd_get_metadata_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_string(sockfd, &(cmd->efsd_get_metadata_cmd.key))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(cmd->efsd_get_metadata_cmd.file))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  D_RETURN_(count2);
}


static int     
read_filechange_event(int sockfd, EfsdEvent *ee)
{
  int count, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(ee->efsd_filechange_event.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_int(sockfd, (int*)&(ee->efsd_filechange_event.changetype))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(ee->efsd_filechange_event.file))) < 0)
    D_RETURN_(-1);
  count2 += count;

  D_RETURN_(count2);
}


static int     
read_reply_event(int sockfd, EfsdEvent *ee)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = efsd_read_command(sockfd, &(ee->efsd_reply_event.command))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_int(sockfd, (int*)&(ee->efsd_reply_event.status))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_int(sockfd, &(ee->efsd_reply_event.errorcode))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_int(sockfd, &(ee->efsd_reply_event.data_len))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  if (ee->efsd_reply_event.data_len > 0)
    {
      ee->efsd_reply_event.data = malloc(ee->efsd_reply_event.data_len);
      if ((count = read_data(sockfd, (ee->efsd_reply_event.data),
			     ee->efsd_reply_event.data_len)) < 0)
	D_RETURN_(-1);
      count2 += count;
    }
  else
    {
      ee->efsd_reply_event.data_len = 0;
      ee->efsd_reply_event.data = NULL;
    }

  D_RETURN_(count2);  
}


static int     
write_file_cmd(int sockfd, EfsdCommand *cmd)
{
  D_ENTER;

  if (write_int(sockfd, cmd->efsd_file_cmd.id) < 0)
    D_RETURN_(-1);

  if (write_int(sockfd, cmd->efsd_file_cmd.options) < 0)
    D_RETURN_(-1);
  
  if (write_string(sockfd, cmd->efsd_file_cmd.file) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


static int     
write_2file_cmd(int sockfd, EfsdCommand *cmd)
{
  D_ENTER;

  if (write_int(sockfd, cmd->efsd_2file_cmd.id) < 0)
    D_RETURN_(-1);
  
  if (write_int(sockfd, cmd->efsd_2file_cmd.options) < 0)
    D_RETURN_(-1);
  
  if (write_string(sockfd, cmd->efsd_2file_cmd.file1) < 0)
    D_RETURN_(-1);

  if (write_string(sockfd, cmd->efsd_2file_cmd.file2) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


static int     
write_chmod_cmd(int sockfd, EfsdCommand *cmd)
{
  D_ENTER;

  if (write_int(sockfd, cmd->efsd_chmod_cmd.id) < 0)
    D_RETURN_(-1);

  if (write_string(sockfd, cmd->efsd_chmod_cmd.file) < 0)
    D_RETURN_(-1);
  
  if (write_data(sockfd, &(cmd->efsd_chmod_cmd.mode),
		 sizeof(cmd->efsd_chmod_cmd.mode)) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}
  

static int     
write_set_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  D_ENTER;

  if (write_int(sockfd, cmd->efsd_set_metadata_cmd.id) < 0)
    D_RETURN_(-1);
  
  if (write_data(sockfd, &(cmd->efsd_set_metadata_cmd.datatype),
		 sizeof(EfsdDatatype)) < 0)
    D_RETURN_(-1);
  
  if (write_int(sockfd, cmd->efsd_set_metadata_cmd.data_len) < 0)
    D_RETURN_(-1);
  
  if (write_data(sockfd, cmd->efsd_set_metadata_cmd.data,
		 cmd->efsd_set_metadata_cmd.data_len) < 0)
    D_RETURN_(-1);
  
  if (write_string(sockfd, cmd->efsd_set_metadata_cmd.key) < 0)
    D_RETURN_(-1);

  if (write_string(sockfd, cmd->efsd_set_metadata_cmd.file) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


static int     
write_get_metadata_cmd(int sockfd, EfsdCommand *cmd)
{
  D_ENTER;

  if (write_int(sockfd, cmd->efsd_get_metadata_cmd.id) < 0)
    D_RETURN_(-1);
    
  if (write_string(sockfd, cmd->efsd_get_metadata_cmd.key) < 0)
    D_RETURN_(-1);

  if (write_string(sockfd, cmd->efsd_get_metadata_cmd.file) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


static int     
write_filechange_event(int sockfd, EfsdEvent *ee)
{
  D_ENTER;

  if (write_int(sockfd, ee->efsd_filechange_event.id) < 0)
    D_RETURN_(-1);

  if (write_int(sockfd, ee->efsd_filechange_event.changetype) < 0)
    D_RETURN_(-1);

  if (write_string(sockfd, ee->efsd_filechange_event.file) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


static int     
write_reply_event(int sockfd, EfsdEvent *ee)
{
  D_ENTER;

  if (efsd_write_command(sockfd, &(ee->efsd_reply_event.command)) < 0)
    D_RETURN_(-1);

  if (write_int(sockfd, ee->efsd_reply_event.status) < 0)
    D_RETURN_(-1);

  if (write_int(sockfd, ee->efsd_reply_event.errorcode) < 0)
    D_RETURN_(-1);

  if (write_int(sockfd, ee->efsd_reply_event.data_len) < 0)
    D_RETURN_(-1);

  if (write_data(sockfd, ee->efsd_reply_event.data,
		 ee->efsd_reply_event.data_len) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


/* Non-static stuff below: */

int      
efsd_write_command(int sockfd, EfsdCommand *cmd)
{
  int result = (-1);

  D_ENTER;

  if (!cmd)
    D_RETURN_(-1);  

  if (write_int(sockfd, cmd->type) < 0)
    D_RETURN_(-1);
  
  switch (cmd->type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_STARTMON:
    case EFSD_CMD_STOPMON:
    case EFSD_CMD_STAT:
    case EFSD_CMD_READLINK:
      result = write_file_cmd(sockfd, cmd);
      break;
    case EFSD_CMD_MOVE:
    case EFSD_CMD_SYMLINK:
      result = write_2file_cmd(sockfd, cmd);
      break;
    case EFSD_CMD_CHMOD:
      result = write_chmod_cmd(sockfd, cmd);
      break;
    case EFSD_CMD_SETMETA:
      result = write_set_metadata_cmd(sockfd, cmd);
      break;
    case EFSD_CMD_GETMETA:
      result = write_get_metadata_cmd(sockfd, cmd); 
      break;
    case EFSD_CMD_CLOSE:
      result = 0;
      break;
    default:
    }

  if (result < 0)
    fprintf(stderr, "error writing command.\n");

  D_RETURN_(result);
}


int      
efsd_read_command(int sockfd, EfsdCommand *cmd)
{
  int result = -1;
  int count = 0;

  D_ENTER;

  if (!cmd)
    D_RETURN_(-1);  

  if ((count = read_int(sockfd, (int*)&(cmd->type))) >= 0)
    {
      switch (cmd->type)
	{
	case EFSD_CMD_REMOVE:
	case EFSD_CMD_LISTDIR:
	case EFSD_CMD_MAKEDIR:
	case EFSD_CMD_STARTMON:
	case EFSD_CMD_STOPMON:
	case EFSD_CMD_STAT:
	case EFSD_CMD_READLINK:
	  result = read_file_cmd(sockfd, cmd);
	  break;
	case EFSD_CMD_MOVE:
	case EFSD_CMD_SYMLINK:
	  result = read_2file_cmd(sockfd, cmd);
	  break;
	case EFSD_CMD_CHMOD:
	  result = read_chmod_cmd(sockfd, cmd);
	  break;
	case EFSD_CMD_SETMETA:
	  result = read_set_metadata_cmd(sockfd, cmd);
	  break;
	case EFSD_CMD_GETMETA:
	  result = read_get_metadata_cmd(sockfd, cmd); 
	  break;
	case EFSD_CMD_CLOSE:
	  result = 0;
	  break;
	default:
	}
    }

  
  D_RETURN_(result + count);
}


int      
efsd_write_event(int sockfd, EfsdEvent *ee)
{
  int result = -1;

  D_ENTER;

  if (!ee)
    D_RETURN_(-1);

  if (write_int(sockfd, ee->type) < 0)
    D_RETURN_(-1);

  switch (ee->type)
    {
    case EFSD_EVENT_FILECHANGE:
      D(("Writing filechange event.\n"));
      result = write_filechange_event(sockfd, ee);    
      break;
    case EFSD_EVENT_REPLY:
      D(("Writing reply event.\n"));
      result = write_reply_event(sockfd, ee);    
      break;
    default:
    }

  if (result < 0)
    fprintf(stderr, "error writing event.\n");

  D_RETURN_(result);
}


int      
efsd_read_event(int sockfd, EfsdEvent *ee)
{
  int result = -1;
  int count = 0;
   
  D_ENTER;

  if (!ee)
    D_RETURN_(-1);

  if ((count = read_int(sockfd, (int*)&(ee->type))) >= 0)
    {
      switch (ee->type)
	{
	case EFSD_EVENT_FILECHANGE:
	  result = read_filechange_event(sockfd, ee);    
	  break;
	case EFSD_EVENT_REPLY:
	  result = read_reply_event(sockfd, ee);    
	  break;
	default:
	}
    }

  D_RETURN_(count + result);
}


void     
efsd_cleanup_command(EfsdCommand *ecom)
{
  D_ENTER;

  if (!ecom)
    D_RETURN;

  switch (ecom->type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_CHMOD:
    case EFSD_CMD_STARTMON:
    case EFSD_CMD_STOPMON:
    case EFSD_CMD_STAT:
    case EFSD_CMD_READLINK:
      FREE(ecom->efsd_file_cmd.file);
      break;
    case EFSD_CMD_MOVE:
    case EFSD_CMD_SYMLINK:
      FREE(ecom->efsd_2file_cmd.file1);
      FREE(ecom->efsd_2file_cmd.file2);
      break;
    case EFSD_CMD_SETMETA:
      FREE(ecom->efsd_set_metadata_cmd.data);
      FREE(ecom->efsd_set_metadata_cmd.key);
      FREE(ecom->efsd_set_metadata_cmd.file);
      break;
    case EFSD_CMD_GETMETA:
      FREE(ecom->efsd_get_metadata_cmd.key);
      FREE(ecom->efsd_get_metadata_cmd.file);
      break;
    case EFSD_CMD_CLOSE:
      break;
    default:
    }
  D_RETURN;
}


void     
efsd_cleanup_event(EfsdEvent *ev)
{
  D_ENTER;

  if (!ev)
    D_RETURN;
  
  switch (ev->type)
    {
    case EFSD_EVENT_REPLY:
      FREE(ev->efsd_reply_event.data);
      break;
    case EFSD_EVENT_FILECHANGE:
      FREE(ev->efsd_filechange_event.file);
      break;
    default:
    }
  D_RETURN;
}
