#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/shape.h>
#include <Imlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>

#ifdef _HAVE_STRING_H
#include <string.h>
#elif _HAVE_STRINGS_H
#include <strings.h>
#endif

#ifndef SYSTEM_FNRC
#ifndef __EMX__
#define SYSTEM_FNRC "/etc/fnrc"
#else
#define SYSTEM_FNRC "/Xfree86/lib/x11/enlightenment/fnrc"
#endif
#endif /* endef SYSTEM_FNRC */

#define FONT_TO_RIGHT 0
#define FONT_TO_DOWN  1
#define FONT_TO_UP    2
#define FONT_TO_LEFT  3

#define MODE_VERBATIM  0
#define MODE_WRAP_CHAR 1
#define MODE_WRAP_WORD 2

typedef struct _FnlibChar
  {
    char               *file;
    ImlibImage         *im;
    int                 x, y;
    int                 width;
    struct
      {
	int                 x, y;
	int                 width;
	int                 rendered_width, rendered_height;
	Pixmap              pmap, mask;
	int                 size;
      }
    current;
  }
FnlibChar;

typedef struct _FnlibFontSize
  {
    int                 size;
    char                orientation;
    int                 default_char;
    int                 num_chars;
    FnlibChar          *chars;
    struct
      {
	int                 size;
      }
    current;
  }
FnlibFontSize;

typedef struct _FnlibFont
  {
    int                 refs;
    char               *name;
    char               *dir;
    int                 num;
    FnlibFontSize      *fonts;
  }
FnlibFont;

typedef struct _FnlibStyle
  {
    char                mode;
    char                orientation;
    int                 justification;
    int                 spacing;
  }
FnlibStyle;

typedef struct _FnlibData
  {
    ImlibData          *id;
    int                 num_dirs;
    char              **dirs;
    int                 num_fonts;
    FnlibFont         **font;
  }
FnlibData;
