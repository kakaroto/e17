/*
 * GeeXboX Enna Media Center.
 * Copyright (C) 2005-2010 The Enna Project
 *
 * This file is part of Enna.
 *
 * Enna is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Enna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Enna; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>

#include <Eina.h>
#include <Edje.h>

#include "enna.h"

#ifdef HAVE_EVIL
# include <Evil.h>
#endif

#include "enna_config.h"
#include "utils.h"
#include "vfs.h"
#include "buffer.h"


typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
   const char *cache;
   const char *data;
   const char *config;
};


static int _util_init_count = -1;
static char *mylocale = NULL;
static Smart_Data *sd;

int
enna_util_init()
{
   /* Prevent multiple loads */
   if (_util_init_count > 0)
     return ++_util_init_count;

   sd = calloc(1, sizeof(Smart_Data));
   sd->cache = eina_stringshare_printf("%s/%s", efreet_cache_home_get(), "/enna");
   sd->config = eina_stringshare_printf("%s/%s", efreet_config_home_get(), "/enna");
   sd->data = eina_stringshare_printf("%s/%s", efreet_data_home_get(), "/enna");

   if (!ecore_file_is_dir(sd->cache))
     ecore_file_mkdir(sd->cache);

   if (!ecore_file_is_dir(sd->config))
     ecore_file_mkdir(sd->config);

   if (!ecore_file_is_dir(sd->data))
     ecore_file_mkdir(sd->data);

   _util_init_count = 1;

   return 1;
}

int
enna_util_shutdown()
{
   _util_init_count--;
   if (_util_init_count == 0)
     {
        eina_stringshare_del(sd->cache);
        eina_stringshare_del(sd->config);
        eina_stringshare_del(sd->data);
        ENNA_FREE(sd);
     }

   return _util_init_count;
}

char *
enna_util_user_home_get()
{
   static char *home = NULL;

   if (home)
     return home;

   home = getenv("HOME")?strdup(getenv("HOME")):NULL;
   if (!home)
     return getenv("CWD")?strdup(getenv("CWD")):NULL;
   if (!home)
     {
        return strdup("/");
     }
   return home;
}

const char *
enna_util_data_home_get()
{
   return sd->data;
}

const char *
enna_util_config_home_get()
{
   return sd->config;
}

const char *
enna_util_cache_home_get()
{
   return sd->cache;
}

int
enna_util_has_suffix(char *str, Eina_List * patterns)
{
   Eina_List *l;
   int result = 0;

   unsigned int i;
   char *tmp;

   if (!patterns || !str || !str[0])
     return 0;

   for (l = patterns; l; l = eina_list_next(l))
     {
        tmp = calloc(1, strlen(str) + 1);
        for (i = 0; i < strlen(str); i++)
          tmp[i] = tolower(str[i]);
        result |= enna_util_str_has_suffix(tmp, (char *)l->data);
        ENNA_FREE(tmp);
     }
   return result;
}



#define MD5_SIZE 33
#define MD5_SUM_SIZE 16

typedef struct md5_s {
   uint64_t len;
   uint8_t block[64];
   uint32_t ABCD[4];
} md5_t;

const int md5_size = sizeof (md5_t);

static const uint8_t S[4][4] = {
  { 7, 12, 17, 22 }, /* Round 1 */
  { 5,  9, 14, 20 }, /* Round 2 */
  { 4, 11, 16, 23 }, /* Round 3 */
  { 6, 10, 15, 21 }  /* Round 4 */
};

static const uint32_t T[64] = { // T[i]= fabs(sin(i+1)<<32)
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, /* Round 1 */
  0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af,
  0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e,
  0x49b40821,

  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, /* Round 2 */
  0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6,
  0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9,
  0x8d2a4c8a,

  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, /* Round 3 */
  0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
  0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8,
  0xc4ac5665,

  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, /* Round 4 */
  0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0,
  0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb,
  0xeb86d391,
};

#define CORE(i, a, b, c, d)                                             \
  t = S[i >> 4][i & 3];                                                 \
  a += T[i];                                                            \
                                                                        \
  if (i < 32) {                                                         \
     if (i < 16) a += (d ^ (b & (c ^ d))) + X[           i & 15 ];      \
     else        a += (c ^ (d & (c ^ b))) + X[ (1 + 5 * i) & 15 ];      \
  } else {                                                              \
     if (i < 48) a += (b ^ c ^d)          + X[ (5 + 3 * i) & 15 ];      \
     else        a += (c ^ (b |~ d))      + X[ (  7 * i)   & 15 ];      \
  }                                                                     \
  a = b + (( a << t ) | ( a >> (32 - t) ));

