        /*
         * 
         * Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.
         * Adopted for EVFS by Alex Taylor
         * 
         * Permission is hereby granted, free of charge, to any person obtaining a copy
         * of this software and associated documentation files (the "Software"), to
         * deal in the Software without restriction, including without limitation the
         * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
         * sell copies of the Software, and to permit persons to whom the Software is
         * furnished to do so, subject to the following conditions:
         * 
         * The above copyright notice and this permission notice shall be included in
         * all copies of the Software and its documentation and acknowledgment shall be
         * given in the documentation and software packages that this Software was
         * used.
         * 
         * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
         * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
         * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
         * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
         * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
         * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
         * 
         */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <evfs.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <Ecore_File.h>

        /*Main file wrappers */
int evfs_file_remove(char *src);
int evfs_file_rename(evfs_client * client, evfs_command * command);

int evfs_client_disconnect(evfs_client * client);
int evfs_monitor_start(evfs_client * client, evfs_command * command);
int evfs_monitor_stop(evfs_client * client, evfs_command * command);
int evfs_file_open(evfs_client * client, EvfsFilereference * file);
int evfs_file_close(EvfsFilereference * file);
int evfs_file_stat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_lstat(evfs_command * command, struct stat *file_stat, int);
int evfs_file_seek(EvfsFilereference * file, long offset, int whence);
int evfs_file_read(evfs_client * client, EvfsFilereference * file,
                   char *bytes, long size);
int evfs_file_write(EvfsFilereference * file, char *bytes, long size);
int evfs_file_create(EvfsFilereference * file);
int evfs_file_mkdir(EvfsFilereference * file);
void evfs_dir_list(evfs_client * client, evfs_command* command,
                   Ecore_List ** directory_list);

        /*Internal functions */
        /* Real data-copying routine, handling holes etc. Returns outcome
         * upon return: 0 when successful, -1 when not.
         */
        /*static int data_copy(char *src_path, struct stat *src_st, char *dst_path);
         * static int file_copy(char *src_path, struct stat *src_st, char *dst_path);
         * static int dir_copy(char *src_path, struct stat *src_st, char *dst_path);
         * static int file_move(char *src_path, struct stat *src_st, char *dst_path);
         * static int dir_move(char *src_path, char *dst_path);
         * static int file_remove(char *path, struct stat *st); */

        /*Misc functions ----------------------------------------- */
int evfs_misc_rename(char *file1, char *file2);
int evfs_misc_remove(char *filename);
void evfs_posix_monitor_remove(evfs_client * client, char *path);

Ecore_Hash *posix_monitor_hash;

evfs_plugin_functions *
evfs_plugin_init()
{
   printf("Initialising the file plugin..\n");
   evfs_plugin_functions *functions = calloc(1, sizeof(evfs_plugin_functions));

   posix_monitor_hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);

   functions->evfs_client_disconnect = &evfs_client_disconnect;

   functions->evfs_file_remove = &evfs_file_remove;
   functions->evfs_monitor_start = &evfs_monitor_start;
   functions->evfs_monitor_stop = &evfs_monitor_stop;
   functions->evfs_file_stat = &evfs_file_stat;
   functions->evfs_file_lstat = &evfs_file_lstat;

   functions->evfs_file_open = &evfs_file_open;
   functions->evfs_file_close = &evfs_file_close;
   functions->evfs_dir_list = &evfs_dir_list;

   functions->evfs_file_seek = &evfs_file_seek;
   functions->evfs_file_read = &evfs_file_read;
   functions->evfs_file_write = &evfs_file_write;
   functions->evfs_file_create = &evfs_file_create;
   functions->evfs_file_mkdir = &evfs_file_mkdir;
   functions->evfs_file_rename = &evfs_file_rename;
   return functions;

}

char *
evfs_plugin_uri_get()
{
   return "file";
}

