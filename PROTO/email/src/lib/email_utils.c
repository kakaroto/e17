#include <stdlib.h>
#include <string.h>
#include "cencode.h"
#include "cdecode.h"
#include "md5.h"

char *
email_base64_encode(const char *string, double len, int *retlen)
{
   base64_encodestate s;
   char *ret = NULL;
   int size[2];

   if ((len < 1) || (!string)) return NULL;

   if (!(ret = malloc(sizeof(char) * ((((len + 2) - ((int)(len + 2) % 3)) / 3) * 4) + 4)))
     return NULL;
   base64_init_encodestate(&s);
   size[0] = base64_encode_block(string, len, ret, &s);
   size[1] = base64_encode_blockend(ret + size[0], &s);
   ret[size[0] + size[1]] = '\0';
   if (ret[size[0] + size[1] - 1] == '\n')
     ret[size[0] + size[1] - 1] = '\0';
   *retlen = size[0] + size[1];

   return ret;
}

char *
email_base64_decode(const char *string, int len, int *retlen)
{
   base64_decodestate s;
   char *ret = NULL;

   if ((len < 1) || (!string)) return NULL;

   if (!(ret = malloc(sizeof(char) * (int)((double)len / (double)(4 / 3)) + 1)))
     return NULL;
   base64_init_decodestate(&s);
   *retlen = base64_decode_block(string, len, ret, &s);
   ret[*retlen] = '\0';

   return ret;
}

void
email_md5_digest_to_str(unsigned char *digest, char *ret)
{
   char hexchars[] = "0123456789abcdef";
   unsigned int x, y;
   for (x = y = 0; x < 33; x++, y++)
     {
        ret[x++] = hexchars[digest[y] >> 4];
        ret[x] = hexchars[digest[y] & 15];
     }
   ret[32] = 0;
}

/* the following is based on public domain
 * hmac md5 code at http://tools.ietf.org/html/rfc2104#ref-MD5
 */
void
email_md5_hmac_encode(unsigned char *digest, const char *string, size_t size, const void *key, size_t ksize)
{
   md5_ctx ctx;
   unsigned char opad[65] = {0}, ipad[65] = {0};
   int i;
   /* if key is longer than 64 bytes reset it to key=MD5(key) */
   if (ksize > 64)
     {
        md5_buffer(key, ksize, digest);
        key = digest;
        ksize = 16;
     }

   /*
    * the HMAC_MD5 transform looks like:
    *
    * MD5(K XOR opad, MD5(K XOR ipad, text))
    *
    * where K is an n byte key
    * ipad is the byte 0x36 repeated 64 times
    * opad is the byte 0x5c repeated 64 times
    * and text is the data being protected
    */

   /* start out by storing key in pads */
   memcpy(ipad, key, ksize);
   memcpy(opad, key, ksize);

   /* XOR key with ipad and opad values */
   for (i = 0; i < 64; i++) {
           ipad[i] ^= 0x36;
           opad[i] ^= 0x5c;
   }
   /*
    * perform inner MD5
    */
   md5_init_ctx(&ctx);                  /* init context for 1st
                                         * pass */
   md5_process_bytes(ipad, 64, &ctx);      /* start with inner pad */
   md5_process_bytes(string, size, &ctx); /* then text of datagram */
   md5_finish_ctx(&ctx, digest);          /* finish up 1st pass */
   /*
    * perform outer MD5
    */
   md5_init_ctx(&ctx);                   /* init context for 2nd
                                         * pass */
   md5_process_bytes(opad, 64, &ctx);     /* start with outer pad */
   md5_process_bytes(digest, 16, &ctx);     /* then results of 1st
                                         * hash */
   md5_finish_ctx(&ctx, digest);          /* finish up 2nd pass */
}
