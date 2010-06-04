#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <netdb.h>
#include <gcrypt.h>
#include <zlib.h>

#include "Elixir.h"

typedef enum {
  REMOTE_SHA1 = 0,
  REMOTE_SIGN = 1,
  REMOTE_CERT = 2,
} Conn_Send_Mask;

# define MAX_KEY_LEN 32
# define MAX_IV_LEN 16

#define KILLD_MARK1 0xdead6a3e
#define KILLD_MARK2 0x007CB81F

#define KILLD_TCP_TIMEOUT 100

static const char *key = NULL;
static struct addrinfo *tcp_addr = NULL;
static Conn_Send_Mask mask = REMOTE_SHA1;

/*********************************************************************
 * This module send a request to a remote server to know if the asked
 * javascript could be run. The private key is set in the environment
 * before calling elixir.
 *
 * For generating the iv and ik needed for AES cipher, we run the pbkdf2
 * algorithm only 1024.
 *
 * It does use a TCP connection and send one packet divided in three
 * part :
 * - [SaltI] (8 bytes)
 * - |AES ciphered with private key & Salt I| (32 bytes)
 *    [Random noise] (4 bytes)
 *    [SaltII] (8 bytes)
 *    [Packet II initial length] (4 bytes)
 *    [Packet II compressed length] (4 bytes)
 *    [Packet II ciphered length] (4 bytes)
 *    [Marker I] (4 bytes)
 *    [Random noise] (4 bytes)
 * - |AES ciphered with private key & Salt II compressed packet| (variable size)
 *    [Random noise (rn0)] (4 bytes)
 *    [Timestamp] (8 bytes)
 *    [Certificate length] (4 bytes)
 *    [Signature length] (4 bytes)
 *    [SHA1 length] (4 bytes - Useless as it will always be the same)
 *    [Marker II] (4 bytes)
 *    [Certificate] (Certificate length)
 *    [Signature] (Signature length)
 *    [SHA1] (20 bytes)
 *
 * It send wait for a ciphered answer in one packet divided in two
 * part :
 * - [SaltIII] (8 bytes)
 * - |AES ciphered with private key & SaltII| (32 bytes)
 *    [Random noise] (4 bytes)
 *    [Random noise (rn0)] (4 bytes)
 *    [Timestamp] (8 bytes)
 *    [Right] (8 bytes)
 *    [Random noise] (8 bytes)
 *
 * Currently only two bits in the first byte of the right is considered.
 * There current meaning are :
 *    - 0 => NOK
 *    - 1 => GRANTED + OK
 *    - 2 => ELIXIR_AUTH + OK
 *    - 3 => ELIXIR_AUTH_REQUIRED + OK
 *
 */
#include <assert.h>
static int
_elixir_data_get(int fd, char *data, int length)
{
   struct pollfd pollfd;
   int position;
   int received;
   int retry = 0;

   pollfd.fd = fd;
   pollfd.events = POLLIN;
   pollfd.revents = 0;

   position = 0;

   /* FIXME: take time into account (better choice) */
   while (poll(&pollfd, 1, KILLD_TCP_TIMEOUT) >= 0 && retry < 30)
     {
	received = recv(fd, data + position, length - position, MSG_DONTWAIT);
	if (received == -1)
	  {
	     if (errno != EAGAIN
		 && errno != EINTR)
	       return 0;
	  }
	else
	  {
	     position += received;

	     if (position == length) return 1;
	  }
	retry++;
     }

   return 0;
}

static int
_elixir_hmac_sha1(const void *key, size_t key_len,
		  const void *data, size_t data_len,
		  unsigned char res[20])
{
   size_t hlen = gcry_md_get_algo_dlen(GCRY_MD_SHA1);
   gcry_md_hd_t mdh;
   unsigned char *hash;
   gpg_error_t err;

   err = gcry_md_open(&mdh, GCRY_MD_SHA1, GCRY_MD_FLAG_HMAC);
   if (err != GPG_ERR_NO_ERROR)
     return 1;

   err = gcry_md_setkey(mdh, key, key_len);
   if (err != GPG_ERR_NO_ERROR)
     goto on_error;

   gcry_md_write(mdh, data, data_len);

   hash = gcry_md_read(mdh, GCRY_MD_SHA1);
   if (!hash)
     goto on_error;

   memcpy(res, hash, hlen);

   gcry_md_close(mdh);

   return 0;

 on_error:
   gcry_md_close(mdh);
   return 1;
}

