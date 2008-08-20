#ifndef _ENESIM_TEST_H_
#define _ENESIM_TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <png.h>
#include <setjmp.h>
#include <alloca.h>
#include <stdlib.h>

#include "Eina.h"
#include "Enesim.h"
#include "config.h"
//#include "SDL.h"

#include "image.h"

#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512
#define PI 3.1415
Enesim_Surface * surface_new(int w, int h, Enesim_Surface_Format fmt);
//void surface_free(Enesim_Surface *s);
//void surface_blt(Enesim_Surface *s, SDL_Surface *sdl);

#endif /*_ENESIM_TEST_H_*/
