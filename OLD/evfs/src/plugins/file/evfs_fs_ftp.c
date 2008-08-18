/*

Copyright (C) 2005 John Kha

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

#include <evfs.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <Ecore_File.h>
#include <curl/curl.h>

struct ftp_conn *connection_handle_get(EvfsFilereference * ref,
                                       evfs_command * com);

void evfs_ftp_dir_list(evfs_client * client, evfs_command * command,
                       Ecore_List ** directory_list);
int evfs_ftp_file_stat(evfs_command * command, struct stat *file_stat);
int evfs_file_open(evfs_client * client, EvfsFilereference * file);
int evfs_file_close(EvfsFilereference * file);
int evfs_file_seek(EvfsFilereference * file, long offset, int whence);
int evfs_file_read(evfs_client * client, EvfsFilereference * file,
                   char *bytes, long size);
int evfs_file_write(EvfsFilereference * file, char *bytes, long size);
int evfs_file_create(EvfsFilereference * file);
int evfs_client_disconnect(evfs_client * client);

typedef enum evfs_ftp_data
{
   EVFS_FTP_CLIENT,
   EVFS_FTP_COMMAND,
   EVFS_FTP_HANDLE,
   EVFS_FTP_FILES
} evfs_ftp_data;

typedef struct ftp_conn
{
   CURL *handle;
   int file;
   int type;
   int filled;
} ftp_conn;

/****************Globals****************/
Ecore_Hash *connections;

/******************Begin Internal Functions*******************************/

ftp_conn *
connection_handle_get(EvfsFilereference * ref, evfs_command * com)
{
   ftp_conn *conn = NULL;

   /*Check for an existing connection, return it if  avaliable
    * create/initialize one if not */
   if ((conn = ecore_hash_get(connections, ref->path)))
     {
        ecore_hash_remove(connections, ref->path);
     }
   else
     {
        conn = NEW(ftp_conn);
        char *url;
        int len = 1;

        len += strlen("ftp://");
        len += strlen(ref->username) + 1 + strlen(ref->password);
        len += 1 + strlen(ref->path);
        if ((com->type == EVFS_CMD_LIST_DIR)
            && (strlen(strrchr(ref->path, '/')) > 1))
          {
             len += 1;
          }
        len *= sizeof(char);
        url = malloc(len);
        snprintf(url, len, "ftp://%s:%s@%s", ref->username, ref->password,
                 ref->path);
        if ((com->type == EVFS_CMD_LIST_DIR)
            && (strlen(strrchr(ref->path, '/')) > 1))
          {
             strcat(url, "/");
          }
        conn->handle = curl_easy_init();
        printf("Setting CURLOPT_URL to %s\n", url);
        curl_easy_setopt(conn->handle, CURLOPT_URL, url);
     }

   return conn;
}

void
connection_handle_save(ftp_conn * conn, EvfsFilereference * ref)
{
   ecore_hash_set(connections, ref->path, conn);

}

EvfsFilereference *
parse_ls_line(ftp_conn * conn, char *line, int is_stat)
{
   char *fieldline = strdup(line);
   EvfsFilereference *ref = NEW(EvfsFilereference);
   char *curfield;
   Ecore_List *fields = ecore_list_new();

   while (fieldline)
     {
        curfield = strdup(strsep(&fieldline, " "));
        if (strlen(curfield))
          {
             ecore_list_append(fields, curfield);
          }
     }
   if (!(conn->filled))
     {
        int columns = ecore_list_count(fields);

        if (columns == 4)
          {
             conn->file = 3;
             conn->type = 2;
          }
        if (columns == 6)
          {
             conn->file = 0;
             conn->type = 0;
          }
        if (columns == 7)
          {
             conn->file = 7;
             conn->type = 0;
          }
        if (columns == 8)
          {
             conn->file = 8;
             conn->type = 0;
          }
        if (columns == 9)
          {
             conn->file = 8;
             conn->type = 0;
          }
        if (columns == 11)
          {
             conn->file = 8;
             conn->type = 0;
          }
        conn->filled = 1;
     }
   ref->path = ecore_list_index_goto(fields, conn->file);
   if (!(strncmp(ecore_list_index_goto(fields, conn->type), "d", 1))
       || strstr(ecore_list_index_goto(fields, conn->type), "DIR"))
     {
        ref->file_type = EVFS_FILE_DIRECTORY;
     }
   else
     {
        ref->file_type = EVFS_FILE_NORMAL;
     }
   free(fieldline);
   return ref;
}

/******************CURL Callbacks*****************************************/

