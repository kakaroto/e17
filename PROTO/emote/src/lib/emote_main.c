#include "Emote.h"
#include "emote_private.h"

#include <libgen.h>
#include <string.h>

/* local function prototypes */
static void _emote_locate_paths(void);

EMAPI int
emote_init(void)
{
   int ret;

   /* hmm, may want to consider multiple entrance ideas wrt this.
    * ie: someone else calls init on the emote lib again (for whatever reason)
    * ... rather than trying to relocate paths, should we make them static 
    * as in they get filled in only once...Perhaps we keep an init count ? */
   _emote_locate_paths();

   ret = 0;
   ret |= emote_protocol_init();

   return ret;
}

EMAPI int
emote_shutdown(void)
{
   emote_protocol_shutdown();
   return 1;
}

/* local functions */
static void
_emote_locate_paths(void)
{
#if HAVE_DLADDR
   Dl_info emote_dl;
#endif

   if (getenv("em_LIBDIR"))
     strncpy(emote_paths.libdir, getenv("em_LIBDIR"), 
             sizeof(emote_paths.libdir));

   // Attempt to use dladdr
#if HAVE_DLADDR
   if ((!emote_paths.libdir[0]) && (dladdr((void*)emote_init, &emote_dl)))
     strncpy(emote_paths.libdir, dirname((char*)emote_dl.dli_fname),
             sizeof(emote_paths.libdir));
#endif

   // Fallbacks
#ifdef PACKAGE_LIB_DIR
   if (!emote_paths.libdir[0])
     strncpy(emote_paths.libdir, PACKAGE_LIB_DIR, sizeof(emote_paths.libdir));
#endif

   if (!emote_paths.libdir[0])
     printf("Cannot determine library path!");

   // Fill in protocoldir which is just based on libdir
   sprintf(emote_paths.protocoldir, "%s/emote/protocols", emote_paths.libdir);
}
