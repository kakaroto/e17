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
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#ifdef __EMX__
#include <strings.h>  /* eeek... OS/2 has bzero(...) there */
#define getcwd _getcwd2
#endif

#include <efsd.h>
#include <efsd_io.h>
#include <efsd_common.h>
#include <libefsd.h>

struct efsd_connection
{
  int        fd;
};


static void      send_command(EfsdConnection *ec, EfsdCommand *com);
static EfsdCmdId get_next_id(void);


static char*
get_full_path(char *file)
{
  char *result;

  if (!file || !file[0])
    return NULL;
  
#ifndef __EMX__
  if (file[0] == '/')
#else  
  if (_fnisabs(file))
#endif  
    return strdup(file);

  result = getcwd(NULL, 0);
  result = realloc(result, strlen(result) + strlen(file) + 2);
  strcat(result, "/");
  strcat(result, file);

  return result;
}


static void      
send_command(EfsdConnection *ec, EfsdCommand *com)
{
  if (!ec || !com)
    return;

  if (efsd_write_command(ec->fd, com) < 0)
    fprintf(stderr, "libefsd: write() error.\n");
}


static EfsdCmdId 
get_next_id(void)
{
  static EfsdCmdId id_counter = 0;

  return ++id_counter;
}


/** API starts here
 */
extern void efsd_cleanup_event(EfsdEvent *ev);


EfsdConnection *
efsd_open(void)
{
  struct sockaddr_un    cli_sun;
  EfsdConnection       *ec;

  ec = (EfsdConnection*)malloc(sizeof(EfsdConnection));
  if (!ec)
    return NULL;

  if ( (ec->fd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      fprintf(stderr, "libefsd: socket() error.\n"); return NULL;
    }

  bzero(&cli_sun, sizeof(cli_sun));
  cli_sun.sun_family = AF_UNIX;
  strncpy(cli_sun.sun_path, efsd_get_socket_file(), sizeof(cli_sun.sun_path));

  if (connect(ec->fd, (struct sockaddr*)&cli_sun, sizeof(cli_sun)) < 0)
    {
      fprintf(stderr, "libefsd: connect() error.\n"); return NULL;      
    }

  if (fcntl(ec->fd, F_SETFL, O_NONBLOCK) < 0)
    {
      fprintf(stderr, "Can not fcntl client's socket -- exiting.\n");
      exit(-1);
    }

  return (ec);
}


int            
efsd_get_connection_fd(EfsdConnection *ec)
{
  if (!ec)
    return (-1);

  return ec->fd;
}


void           
efsd_close(EfsdConnection *ec)
{
  EfsdCommand cmd;

  if (!ec)
    return;

  cmd.type = CLOSE;
  send_command(ec, &cmd);

  close(ec->fd);
  free(ec);
}


int           
efsd_next_event(EfsdConnection *ec, EfsdEvent *ev)
{
  if (!ec || !ev || ec->fd < 0)
    return (-1);

  return (efsd_read_event(ec->fd, ev));
}


int           
efsd_wait_event(EfsdConnection *ec, EfsdEvent *ev)
{
  fd_set    fdset;

  if (!ec || !ev || ec->fd < 0)
    return (-1);

  FD_ZERO(&fdset);
  FD_SET(ec->fd, &fdset);
  select(ec->fd+1, &fdset, NULL, NULL, NULL);

  return (efsd_read_event(ec->fd, ev));
}


EfsdCmdId      
efsd_remove(EfsdConnection *ec, char *filename)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0')
    return -1;

  cmd.type = REMOVE;
  cmd.efsd_file_cmd.id = get_next_id();
  f = get_full_path(filename);
  cmd.efsd_file_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_file_cmd.id;
}


EfsdCmdId      
efsd_move(EfsdConnection *ec, char *from_file, char *to_file)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !from_file || from_file[0] == '\0' || !to_file || to_file[0] == '\0')
    return -1;

  cmd.type = MOVE;
  cmd.efsd_2file_cmd.id = get_next_id();
  f = get_full_path(from_file);
  cmd.efsd_2file_cmd.file1 = strdup(f);
  free(f);
  f = get_full_path(to_file);
  cmd.efsd_2file_cmd.file2 = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_2file_cmd.id;
}