#define CORE2(i)                                \
  CORE(i,a,b,c,d)                               \
  CORE((i+1),d,a,b,c)                           \
  CORE((i+2),c,d,a,b)                           \
  CORE((i+3),b,c,d,a)

#define CORE4(i)                                \
  CORE2(i)                                      \
  CORE2((i+4))                                  \
  CORE2((i+8))                                  \
  CORE2((i+12))

static void
body (uint32_t ABCD[4], uint32_t X[16])
{
   int t;
   unsigned int a = ABCD[3];
   unsigned int b = ABCD[2];
   unsigned int c = ABCD[1];
   unsigned int d = ABCD[0];

   CORE4 (0);
   CORE4 (16);
   CORE4 (32);
   CORE4 (48);

   ABCD[0] += d;
   ABCD[1] += c;
   ABCD[2] += b;
   ABCD[3] += a;
}

static void
md5_init (md5_t *ctx)
{
   ctx->len = 0;

   ctx->ABCD[0] = 0x10325476;
   ctx->ABCD[1] = 0x98badcfe;
   ctx->ABCD[2] = 0xefcdab89;
   ctx->ABCD[3] = 0x67452301;
}

static void
md5_update (md5_t *ctx, const uint8_t *src, const int len)
{
   int i, j;

   j = ctx->len & 63;
   ctx->len += len;

   for (i = 0; i < len; i++)
     {
        ctx->block[j++] = src[i];
        if (j == 64)
          {
             body (ctx->ABCD, (uint32_t*) ctx->block);
             j = 0;
          }
     }
}

static void
md5_final (md5_t *ctx, uint8_t *dst)
{
   int i;
   uint64_t finalcount = ctx->len << 3;

   md5_update (ctx, (const uint8_t *) "\200", 1);
   while ((ctx->len & 63) < 56)
     md5_update (ctx, (const uint8_t *) "", 1);

   md5_update (ctx, (uint8_t *) &finalcount, 8);

   for (i = 0; i < 4; i++)
     ((uint32_t *) dst)[i] = ctx->ABCD[3-i];
}

static void
md5_sum (uint8_t *dst, const uint8_t *src, const int len)
{
   md5_t ctx[1];

   md5_init (ctx);
   md5_update (ctx, src, len);
   md5_final (ctx, dst);
}

char *
md5sum (char *str)
{
   unsigned char sum[MD5_SUM_SIZE];
   char md5[MD5_SIZE];
   int i;

   if (!str)
     return NULL;

   md5_sum (sum, (const uint8_t *) str, strlen(str));
   memset (md5, '\0', MD5_SIZE);

   for (i = 0; i < MD5_SUM_SIZE; i++)
     {
        char tmp[3];
        sprintf (tmp, "%02x", sum[i]);
        strcat (md5, tmp);
     }

   return strdup (md5);
}

char *init_locale (void)
{
   char *curlocale=setlocale(LC_ALL, "");
   curlocale=curlocale?curlocale:setlocale(LC_ALL, "C");
   mylocale = curlocale?strdup(curlocale):NULL;
   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   return mylocale;
}

char *get_locale (void)
{
   return mylocale;
}

char *get_lang (void)
{
   char *lang;

   if (mylocale && (strncmp(mylocale, "C", 1) > 0))
     {
        lang = malloc(3);
        strncpy(lang, mylocale, 2);
        lang[2] = '\0';
     }
   else
     lang = strdup ("en");

   return lang;
}

void
enna_util_env_set(const char *var, const char *val)
{
   if (val)
     setenv(var, val, 1);
   else
     unsetenv(var);
}


/**
 * Remove Trailing spaces from str
 */
char *
enna_util_str_chomp(char *str)
{
   char *c;

   if (!str)
     return NULL;

   if (!*str)
     return str;

   for (c = str + strlen(str) - 1; c >= str && isspace(*c); c--)
     *c = '\0';

   return str;
}

double
enna_util_atof(const char *nptr)
{
   double div = 1.0;
   int res, integer;
   unsigned int frac = 0, start = 0, end = 0;

   while (*nptr && !isdigit((int) (unsigned char) *nptr) && *nptr != '-')
     nptr++;

   if (!*nptr)
     return 0.0;

   res = sscanf(nptr, "%i.%n%u%n", &integer, &start, &frac, &end);
   if (res < 1)
     return 0.0;

   if (!frac)
     return integer;

   if (integer < 0)
     div = -div;

   div *= pow(10.0, end - start);
   return integer + frac / div;
}

Eina_List *
enna_util_tuple_get (const char *str, const char *delimiter)
{
   Eina_List *tuple = NULL;
   char *l, *s = NULL;

   if (!str || !delimiter)
     return NULL;

   l = strdup(str);
   s = strtok(l, delimiter);
   do
     {
        if (s)
          tuple = eina_list_append(tuple, strdup(s));
        s = strtok(NULL, delimiter);
     } while (s);

   ENNA_FREE(l);

   return tuple;
}

