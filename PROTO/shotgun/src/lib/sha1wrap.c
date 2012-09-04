/* zrpc: RPC database+server for Zentific
 * Copyright (C) 2007, 2008, 2009, 2010, 2011 Zentific LLC and AUTHORS
 *
 * All rights reserved.
 * Use is subject to license terms.
 *
 * Please visit http://zentific.com for news and updates
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include "shotgun_private.h"
#include "sha1.h"

static const char *
_sha1_to_string(const unsigned char *hashout)
{
   const char hextab[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
   char sha1[41] = {0};
   unsigned int i = 0;

   for (i = 0; i < 20; i++)
     {
        sha1[2 * i] = hextab[(hashout[i] >> 4) & 0x0f];
        sha1[2 * i + 1] = hextab[hashout[i] & 0x0f];
     }
   return eina_stringshare_add(sha1);
}

const char *
sha1_buffer(const unsigned char *data, size_t len)
{
   SHA_CTX2 ctx;
   unsigned char hashout[20];
   unsigned char *buf;

   if (EINA_UNLIKELY(len > 65000))
     buf = malloc(len);
   else
     buf = alloca(len);
   EINA_SAFETY_ON_NULL_RETURN_VAL(buf, NULL);
   memcpy(buf, data, len);

   SHA1_Init2(&ctx);
   SHA1_Update2(&ctx, buf, len);
   SHA1_Final2(hashout, &ctx);
   if (EINA_UNLIKELY(len > 65000)) free(buf);
   return _sha1_to_string(hashout);
}

