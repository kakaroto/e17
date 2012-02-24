#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Exotic.h>

EAPI pid_t
getpid(void)
{
   return 1;
}

/* This function call is only used by ecore_file */
EAPI int
access(const char *path __UNUSED__, int __amode __UNUSED__)
{
   return 0;
}

EAPI int
chmod(const char *path __UNUSED__, mode_t __mode __UNUSED__)
{
   return -1;
}

EAPI int
mkstemp(char *path __UNUSED__)
{
   return -1;
}

EAPI int
symlink(const char *n1 __UNUSED__, const char *n2 __UNUSED__)
{
   return -1;
}

EAPI ssize_t
readlink(const char *path __UNUSED__, char *buf __UNUSED__, size_t buflen __UNUSED__)
{
   return -1;
}

EAPI int
remove(const char *path __UNUSED__)
{
   return -1;
}

EAPI int
rmdir(const char *path __UNUSED__)
{
   return -1;
}

/* This function call is only used by ecore_pipe.c */
EAPI int
pipe(int __fildes[2] __UNUSED__)
{
   return -1;
}

EAPI ssize_t
read(int fd __UNUSED__, void *buf __UNUSED__, size_t byte __UNUSED__)
{
   return -1;
}

EAPI ssize_t
write(int fd __UNUSED__, const void *buffer __UNUSED__, size_t byte __UNUSED__)
{
   return -1;
}

