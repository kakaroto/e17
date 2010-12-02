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

/**
 * @brief Base64 encode a string of known length
 * @param string The string to encode
 * @param len The length of the string
 * @return Allocated base64 encoded string or #NULL on error
 * This calls base64 encode functions to encode @p string, allocating
 * memory for the encoded string.
 */
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

/**
 * @brief Base64 decode a string of known length
 * @param string The string to decode
 * @param len The length of the string
 * @return Allocated decoded string or #NULL on error
 * This calls base64 decode functions to decode @p string, allocating
 * memory for the decoded string.
 */
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

/**
 * @brief Find a string of known length in a larger string of known length
 * @param big The large string
 * @param small The string to find
 * @param big_len The length of @p big
 * @param small_len The length of @p small
 * @return Pointer to the first occurrence of @p small, or #NULL on error/failure
 * This can be considered strnstr, a utility function for finding a bounded string
 * in another bounded string.  It compares using unsigned char, however, so non-ascii
 * data can be found as well.
 */
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

/**
 * @brief Read a UUID from /proc/sys/kernel/random/uuid and stringshare it
 * @return The stringshared uuid
 * This function is used to return a stringshared random UUID.  UUIDS are
 * Universally Unique IDentifiers, strings of 36 characters such as:
 * 550e8400-e29b-41d4-a716-446655440000
 */
const char *
azy_uuid_new(void)
{
   char uuid[40];
   FILE *f;
   const char *ret = NULL;

   if (!(f = fopen("/proc/sys/kernel/random/uuid", "r")))
     return NULL;

   if (fgets(uuid, 37, f))
     ret = eina_stringshare_add_length(uuid, 36);

   fclose(f);

   return ret;
}
