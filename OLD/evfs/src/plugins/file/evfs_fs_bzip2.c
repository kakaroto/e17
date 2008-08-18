/*

Copyright (C) 2005 <alex@logisticchaos.com> - Alex Taylor

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
#include <bzlib.h>

/*Main file wrappers*/
int evfs_file_remove(char *src);
int evfs_file_rename(char *src, char *dst);

int evfs_client_disconnect(evfs_client * client);
int evfs_monitor_start(evfs_client * client, evfs_command * command);
int evfs_monitor_stop(evfs_client * client, evfs_command * command);
int evfs_file_open(evfs_client *, EvfsFilereference * file);
int evfs_file_close(EvfsFilereference * file);
int evfs_file_stat(evfs_command * command, struct stat *file_stat);
int evfs_file_seek(EvfsFilereference * file, long offset, int whence);
int evfs_file_read(evfs_client * client, EvfsFilereference * file,
                   char *bytes, long size);
int evfs_file_write(EvfsFilereference * file, char *bytes, long size);
int evfs_file_create(EvfsFilereference * file);
void evfs_dir_list(evfs_client * client, EvfsFilereference* file);

#define BZIP2_BUFFER 5000
#define EVFS_BZ2_GOT_DATA -1
#define EVFS_BZ2_ERROR -2

Ecore_Hash *bzip_hash;
typedef struct bzip2_file
{
   EvfsFilereference *ref;
   bz_stream stream;
   char *buffer;

} bzip2_file;

evfs_plugin_functions *
evfs_plugin_init()
{
   printf("Initialising the bzip2 plugin..\n");
   evfs_plugin_functions *functions = calloc(1, sizeof(evfs_plugin_functions));

   functions->evfs_client_disconnect = &evfs_client_disconnect;
   functions->evfs_file_open = &evfs_file_open;
   functions->evfs_file_read = &evfs_file_read;
   functions->evfs_file_close = &evfs_file_close;

   /*functions->evfs_dir_list = &evfs_dir_list;
    * 
    * functions->evfs_file_remove= &evfs_file_remove;
    * functions->evfs_monitor_start = &evfs_monitor_start;
    * functions->evfs_monitor_stop = &evfs_monitor_stop;
    * functions->evfs_file_stat = &evfs_file_stat;
    * 
    * 
    * 
    * 
    * functions->evfs_file_seek = &evfs_file_seek;
    * 
    * functions->evfs_file_write = &evfs_file_write;
    * functions->evfs_file_create = &evfs_file_create; */

   /*FIXME - This is bad - by using a direct compare, we preclude clients using
    * an 'identical' EvfsFilereference with a different pointer*/
   /*TODO - Fix this by creating EvfsFilereference_compare for Ecore_Hash */
   bzip_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

   return functions;

}

char *
evfs_plugin_uri_get()
{
   return "bzip2";
}

int
evfs_client_disconnect(evfs_client * client)
{
   /*printf("Received disconnect for client at evfs_fs_bzip2.c for client %lu\n",
          client->id);*/
   return 1;
}

int
evfs_file_open(evfs_client * client, EvfsFilereference * file)
{
   EvfsFilereference *f_par = file->parent;
   bzip2_file *bfile;

   /*Handle decomp init */
   printf("Opening bzip file '%s'\n", file->path);

   bfile = NEW(bzip2_file);
   bfile->buffer = malloc(sizeof(char) * BZIP2_BUFFER);
   bfile->stream.next_in = (char *)bfile->buffer;
   bfile->stream.avail_in = 0;

   if (BZ2_bzDecompressInit(&bfile->stream, 0, 0) != BZ_OK)
     {
        printf("Error in bzip2 init\n");
     }

   ecore_hash_set(bzip_hash, file, bfile);

   /*Open the file in the parent */
   return evfs_uri_open(client, f_par);

}

/*FUTURE DOC NOTE - Takes the child file, not the parent */
int
evfs_bzip2_populate_buffer(evfs_client * client, EvfsFilereference * ref)
{
   int res;
   bzip2_file *bfile = ecore_hash_get(bzip_hash, ref);

   if (bfile->stream.avail_in > 0)
     {
        //printf("No need to read, got data already..\n");
        return EVFS_BZ2_GOT_DATA;
     }

   res = evfs_uri_read(client, ref->parent, bfile->buffer, BZIP2_BUFFER);
   /*printf("Read %d bytes at bzip2_read from fd %d using filename %s\n", res, ref->fd, ref->path);*/

   if (res > 0)
     {
        //printf("  Allocated %d bytes to bzip2 buffer\n",res);
        bfile->stream.next_in = (char *)bfile->buffer;
        bfile->stream.avail_in = res;
     }
   else if (res == 0) {
	   //printf("res 0\n");
	   return 0;
   } else {
        //printf("Res returnde an error: %d\n", res);
	return EVFS_BZ2_ERROR;
        //exit(0);
     }

   return res;
}

int
evfs_file_read(evfs_client * client, EvfsFilereference * file, char *bytes,
               long size)
{
   bzip2_file *bfile = ecore_hash_get(bzip_hash, file);
   int bz_result;

   //printf("Client requested %d bytes from bzip2\n", size);

   bfile->stream.next_out = bytes;
   bfile->stream.avail_out = size;

   while (bfile->stream.avail_out != 0)
     {
        int res = evfs_bzip2_populate_buffer(client, file);

        //printf("Avail_out starts at %d\n", bfile->stream.avail_out);
        bz_result = BZ2_bzDecompress(&bfile->stream);
        //printf("Avail_out goes to %d\n", bfile->stream.avail_out);

        //printf ("             BZ Result is: %d -> %d\n", bz_result,res);

	if (res == 0) 
		return 0; /*No data left..*/

        if (bz_result == -1)
           break;

     }

   if (!bfile->stream.avail_out)
     {
        return size;            /*Normal */
     }
   else if (bfile->stream.avail_out > 0 && bfile->stream.avail_out < size)
     {
        return (size - bfile->stream.avail_out);        /*Nearly eof */
     }
   else if (bfile->stream.avail_out == size)
     {
        return 0;
      /*EOF*/
     }
   return 0;
}

int
evfs_file_close(EvfsFilereference * file)
{
   bzip2_file *bfile = ecore_hash_get(bzip_hash, file);

   if (BZ2_bzDecompressEnd(&bfile->stream) != BZ_OK)
     {
        printf("Error in bzip2 end\n");
     }

   free(bfile->buffer);
   ecore_hash_remove(bzip_hash, file);
   free(bfile);

   return 1;
}
