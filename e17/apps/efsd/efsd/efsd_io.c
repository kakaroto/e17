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

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#ifdef __EMX__
#include <sys/select.h>
#include <strings.h>
#endif 

#include <efsd_debug.h>
#include <efsd_globals.h>
#include <efsd_misc.h>
#include <efsd_io.h>

/* The maximum number of data chunks (ints, char*s, void*s ...)
   a command/event consists of. Major laziness.
*/
#define EFSD_MAX_IOVEC  256

/* Structure that holds entire messages -- 
   we need to capsulate them so that we
   can assemble multiple ones in parallel...
 */
typedef struct efsd_iov
{
  /* data pointers */
  struct iovec vec[EFSD_MAX_IOVEC];

  /* vector of temporarily needed data */
  int          dat[EFSD_MAX_IOVEC];

  /* current indices into VEC and DAT */
  int          v;
  int          d;
}
EfsdIOV;

#if HAVE_ECORE
typedef struct ecore_ipc_message ecore_ipc_message;
struct ecore_ipc_message {
	int major;
	int minor;
	int ref;
	int ref_to;
	int response;
	void* data;
	int len;
};

ecore_ipc_message* ecore_ipc_message_new(int major, int minor, int ref, int ref_to, int response, void* data, int len) {
	ecore_ipc_message* msg= malloc(sizeof(ecore_ipc_message));

	msg->major = major;
	msg->minor = minor;
	msg->ref = ref;
	msg->ref_to = ref_to;
	msg->response = response;
	msg->data = data;
	msg->len = len;

	return msg;
}
#endif


static int     read_data(int sockfd, void *dest, int size);
static int     read_int(int sockfd, int *dest);
static int     read_string(int sockfd, char **s);
static int     write_data(int sockfd, struct msghdr *msg);

static int     read_file_cmd(int sockfd, EfsdCommand *cmd);
static int     read_chmod_cmd(int sockfd, EfsdCommand *cmd);
static int     read_set_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     read_get_metadata_cmd(int sockfd, EfsdCommand *cmd);
static int     read_filechange_event(int sockfd, EfsdEvent *ee);
static int     read_metadata_change_event(int sockfd, EfsdEvent *ee);
static int     read_reply_event(int sockfd, EfsdEvent *ee);
static int     read_getmeta_op(int sockfd, EfsdOption *eo);

#if HAVE_ECORE
static Ecore_List*
fill_file_cmd(EfsdCommand *ec);
#else
static void
fill_file_cmd(EfsdIOV *iov, EfsdCommand *ec);
#endif

static void    fill_chmod_cmd(EfsdIOV *iov, EfsdCommand *ec);
static void    fill_set_metadata_cmd(EfsdIOV *iov, EfsdCommand *ec);
static void    fill_get_metadata_cmd(EfsdIOV *iov, EfsdCommand *ec);
static void    fill_close_cmd(EfsdIOV *iov, EfsdCommand *ec);
static void    fill_filechange_event(EfsdIOV *iov, EfsdEvent *ee);
static void    fill_metadata_change_event(EfsdIOV *iov, EfsdEvent *ee);

#if HAVE_ECORE
static Ecore_List*
fill_reply_event(EfsdEvent *ee);
#else
static void    
fill_reply_event(EfsdIOV *iov, EfsdEvent *ee);
#endif

#if HAVE_ECORE
static Ecore_List*
fill_event(EfsdEvent *ee);
#else

static void
fill_event(EfsdIOV *iov, EfsdEvent *ee);
#endif

#if HAVE_ECORE
static Ecore_List*  
fill_command(EfsdCommand *ec);
#else
static void
fill_command(EfsdIOV *iov, EfsdCommand *ec);
#endif

static void    fill_option(EfsdIOV *iov, EfsdOption *eo);


