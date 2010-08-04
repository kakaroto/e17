#ifndef EWL_ENGINE_EVAS_SOFTWARE_XLIB_H
#define EWL_ENGINE_EVAS_SOFTWARE_XLIB_H

#include "ewl_base.h"
#include <Ecore_X.h>
#include <Evas.h>
#include <Evas_Engine_Software_X11.h>

#define EWL_ENGINE_EVAS_SOFTWARE_XLIB(engine) \
                ((Ewl_Engine_Evas_Software_Xlib *)engine)

typedef struct Ewl_Engine_Evas_Software_Xlib Ewl_Engine_Evas_Software_Xlib;
struct Ewl_Engine_Evas_Software_Xlib
{
        Ewl_Engine engine;
};

#endif

