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
#include <zlib.h>



/*Main file wrappers*/
int evfs_file_remove(char* src);
int evfs_file_rename(char* src, char* dst);

int evfs_client_disconnect(evfs_client* client);
int evfs_monitor_start(evfs_client* client, evfs_command* command);
int evfs_monitor_stop(evfs_client* client, evfs_command* command);
int evfs_file_open(evfs_client*, evfs_filereference* file);
int evfs_file_close(evfs_filereference* file);
int evfs_file_stat(evfs_command* command, struct stat* file_stat);
int evfs_file_seek(evfs_filereference* file, long offset, int whence);
int evfs_file_read(evfs_client* client, evfs_filereference* file, char* bytes, long size);
int evfs_file_write(evfs_filereference* file, char* bytes, long size);
int evfs_file_create(evfs_filereference* file);
void evfs_dir_list(evfs_client* client, evfs_command* file);


#define GZIP_BUFFER 16384

Ecore_Hash* gzip_hash;
typedef struct gzip_file {
	evfs_filereference* ref;
	z_stream stream;
	unsigned char* buffer;
	
	
} gzip_file;




evfs_plugin_functions* evfs_plugin_init() {
        printf("Initialising the gzip plugin..\n");
        evfs_plugin_functions* functions = calloc(1, sizeof(evfs_plugin_functions));

        functions->evfs_client_disconnect = &evfs_client_disconnect;
        functions->evfs_file_open = &evfs_file_open;
	functions->evfs_file_read = &evfs_file_read;
	functions->evfs_file_close = &evfs_file_close;
	
	/*functions->evfs_dir_list = &evfs_dir_list;

        functions->evfs_file_remove= &evfs_file_remove;
        functions->evfs_monitor_start = &evfs_monitor_start;
        functions->evfs_monitor_stop = &evfs_monitor_stop;
        functions->evfs_file_stat = &evfs_file_stat;

        


        functions->evfs_file_seek = &evfs_file_seek;
        
        functions->evfs_file_write = &evfs_file_write;
        functions->evfs_file_create = &evfs_file_create;*/



	/*FIXME - This is bad - by using a direct compare, we preclude clients using
	 * an 'identical' evfs_filereference with a different pointer*/
	/*TODO - Fix this by creating evfs_filereference_compare for Ecore_Hash*/
	gzip_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
	
        return functions;


}

char* evfs_plugin_uri_get() {
        return "gzip";
}

int
evfs_client_disconnect(evfs_client* client) {
        printf ("Received disconnect for client at evfs_fs_gzip.c for client %d\n", client->id);
}


int evfs_file_open(evfs_client* client, evfs_filereference* file) {
	evfs_filereference* f_par = file->parent;
	gzip_file* gfile;
	int open_ret = 0;
	char bytes[10];

	/*Handle decomp init*/
	printf("Opening gzip file '%s'\n", f_par->path);

	gfile = NEW(gzip_file);
	gfile->buffer =  malloc(sizeof(unsigned char)*GZIP_BUFFER);
	gfile->stream.next_in = (unsigned char*)gfile->buffer;
	gfile->stream.avail_in = 0;
	gfile->stream.zalloc = NULL;
	gfile->stream.zfree = NULL;
	gfile->stream.opaque = NULL;

	if (inflateInit2(&gfile->stream, -MAX_WBITS) != Z_OK) {
		printf("Error in gzip init\n");
	}

	open_ret = evfs_uri_open(client, f_par);

	/*Read the gzip header - ignore for nwo. Size is 10 bytes*/
	evfs_uri_read(client, f_par, bytes, 10);

	printf("Read header, continuting..\n");

	ecore_hash_set(gzip_hash, file, gfile);

	/*Open the file in the parent*/
	return open_ret;

}

/*FUTURE DOC NOTE - Takes the child file, not the parent */
int evfs_gzip_populate_buffer(evfs_client* client, evfs_filereference* ref) {
	int res;
	gzip_file* gfile = ecore_hash_get(gzip_hash, ref);

	if (!gfile) {
		printf("Could not find file in hash!\n");
		return -1;
	}
	
	if (gfile->stream.avail_in > 0) {
		//printf("No need to read, got data already..\n");
		return 0;
	}

	
	res = evfs_uri_read(client,ref->parent, gfile->buffer, GZIP_BUFFER);
	//printf("Read %d bytes at gzip_read from fd %d using filename %s\n", res, ref->parent->fd, ref->parent->path);

	if (res > 0) {
		//printf("  Allocated %d bytes to gzip buffer\n",res);
		gfile->stream.next_in = (unsigned char*)gfile->buffer;
		gfile->stream.avail_in = res;
	} else {
		printf("Res returnde an error: %d\n", res);
		//exit(0);
	}

	return res;
}

int evfs_file_read(evfs_client* client, evfs_filereference* file, char* bytes, long size) {
	evfs_filereference* f_par = file->parent;
	gzip_file* gfile = ecore_hash_get(gzip_hash, file);
	int z_result;
	int r_size;
	
	//printf("Client requested %d bytes from gzip\n", size);
	

	gfile->stream.next_out = (unsigned char*)bytes;
	gfile->stream.avail_out = size;

	while (gfile->stream.avail_out != 0) {
		int res = evfs_gzip_populate_buffer(client,file);
		//printf("Avail_out starts at %d\n", bfile->stream.avail_out);
		z_result = inflate(&gfile->stream, Z_NO_FLUSH);


		if (z_result == Z_STREAM_END) break;
		if (z_result == Z_DATA_ERROR) {
			printf("gzip datastream error..\n");
		}
		
	}

	if (!gfile->stream.avail_out) {
		return size; /*Normal*/
	} else if (gfile->stream.avail_out > 0 && gfile->stream.avail_out < size) {
		return (size - gfile->stream.avail_out); /*Nearly eof*/
	} else if (gfile->stream.avail_out == size) {
		return 0; /*EOF*/
	}
}

int evfs_file_close(evfs_filereference* file) {
	gzip_file* gfile = ecore_hash_get(gzip_hash, file);
	
	if (inflateEnd(&gfile->stream) != Z_OK) {
		printf("Error in gzip end\n");
	}

	free(gfile->buffer);
	ecore_hash_remove(gzip_hash, file);
	free(gfile);

}


