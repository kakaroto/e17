#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Exotic.h>

#undef ntohl
#undef htonl
#undef NULL
#undef realloc
#undef putchar

#include "lib.h"

extern "C" {
  EAPI void __assert_func(const char *file, int line, const char *function, const char *test);
}

EAPI void
__assert_func(const char *file, int line, const char *function, const char *test)
{
   printf("%s:%i:'%s': %s\n", file, line, function, test);
   abort();
}

