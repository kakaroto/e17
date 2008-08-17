/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ENGINE_EVAS_SOFTWARE_SDL_H
#define EWL_ENGINE_EVAS_SOFTWARE_SDL_H

#include "ewl_base.h"
#include <Ecore_Sdl.h>
#include <Evas.h>
#include <Evas_Engine_SDL.h>

#define EWL_ENGINE_EVAS_SOFTWARE_SDL(engine) \
                ((Ewl_Engine_Evas_Software_SDL *)engine)

typedef struct Ewl_Engine_Evas_Software_SDL Ewl_Engine_Evas_Software_SDL;
struct Ewl_Engine_Evas_Software_SDL
{
        Ewl_Engine engine;

        /* There can be only one window in sdl land */
        Ewl_Window *window;
};

#endif

