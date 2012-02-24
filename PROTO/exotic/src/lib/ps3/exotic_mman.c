#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "Exotic.h"

/***** API *****/

EAPI void *
exotic_mmap(void  *addr __UNUSED__,
	    size_t len,
	    int    prot,
	    int    flags,
	    int    fd,
	    off_t  offset)
{
   void *data;
   size_t size;
   off_t current;

   data = malloc(len);
   if (!data)
     {
        fprintf(stderr, "[Exotic] [mmap] malloc failed\n");
        return MAP_FAILED;
     }

   current = lseek(fd, 0, SEEK_CUR);

   if (lseek(fd, offset, SEEK_SET) == -1)
     {
        fprintf(stderr, "[Exotic] [mmap] offset lseek failed\n");
        free(data);
        return MAP_FAILED;
     }

   size = read(fd, data, len);
   if (size != len)
     {
        fprintf(stderr, "[Exotic] [mmap] read failed\n");
        free(data);
        return MAP_FAILED;
     }

   if (lseek(fd, -len, SEEK_CUR) == -1)
     {
        fprintf(stderr, "[Exotic] [mmap] lseek failed\n");
        free(data);
        return MAP_FAILED;
     }

   return data;
}

EAPI int
exotic_munmap(void  *addr,
	      size_t len __UNUSED__)
{
   if (addr && (addr != MAP_FAILED))
     free(addr);

   return 0;
}

EAPI int
exotic_madvise(void  *addr __UNUSED__,
	       size_t length __UNUSED__,
	       int    advice __UNUSED__)
{
   return 0;
}

