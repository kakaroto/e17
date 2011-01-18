/* We don't want to open javascript file with size over 2^32. */
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <eina_hash.h>

#ifdef HAVE_ZLIB
# include <zlib.h>
#endif

#ifdef HAVE_GCRYPT
# include <gcrypt.h>
#else
# ifdef HAVE_TOMCRYPT
#  include <tomcrypt.h>
# endif
#endif

#include "Elixir.h"

static char                               key[16] = "";

static Eina_Hash			 *ids = NULL;

static const Elixir_Loaded_File         **id = NULL;
static char                             **cid = NULL;
static int                                id_max_size = 0;
static int                                id_active_size = 0;
static char				 *gid = NULL;

#ifdef HAVE_GCRYPT
#define BLOCK_SIZE 16
#define SIGN_VAR				\
  unsigned char *tmp;				\
  gcry_cipher_hd_t sk;				\
  gcry_md_hd_t md;				\
  size_t hlen;

#define SIGN_INIT							\
  gcry_cipher_open(&sk, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_ECB, 0);	\
  gcry_cipher_setkey(sk, key, 16);					\
  gcry_md_open(&md, GCRY_MD_SHA256, 0);

#define SIGN_COMPUTE(Source)					  \
  {								  \
    gcry_cipher_encrypt(sk, out, BLOCK_SIZE, Source, BLOCK_SIZE); \
    gcry_md_write(md, out, BLOCK_SIZE);				  \
  }

#define SIGN_RETRIEVE				\
  hlen = gcry_md_get_algo_dlen(GCRY_MD_SHA256);	\
  tmp = gcry_md_read(md, 0);

#define SIGN_SHUTDOWN				\
  gcry_md_close(md);				\
  gcry_cipher_close(sk);

#else
# ifdef HAVE_TOMCRYPT
# define BLOCK_SIZE 8

# define SIGN_VAR				\
  unsigned char tmp[33];			\
  symmetric_key sk;				\
  hash_state md;

# define SIGN_INIT				\
  rc5_setup((unsigned char*) key, 16, 0, &sk);	\
  sha256_init(&md);

# define SIGN_COMPUTE(Source)			\
  {						\
     rc5_ecb_encrypt(Source, out, &sk);		\
     sha256_process(&md, out, BLOCK_SIZE);	\
  }

# define SIGN_RETRIEVE				\
  sha256_done(&md, tmp);

# define SIGN_SHUTDOWN				\
  rc5_done(&sk);

# else
# error "No fallback method"
# endif
#endif

EAPI Eina_Bool
elixir_id_compute(char sign[ELIXIR_SIGNATURE_SIZE], int length, const char *buffer)
{
#ifdef HAVE_ZLIB
# define CHUNK 16384
   unsigned char comp[CHUNK];
   z_stream strm;
#endif
   unsigned char out[BLOCK_SIZE];
   SIGN_VAR;
   unsigned int i;
   unsigned int j;

#ifdef HAVE_ZLIB
   memset(&strm, 0, sizeof (strm));
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   if (deflateInit(&strm, 4) != Z_OK)
     return -1;

   strm.avail_in = length;
   strm.next_in = (unsigned char*) buffer;
   strm.avail_out = CHUNK;
   strm.next_out = (unsigned char*) comp;
#endif

   fprintf(stderr, "sign init\n");

   SIGN_INIT;

#ifdef HAVE_ZLIB
   do {
      strm.avail_out = CHUNK;
      strm.next_out = (unsigned char*) comp;

      deflate(&strm, Z_FINISH);

      for (i = 0; i < (CHUNK - strm.avail_out) / BLOCK_SIZE; ++i)
	SIGN_COMPUTE(comp + i * BLOCK_SIZE);

      for (i = 0; i < (CHUNK - strm.avail_out) % BLOCK_SIZE; ++i)
	out[i] = comp[CHUNK - strm.avail_out - i - 1];

      SIGN_COMPUTE(out);
   } while (strm.avail_out == 0);

   deflateEnd(&strm);
#else
   for (i = 0; i < length / BLOCK_SIZE; ++i)
     SIGN_COMPUTE(buffer + i * BLOCK_SIZE);

   for (i = 0; i < length % BLOCK_SIZE; ++i)
     out[i] = buffer[length - i - 1];

   SIGN_COMPUTE(out);
#endif

   SIGN_RETRIEVE;

   for (i = 0; i < ELIXIR_SIGNATURE_SIZE && i < BLOCK_SIZE; ++i)
     sign[i] = out[i];

   if (i < ELIXIR_SIGNATURE_SIZE)
     while (i < ELIXIR_SIGNATURE_SIZE)
       for (j = 0; i < ELIXIR_SIGNATURE_SIZE && j < BLOCK_SIZE; ++i, ++j)
	 sign[i] = out[j];
   else
     while (i < BLOCK_SIZE)
       for (j = 0; j < ELIXIR_SIGNATURE_SIZE && i < BLOCK_SIZE; ++i, ++j)
	 sign[j] ^= out[i];

   SIGN_SHUTDOWN;

   return EINA_TRUE;
}

