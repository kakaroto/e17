#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#undef NULL

#include <File.h>
#include <Time.h>

#include "exotic_private.h"

#include <Exotic.h>

typedef struct _Exotic_Map Exotic_Map;
struct _Exotic_Map
{
   Exotic_Map *next;

   EXOTIC_FILE *f;
   File *osf;

   unsigned char *data;

   unsigned long long timestamp;

   unsigned long length;
   unsigned long offset;
   int fd;
   int refcount;

   unsigned char read_only : 1;
};

struct _EXOTIC_FILE
{
   EXOTIC_FILE *next;

   Exotic_Map *current;

   Exotic_Map *maps;

   const char file[1];
};

static int _exotic_current_fd = 3;
static EXOTIC_FILE *_exotic_files = NULL;

static FileSystem *fatfs = NULL;

EAPI EXOTIC_FILE *exotic_stderr = NULL;
EAPI EXOTIC_FILE *exotic_stdout = NULL;
EAPI EXOTIC_FILE *exotic_stdin = NULL;

void
exotic_filesystem_set(void *fs)
{
   fatfs = (FileSystem *) fs;
}

static EXOTIC_FILE *
_exotic_lookup_from_file(const char *pathname)
{
   EXOTIC_FILE *tmp = _exotic_files;

   while (tmp && strcmp(tmp->file, pathname))
     tmp = tmp->next;

   return tmp;
}

static Exotic_Map *
_exotic_lookup_from_fd(int fd)
{
   EXOTIC_FILE *tmp;

   for (tmp = _exotic_files; tmp; tmp = tmp->next)
     {
        Exotic_Map *map;

        for (map = tmp->maps; map; map = map->next)
          if (map->fd == fd)
            return map;
     }

   return NULL;
}

static Exotic_Map *
_exotic_lookup_from_map(void *addr)
{
   EXOTIC_FILE *tmp;

   for (tmp = _exotic_files; tmp; tmp = tmp->next)
     {
        Exotic_Map *map;

        for (map = tmp->maps; map; map = map->next)
          if (map->data <= addr && map->data + map->length > addr)
            return map;
     }

   return NULL;
}

static void
_exotic_destroy(EXOTIC_FILE *f)
{
   EXOTIC_FILE *walk;
   EXOTIC_FILE *prev;

   if (f->maps) return ;

   for (prev = NULL, walk = _exotic_files; walk && walk != f; prev = walk, walk = walk->next)
     ;

   if (!walk)
     {
        printf("something really bad happen Harry !\n");
     }
   else
     {
        if (prev) prev->next = walk->next;
        else _exotic_files = walk->next;
     }

   free(f);
}

static EXOTIC_FILE *
_exotic_file_new(const char *pathname)
{
   EXOTIC_FILE *r;
   size_t length;

   length = strlen(pathname);

   r = (EXOTIC_FILE *) calloc(1, sizeof (EXOTIC_FILE) + length);
   if (!r) return NULL;

   memcpy((void*) r->file, pathname, length + 1);

   r->next = _exotic_files;
   _exotic_files = r;

   return r;
}

static Exotic_Map *
_exotic_open(const char *pathname, unsigned char read_only)
{
   EXOTIC_FILE *f;
   Exotic_Map *m = NULL;

   f = _exotic_lookup_from_file(pathname);
   if (!f) f = _exotic_file_new(pathname);

   if (read_only)
     {
        if (f->current && f->current->read_only)
          {
             f->current->refcount++;
             return f->current;
          }

        m = (Exotic_Map*) calloc(1, sizeof (Exotic_Map));
        if (!m) goto on_error;

        m->data = (unsigned char *) fatfs->Load(pathname, &m->length, NULL);
        if (!m->data) goto on_error;

	m->read_only = 1;
        f->current = m;
     }
   else
     {
        m = (Exotic_Map*) calloc(1, sizeof (Exotic_Map));
        if (!m) goto on_error;

        m->osf = fatfs->Open(pathname);
        if (!m->osf) goto on_error;

        m->length = m->osf->GetSize();
        m->data = (unsigned char *) malloc(sizeof (unsigned char) * m->length);
        if (!m->data) goto on_error;

        m->osf->Read(0, m->data, m->length, NULL);

        f->current = m;
     }

   m->f = f;
   m->next = f->maps;
   f->maps = m;

   m->timestamp = Time::GetTime();

   m->fd = _exotic_current_fd++;
   m->refcount++;

   return m;

 on_error:
   printf("failed to open '%s'\n", pathname);
   _exotic_destroy(f);
   free(m);
   return NULL;
}