static int
_elixir_pbkdf2_sha1(const unsigned char *key, size_t key_len,
		    const unsigned char *salt, size_t salt_len,
		    int iter,
		    unsigned char *res, size_t res_len)
{
   unsigned char digest[20];
   unsigned char tab[4];
   unsigned char *p = res;
   unsigned char *buf;
   unsigned long i;
   int len = res_len;
   int tmp_len;
   int j, k;

   buf = alloca(salt_len + 4);
   if (!buf) return 1;

   for (i = 1; len; len -= tmp_len, p += tmp_len, i++)
     {
	if (len > 20) tmp_len = 20;
	else tmp_len = len;

	tab[0] = (unsigned char)(i & 0xff000000) >> 24;
	tab[1] = (unsigned char)(i & 0x00ff0000) >> 16;
	tab[2] = (unsigned char)(i & 0x0000ff00) >> 8;
	tab[3] = (unsigned char)(i & 0x000000ff) >> 0;

	memcpy(buf, salt, salt_len);
	memcpy(buf + salt_len, tab, 4);

	if (_elixir_hmac_sha1(key, key_len, buf, salt_len + 4, digest))
	  return 1;

	memcpy(p, digest, tmp_len);

	for (j = 1; j < iter; j++)
	  {
	     if (_elixir_hmac_sha1(key, key_len, digest, 20, digest))
	       return 1;

	     for (k = 0; k < tmp_len; k++)
	       p[k] ^= digest[k];
	  }
     }

   return 0;
}

#if 0
static void
_dump_data(const char *name, const void *data, int length)
{
   const char *map = "0123456789abcdef";
   const char *over = data;
   int i;

   fprintf(stderr, "%s: ", name);
   for (i = 0; i < length; ++i)
     if (i & 0x1)
       fprintf(stderr, "%c%c ", map[(over[i] >> 4) & 0xF], map[over[i] & 0xF]);
     else
       fprintf(stderr, "%c%c", map[(over[i] >> 4) & 0xF], map[over[i] & 0xF]);
   fprintf(stderr, "\n");
}
#else
#define _dump_data(Name, Data, Length) (void) 0;
#endif

static gcry_cipher_hd_t
_elixir_init_sk_iv_ik(uint64_t salt)
{
   unsigned char iv[MAX_IV_LEN];
   unsigned char ik[MAX_KEY_LEN];
   unsigned char key_material[MAX_IV_LEN + MAX_KEY_LEN];
   gcry_cipher_hd_t sk;
   gpg_error_t err;

   if (_elixir_pbkdf2_sha1((unsigned char*) key, strlen(key),
			   (unsigned char *)&salt, sizeof (uint64_t),
			   512,
			   key_material, MAX_KEY_LEN + MAX_IV_LEN))
     return NULL;

   memcpy(iv, key_material, MAX_IV_LEN);
   memcpy(ik, key_material + MAX_IV_LEN, MAX_KEY_LEN);

   _dump_data("salt", &salt, sizeof (uint64_t));
   _dump_data("iv", iv, MAX_IV_LEN);
   _dump_data("ik", ik, MAX_KEY_LEN);

   gcry_cipher_open(&sk, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CBC, 0);
   err = gcry_cipher_setiv(sk, iv, MAX_IV_LEN);
   if (err) return NULL;
   err = gcry_cipher_setkey(sk, ik, MAX_KEY_LEN);
   if (err) return NULL;

   return sk;
}

static inline uint64_t
elixir_htonll(uint64_t n)
{
   return (((uint64_t) htonl(n >> 32)) << 32) | htonl((uint32_t) n & 0xFFFFFFFF);
}

