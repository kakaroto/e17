#include "Emote.h"
#include "libemote.h"

#include <libgen.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

#if HAVE_DLFCN_H
# include <dlfcn.h>
#endif

static void _emote_locate_paths();

EMAPI int
emote_init()
{
   int ret;

   _emote_locate_paths();

   ret = 0;
   ret |= emote_protocol_init();

   return ret;
}

EMAPI void
emote_shutdown()
{
   emote_protocol_shutdown();
}

static void
_emote_locate_paths()
{
   #if HAVE_DLADDR
   Dl_info emote_dl;
   #endif

   if(getenv("em_LIBDIR"))
      strncpy(emote_paths.libdir, getenv("em_LIBDIR"), sizeof(emote_paths.libdir));

   // Attempt to use dladdr
   #if HAVE_DLADDR
   if(!emote_paths.libdir[0] && dladdr((void*)emote_init, &emote_dl))
   {
      strncpy(emote_paths.libdir, dirname((char*)emote_dl.dli_fname),
            sizeof(emote_paths.libdir));
   }
   #endif

   // Fallbacks
   #ifdef PACKAGE_LIB_DIR
   if(!emote_paths.libdir[0])
   {
      strncpy(emote_paths.libdir, PACKAGE_LIB_DIR, sizeof(emote_paths.libdir));
   }
   #endif

   if(!emote_paths.libdir[0])
   {
      printf("Cannot determine library path!");
   }

   // Fill in protocoldir which is just based on libdir
   sprintf(emote_paths.protocoldir, "%s/emote/protocols", emote_paths.libdir);
}
