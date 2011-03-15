#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "Elixir.h"

struct _Elixir_Loader_File
{
   int fd;
   unsigned int length;

   const char *content;
   char *filename;
};

/*
 * params[0] == filename
 */
static const Elixir_Loader_File *
_elixir_text_request(int param, const char **params)
{
   Elixir_Loader_File *elf;
   char *filename = NULL;
   void *data = NULL;
   struct stat stf;
   int fd = -1;

   if (param != 1) return NULL;

   filename = elixir_exe_canonicalize(params[0]);
   if (!filename) return NULL;

   fd = open(filename, O_RDONLY);
   if (fd == -1) goto on_error;

   if (fcntl(fd, F_SETFD, FD_CLOEXEC) != 0) goto on_error;

   if (fstat(fd, &stf) != 0) goto on_error;
   if (!S_ISREG(stf.st_mode)) goto on_error;
   if (stf.st_size == 0) goto on_error;

   data = mmap(NULL, stf.st_size, PROT_READ, MAP_SHARED, fd, 0);
   if (!data) goto on_error;

   madvise(data, stf.st_size, MADV_WILLNEED);

   if (memchr(data, '\0', stf.st_size))
     goto on_error;

   elf = malloc(sizeof (Elixir_Loader_File));
   if (!elf) goto on_error;

   elf->fd = fd;
   elf->length = stf.st_size;
   elf->content = data;
   elf->filename = filename;

   elixir_security_set(ELIXIR_GRANTED);

   return elf;

 on_error:
   if (data) munmap(data, stf.st_size);
   if (fd != -1) close(fd);
   free(filename);

   return NULL;
}

static Eina_Bool
_elixir_text_release(const Elixir_Loader_File *file)
{
   munmap((void*) file->content, file->length);
   free(file->filename);
   free((void*) file);

   return EINA_TRUE;
}

static const void *
_elixir_text_get(const Elixir_Loader_File *file, unsigned int *length)
{
   elixir_debug_print("get content");
   if (length) *length = file->length;
   return file->content;
}

static const char *
_elixir_text_filename(const Elixir_Loader_File *file)
{
   return file->filename;
}

static Eina_Bool
_elixir_text_id(const Elixir_Loader_File *file, char sign[ELIXIR_SIGNATURE_SIZE])
{
   return elixir_id_compute(sign, file->length, file->content);
}

static const Elixir_Loader text_loader = {
  "text",
  "cedric.bail@free.fr",
  1, 1,
  {
    _elixir_text_request,
    _elixir_text_release,
    _elixir_text_get,
    NULL,
    _elixir_text_filename,
    _elixir_text_id,
    NULL
  }
};

Eina_Bool
text_loader_init(void)
{
   return elixir_loader_register(&text_loader);
}

void
text_loader_shutdown(void)
{
   elixir_loader_unregister(&text_loader);
}

#ifndef EINA_STATIC_BUILD_TEXT
EINA_MODULE_INIT(text_loader_init);
EINA_MODULE_SHUTDOWN(text_loader_shutdown);
#endif
