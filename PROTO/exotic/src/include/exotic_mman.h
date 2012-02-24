#ifndef EXOTIC_MMAN_H_
# define EXOTIC_MMAN_H_

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EINA_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef EXOTIC_PROVIDE_MMAP

#include <sys/stat.h>

#define MAP_FAILED NULL
#define MAP_SHARED 0

#define MADV_RANDOM 0
#define MADV_SEQUENTIAL 0
#define MADV_WILLNEED 0

#define PROT_READ 0
#define PROT_WRITE 1

#ifdef mmap
# undef mmap
#endif
#define mmap exotic_mmap

EAPI void* exotic_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

#ifdef munmap
# undef munmap
#endif
#define munmap exotic_munmap

EAPI int exotic_munmap(void *addr, size_t length);

#ifdef madvise
# undef madvise
#endif
#define madvise exotic_madvise

EAPI int exotic_madvise(void *addr, size_t length, int advice);

#ifdef open
# undef open
#endif
#define open exotic_open

EAPI int exotic_open(const char *pathname, int flags, mode_t mode);

#ifdef close
# undef close
#endif
#define close exotic_close

EAPI int exotic_close(int fd);

#ifdef ftruncate
# undef ftruncate
#endif
#define ftruncate exotic_ftruncate

EAPI int exotic_ftruncate(int fd, unsigned long length);

struct exotic_stat
{
   dev_t	st_dev;
   ino_t	st_ino;
   mode_t	st_mode;
   nlink_t	st_nlink;
   uid_t	st_uid;
   gid_t	st_gid;
   dev_t	st_rdev;
   off_t	st_size;
   unsigned long       st_atime;
   unsigned long       st_mtime;
   unsigned long       st_ctime;
};

#ifdef stat
# define stat
#endif
#define stat exotic_stat

EAPI int exotic_stat(const char *path, struct stat *__buf);

#ifdef fstat
# undef fstat
#endif
#define fstat exotic_fstat

EAPI int exotic_fstat(int fd, struct stat *__buf);

#endif

#endif