static int 
read_data(int sockfd, void *dest, int size)
{
  int             n, result;
  fd_set          fdset;
  struct timeval  tv;
  struct msghdr   msg;
  struct iovec    iov[1];

  D_ENTER;

  if (sockfd < 0)
    {
      D("Socket < 0 ???\n");
      D_RETURN_(-1);
    }

  tv.tv_sec  = 1;
  tv.tv_usec = 0;
  FD_ZERO(&fdset);
  FD_SET(sockfd, &fdset);

  while ((result = select(sockfd + 1, &fdset, NULL, NULL, &tv)) < 0)
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

  if (result == 0)
    {
      D("Read timed out... %i\n", errno);
      D_RETURN_(-1);
    }

  iov[0].iov_base = dest;
  iov[0].iov_len  = size;
  memset(&msg, 0, sizeof(struct msghdr));
  msg.msg_iov = iov;
  msg.msg_iovlen = 1;

#ifndef __EMX__
  if ((n = recvmsg(sockfd, &msg, MSG_WAITALL)) < 0)
#else
  if ((n = recvmsg(_getsockhandle(sockfd), &msg, MSG_WAITALL)) < 0)
#endif
    {
      perror("Error");
      D_RETURN_(-1);
    }

  D_RETURN_(n);
}


static int     
read_int(int sockfd, int *dest)
{
  int count = 0;
   
  D_ENTER;

  if ( (count = read_data(sockfd, dest, sizeof(int))) != sizeof(int))
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
write_data(int sockfd, struct msghdr *msg)
{
  int             n;

  D_ENTER;

  if (sockfd < 0)
    D_RETURN_(-1);

#ifndef __EMX__
  if ((n = sendmsg(sockfd, msg, 0)) < 0)
#else
  if ((n = sendmsg(_getsockhandle(sockfd), msg, 0)) < 0)
#endif
    {
      if (errno == EAGAIN)
	{
	  D("Couldn't write data -- buffer not ready.\n");
	}
      else
	{
	  D("Sendmsg error -- %i\n", n);
	  perror("error");      
	}
    }

  D_RETURN_(n);
}


static int     
read_file_cmd(int sockfd, EfsdCommand *ec)
{
  int count = 0, count2, i;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(ec->efsd_file_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_int(sockfd, &(ec->efsd_file_cmd.num_files))) < 0)
    D_RETURN_(-1);
  count2 += count;

  ec->efsd_file_cmd.files = malloc(sizeof(char*) * ec->efsd_file_cmd.num_files);

  for (i = 0; i < ec->efsd_file_cmd.num_files; i++)
    {
      if ((count = read_string(sockfd, &(ec->efsd_file_cmd.files[i]))) < 0)
	D_RETURN_(-1);

      count2 += count;
      efsd_misc_remove_trailing_slashes(ec->efsd_file_cmd.files[i]);
    }

  if ((count = read_int(sockfd, &(ec->efsd_file_cmd.num_options))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if (ec->efsd_file_cmd.num_options > 0)
    {
      ec->efsd_file_cmd.options = (EfsdOption*)
	malloc(sizeof(EfsdOption) * ec->efsd_file_cmd.num_options);
  
      for (i = 0; i < ec->efsd_file_cmd.num_options; i++)
	{
	  efsd_io_read_option(sockfd, &(ec->efsd_file_cmd.options[i]));
	}
    }

  D_RETURN_(count2);
}


static int     
read_chmod_cmd(int sockfd, EfsdCommand *ec)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(ec->efsd_chmod_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_string(sockfd, &(ec->efsd_chmod_cmd.file))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_data(sockfd, &(ec->efsd_chmod_cmd.mode), sizeof(mode_t))) != sizeof(mode_t))
    D_RETURN_(-1);
  count2 += count;
  
  D_RETURN_(count2);
}


