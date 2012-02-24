#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string.h>

#include <Exotic.h>

EAPI int
exotic_isspace(int c)
{
   if (c < 0) return 0;
   return strchr("\f\n\r\t\v", c & 0xFF) ? 1 : 0;
}

EAPI int
exotic_tolower(int c)
{
   if (c >= 'A' && c <= 'Z')
     {
        return c - 'A' + 'a';
     }
   return c;
}

EAPI int
exotic_isalpha(int c)
{
   return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'));
}

EAPI int
exotic_isdigit(int c)
{
   return (c >= '0') && (c <= '9');
}

EAPI int
exotic_isalnum(int c)
{
   return exotic_isalpha(c) || exotic_isdigit(c);
}