static elixir_check_validity_t
_elixir_security_check(const char *sha1, int sha1_length, const char *sign, int sign_length, const char *cert, int cert_length)
{
   char packetI_ciphered[32];
   char packetI[32];
   uint64_t salt[2];
   int noise[3];
   struct timeval tv;
   gcry_cipher_hd_t sk;
   char *packetII_comp;
   char *packetII;
   int *dec;
   uLongf len_pII_comp;
   time_t tv_sec;
   gpg_error_t err;
   long flags;
   unsigned int len_pII;
   unsigned int len_pII_out;
   int fd = -1;
   int cl = 0;
   int sl = 0;
   int state = 0;
   int i;

   /* Generate SaltI, SaltII */
   gcry_randomize(&salt, sizeof (salt), GCRY_STRONG_RANDOM);
   gcry_create_nonce(&noise, sizeof (noise));

   /* Get timestamp. */
   if (gettimeofday(&tv, NULL) != 0)
     return ELIXIR_CHECK_NOK;

   /* Build PacketII */
   len_pII = 4 + 8 + 4 + 4 + 4 + 4;
   switch (mask)
     {
      case REMOTE_CERT:
	 len_pII += cert_length;
	 cl = cert_length;
      case REMOTE_SIGN:
	 len_pII += sign_length;
	 sl = sign_length;
      case REMOTE_SHA1:
	 len_pII += sha1_length;
      default:
	 break;
     }

   /*  Malloc needed size according to Conn_Send_Mask for uncompressed buffer. */
   packetII = malloc(len_pII);
   if (!packetII) return ELIXIR_CHECK_NOK;

   packetII_comp = malloc(len_pII * 110 / 100);
   if (!packetII_comp) goto on_error;

   len_pII_comp = len_pII * 110 / 100;

   /*  Pack and set uncompressed buffer. */
   dec = (int*) packetII;
   /* [noise][timestamp][cert_length][sign_length][sha1_length][mark2][cert][sign][sha1] */
   dec[0] = htonl(noise[0]);
#if __WORDSIZE == 32
   dec[1] = 0;
   dec[2] = htonl(tv.tv_sec);
#elif __WORDSIZE == 64
   dec[1] = htonl((int)(tv.tv_sec >> 32));
   dec[2] = htonl(tv.tv_sec & 0xFFFFFFFF);
#else
# error "Unknow WORDSIZE"
#endif
   dec[3] = htonl(cl);
   dec[4] = htonl(sl);
   dec[5] = htonl(sha1_length);
   dec[6] = htonl(KILLD_MARK2);

   memcpy(packetII + sizeof (int) * 7, cert, cl);
   memcpy(packetII + sizeof (int) * 7 + cl, sign, sl);
   memcpy(packetII + sizeof (int) * 7 + cl + sl, sha1, sha1_length);

   /*  Compress buffer. */
   state = 1;
   if (compress2((Bytef *)packetII_comp, &len_pII_comp,
		 (Bytef *)packetII, (uLong)len_pII,
		 1) != Z_OK)
     goto on_error;

   /*  Pad compressed buffer. */
   len_pII_out = len_pII_comp;
   if ((len_pII_comp & 0x1F) != 0)
     {
	gcry_create_nonce(packetII_comp + len_pII_comp, 32 - (len_pII_comp & 0x1F));
	len_pII_comp += 32 - (len_pII_comp & 0x1F);
     }

   /*  Cipher pII with private key and saltII. */
   if (len_pII < len_pII_comp)
     packetII = realloc(packetII, len_pII_comp);

   state = 2;
   sk = _elixir_init_sk_iv_ik(salt[1]);
   err = gcry_cipher_encrypt(sk, packetII, len_pII_comp, packetII_comp, len_pII_comp);
   gcry_cipher_close(sk);
   if (err) goto on_error;

   /*** PacketII ready to fire. ***/

   /* Build PacketI */
   dec = (int*) &packetI[0];

   /*  Pack and set in static buffer. */
   dec[0] = noise[1];
   dec[1] = salt[1] & 0xFFFFFFFF;
   dec[2] = salt[1] >> 32;
   dec[3] = len_pII;
   dec[4] = len_pII_out;
   dec[5] = len_pII_comp;
   dec[6] = KILLD_MARK1;
   dec[7] = noise[2];

   for (i = 1; i < 7; ++i)
     dec[i] = htonl(dec[i]);

   /*  Cipher pI with private key and saltI. */
   state = 3;

   sk = _elixir_init_sk_iv_ik(salt[0]);
   err = gcry_cipher_encrypt(sk, packetI_ciphered, 32, packetI, 32);
   gcry_cipher_close(sk);
   if (err) goto on_error;

   /*** PacketI ready to fire. ***/
   state = 4;

   /* Connect to recorded adress. */
   fd = socket(tcp_addr->ai_family,
	       tcp_addr->ai_socktype,
	       tcp_addr->ai_protocol);
   if (fd < 0) goto on_error;

   state = 5;

   if (connect(fd,
	       tcp_addr->ai_addr,
	       tcp_addr->ai_addrlen) < 0)
     goto on_error;

   state = 6;

   flags = fcntl(fd, F_GETFL, NULL);
   if (flags < 0) goto on_error;

   state = 7;

   flags |= O_NONBLOCK;
   if (fcntl(fd, F_SETFL, flags) < 0)
     goto on_error;

   state = 8;

   /* Send SALTI */
   salt[0] = elixir_htonll(salt[0]);

   if (send(fd, &salt[0], sizeof (salt[0]), MSG_MORE) < 0)
     goto on_error;

   state = 9;

   /* Send PacketI */
   if (send(fd, packetI_ciphered, sizeof (packetI_ciphered), MSG_MORE) < 0)
     goto on_error;

   state = 10;

   /* Send PacketII */
   if (send(fd, packetII, len_pII_comp, 0) < 0)
     goto on_error;

   state = 11;

   /* Wait for reply packet or disconnection. */
   if (!_elixir_data_get(fd, (char*) &salt[0], sizeof (salt[0])))
     goto on_error;

   state = 12;

   if (!_elixir_data_get(fd, (char*) &packetI_ciphered, sizeof (packetI_ciphered)))
     goto on_error;

   /* Disconnect if needed. */
   close(fd);

   state = 13;

   /* Uncipher answer. */
   sk = _elixir_init_sk_iv_ik(salt[0]);
   err = gcry_cipher_decrypt(sk, packetI, 32, packetI_ciphered, 32);
   gcry_cipher_close(sk);
   if (err) goto on_error;

   /* Check answer. */
   dec = (int*) &packetI;

   for (i = 0; i < 8; ++i)
     dec[i] = ntohl(dec[i]);

   state = 14;

   if (dec[1] != noise[0]) /* Be sure that we get the reply to our message. */
     goto on_error;

#if __WORDSIZE == 32
   if (dec[2] != 0) goto on_error; /* Bad news, still running a 2009 piece of code 30 years later ?!? */
   tv_sec = dec[3];
#elif __WORDSIZE == 64
   tv_sec = ((time_t) dec[2] << 32) + dec[3];
#else
# error "Unknow Word size."
#endif
   state = 15;

   if (tv_sec < tv.tv_sec) /* The server should answer with a record up to date. */
     goto on_error;

   state = 16;

   /* Set level */

   /* Return authorization (First bit in [Right] . */
   switch ((dec[4] & 0xC0000000) >> 30)
     {
      case 0:
	 /* No security right received */
	 return ELIXIR_CHECK_NOK;
      case 1:
	 elixir_security_set(ELIXIR_GRANTED);
	 return ELIXIR_CHECK_OK;
      case 2:
	 elixir_security_set(ELIXIR_AUTH);
	 return ELIXIR_CHECK_OK;
      case 3:
	 elixir_security_set(ELIXIR_AUTH_REQUIRED);
	 return ELIXIR_CHECK_OK;
     }

   return ELIXIR_CHECK_NOK;

 on_error:
   fprintf(stderr, "error state = %i\n", state);

   if (packetII) free(packetII);
   if (packetII_comp) free(packetII_comp);
   if (fd >= 0) close(fd);

   return ELIXIR_CHECK_NOK;
}

