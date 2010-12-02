/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 * See COPYING for license details
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "cencode.h"
#include "cdecode.h"
#include "Azy.h"

char *
azy_base64_encode(const char *string,
                   double      len)
{
   base64_encodestate s;
   char *ret = NULL;
   int retlen[2];

   if (len < 1) return NULL;

   if (!(ret = malloc(sizeof(char) * ((((len + 2) - ((int)(len + 2) % 3)) / 3) * 4) + 4)))
     return NULL;
   base64_init_encodestate(&s);
   retlen[0] = base64_encode_block(string, len, ret, &s);
   retlen[1] = base64_encode_blockend(ret + retlen[0], &s);
   ret[retlen[0] + retlen[1]] = '\0';

   return ret;
}

char *
azy_base64_decode(const char *string,
                   int         len)
{
   base64_decodestate s;
   char *ret = NULL;
   int retlen;

   if (len < 1) return NULL;

   if (!(ret = malloc(sizeof(char) * (int)((double)len / (double)(4 / 3)) + 1)))
     return NULL;
   base64_init_decodestate(&s);
   retlen = base64_decode_block(string, len, ret, &s);
   ret[retlen] = '\0';

   return ret;
}

unsigned char *
azy_memstr(const unsigned char *big,
            const unsigned char *small,
            size_t               big_len,
            size_t               small_len)
{
   unsigned char *x = (unsigned char *)big;

   if ((!big) || (!small) || (big_len < 1) || (small_len < 1) || (big_len < small_len))
     return NULL;

   for (; big_len >= small_len; x++, big_len--)
     if (!memcmp(x, small, small_len))
       return x;
   return NULL;
}

const char *
azy_uuid_new(void)
{
   char uuid[48];
   FILE *f;
   const char *ret = NULL;

   if (!(f = fopen("/proc/sys/kernel/random/uuid", "r")))
     return NULL;

   if (fgets(uuid, 36, f))
     ret = eina_stringshare_add_length(uuid, 36);

   fclose(f);

   return ret;
}
