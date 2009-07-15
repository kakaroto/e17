#ifndef EFL_DEBUG_H
#define EFL_DEBUG_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef CWDEBUG
#include <libcwd/libraries_debug.h>

namespace efl {
  namespace channels {
    namespace dc {
      using namespace libcwd::channels::dc;
      extern libcwd::channel_ct warp;
      // Add new channels here...
    }
  }
}
#endif // CWDEBUG

// Define private debug output macros for use in header files of the library,
// there is no reason to do this for normal applications.
// We use a literal efl::channels here and not LIBCWD_DEBUGCHANNELS!
#define eflDebug(STATEMENT...) LibcwDebug(efl::channels, STATEMENT)
#define eflDout(cntrl, data) LibcwDout(efl::channels, libcwd::libcw_do, cntrl, data)
#define eflDoutFatal(cntrl, data) LibcwDoutFatal(efl::channels, libcwd::libcw_do, cntrl, data)
#define eflForAllDebugChannels(STATEMENT...) LibcwdForAllDebugChannels(efl::channels, STATEMENT)
#define eflForAllDebugObjects(STATEMENT...) LibcwdForAllDebugObjects(efl::channels, STATEMENT)

// All other macros might be used in header files of EFL, but need to be
// defined by the debug.h of the application that uses it.
// EFL_INTERNAL is defined when the library itself is being compiled (see below).
#if !defined(eflDebug) && !defined(EFL_INTERNAL)
#error The application source file (.cc or .cpp) must use '#include "efldebug.h"' _before_ including the header file that it includes now, that led to this error.
#endif

#endif // EFL_DEBUG_H