void
elixir_id_init(void)
{
   FILE*        fl;
   int          size;
   int          i;

   eina_init();

   ids = eina_hash_string_superfast_new(free);

   fl = fopen(ELIXIR_ID_KEY_FILE, "r");
   if (!fl)
     fl = fopen("/dev/urandom", "r");
   if (!fl)
     return ;
   size = fread(key, 1, 16, fl);
   if (size != 16)
     for (i = size; i < 16; ++i)
       key[i] = key[i % size];
   fclose(fl);
}

int
elixir_id_push(const Elixir_Loaded_File *file)
{
   char buffer[ELIXIR_SIGNATURE_SIZE];
   char destination[ELIXIR_SIGNATURE_SIZE * 2 + 1];
   unsigned int *value;
   void *tmp;
   unsigned int i;
   int position;
   int length;

   memset(buffer, 0, ELIXIR_SIGNATURE_SIZE);

   if (!elixir_loader_id(file, buffer)) return -1;

   for (i = 0, position = 0;
	i < ELIXIR_SIGNATURE_SIZE / sizeof (unsigned int);
	++i)
     {
	value = (void*) &buffer[i * sizeof (unsigned int)];
	position += eina_convert_xtoa(*value, destination + position) - 1;
     }

   position++;

   if (id_active_size + 1 > id_max_size)
     {
        id_max_size += 8;
        tmp = realloc(id, sizeof (Elixir_Loaded_File *) * id_max_size);
        if (!tmp) return EINA_FALSE;
        id = tmp;
	tmp = realloc(cid, sizeof (char *) * id_max_size);
	if (!tmp) return EINA_FALSE;
	cid = tmp;
     }
   cid[id_active_size] = strdup(destination);
   id[id_active_size++] = file;

   if (!gid)
     {
	gid = strdup(destination);
	return EINA_TRUE;
     }

   length = strlen(gid) + strlen(destination) + 1;
   tmp = realloc(gid, length);
   if (!tmp) return EINA_FALSE;
   gid = tmp;

   strcat(gid, destination);

   return EINA_TRUE;
}

void
elixir_id_pop(void)
{
   if (!id || !cid) return ;

   id_active_size--;
   id[id_active_size] = NULL;

   free(cid[id_active_size]);
   cid[id_active_size] = NULL;
}

const char*
elixir_id_gid(void)
{
   return gid;
}

const char*
elixir_id_cid(void)
{
   if (id_active_size == 0)
     return NULL;
   return cid[id_active_size - 1];
}

const unsigned char*
elixir_id_content(unsigned int* size)
{
   if (id_active_size == 0)
     return NULL;

   return elixir_loader_content(id[id_active_size - 1], size);
}

const char*
elixir_id_filename(void)
{
   if (id_active_size == 0)
     return NULL;

   return elixir_loader_filename(id[id_active_size - 1]);
}

const char*
elixir_id_section(void)
{
   if (id_active_size == 0)
     return NULL;

   return elixir_loader_section(id[id_active_size - 1]);
}

void
elixir_id_close(void)
{
   if (gid)
     free(gid);
   gid = NULL;
}

void
elixir_id_shutdown(void)
{
   eina_hash_free(ids);
   ids = NULL;

   elixir_id_close();

   eina_shutdown();
}