int
evfs_client_disconnect(evfs_client * client)
{
   Ecore_List *mon_list;
   Ecore_List *indiv_list;
   evfs_file_monitor *mon;
   char *key;
   Ecore_List *watched_keys = ecore_list_new();

   /*printf("Received disconnect for client at evfs_fs_posix.c for client %lu\n",
          client->id);
   printf("Scanning monitors for client and removing...\n");*/

   mon_list = ecore_hash_keys(posix_monitor_hash);
   if (mon_list)
     {
        while ((key = ecore_list_first_remove(mon_list)))
          {
             /*printf("Looking for clients for '%s'\n", key);*/

             indiv_list = ecore_hash_get(posix_monitor_hash, key);
             ecore_list_first_goto(indiv_list);

             while ((mon = ecore_list_next(indiv_list)))
               {
                  if (mon->client == client)
                    {
                       ecore_list_append(watched_keys, key);
                    }
               }
          }
        ecore_list_destroy(mon_list);
     }
   else
     {
        /*printf("No directories/files monitored by any client\n");*/
     }

   while ((key = ecore_list_first_remove(watched_keys)))
     {
        evfs_posix_monitor_remove(client, key);
     }
   ecore_list_destroy(watched_keys);
   return 1;
}

void
evfs_file_monitor_fam_handler(void *data, Ecore_File_Monitor * em,
                              Ecore_File_Event event, const char *path)
{
   Ecore_List *mon_list;
   int type;
   int filetype = EVFS_FILE_NORMAL;

   /*printf("Got an event for %s..", path); */

   switch (event)
     {
     case ECORE_FILE_EVENT_MODIFIED:
        /*printf("A modified event..\n"); */
        type = EVFS_FILE_EV_CHANGE;
        break;
     case ECORE_FILE_EVENT_CREATED_FILE:
        type = EVFS_FILE_EV_CREATE;
        /*printf("File created - '%s'\n", path); */
        break;
     case ECORE_FILE_EVENT_CREATED_DIRECTORY:
        type = EVFS_FILE_EV_CREATE;
	filetype = EVFS_FILE_DIRECTORY;
        break;
     case ECORE_FILE_EVENT_DELETED_FILE:
        type = EVFS_FILE_EV_REMOVE;
        break;

     case ECORE_FILE_EVENT_DELETED_DIRECTORY:
        type = EVFS_FILE_EV_REMOVE_DIRECTORY;
        break;

     }

   /*Looking for ppl no notify */
   mon_list = ecore_hash_get(posix_monitor_hash, (char *)data);

   if (mon_list)
     {
        evfs_file_monitor *mon;

        ecore_list_first_goto(mon_list);
        while ((mon = ecore_list_next(mon_list)))
          {
             evfs_file_monitor_event_create(mon->client, type, path, filetype, "file");  /*Find a better way to do the plugin */
             /*We should really use an EvfsFilereference here */
          }
     }

}

/*This is potentially inefficient with a lot of clients monitoring the same dir/file*/
int
client_already_monitoring(evfs_client * client, Ecore_List * mon_list)
{
   evfs_file_monitor *mon;

   ecore_list_first_goto(mon_list);

   while ((mon = ecore_list_next(mon_list)))
     {
        if (mon->client == client)
          {
             return 1;
          }
     }

   return 0;

}

int
posix_monitor_add(evfs_client * client, evfs_command * command)
{
   Ecore_List *mon_list =
      ecore_hash_get(posix_monitor_hash, evfs_command_first_file_get(command)->path);
   evfs_file_monitor *mon;
   evfs_file_monitor *old;

   mon = calloc(1, sizeof(evfs_file_monitor));
   mon->client = client;
   mon->monitor_path = strdup(evfs_command_first_file_get(command)->path);

   /*Check if we are already monitoring, if not, make a new list of monitors.. */
   if (!mon_list)
     {
        /*printf("No previous instance, making a new list, monitoring..\n"); */

        mon_list = ecore_list_new();
        ecore_hash_set(posix_monitor_hash, mon->monitor_path, mon_list);

        printf("Adding monitor on path '%s'\n", mon->monitor_path);
        if (!
            (mon->em =
             ecore_file_monitor_add(mon->monitor_path,
                                    &evfs_file_monitor_fam_handler,
                                    mon->monitor_path)))
          {
             fprintf(stderr, "EVFS: Error monitoring '%s'\n",
                     mon->monitor_path);
          }

        ecore_list_append(mon_list, mon);
     }
   else
     {
        if (!client_already_monitoring(client, mon_list))
          {
             /*We assume there is something already in the list.  This is probably bad a bad assumption */
             ecore_list_first_goto(mon_list);
             old = ecore_list_current(mon_list);

             /*Make sure we have the ecore ref, so the last monitor can nuke it */
             mon->em = old->em;
             ecore_list_append(mon_list, mon);
          }
        else
          {
             printf("Oi, dufus, you're already monitoring this object\n");
          }

     }

   return 0;

}