static int     
read_set_metadata_cmd(int sockfd, EfsdCommand *ec)
{
  int  i;
  int count = 0, count2;

  D_ENTER;

  if ((count = read_int(sockfd, &(ec->efsd_set_metadata_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_data(sockfd, &(ec->efsd_set_metadata_cmd.datatype),
		sizeof(EfsdDatatype))) != sizeof(EfsdDatatype))
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_int(sockfd, &(ec->efsd_set_metadata_cmd.data_len))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  i = ec->efsd_set_metadata_cmd.data_len;

  ec->efsd_set_metadata_cmd.data = malloc(i);
  if ((count = read_data(sockfd, ec->efsd_set_metadata_cmd.data, i)) != i)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(ec->efsd_set_metadata_cmd.key))) < 0)
    D_RETURN_(-1);
  count2 += count;
  
  if ((count = read_string(sockfd, &(ec->efsd_set_metadata_cmd.file))) < 0)
    D_RETURN_(-1);
  count2 += count;
   
  D_RETURN_(count2);
}


static int     
read_get_metadata_cmd(int sockfd, EfsdCommand *ec)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(ec->efsd_get_metadata_cmd.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_data(sockfd, &(ec->efsd_get_metadata_cmd.datatype),
		sizeof(EfsdDatatype))) != sizeof(EfsdDatatype))
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(ec->efsd_get_metadata_cmd.key))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(ec->efsd_get_metadata_cmd.file))) < 0)
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
read_metadata_change_event(int sockfd, EfsdEvent *ee)
{
  int count, count2;
   
  D_ENTER;

  if ((count = read_int(sockfd, &(ee->efsd_metachange_event.id))) < 0)
    D_RETURN_(-1);
  count2 = count;

  if ((count = read_string(sockfd, &(ee->efsd_metachange_event.key))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_string(sockfd, &(ee->efsd_metachange_event.file))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_int(sockfd, (int*)&(ee->efsd_metachange_event.datatype))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if ((count = read_int(sockfd, (int*)&(ee->efsd_metachange_event.data_len))) < 0)
    D_RETURN_(-1);
  count2 += count;

  if (ee->efsd_metachange_event.data_len > 0)
    {
      ee->efsd_metachange_event.data =
	malloc(ee->efsd_metachange_event.data_len);
      
      if ((count = read_data(sockfd, (ee->efsd_metachange_event.data),
			     ee->efsd_metachange_event.data_len)) < 0)
	D_RETURN_(-1);
      count2 += count;
    }
  else
    {
      ee->efsd_metachange_event.data_len = 0;
      ee->efsd_metachange_event.data = NULL;
    }

  D_RETURN_(count2);
}


static int     
read_reply_event(int sockfd, EfsdEvent *ee)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = efsd_io_read_command(sockfd, &(ee->efsd_reply_event.command))) < 0)
    D_RETURN_(-1);
  count2 = count;

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
read_getmeta_op(int sockfd, EfsdOption *eo)
{
  int count = 0, count2;
   
  D_ENTER;

  if ((count = read_string(sockfd, &(eo->efsd_op_getmeta.key))) < 0)
    D_RETURN_(-1);
  count2 = count;
   
  if ((count = read_int(sockfd, (int*)&(eo->efsd_op_getmeta.datatype))) < 0)
    D_RETURN_(-1);
  count2 += count;

  D_RETURN_(count2);
}

