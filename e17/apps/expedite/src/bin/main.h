#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include <Evas.h>
#include "config.h"
#include "engine_software_x11.h"
#include "engine_gl_x11.h"
#include "engine_xrender_x11.h"
#include "engine_software_16_x11.h"
#include "ui.h"
#include "about.h"

#define OBNUM 128

extern Evas *evas;
extern int   win_w, win_h;

void srnd(void);
unsigned int rnd(void);
double get_time(void);
void   engine_loop(void);
int    engine_abort(void);

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
