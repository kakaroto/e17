/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/*
 *  Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies of the Software, its documentation and marketing & publicity
 *  materials, and acknowledgment shall be given in the documentation, materials
 *  and software packages that this Software was used.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *  */

#include "config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#if HAVE_STRDUP
#define USE_LIBC_STRDUP  1	/* Use libc strdup if present */
#endif

#define LIST_FINDBY_NAME        0
#define LIST_FINDBY_ID          1
#define LIST_FINDBY_BOTH        2
#define LIST_FINDBY_NONE        3

#define LIST_TYPE_COUNT         18
#define LIST_TYPE_ANY            0
#define LIST_TYPE_CLIENT         1

typedef struct _list
{
   int                 type;
   char               *name;
   int                 id;
   void               *item;

   struct _list       *next;
}
List;

typedef struct _client
{
   char               *name;
   Window              win;
   char               *msg;
   char               *clientname;
   char               *version;
   char               *author;
   char               *email;
   char               *web;
   char               *address;
   char               *info;
   Pixmap              pmap;
}
Client;

typedef struct _root
{
   Window              win;
   Visual             *vis;
   int                 depth;
   Colormap            cmap;
   int                 scr;
   int                 w, h;
   Window              focuswin;
}
Root;

int                 EExit(void *code);
void               *Emalloc(int size);
void               *Erealloc(void *ptr, int size);
void                Efree(void *ptr);

void               *FindItem(char *name, int id, int find_by, int type);
void                AddItem(void *item, char *name, int id, int type);
void               *RemoveItem(char *name, int id, int find_by, int type);
void              **ListItemType(int *num, int type);
char              **ListItems(int *num, int type);
void              **ListItemTypeID(int *num, int type, int id);

void                SetupX(void);

void                CommsSetup(void);
void                CommsFindCommsWindow(void);
void                CommsSend(Client * c, char *s);
char               *CommsGet(Client ** c, XEvent * ev);
Client             *MakeClient(Window win);
void                ListFreeClient(void *ptr);
void                DeleteClient(Client * c);
void                HandleComms(XEvent * ev);

#if defined(__FILE__) && defined(__LINE__)
#define Efree(x) \
{ \
  if (x) \
    __Efree(x); \
  else \
    Alert("%s:%d:  Attempt to free a NULL pointer\n", __FILE__, __LINE__); \
}
#define Emalloc(x) \
__Emalloc(x)
#define Erealloc(x, y) \
__Erealloc(x, y)
#else
#define Efree(x) \
{ \
  if (x) \
    __Efree(x); \
  else \
    Alert("??:??:  Attempt to free a NULL pointer\n"); \
}
#define Emalloc(x) \
__Emalloc(x)
#define Erealloc(x, y) \
__Erealloc(x, y)
#endif

#if USE_LIBC_STRDUP
#define Estrdup(s) ((s) ? strdup(s) : NULL)
#else
char               *Estrdup(const char *s);
#endif

void               *__Emalloc(int size);
void               *__Erealloc(void *ptr, int size);
void                __Efree(void *ptr);

#define FILEPATH_LEN_MAX 4096
/* This turns on E's internal stack tracking system for  coarse debugging */
/* and being able to trace E for profiling/optimisation purposes (which */
/* believe it or not I'm actually doing) */

/* #define DEBUG 1 */

#ifdef DEBUG
extern int          call_level;
extern int          debug_level;

#endif
#ifdef DEBUG
#define EDBUG(l,x) \
{ \
  int i_call_level; \
  if (l<debug_level) \
{ \
      for(i_call_level=0;i_call_level<call_level;i_call_level++) \
        putchar('-'); \
      printf(" %8x %s\n",(unsigned int)time(NULL),x); \
      fflush(stdout); \
} \
  call_level++; \
}
#else
#define EDBUG(l,x)  \
;
#endif

#ifdef DEBUG
#define EDBUG_RETURN(x)  \
{ \
/*  int i_call_level; */\
  call_level--; \
/*  for(i_call_level=0;i_call_level<call_level;i_call_level++) */\
/*    putchar('-'); */\
/*  putchar('\n'); */\
  return (x); \
}
#define EDBUG_RETURN_  \
{ \
/*  int i_call_level; */\
  call_level--; \
/*  for(i_call_level=0;i_call_level<call_level;i_call_level++) */\
/*    putchar('-'); */\
/*  putchar('\n'); */\
  return; \
}
#else
#define EDBUG_RETURN(x)  \
{ \
  return (x); \
}
#define EDBUG_RETURN_  \
{ \
  return; \
}
#endif

void                Alert(char *fmt, ...);

void                EDisplayMemUse(void);

extern Display     *disp;
extern List         lists;
extern Window       comms_win;
extern Window       my_win;
extern Root         root;
extern char        *display_name;