#if HAVE_ECORE
static Ecore_List*
fill_file_cmd(EfsdCommand *ec)
#else
static void
fill_file_cmd(EfsdIOV *iov, EfsdCommand *ec)
#endif
{
  #if HAVE_ECORE
  Ecore_List* el;
  
  #endif
	
  	
  int i;

  D_ENTER;

  
  #if HAVE_ECORE
  el = ecore_list_new();
  

  	
  
  ecore_list_append(el, ecore_ipc_message_new(1, 1, 0,0,0, &ec->type, sizeof(EfsdCommandType)));
  
  ecore_list_append(el, ecore_ipc_message_new(1, 2, 0,0,0, &ec->efsd_file_cmd.id, sizeof(EfsdCmdId)));

  for (i = 0; i < ec->efsd_file_cmd.num_files; i++) {
	  printf ("ERR: Writing filename %s\n", ec->efsd_file_cmd.files[i]);
	  ecore_list_append(el, ecore_ipc_message_new(1, 3, 0,0,0, ec->efsd_file_cmd.files[i], strlen(ec->efsd_file_cmd.files[i]) + 1)); 
  }

  for (i = 0; i < ec->efsd_file_cmd.num_options; i++) {
	  printf ("ERR: Writing option %d\n", ec->efsd_file_cmd.options[i]);
	  ecore_list_append(el, ecore_ipc_message_new(1, 4, 0,0,0, &ec->efsd_file_cmd.options[i], sizeof(EfsdOption)));

  }

  return el;
  
  #else
  iov->vec[iov->v].iov_base   = &ec->type;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCommandType);
  iov->vec[++iov->v].iov_base = &ec->efsd_file_cmd.id;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCmdId);
  iov->vec[++iov->v].iov_base = &ec->efsd_file_cmd.num_files;
  iov->vec[iov->v].iov_len    = sizeof(int);

  for (i = 0; i < ec->efsd_file_cmd.num_files; i++)
    {
      iov->dat[iov->d] = strlen(ec->efsd_file_cmd.files[i]) + 1;

      iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
      iov->vec[iov->v].iov_len    = sizeof(int);
      iov->vec[++iov->v].iov_base = ec->efsd_file_cmd.files[i];
      iov->vec[iov->v].iov_len    = iov->dat[iov->d];

      iov->d++;
    }

  iov->vec[++iov->v].iov_base = &ec->efsd_file_cmd.num_options;
  iov->vec[iov->v].iov_len    = sizeof(int);

  iov->v++;

  /* Fill in options, if they exist */
  if (ec->efsd_file_cmd.num_options > 0)
    {
      for (i = 0; i < ec->efsd_file_cmd.num_options; i++)
	{
	  fill_option(iov, &(ec->efsd_file_cmd.options[i]));
	}
    }

  D_RETURN;

  #endif
}


static void   
fill_chmod_cmd(EfsdIOV *iov, EfsdCommand *ec)
{
  D_ENTER;

  iov->dat[iov->d] = strlen(ec->efsd_chmod_cmd.file) + 1;

  iov->vec[iov->v].iov_base   = &ec->type;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCommandType);
  iov->vec[++iov->v].iov_base = &ec->efsd_chmod_cmd.id;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCmdId);
  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ec->efsd_chmod_cmd.file;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d];
  iov->vec[++iov->v].iov_base = &ec->efsd_chmod_cmd.mode;
  iov->vec[iov->v].iov_len    = sizeof(mode_t);

  iov->d++;
  iov->v++;

  D_RETURN;
}
  

static void
fill_set_metadata_cmd(EfsdIOV *iov, EfsdCommand *ec)
{
  D_ENTER;

  iov->dat[iov->d]   = strlen(ec->efsd_set_metadata_cmd.key) + 1;
  iov->dat[iov->d+1] = strlen(ec->efsd_set_metadata_cmd.file) + 1;
  
  iov->vec[iov->v].iov_base   = &ec->type;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCommandType);
  iov->vec[++iov->v].iov_base = &ec->efsd_set_metadata_cmd.id;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCmdId);
  iov->vec[++iov->v].iov_base = &ec->efsd_set_metadata_cmd.datatype;
  iov->vec[iov->v].iov_len    = sizeof(EfsdDatatype);
  iov->vec[++iov->v].iov_base = &ec->efsd_set_metadata_cmd.data_len;
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ec->efsd_set_metadata_cmd.data;
  iov->vec[iov->v].iov_len    = ec->efsd_set_metadata_cmd.data_len;
  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ec->efsd_set_metadata_cmd.key;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d];
  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d+1]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ec->efsd_set_metadata_cmd.file;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d+1];

  iov->d += 2;
  iov->v++;

  D_RETURN;
}