static Exotic_Map *
_exotic_create(const char *pathname)
{
   EXOTIC_FILE *f;
   Exotic_Map *m = NULL;

   f = _exotic_lookup_from_file(pathname);
   if (!f) f = _exotic_file_new(pathname);

   m = (Exotic_Map *) calloc(1, sizeof (Exotic_Map));
   if (!m) goto on_error;

   m->osf = fatfs->Create(pathname, NULL, true);
   if (!m->osf) goto on_error;

   m->data = NULL;

   m->f = f;
   m->next = f->maps;
   f->maps = m;

   m->timestamp = Time::GetTime();

   m->fd = _exotic_current_fd++;
   m->length = 0;
   m->refcount = 1;

   return m;

 on_error:
   _exotic_destroy(f);
   free(m);
   return NULL;
}

static void
_exotic_close(Exotic_Map *map)
{
   EXOTIC_FILE *parent;
   Exotic_Map *walk;
   Exotic_Map *prev;

   if (!map) return ;
   parent = map->f;

   map->refcount--;
   if (map->refcount > 0) return ;

   for (prev = NULL, walk = parent->maps; walk && walk != map; prev = walk, walk = walk->next)
     ;

   if (!walk)
     {
        printf("something bad happen Harry !\n");
     }
   else
     {
        if (prev) prev->next = walk->next;
        else parent->maps = walk->next;

        /* If the current file is closed and is read only, drop prev as the current one */
        if (parent->current == walk)
          {
             parent->current = walk->read_only ? prev : NULL;
          }
     }

   delete map->osf;
   free(map->data);
   free(map);

   _exotic_destroy(parent);
}

EAPI int
exotic_open(const char *pathname, int flags, mode_t mode __UNUSED__)
{
   Exotic_Map *m = NULL;

   if (flags == O_RDONLY)
     {
        m = _exotic_open(pathname, 1);
     }
   else if ((flags & (O_CREAT | O_TRUNC | O_RDWR)) == (O_CREAT | O_TRUNC | O_RDWR))
     {
        m = _exotic_create(pathname);
     }

   if (!m) return -1;
   return m->fd;
}

EAPI int
exotic_close(int fd)
{
   Exotic_Map *m;

   m = _exotic_lookup_from_fd(fd);
   if (!m) return -1;

   _exotic_close(m);
   return 0;
}

EAPI int
exotic_fstat(int fd, struct stat *__buf)
{
   Exotic_Map *m;

   if (!__buf) return -1;

   m = _exotic_lookup_from_fd(fd);
   if (!m) return -1;

   memset(__buf, 0, sizeof (struct stat));

   __buf->st_ino = 0;
   __buf->st_atime = m->timestamp;
   __buf->st_mtime = m->timestamp;
   __buf->st_ctime = m->timestamp;
   __buf->st_size = m->length;

   return 0;
}

EAPI int
exotic_stat(const char *path, struct stat *__buf)
{
   EXOTIC_FILE *f;

   if (!__buf) return -1;

   memset(__buf, 0, sizeof (struct stat));

   f = _exotic_lookup_from_file(path);
   if (!f || !f->current)
     {
        File *osf;

        osf = fatfs->Open(path);
        if (!osf) return -1;

        __buf->st_ino = 0;
        __buf->st_atime = Time::GetTime();
        __buf->st_mtime = __buf->st_atime;
        __buf->st_ctime = __buf->st_atime;
        __buf->st_size = osf->GetSize();

        delete osf;

        return 0;
     }

   __buf->st_ino = 0;
   __buf->st_atime = f->current->timestamp;
   __buf->st_mtime = f->current->timestamp;
   __buf->st_ctime = f->current->timestamp;
   __buf->st_size = f->current->length;

   return 0;
}

