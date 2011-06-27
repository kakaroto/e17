#include <stdlib.h>
#include "cencode.h"
#include "cdecode.h"

char *
email_base64_encode(const char *string, double len)
{
   base64_encodestate s;
   char *ret = NULL;
   int retlen[2];

   if ((len < 1) || (!string)) return NULL;

   if (!(ret = malloc(sizeof(char) * ((((len + 2) - ((int)(len + 2) % 3)) / 3) * 4) + 4)))
     return NULL;
   base64_init_encodestate(&s);
   retlen[0] = base64_encode_block(string, len, ret, &s);
   retlen[1] = base64_encode_blockend(ret + retlen[0], &s);
   ret[retlen[0] + retlen[1]] = '\0';
   if (ret[retlen[0] + retlen[1] - 1] == '\n')
     ret[retlen[0] + retlen[1] - 1] = '\0';

   return ret;
}

char *
email_base64_decode(const char *string, int len)
{
   base64_decodestate s;
   char *ret = NULL;
   int retlen;

   if ((len < 1) || (!string)) return NULL;

   if (!(ret = malloc(sizeof(char) * (int)((double)len / (double)(4 / 3)) + 1)))
     return NULL;
   base64_init_decodestate(&s);
   retlen = base64_decode_block(string, len, ret, &s);
   ret[retlen] = '\0';

   return ret;
}