static void
fill_get_metadata_cmd(EfsdIOV *iov, EfsdCommand *ec)
{
  D_ENTER;

  iov->dat[iov->d]   = strlen(ec->efsd_get_metadata_cmd.key) + 1;
  iov->dat[iov->d+1] = strlen(ec->efsd_get_metadata_cmd.file) + 1;
  
  iov->vec[iov->v].iov_base   = &ec->type;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCommandType);
  iov->vec[++iov->v].iov_base = &ec->efsd_set_metadata_cmd.id;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCmdId);
  iov->vec[++iov->v].iov_base = &ec->efsd_set_metadata_cmd.datatype;
  iov->vec[iov->v].iov_len    = sizeof(EfsdDatatype);
  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ec->efsd_get_metadata_cmd.key;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d];
  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d+1]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ec->efsd_get_metadata_cmd.file;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d+1];

  iov->d += 2;
  iov->v++;

  D_RETURN;
}


static void
fill_close_cmd(EfsdIOV *iov, EfsdCommand *ec)
{
  D_ENTER;

  iov->vec[iov->v].iov_base = &ec->type;
  iov->vec[iov->v].iov_len  = sizeof(EfsdCommandType);

  iov->v++;

  D_RETURN;
}


static void
fill_filechange_event(EfsdIOV *iov, EfsdEvent *ee)
{
  D_ENTER;

  iov->dat[iov->d] = strlen(ee->efsd_filechange_event.file) + 1;

  iov->vec[iov->v].iov_base   = &ee->type;
  iov->vec[iov->v].iov_len    = sizeof(EfsdEventType);
  iov->vec[++iov->v].iov_base = &ee->efsd_filechange_event.id;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCmdId);
  iov->vec[++iov->v].iov_base = &ee->efsd_filechange_event.changetype;
  iov->vec[iov->v].iov_len    = sizeof(EfsdFilechangeType);
  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ee->efsd_filechange_event.file;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d];

  iov->d++;
  iov->v++;

  D_RETURN;
}


static void
fill_metadata_change_event(EfsdIOV *iov, EfsdEvent *ee)
{
  D_ENTER;

  iov->dat[iov->d]   = strlen(ee->efsd_metachange_event.key) + 1;
  iov->dat[iov->d+1] = strlen(ee->efsd_metachange_event.file) + 1;

  iov->vec[iov->v].iov_base = &ee->type;
  iov->vec[iov->v].iov_len  = sizeof(EfsdEventType);

  iov->vec[++iov->v].iov_base = &ee->efsd_metachange_event.id;
  iov->vec[iov->v].iov_len    = sizeof(EfsdCmdId);

  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ee->efsd_metachange_event.key;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d];

  iov->vec[++iov->v].iov_base = &(iov->dat[iov->d+1]);
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ee->efsd_metachange_event.file;
  iov->vec[iov->v].iov_len    = iov->dat[iov->d+1];

  iov->vec[++iov->v].iov_base = &ee->efsd_metachange_event.datatype;
  iov->vec[iov->v].iov_len    = sizeof(int);

  iov->vec[++iov->v].iov_base = &ee->efsd_metachange_event.data_len;
  iov->vec[iov->v].iov_len    = sizeof(int);

  iov->vec[++iov->v].iov_base = ee->efsd_metachange_event.data;
  iov->vec[iov->v].iov_len    = ee->efsd_metachange_event.data_len;

  iov->d += 2;
  iov->v++;

  D_RETURN;
}

#if HAVE_ECORE
static Ecore_List*
fill_reply_event(EfsdEvent *ee)
#else
static void    
fill_reply_event(EfsdIOV *iov, EfsdEvent *ee)
#endif
	