EAPI void*
exotic_mmap(void *addr __UNUSED__, size_t length __UNUSED__, int prot __UNUSED__, int flags, int fd, off_t offset)
{
   Exotic_Map *m;

   m = _exotic_lookup_from_fd(fd);
   if (!m) return MAP_FAILED;

   /* handle PROT_READ only flags */
   if (!m->read_only) return MAP_FAILED;
   /* check off limit */
   if (m->length < offset + length) return MAP_FAILED;

   m->refcount++;

   return m->data + offset;
}

EAPI int
exotic_munmap(void *addr, size_t length)
{
   Exotic_Map *m;

   m = _exotic_lookup_from_map(addr);
   _exotic_close(m);

   return 0;
}

EAPI int
exotic_ftruncate(int fd, unsigned long length)
{
   Exotic_Map *m;
   unsigned char *tmp;

   m = _exotic_lookup_from_fd(fd);
   if (!m || m->read_only || !m->osf) return -1;

   length = m->osf->Adjust(length);

   tmp = (unsigned char *) realloc(m->data, length);
   if (tmp == NULL) return -1;

   m->data = tmp;
   m->length = length;

   return -1;
}

EAPI FILE *
exotic_fopen(const char *name, const char *type)
{
   Exotic_Map *m;

   /* need to handle rb and wb */
   if (!strcmp(type, "rb"))
     {
        m = _exotic_open(name, 1);
     }
   else if (!strcmp(type, "wb"))
     {
        m = _exotic_open(name, 0);
     }
   else
     {
        return NULL;
     }

   return (FILE*) m;
}

EAPI FILE *
exotic_fdopen(int fd, const char *type)
{
   Exotic_Map *m;

   /* only need to implement wb */
   if (strcmp(type, "wb"))
     return NULL;

   m = _exotic_lookup_from_fd(fd);
   if (!m || m->read_only) return NULL;

   return (FILE*) m;
}

EAPI int
exotic_fclose(FILE *f)
{
   Exotic_Map *m;

   m = (Exotic_Map *) f;
   if (!m) return -1;

   _exotic_close(m);
   return 0;
}

EAPI int
exotic_fflush(FILE *f)
{
   Exotic_Map *m;

   m = (Exotic_Map *) f;
   if (!m || !m->osf) return -1;

   if ((unsigned long) m->osf->Write(0, m->data, m->length, NULL) != m->length)
     return -1;
   return 0;
}

EAPI int
exotic_fseek(FILE *f, long offset, int type)
{
   Exotic_Map *m;

   m = (Exotic_Map *) f;
   if (!m) return -1;

   switch (type)
     {
      case SEEK_SET:
         if (offset > (long) m->length) return -1;
         if (offset < 0) return -1;
         m->offset = offset;
         return 0;
      case SEEK_CUR:
         if (m->offset + offset > m->length) return -1;
         if (m->offset + offset < 0) return -1;
         m->offset += offset;
         return 0;
      case SEEK_END:
         if (m->length + offset > m->length) return -1;
         if (m->length + offset < 0) return -1;
         m->offset = m->length + offset;
         return 0;
      default:
         return -1;
     }
}

EAPI size_t
exotic_fread(void *buffer, size_t size, size_t n, FILE *f)
{
   Exotic_Map *m;
   unsigned long total;

   m = (Exotic_Map *) f;
   if (!m || !m->osf) return -1;
   if (size == 0 || n == 0) return 0;

   if (!m->data)
     {
        m->length = m->osf->GetSize();
        m->data = (unsigned char *) malloc(m->length);
        if (!m->data) return -1;
        m->length = m->osf->Read(0, m->data, m->length, NULL);
     }

   total = size * n;
   if (m->offset + total > m->length)
     {
        total = ((m->length - m->offset) / size) * size;
     }
   if (total == 0) return 0;

   memcpy(buffer, m->data + m->offset, total);
   m->offset += total;

   return total / size;
}