size_t
evfs_ftp_dir_list_cb(void *buffer, size_t size, size_t nmemb, void *cbdata)
{
   char *dirs = strdup(buffer);
   Ecore_List *files =
      ecore_hash_get((Ecore_Hash *) cbdata, (int *)EVFS_FTP_FILES);
   evfs_client *client =
      ecore_hash_get((Ecore_Hash *) cbdata, (int *)EVFS_FTP_CLIENT);
   evfs_command *command =
      ecore_hash_get((Ecore_Hash *) cbdata, (int *)EVFS_FTP_COMMAND);
   ftp_conn *conn =
      ecore_hash_get((Ecore_Hash *) cbdata, (int *)EVFS_FTP_HANDLE);
   EvfsFilereference *ref;
   char *curline;

   while (dirs)
     {
        curline = strdup(strsep(&dirs, "\r\n"));
        if (strlen(curline))
          {
             ref = parse_ls_line(conn, curline, 0);
             ecore_list_append(files, ref);
          }
     }

   free(dirs);
   return strlen(buffer);
}

/******************Begin EVFS Plugin Functions****************************/

evfs_plugin_functions *
evfs_plugin_init()
{
   int err;

   printf("Initialising the ftp plugin..\n");

   /*Set up callbacks for the evfs server */
   evfs_plugin_functions *functions = malloc(sizeof(evfs_plugin_functions));

   functions->evfs_dir_list = &evfs_ftp_dir_list;
   functions->evfs_file_stat = &evfs_ftp_file_stat;
   functions->evfs_file_open = &evfs_file_open;
   functions->evfs_file_close = &evfs_file_close;
   functions->evfs_file_seek = &evfs_file_seek;
   functions->evfs_file_read = &evfs_file_read;
   functions->evfs_file_write = &evfs_file_write;
   functions->evfs_file_create = &evfs_file_create;

   functions->evfs_client_disconnect = &evfs_client_disconnect;

   /*initialize the connection cache */
   connections = ecore_hash_new(ecore_str_hash, ecore_str_compare);

   printf("Functions at '%p'\n", &functions);

   return functions;

}

char *
evfs_plugin_uri_get()
{
   return "ftp";
}

void
evfs_ftp_dir_list(evfs_client * client, evfs_command * command,
/*Returns...*/
                  Ecore_List ** directory_list)
{

   printf("FTP: Listing dir.\n");

   Ecore_Hash *data = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
   ftp_conn *conn =
      connection_handle_get(evfs_command_first_file_get(command), command);
   Ecore_List *files = ecore_list_new();
   char *error = malloc(CURL_ERROR_SIZE);

   /*Prepare data related to the operation.  This is used in the parsing callback. */
   ecore_hash_set(data, (int *)EVFS_FTP_CLIENT, client);
   ecore_hash_set(data, (int *)EVFS_FTP_COMMAND, command);
   ecore_hash_set(data, (int *)EVFS_FTP_HANDLE, conn);
   ecore_hash_set(data, (int *)EVFS_FTP_FILES, files);

   curl_easy_setopt(conn->handle, CURLOPT_ERRORBUFFER, error);
   curl_easy_setopt(conn->handle, CURLOPT_WRITEDATA, (FILE *) data);
   curl_easy_setopt(conn->handle, CURLOPT_WRITEFUNCTION, evfs_ftp_dir_list_cb);

   printf("Executing curl_easy_perform()...\n");
   int status = curl_easy_perform(conn->handle);

   if (status)
     {
        printf("Error: curl_easy_perform() returned %i: %s\n", status, error);
        *directory_list = NULL;
     }
   else
     {
        printf("Listed %i files.\n", ecore_list_count(files));

        /*Set out return pointer.. */
        *directory_list = files;
     }
   connection_handle_save(conn, evfs_command_first_file_get(command));
}

int
evfs_ftp_file_stat(evfs_command * command, struct stat *file_stat)
{

   return 0;
}

int
evfs_file_open(evfs_client * client, EvfsFilereference * file)
{

   return 0;
}

int
evfs_file_close(EvfsFilereference * file)
{

   return 0;
}

int
evfs_file_seek(EvfsFilereference * file, long pos, int whence)
{

   return 0;
}

int
evfs_file_read(evfs_client * client, EvfsFilereference * file, char *bytes,
               long size)
{

   return 0;
}

int
evfs_file_write(EvfsFilereference * file, char *bytes, long size)
{

   return 0;
}

int
evfs_file_create(EvfsFilereference * file)
{

   return 0;
}

int
evfs_client_disconnect(evfs_client * client)
{
   /*Temp, move to plugin unloading when avaliable.  Closes the curl library, and
    * resets the connection cache.  This will just destroy the cache when moved. */
   /*printf("Received disconnect for client at evfs_fs_ftp.c for client %ld\n",
          client->id);*/
   curl_global_cleanup();
   ecore_hash_destroy(connections);
   ecore_hash_new(ecore_str_hash, ecore_str_compare);
}