/*The function root for evfs's plugin requests*/
int
evfs_monitor_start(evfs_client * client, evfs_command * command)
{

   /*printf("Received monitor request at plugin for %s..\n",command->file_command->files[0]->path ); */
   return posix_monitor_add(client, command);
}

int
evfs_monitor_stop(evfs_client * client, evfs_command * command)
{
   evfs_posix_monitor_remove(client, evfs_command_first_file_get(command)->path);
   return 0;
}

void
evfs_posix_monitor_remove(evfs_client * client, char *path)
{
   Ecore_List *mon_list = ecore_hash_get(posix_monitor_hash, path);
   Ecore_File_Monitor *em = NULL;

   if (!mon_list)
     {
        /*There is no one monitoring this - so this client can't be... */
        return;
     }
   else
     {
        evfs_file_monitor *mon = NULL;
        evfs_file_monitor *check_last = NULL;

        ecore_list_first_goto(mon_list);
        while ((mon = ecore_list_current(mon_list)))
          {
             if (mon->client == client)
               {
                  em = mon->em;
                  ecore_list_remove(mon_list);
                  goto final;
               }

             ecore_list_next(mon_list);
          }
        goto out;

      final:
        ecore_list_first_goto(mon_list);
        check_last = ecore_list_current(mon_list);
        if (!check_last)
          {
             printf("Removing last watcher on '%s'..\n", mon->monitor_path);
             if (em)
               {
                  ecore_file_monitor_del(em);
               }
             else
               {
                  fprintf(stderr,
                          "EVFS: Error - attempt to remove monitor on NULL Ecore_File_Monitor object\n");
               }
             ecore_list_destroy(mon_list);
             ecore_hash_remove(posix_monitor_hash, path);
          }
        evfs_cleanup_file_monitor(mon);

      out:
        return;
     }

}

int
evfs_file_remove(char *src)
{
   struct stat stat_src;

   if (!lstat(src, &stat_src))
     {
        return remove(src);
     }
   else
     {
        printf("Could not stat..\n");
        return 1;
     }
}

int
evfs_file_rename(evfs_client * client, evfs_command * command)
{
   EvfsFilereference *from = evfs_command_first_file_get(command);
   EvfsFilereference *to = evfs_command_second_file_get(command);

   printf("Renaming %s to %s\n", from->path, to->path);
   return evfs_misc_rename(from->path, to->path);
}

int
evfs_file_stat(evfs_command * command, struct stat *file_stat, int file_number)
{
   int res;
	
   #ifdef  __USE_LARGEFILE64
   res = stat64(evfs_command_nth_file_get(command,file_number)->path, file_stat);
   #else
   res = stat(evfs_command_nth_file_get(command,file_number)->path, file_stat);
   #endif

   if (!res)
      return EVFS_SUCCESS;
   else
      return EVFS_ERROR;
}

int
evfs_file_lstat(evfs_command * command, struct stat *file_stat, int file_number)
{
   //printf("Getting file stat...\n");
   int res = lstat(evfs_command_nth_file_get(command,file_number)->path, file_stat);

   //printf("File size: %d\n", file_stat->st_size);
   //
   if (!res)
      return EVFS_SUCCESS;
   else
      return EVFS_ERROR;

}

int
evfs_file_open(evfs_client * client, EvfsFilereference * file)
{
   int fd;
   
   #ifdef  __USE_LARGEFILE64
   fd = open(file->path, O_RDONLY | O_LARGEFILE);
   #else
   fd = open(file->path, O_RDONLY);
   #endif

   //printf("Assigned fd %d\n", fd);
   file->fd = fd;

   return fd;
}

