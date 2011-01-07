/* Copyright 2011 Mike Blumenkrantz
 * This code may be freely distributed or modified with or without this header
 * gcc -fPIC -shared -Wl,-soname,libfd_catcher.so.1 -ldl -o libfd_catcher.so.1.0 fd_catcher.c -g
 * use with LD_PRELOAD to catch instances of the epoll fd in ecore being closed
 */
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

int thisistheepollfd = -1;

static int (*real_close)(int fd) = NULL;

int
close(int fd)
{
   if (!real_close)
     {
        real_close = dlsym(RTLD_NEXT, "close");
        if (dlerror())
          {
             fprintf(stderr, "close wrap failed: %s\n", dlerror());
             exit(1);
          }
     }

   else if ((thisistheepollfd > 0) && (fd == thisistheepollfd))
     {
        printf("got it!\n");
        pause();
     }
   return real_close(fd);
}
