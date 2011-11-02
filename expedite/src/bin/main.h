#ifndef MAIN_H
#define MAIN_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#ifndef _MSC_VER
# include <sys/time.h>
#endif

#ifdef _WIN32
# include <windows.h>
#endif

#ifdef HAVE_EVIL
# include <Evil.h>
#endif

#include <Evas.h>
#if HAVE_EVAS_SOFTWARE_XLIB
#include "engine_software_xlib.h"
#endif
#if HAVE_EVAS_XRENDER_X11
#include "engine_xrender_x11.h"
#endif
#if HAVE_EVAS_OPENGL_X11
#include "engine_gl_x11.h"
#endif
#if HAVE_EVAS_SOFTWARE_XCB
#include "engine_software_xcb.h"
#endif
#if HAVE_EVAS_XRENDER_XCB
#include "engine_xrender_xcb.h"
#endif
#if HAVE_EVAS_SOFTWARE_GDI
#include "engine_software_gdi.h"
#endif
#if HAVE_EVAS_SOFTWARE_DDRAW
#include "engine_software_ddraw.h"
#endif
#if HAVE_EVAS_DIRECT3D
#include "engine_direct3d.h"
#endif
#if HAVE_EVAS_SOFTWARE_SDL
#include "engine_software_sdl.h"
#endif
#if HAVE_EVAS_OPENGL_SDL
#include "engine_gl_sdl.h"
#endif
#if HAVE_EVAS_FB
#include "engine_fb.h"
#endif
#if HAVE_EVAS_DIRECTFB
#include "engine_directfb.h"
#endif
#if HAVE_EVAS_OPENGL_COCOA
#include "engine_gl_cocoa.h"
#endif
#if HAVE_EVAS_SOFTWARE_16_X11
#include "engine_software_16_x11.h"
#endif
#if HAVE_EVAS_SOFTWARE_16_DDRAW
#include "engine_software_16_ddraw.h"
#endif
#if HAVE_EVAS_SOFTWARE_16_WINCE
#include "engine_software_16_wince.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "ui.h"
#include "about.h"

#define EVAS_FRAME_QUEUING	// for test

#define OBNUM 128
#define LOOPS 128

extern Evas *evas;
extern int   win_w, win_h;
extern int   loops;
extern int   fullscreen;

typedef struct _Expedite_Resolution Expedite_Resolution;
typedef struct _Expedite_Engine Expedite_Engine;

struct _Expedite_Resolution
{
   const char *name;
   int width;
   int height;
};

struct _Expedite_Engine
{
   const char *name;
   Eina_Bool (*init)(const char *engine, int width, int height);
   void (*loop)(void);
   void (*shutdown)(void);
};

void         srnd(void);
unsigned int rnd(void);
double       get_time(void);
const char  *build_path(const char *filename);
void         engine_loop(void);
int          engine_abort(void);

#ifdef __cplusplus
}
#endif

#define KEY_STD \
   if ((!strcmp(key, "Escape")) || (!strcmp(key, "q")) || (!strcmp(key, "Q")) || (!strcmp(key, "Return"))) \
       { \
	  _cleanup(); \
	  ui_menu(); \
       }

#define FPS_STD(x) \
   if ((f >= loops) && (!done)) \
       { \
	  double fps; \
	  fps = (double)f / t; \
	  ui_fps(fps); \
	  printf("%4.2f , %s\n", fps, x); \
	  done = 1; \
       }

#define SLOW 5.0

#endif
