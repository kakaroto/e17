#ifndef AUDIOFILE_VFS_H
#define AUDIOFILE_VFS_H 1

#include <stdio.h>

struct _AF_VirtualFile {
  int  (*read)   (AF_VirtualFile *vfile, unsigned char *data, int nbytes);
  long (*length) (AF_VirtualFile *vfile);
  int  (*write)  (AF_VirtualFile *vfile, const unsigned char *data, int nbytes);
  void (*destroy)(AF_VirtualFile *vfile);
  long (*seek)   (AF_VirtualFile *vfile, long offset, int is_relative);
  long (*tell)   (AF_VirtualFile *vfile);

  void *closure;
};

AF_VirtualFile *af_virtual_file_new(void);
AF_VirtualFile *af_virtual_file_new_for_file(FILE *fh);
void af_virtual_file_destroy(AF_VirtualFile *vfile);

size_t af_fread(void *data, size_t size, size_t nmemb, AF_VirtualFile *vfile);
size_t af_fwrite(const void *data, size_t size, size_t nmemb, AF_VirtualFile *vfile);
int af_fclose(AF_VirtualFile *vfile);
long af_flength(AF_VirtualFile *vfile);
int af_fseek(AF_VirtualFile *vfile, long offset, int whence);
long af_ftell(AF_VirtualFile *vfile);

#endif