{
  #if HAVE_ECORE
  Ecore_List* el;	
  ecore_ipc_message* msg;
  #endif
	
  D_ENTER;

  #if HAVE_ECORE
  el = ecore_list_new();

  ecore_list_append(el, ecore_ipc_message_new(EFSD_EVENT_REPLY, 1, 0,0,0,&ee->type, sizeof(EfsdEventType)));
  ecore_list_append(el, ecore_ipc_message_new(EFSD_EVENT_REPLY, 2, 0,0,0,ee->efsd_reply_event.data, strlen(ee->efsd_reply_event.data)));


  return el;

  
  
  
  
  #else

  iov->vec[iov->v].iov_base = &ee->type;
  iov->vec[iov->v].iov_len  = sizeof(EfsdEventType);
  iov->v++;

  fill_command(&ee->efsd_reply_event.command);

  iov->vec[iov->v].iov_base   = &ee->efsd_reply_event.errorcode;
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = &ee->efsd_reply_event.data_len;
  iov->vec[iov->v].iov_len    = sizeof(int);
  iov->vec[++iov->v].iov_base = ee->efsd_reply_event.data;
  iov->vec[iov->v].iov_len    = ee->efsd_reply_event.data_len;

  iov->v++;
  #endif

  D_RETURN;
}


#if HAVE_ECORE
static Ecore_List*
fill_event(EfsdEvent *ee)
#else

static void
fill_event(EfsdIOV *iov, EfsdEvent *ee)
#endif
{
  D_ENTER;

  switch (ee->type)
    {
    case EFSD_EVENT_FILECHANGE:
      fill_filechange_event(NULL, ee);
      break;
    case EFSD_EVENT_METADATA_CHANGE:
      fill_metadata_change_event(NULL, ee);
      break;
    case EFSD_EVENT_REPLY:
      return fill_reply_event(ee);
      break;
    default:
      D(("Unknown event.\n"));
    }

  D_RETURN;
}


#if HAVE_ECORE
static Ecore_List*  
fill_command(EfsdCommand *ec)
#else
static void
fill_command(EfsdIOV *iov, EfsdCommand *ec)
#endif
{
  D_ENTER;

  switch (ec->type)
    {
    case EFSD_CMD_REMOVE:
    case EFSD_CMD_MAKEDIR:
    case EFSD_CMD_LISTDIR:
    case EFSD_CMD_STARTMON_FILE:
    case EFSD_CMD_STARTMON_DIR:
    case EFSD_CMD_STARTMON_META:
    case EFSD_CMD_STOPMON_FILE:
    case EFSD_CMD_STOPMON_DIR:
    case EFSD_CMD_STOPMON_META:
    case EFSD_CMD_STAT:
    case EFSD_CMD_LSTAT:
    case EFSD_CMD_READLINK:
    case EFSD_CMD_GETFILETYPE:
    case EFSD_CMD_MOVE:
    case EFSD_CMD_COPY:
    case EFSD_CMD_SYMLINK:
	#if HAVE_ECORE
	    return fill_file_cmd(ec);
	#else
	    fill_file_cmd(iov, ec);
	#endif
      break;
    case EFSD_CMD_CHMOD:
      fill_chmod_cmd(NULL,ec);
      break;
    case EFSD_CMD_SETMETA:
      fill_set_metadata_cmd(NULL, ec);
      break;
    case EFSD_CMD_GETMETA:
      fill_get_metadata_cmd(NULL, ec);
      break;
    case EFSD_CMD_CLOSE:
      fill_close_cmd(NULL, ec);
      break;
    default:
      D(("Unknown command.\n"));
    }

  D_RETURN;
}


static void    
fill_option(EfsdIOV *iov, EfsdOption *eo)
{
  D_ENTER;

  iov->vec[iov->v].iov_base   = &eo->type;
  iov->vec[iov->v].iov_len    = sizeof(EfsdOptionType);
  
  switch (eo->type)
    {
    case EFSD_OP_FORCE:
    case EFSD_OP_RECURSIVE:
    case EFSD_OP_GET_STAT:
    case EFSD_OP_GET_LSTAT:
    case EFSD_OP_ALL:
    case EFSD_OP_GET_FILETYPE:
    case EFSD_OP_SORT:
      break;
    case EFSD_OP_GET_META:
      iov->dat[iov->d] = strlen(eo->efsd_op_getmeta.key) + 1;
      
      iov->vec[++iov->v].iov_base = &(iov->dat[iov->d]);
      iov->vec[iov->v].iov_len    = sizeof(int);
      iov->vec[++iov->v].iov_base = eo->efsd_op_getmeta.key;
      iov->vec[iov->v].iov_len    = iov->dat[iov->d];
      iov->vec[++iov->v].iov_base = &(eo->efsd_op_getmeta.datatype);
      iov->vec[iov->v].iov_len    = sizeof(int);

      iov->d++;
      break;
    default:
      D(("Unknown option.\n"));
    }

  iov->v++;

  D_RETURN;
}


