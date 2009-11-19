#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Elixir.h"

static elixir_check_validity_t
_elixir_let_check(const char *sha1, int sha1_length, const char *sign, int sign_length, const char *cert, int cert_length)
{
   elixir_security_set(ELIXIR_AUTH);

   return ELIXIR_CHECK_OK;
}

static const Elixir_Security let_security = {
  "let",
  "cedric.bail@free.fr",
  {
    _elixir_let_check
  }
};

Eina_Bool
let_security_init(void)
{
   if (elixir_security_register(&let_security) == EINA_FALSE)
     return EINA_FALSE;

   return EINA_TRUE;
}

void
let_security_shutdown(void)
{
   elixir_security_unregister(&let_security);
}

#ifndef EINA_STATIC_BUILD_LET
EINA_MODULE_INIT(let_security_init);
EINA_MODULE_SHUTDOWN(let_security_shutdown);
#endif
