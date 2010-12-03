#ifndef         _ELIXIR_PRIVATE_H
# define        _ELIXIR_PRIVATE_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
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

#include <ctype.h>
#include "config.h"

EAPI void       slprintf(char *str, size_t size, const char *format, ...);

#ifndef HAVE_STRNDUP

static inline char *strndup(const char *str, size_t n)
{
   char *__new;
   size_t __len;

   for (__len = 0; __len < n && str[__len] != '\0'; ++__len)
     ;

   __new = malloc(__len);
   if (!__new) return NULL;

   return (char*) memcpy(__new, str, __len);
}

#endif

#endif