char *
enna_util_tuple_set (Eina_List *tuple, const char *delimiter)
{
   char *buf;
   Enna_Buffer *b;
   char *c;
   Eina_List *l;

   if (!tuple || !delimiter)
     return NULL;

   b =  enna_buffer_new();
   EINA_LIST_FOREACH(tuple, l, c)
     {
        if (b->len > 0)
          enna_buffer_append(b, delimiter);
        enna_buffer_append(b, c);
     }

   buf = strdup(b->buf);
   enna_buffer_free(b);

   return buf;
}

static Eina_Bool
enna_util_str_has_suffix_helper(const char *str,
                                const char *suffix,
                                int (*cmp)(const char *, const char *))
{
   size_t str_len;
   size_t suffix_len;

   str_len = strlen(str);
   suffix_len = strlen(suffix);
   if (suffix_len > str_len)
     return EINA_FALSE;

   return cmp(str + str_len - suffix_len, suffix) == 0;
}

/**
 * @brief Check if the given string has the given prefix.
 *
 * @param str The string to work with.
 * @param prefix The prefix to check for.
 * @return #EINA_TRUE if the string has the given prefix, #EINA_FALSE otherwise.
 *
 * This function returns #EINA_TRUE if @p str has the prefix
 * @p prefix, #EINA_FALSE otherwise. If the length of @p prefix is
 * greater than @p str, #EINA_FALSE is returned.
 */
Eina_Bool
enna_util_str_has_prefix(const char *str, const char *prefix)
{
   size_t str_len;
   size_t prefix_len;

   str_len = strlen(str);
   prefix_len = strlen(prefix);
   if (prefix_len > str_len)
     return EINA_FALSE;

   return (strncmp(str, prefix, prefix_len) == 0);
}

/**
 * @brief Check if the given string has the given suffix.
 *
 * @param str The string to work with.
 * @param suffix The suffix to check for.
 * @return #EINA_TRUE if the string has the given suffix, #EINA_FALSE otherwise.
 *
 * This function returns #EINA_TRUE if @p str has the suffix
 * @p suffix, #EINA_FALSE otherwise. If the length of @p suffix is
 * greater than @p str, #EINA_FALSE is returned.
 */
/**
 * @param str the string to work with
 * @param suffix the suffix to check for
 * @return true if str has the given suffix
 * @brief checks if the string has the given suffix
 */
Eina_Bool
enna_util_str_has_suffix(const char *str, const char *suffix)
{
   return enna_util_str_has_suffix_helper(str, suffix, strcmp);
}

/**
 * @brief Check if the given string has the given suffix.
 *
 * @param str The string to work with.
 * @param ext The  extension to check for.
 * @return #EINA_TRUE if the string has the given extension, #EINA_FALSE otherwise.
 *
 * This function does the same like enna_util_str_has_suffix(), but with a
 * case insensitive compare.
 */
Eina_Bool
enna_util_str_has_extension(const char *str, const char *ext)
{
   return enna_util_str_has_suffix_helper(str, ext, strcasecmp);
}

const char *
enna_util_duration_to_string(const char *length)
{
   const char *str;
   Enna_Buffer *buf;

   int hh = 0;
   int mm = 0;
   int ss = 0;

   hh = (int) (atoi(length) / 3600 / 1000);
   mm = (int) ((atoi(length) / 60 / 1000) - (60 * hh));
   ss = (int) ((atoi(length) / 1000) - (60 * mm) - (3600 * hh));

   buf = enna_buffer_new();
   if (hh)
     enna_buffer_appendf(buf, "%.2dh", hh);
   if (mm)
     enna_buffer_appendf(buf, "%.2d:", mm);

   enna_buffer_appendf(buf, "%.2d", ss);

   str = eina_stringshare_add(buf->buf);
   enna_buffer_free(buf);
   return str;
}


Eina_List *
enna_util_stringlist_get(const char *str)
{
   Eina_List *list = NULL;
   const char *s, *b;
   if (!str) return NULL;
   for (b = s = str; 1; s++)
     {
        if ((*s == ' ') || (!*s))
          {
             char *t = malloc(s - b + 1);
             if (t)
               {
                  strncpy(t, b, s - b);
                  t[s - b] = 0;
                  list = eina_list_append(list, eina_stringshare_add(t));
                  free(t);
               }
             b = s + 1;
          }
        if (!*s) break;
     }
   return list;
}

void
enna_util_stringlist_free(Eina_List *list)
{
   const char *s;
   EINA_LIST_FREE(list, s) eina_stringshare_del(s);
}