/* Non-static stuff below: */

#if HAVE_ECORE
int efsd_io_write_command(Ecore_Ipc_Server* server, EfsdCommand *ec)
#else
int      
efsd_io_write_command(int sockfd, EfsdCommand *ec)
#endif
{

  #if HAVE_ECORE
	Ecore_List* cmd;
	ecore_ipc_message* emsg;
  #endif



	
  EfsdIOV         iov;
  struct msghdr   msg;
  int             n;
  int		  i;

  D_ENTER;


  #if HAVE_ECORE
  printf("ERR: ecore Sending command: efsd_io_write_command\n");
  

  cmd = fill_command(ec);

  ecore_list_goto_first(cmd);
  while ( (emsg = ecore_list_next(cmd))) {
	  ecore_ipc_server_send(server, emsg->major, emsg->minor, emsg->ref, emsg->ref_to, emsg->response, emsg->data, emsg->len);
  }

  /*Notify the server that we are readyt to burn*/
  ecore_ipc_server_send(server, 1, 100, 0,0,0, NULL, 0);
  
    
 
  D_RETURN_(0);
  #else
  if (!ec)
    D_RETURN_(-1);

  iov.v = iov.d = 0;

  memset(&msg, 0, sizeof(struct msghdr));

  

  msg.msg_iov = iov.vec;
  msg.msg_iovlen = iov.v;

  
  cmd = fill_command(&iov, ec);


  if ((n = write_data(sockfd, &msg)) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
  #endif


}

int      
efsd_io_read_command(int sockfd, EfsdCommand *ec)
{
  int result = -1;
  int count = 0;

  D_ENTER;

  if (!ec)
    D_RETURN_(-1);  

  memset(ec, 0, sizeof(EfsdCommand));

  if ((count = read_int(sockfd, (int*)&(ec->type))) >= 0)
    {
      switch (ec->type)
	{
	case EFSD_CMD_REMOVE:
	case EFSD_CMD_MAKEDIR:
	case EFSD_CMD_LISTDIR:
	case EFSD_CMD_STARTMON_FILE:
	case EFSD_CMD_STARTMON_DIR:
	case EFSD_CMD_STARTMON_META:
	case EFSD_CMD_STOPMON_FILE:
	case EFSD_CMD_STOPMON_DIR:
	case EFSD_CMD_STOPMON_META:
	case EFSD_CMD_STAT:
	case EFSD_CMD_LSTAT:
	case EFSD_CMD_READLINK:
	case EFSD_CMD_GETFILETYPE:
	case EFSD_CMD_COPY:
	case EFSD_CMD_MOVE:
	case EFSD_CMD_SYMLINK:
	  result = read_file_cmd(sockfd, ec);
	  break;
	case EFSD_CMD_CHMOD:
	  result = read_chmod_cmd(sockfd, ec);
	  break;
	case EFSD_CMD_SETMETA:
	  result = read_set_metadata_cmd(sockfd, ec);
	  break;
	case EFSD_CMD_GETMETA:
	  result = read_get_metadata_cmd(sockfd, ec); 
	  break;
	case EFSD_CMD_CLOSE:
	  result = 0;
	  break;
	default:
	  D(("Unknown command\n"));
	}
    }

  D_RETURN_(result + count);
}


#if HAVE_ECORE
int      
efsd_io_write_event(Ecore_Ipc_Client* sockfd, EfsdEvent *ee)
#else
int      
efsd_io_write_event(int sockfd, EfsdEvent *ee)
#endif
{
  
  #if HAVE_ECORE
  Ecore_List* cmd;	
  ecore_ipc_message* msg;

  printf("ERR: Sending event..\n");
  /*ecore_ipc_client_send(sockfd, ee->type, 1, 0,0,0, ee->efsd_reply_event.data,strlen(ee->efsd_reply_event.data)+1);*/
  
  cmd = fill_event(ee);

  /*Write these messages to the clients*/
  ecore_list_goto_first(cmd);
  while ( (msg = ecore_list_next(cmd)) ) {
	  ecore_ipc_client_send(sockfd, msg->major, msg->minor, msg->ref, msg->ref_to, msg->response,msg->data, msg->len);
  }

	
  #else
  EfsdIOV         iov;
  struct msghdr   msg;
  int             n;

  D_ENTER;

  if (!ee)
    D_RETURN_(-1);

  iov.v = iov.d = 0;

  memset(&msg, 0, sizeof(struct msghdr));

  fill_event(&iov, ee);

  msg.msg_iov = iov.vec;
  msg.msg_iovlen = iov.v;

  if ((n = write_data(sockfd, &msg)) < 0)
    D_RETURN_(-1);
  
  D_RETURN_(0);
  #endif
}


int      
efsd_io_read_event(int sockfd, EfsdEvent *ee)
{
  int result = -1;
  int count = 0;
   
  D_ENTER;

  if (!ee)
    D_RETURN_(-1);

  memset(ee, 0, sizeof(EfsdEvent));

  if ((count = read_int(sockfd, (int*)&(ee->type))) >= 0)
    {
      switch (ee->type)
	{
	case EFSD_EVENT_FILECHANGE:
	  result = read_filechange_event(sockfd, ee);    
	  break;
	case EFSD_EVENT_METADATA_CHANGE:
	  result = read_metadata_change_event(sockfd, ee);    
	  break;
	case EFSD_EVENT_REPLY:
	  result = read_reply_event(sockfd, ee);    
	  break;
	default:
	  D(("Unknown event.\n"));
	}
    }

  D_RETURN_(count + result);
}


int      
efsd_io_write_option(int sockfd, EfsdOption *eo)
{
  EfsdIOV         iov;
  struct msghdr   msg;
  int             n;

  D_ENTER;

  if (!eo)
    D_RETURN_(-1);

  iov.v = iov.d = 0;

  memset(&msg, 0, sizeof(struct msghdr));

  fill_option(&iov, eo);

  msg.msg_iov = iov.vec;
  msg.msg_iovlen = iov.v;

  if ((n = write_data(sockfd, &msg)) < 0)
    D_RETURN_(-1);

  D_RETURN_(0);
}


int      
efsd_io_read_option(int sockfd, EfsdOption *eo)
{
  int result = -1;
  int count = 0;
   
  D_ENTER;

  if (!eo)
    D_RETURN_(-1);

  if ((count = read_int(sockfd, (int*)&(eo->type))) >= 0)
    {
      switch (eo->type)
	{
	case EFSD_OP_FORCE:
	  D(("Read force option\n"));
	  break;
	case EFSD_OP_RECURSIVE:
	  D(("Read rec option\n"));
	  break;
	case EFSD_OP_ALL:
	  D(("Read all option\n"));
	  break;
	case EFSD_OP_GET_STAT:
	  D(("Read stat option\n"));
	  break;
	case EFSD_OP_GET_LSTAT:
	  D(("Read stat option\n"));
	  break;
	case EFSD_OP_GET_FILETYPE:
	  D(("Read filetype option\n"));
	  break;
	case EFSD_OP_SORT:
	  D(("Read sort option\n"));
	  break;
	case EFSD_OP_GET_META:
	  D(("Read meta option\n"));
	  result = read_getmeta_op(sockfd, eo);
	  break;
	default:
	  D(("Unknown option.\n"));
	}
    }

  D_RETURN_(count + result);
}

