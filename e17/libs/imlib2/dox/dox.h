#ifndef _DOX_H_
#define _DOX_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <Imlib2.h>

typedef struct __dox_hotspot
{
   char *name;
   int x;
   int y;
   int w;
   int h;
   struct __dox_hotspot *next;
} DoxHotspot;

void  parse_args( int argc, char **argv );

void  init_hotspot();
void  add_hotspot( char *target, int x, int y, int w, int h );
char *check_hotspot( int x, int y );
void  tidy_hotspots();

void  load_dox( char *file );
void  parse_dox( char *file );

void  load_dss( char *file );


#endif /* _DOX_H_ */
