#include "em_private.h"

#include <libgen.h>
#include <sys/stat.h>

static void _find_prefixes();

/**
 * @return 1 on success, 0 on fail
 * @brief Initialize emote library
 */
int em_init()
{
  _find_prefixes();

  em_protocol_init();

  return 1;
}

void em_shutdown()
{
  em_protocol_shutdown();
}

static void
_find_prefixes()
{
  #if HAVE_DLADDR
  Dl_info em_dl;
  #endif

  if(getenv("em_LIBDIR"))
    strncpy(em_paths.libdir, getenv("em_LIBDIR"), sizeof(em_paths.libdir));

  // Attempt to use dladdr
  #if HAVE_DLADDR
  if(!em_paths.libdir[0] && dladdr((void*)em_init, &em_dl))
  {
    strncpy(em_paths.libdir, dirname((char*)em_dl.dli_fname),
            sizeof(em_paths.libdir));
  }
  #endif

  // Fallbacks
  #ifdef PACKAGE_LIB_DIR
  if(!em_paths.libdir[0])
  {
    strncpy(em_paths.libdir, PACKAGE_LIB_DIR, sizeof(em_paths.libdir));
  }
  #endif

  if(em_paths.libdir[0])
  {
    printf("Cannot determine library path!");
  }

  sprintf(em_paths.protocoldir, "%s/emote/protocols", em_paths.libdir);
}