EfsdCmdId      
efsd_copy(EfsdConnection *ec, char *from_file, char *to_file)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !from_file || from_file[0] == '\0' || !to_file || to_file[0] == '\0')
    return -1;

  cmd.type = COPY;
  cmd.efsd_2file_cmd.id = get_next_id();
  f = get_full_path(from_file);
  cmd.efsd_2file_cmd.file1 = strdup(f);
  free(f);
  f = get_full_path(to_file);
  cmd.efsd_2file_cmd.file2 = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_2file_cmd.id;
}


EfsdCmdId      
efsd_symlink(EfsdConnection *ec, char *from_file, char *to_file)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !from_file || from_file[0] == '\0' || !to_file || to_file[0] == '\0')
    return -1;

  cmd.type = SYMLINK;
  cmd.efsd_2file_cmd.id = get_next_id();
  f = get_full_path(from_file);
  cmd.efsd_2file_cmd.file1 = strdup(f);
  free(f);
  f = get_full_path(to_file);
  cmd.efsd_2file_cmd.file2 = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_2file_cmd.id;
}


EfsdCmdId      
efsd_listdir(EfsdConnection *ec, char *dirname)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !dirname || dirname[0] == '\0')
    return -1;

  cmd.type = LISTDIR;
  cmd.efsd_file_cmd.id = get_next_id();
  f = get_full_path(dirname);
  cmd.efsd_file_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_file_cmd.id;
}


EfsdCmdId      
efsd_makedir(EfsdConnection *ec, char *dirname)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !dirname || dirname[0] == '\0')
    return -1;

  cmd.type = MAKEDIR;
  cmd.efsd_file_cmd.id = get_next_id();
  f = get_full_path(dirname);
  cmd.efsd_file_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_file_cmd.id;
}


EfsdCmdId      
efsd_chmod(EfsdConnection *ec, char *filename,  mode_t mode)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0')
    return -1;

  cmd.type = CHMOD;
  cmd.efsd_chmod_cmd.id = get_next_id();
  cmd.efsd_chmod_cmd.mode = mode;
  f = get_full_path(filename);
  cmd.efsd_chmod_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_chmod_cmd.id;
}


EfsdCmdId      
efsd_set_metadata(EfsdConnection *ec, char *key, char *filename,
		     EfsdDatatype datatype, int data_len, void *data)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0' ||
      !key || key[0] == '\0' || !data)
    return -1;

  cmd.type = SETMETA;
  cmd.efsd_set_metadata_cmd.id = get_next_id();
  cmd.efsd_set_metadata_cmd.datatype = datatype;
  cmd.efsd_set_metadata_cmd.data_len = data_len;
  cmd.efsd_set_metadata_cmd.data = data;
  cmd.efsd_set_metadata_cmd.key = strdup(key);
  f = get_full_path(filename);
  cmd.efsd_set_metadata_cmd.file = strdup(f);
  free(f);
  
  send_command(ec, &cmd);
  return cmd.efsd_set_metadata_cmd.id;
}


EfsdCmdId      
efsd_get_metadata(EfsdConnection *ec, char *key, char *filename)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0'
      || !key || key[0] == '\0')
    return -1;

  cmd.type = GETMETA;
  cmd.efsd_get_metadata_cmd.id = get_next_id();
  cmd.efsd_get_metadata_cmd.key = strdup(key);
  f = get_full_path(filename);
  cmd.efsd_get_metadata_cmd.file = strdup(f);
  free(f);
  
  send_command(ec, &cmd);
  return cmd.efsd_get_metadata_cmd.id;
}


EfsdCmdId      
efsd_start_monitor(EfsdConnection *ec, char *filename)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0')
    return -1;

  cmd.type = STARTMON;
  cmd.efsd_file_cmd.id = get_next_id();
  f = get_full_path(filename);
  cmd.efsd_file_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_file_cmd.id;
}


EfsdCmdId      
efsd_stop_monitor(EfsdConnection *ec, char *filename)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0')
    return -1;

  cmd.type = STOPMON;
  cmd.efsd_file_cmd.id = get_next_id();
  f = get_full_path(filename);
  cmd.efsd_file_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_file_cmd.id;
}


EfsdCmdId      
efsd_stat(EfsdConnection *ec, char *filename)
{
  char        *f;
  EfsdCommand  cmd;

  if (!ec || !filename || filename[0] == '\0')
    return -1;

  cmd.type = STAT;
  cmd.efsd_file_cmd.id = get_next_id();
  f = get_full_path(filename);
  cmd.efsd_file_cmd.file = strdup(f);
  free(f);

  send_command(ec, &cmd);
  return cmd.efsd_file_cmd.id;
}