EAPI size_t
exotic_fwrite(const void *buffer, size_t size, size_t n, FILE *f)
{
   Exotic_Map *m;

   m = (Exotic_Map *) f;
   if (!m || !m->osf) return -1;
   if (size == 0 || n == 0) return 0;

   if (!m->data)
     {
        m->length = m->osf->GetSize();
        m->data = (unsigned char *) malloc(m->length);
        m->length = m->osf->Read(0, m->data, m->length, NULL);
     }

   if (m->offset + size * n > m->length)
     {
        unsigned char *tmp;

        tmp = (unsigned char *) realloc(m->data, m->offset + size * n);
        if (!tmp) return -1;

        m->data = tmp;
        m->length = m->offset + size * n;
     }

   memcpy(m->data + m->offset, buffer, size * n);

   return size * n;
}

EAPI int
exotic_rename(const char *src, const char *dst)
{
   EXOTIC_FILE *fsrc;
   EXOTIC_FILE *fdst;
   File *out;
   unsigned char need_close = 0;

   fsrc = _exotic_lookup_from_file(src);

   /* Destination read content will be invalid after rename */
   fdst = _exotic_lookup_from_file(dst);
   if (fdst && fdst->current)
     {
        fdst->current = NULL;
     }

   if (fsrc && fsrc->current)
     {
        if (fsrc->current->osf)
          {
             if (fsrc->current->osf->Rename(dst))
               {
                  return 0;
               }

             if (!fsrc->current->data)
               {
                  fsrc->current->length = fsrc->current->osf->GetSize();
                  fsrc->current->data = (unsigned char *) malloc(fsrc->current->length);
                  if (!fsrc->current->data) return -1;
                  fsrc->current->length = fsrc->current->osf->Read(0, fsrc->current->data, fsrc->current->length, NULL);
               }
          }
     }

   if (!fsrc->current)
     {
        need_close = 1;
        fsrc->current = _exotic_open(src, 1);
        if (!fsrc->current) return -1;
     }

   out = fatfs->Open(dst);
   out->Write(0, fsrc->current->data, fsrc->current->length, NULL);
   delete out;

   if (need_close)
     {
        _exotic_close(fsrc->current);
     }

   fsrc->current = NULL;

   fatfs->Delete(src);

   return 0;
}

EAPI int
exotic_unlink(const char *path)
{
   EXOTIC_FILE *f;

   f = _exotic_lookup_from_file(path);
   if (f && f->current)
     f->current = NULL;
   fatfs->Delete(path);
   return 0;
}

EAPI int
exotic_mkdir(const char *path, mode_t __mode __UNUSED__)
{
   delete fatfs->MakeDirectory(path);
   return -1;
}

EAPI int
exotic_ferror(FILE *stream __UNUSED__)
{
   return ENOSPC;
}

EAPI int
exotic_madvise(void *addr __UNUSED__, size_t length __UNUSED__, int advice __UNUSED__)
{
   /* nothing to do here in that context */
   return 0;
}

EAPI int
 exotic_fputc(int c, FILE *f)
{
   /* useless to implement, but must be done here to prevent segv */
   return EOF;
}

EAPI int
exotic_fputs(const char *line __UNUSED__, FILE *f __UNUSED__)
{
   /* useless to implement, but must be done here to prevent segv */
   return EOF;
}

EAPI int
exotic_fprintf(FILE *f __UNUSED__, const char *format __UNUSED__, ...)
{
   /* useless to implement, but must be done here to prevent segv */
   return -1;
}

EAPI int
exotic_fscanf(FILE *f __UNUSED__, const char *format __UNUSED__, ...)
{
   /* useless to implement, but must be done here to prevent segv */
   return EOF;
}

EAPI char *
exotic_fgets(char *buffer __UNUSED__, int len __UNUSED__, FILE *f __UNUSED__)
{
   /* useless to implement, but must be done here to prevent segv */
   return NULL;
}