static const Elixir_Security remote_security = {
  "remote",
  "cedric.bail@free.fr",
  {
    _elixir_security_check
  }
};

Eina_Bool
remote_security_init(void)
{
   struct addrinfo hints;
   const char *tcp_host;
   const char *tcp_port;
   const char *emask;
   int result;

   if (elixir_security_register(&remote_security) == EINA_FALSE)
     return EINA_FALSE;

   /* 0. check env */
   tcp_host = getenv("ELIXIR_REMOTE_TCP_HOST");
   tcp_port = getenv("ELIXIR_REMOTE_TCP_PORT");
   emask = getenv("ELIXIR_REMOTE_MASK");
   key = getenv("ELIXIR_REMOTE_KEY");
   if (!tcp_host || !tcp_port || !key)
     goto on_error;

   /* 1. dns request. */
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_INET6;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_V4MAPPED;
   hints.ai_protocol = IPPROTO_TCP;
   hints.ai_canonname = NULL;
   hints.ai_next = NULL;
   hints.ai_addr = NULL;

   result = getaddrinfo(tcp_host, tcp_port, &hints, &tcp_addr);
   if (result != 0) goto on_error;

   /* 2. Compute send mask. */
   if (emask)
     {
	if (!strncasecmp(emask, "sign", 4))
	  mask = REMOTE_SIGN;
	else if (!strncasecmp(emask, "cert", 4))
	  mask = REMOTE_CERT;
     }

   /* 3. Initialize crypto stuff if needed. */
   if (gcry_control(GCRYCTL_ANY_INITIALIZATION_P) == 0)
     {
	/* Before the library can be used, it must initialize itself. */
	gcry_check_version(NULL);

	/* Disable warning messages about problems with the secure memory subsystem.
	   This command should be run right after gcry_check_version. */
	gcry_control(GCRYCTL_DISABLE_SECMEM_WARN);

	/* This command is used to allocate a pool of secure memory and thus
	   enabling the use of secure memory. It also drops all extra privileges the
	   process has (i.e. if it is run as setuid (root)). If the argument nbytes
	   is 0, secure memory will be disabled. The minimum amount of secure memory
	   allocated is currently 16384 bytes; you may thus use a value of 1 to
	   request that default size. */
	gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
     }

   return EINA_TRUE;

 on_error:
   elixir_security_unregister(&remote_security);
   return EINA_FALSE;
}

void
remote_security_shutdown(void)
{
  if (tcp_addr) freeaddrinfo(tcp_addr);
  tcp_addr = NULL;
}

#ifndef EINA_STATIC_BUILD_REMOTE
EINA_MODULE_INIT(remote_security_init);
EINA_MODULE_SHUTDOWN(remote_security_shutdown);
#endif
