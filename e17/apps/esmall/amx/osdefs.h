/* _Windows     set when compiling for any version of Microsoft Windows
 * __WIN32__    set when compiling for Windows95 or WindowsNT (32 bit mode)
 * __32BIT__    set when compiling in 32-bit "flat" mode (DOS or Windows)
 *
 * Copyright 1998-1999, ITB CompuPhase, The Netherlands.
 * info@compuphase.com.
 */

#ifndef _OSDEFS_H
#define _OSDEFS_H

/* Every compiler uses different "default" macros to indicate the mode
 * it is in. Throughout the source, we use the Borland C++ macros, so
 * the macros of Watcom C/C++ and Microsoft Visual C/C++ are mapped to
 * those of Borland C++.
 */
#if defined(__WATCOMC__)
#  if defined(__WINDOWS__) || defined(__NT__)
#    define _Windows
#  endif
#  ifdef __386__
#    define __32BIT__
#  endif
#  if defined(_Windows) && defined(__32BIT__)
#    define __WIN32__
#  endif
#elif defined(_MSC_VER)
#  if defined(_WINDOWS) || defined(_WIN32)
#    define _Windows
#  endif
#  ifdef _WIN32
#    define __WIN32__
#    define __32BIT__
#  endif
#endif

#if defined(_Windows) && !defined(_INC_WINDOWS) && !defined(__WINDOWS_H) && !defined(NOWINDOWS)
#  include <windows.h>
#  include <windowsx.h>
#endif

#endif  /* _OSDEFS_H */
