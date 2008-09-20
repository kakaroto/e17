#ifndef MAIN_H
#define MAIN_H

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#ifdef _WIN32
# include <windows.h>
#endif

#include <Evas.h>
#if HAVE_EVAS_SOFTWARE_X11
#include "engine_software_x11.h"
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
#if HAVE_EVAS_SOFTWARE_DDRAW
#include "engine_software_ddraw.h"
#endif
#if HAVE_EVAS_DIRECT3D
#include "engine_direct3d.h"
#endif
#if HAVE_EVAS_OPENGL_GLEW
#include "engine_gl_glew.h"
#endif
#if HAVE_EVAS_SOFTWARE_SDL
#include "engine_software_sdl.h"
#endif
#if HAVE_EVAS_FB
#include "engine_fb.h"
#endif
#if HAVE_EVAS_DIRECTFB
#include "engine_directfb.h"
#endif
#if HAVE_EVAS_QUARTZ
#include "engine_quartz.h"
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

#include "ui.h"
#include "about.h"

#define OBNUM 128

extern Evas *evas;
extern int   win_w, win_h;

void         srnd(void);
unsigned int rnd(void);
double       get_time(void);
const char  *build_path(const char *filename);
void         engine_loop(void);
int          engine_abort(void);

#define KEY_STD \
   if ((!strcmp(key, "Escape")) || (!strcmp(key, "q")) || (!strcmp(key, "Q")) || (!strcmp(key, "Return"))) \
       { \
	  _cleanup(); \
	  ui_menu(); \
       }

#define FPS_STD(x) \
   if ((f >= 32) && (!done)) \
       { \
	  double fps; \
	  fps = (double)f / t; \
	  ui_fps(fps); \
	  printf("%4.2f , %s\n", fps, x); \
	  done = 1; \
       }

#define SLOW 5.0

#endif