int
evfs_file_close(EvfsFilereference * file)
{
   int res;

   //printf("Close file '%s'\n", file->path);

   res = close(file->fd);
   file->fd = 0;

   return 0;
}

int
evfs_file_seek(EvfsFilereference * file, long offset, int whence)
{
   /*printf ("Seek in file '%s' forward by '%d'\n", file->path, offset); */

   lseek(file->fd, offset, SEEK_SET);

   return 0;
}

int
evfs_file_read(evfs_client * client, EvfsFilereference * file, char *bytes,
               long size)
{
   int bytes_read;

   /*printf("Reading %d bytes from %s\n", size, file->path); */

   bytes_read = read(file->fd, bytes, size);
   return bytes_read;

   if (bytes_read)
     {
        //bytes[bytes_read] = '\0';
        //printf ("Read '%s'\n", bytes);
     }
   else
     {
        return -1;
     }

}

int
evfs_file_write(EvfsFilereference * file, char *bytes, long size)
{
   ssize_t i;

   /*printf("Writing %d bytes for %s\n", size, file->path); */

   i = write(file->fd, bytes, size);
   /*printf("Wrote %d bytes\n", i); */

   return i;
}

int
evfs_file_create(EvfsFilereference * file)
{
   printf ("Creating file '%s'\n", file->path);

   file->fd = open(file->path, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);

   return 0;
}

void
evfs_dir_list(evfs_client * client, evfs_command* command,
              Ecore_List ** directory_list)
{
   struct dirent *de;
   struct stat st;
   DIR *dir;
   Ecore_List *files = ecore_list_new();
   char full_name[PATH_MAX];

   EvfsFilereference* file = evfs_command_first_file_get(command);

   dir = opendir(file->path);
   while ((de = readdir(dir)))
     {
        if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")
            && strcmp(de->d_name, "."))
          {
             EvfsFilereference *ref = NEW(EvfsFilereference);

             snprintf(full_name, 1024, "%s/%s",
                      file->path, de->d_name);
             stat(full_name, &st);
             if (S_ISDIR(st.st_mode))
               {
                  ref->file_type = EVFS_FILE_DIRECTORY;
               }
             else
               {
                  ref->file_type = EVFS_FILE_NORMAL;
               }

             ref->path = strdup(full_name);
             ref->plugin_uri = strdup("file");
             ecore_list_append(files, ref);
          }
     }
   closedir(dir);

   *directory_list = evfs_file_list_sort(files);

}

int
evfs_file_mkdir(EvfsFilereference * file)
{
   return mkdir(file->path, S_IRWXU);
}

/*Misc functions -----------------------------------------*/
int
evfs_misc_remove(char *filename)
{
   struct stat st;

   D_ENTER;

   if (!filename || filename[0] == '\0')
     {
        errno = EINVAL;
        D_RETURN_(FALSE);
     }

   if (lstat(filename, &st) < 0)
      D_RETURN_(FALSE);

   /*if (S_ISDIR(st.st_mode))
    * efsd_meta_dir_cleanup(filename); */

   //if (remove(filename) == 0)
   //{
   /* File is removed -- now remove
    * any cached stat data ...
    */
   /*efsd_stat_remove(filename, TRUE); */

   /* .. and any metadata. We don't
    * care about the result (maybe
    * no metadata existed etc).
    */
   /*efsd_meta_remove_data(filename); */

   D_RETURN_(TRUE);
   //}

   D("Removing %s failed.\n", filename);

   D_RETURN_(FALSE);
}

int
evfs_misc_rename(char *file1, char *file2)
{
   D_ENTER;

   if (!file1 || file1[0] == '\0' || !file2 || file2[0] == '\0')
     {
        errno = EINVAL;
        D_RETURN_(FALSE);
     }

   if (rename(file1, file2) == 0)
     {
        /* Update stat cache to new name ... */
        //efsd_stat_change_filename(file1, file2);

        /* ... and metadata. */
        //efsd_meta_move_data(file1, file2);

        D_RETURN_(TRUE);
     }

   D_RETURN_(FALSE);
}
