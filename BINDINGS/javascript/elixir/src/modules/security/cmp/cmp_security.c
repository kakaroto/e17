#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "Elixir.h"

#ifndef DER_CERT_FILE
# define DER_CERT_FILE "/etc/elixir.der"
#endif

static int length = 0;
static const char *memory = NULL;
static int fd = -1;

static elixir_check_validity_t
_elixir_cmp_check(const char *sha1, int sha1_length, const char *sign, int sign_length, const char *cert, int cert_length)
{
   if (!memory) return ELIXIR_CHECK_NOK;
   if (!sign || !cert) return ELIXIR_CHECK_NOK;

   if (cert_length != length) return ELIXIR_CHECK_NOK;
   if (memcmp(cert, memory, length) == 0)
     {
	elixir_security_set(ELIXIR_AUTH_REQUIRED);

	return ELIXIR_CHECK_OK;
     }
   return ELIXIR_CHECK_NOK;
}

static const Elixir_Security cmp_security = {
  "cmp",
  "cedric.bail@free.fr",
  {
    _elixir_cmp_check
  }
};

Eina_Bool
cmp_security_init(void)
{
   struct stat stf;

   if (elixir_security_register(&cmp_security) == EINA_FALSE)
     return EINA_FALSE;

   fd = open(DER_CERT_FILE, O_RDONLY);
   if (fd == -1) goto on_error;

   if (fcntl(fd, F_SETFD, FD_CLOEXEC) != 0) goto on_error;

   if (fstat(fd, &stf) != 0) goto on_error;
   if (!S_ISREG(stf.st_mode)) goto on_error;
   if (stf.st_size == 0) goto on_error;

   memory = mmap(NULL, stf.st_size, PROT_READ, MAP_SHARED, fd, 0);
   if (!memory) goto on_error;

   length = stf.st_size;

   return EINA_TRUE;

 on_error:
   if (fd >= 0) close(fd);
   elixir_security_unregister(&cmp_security);
   return EINA_FALSE;
}

void
cmp_security_shutdown(void)
{
   if (memory) munmap((void*) memory, length);
   if (fd >= 0) close(fd);
   memory = NULL;
   fd = -1;

   elixir_security_unregister(&cmp_security);
}

#ifndef EINA_STATIC_BUILD_CMP
EINA_MODULE_INIT(cmp_security_init);
EINA_MODULE_SHUTDOWN(cmp_security_shutdown);
#endif
