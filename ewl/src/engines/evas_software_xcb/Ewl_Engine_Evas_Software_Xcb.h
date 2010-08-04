#ifndef EWL_ENGINE_EVAS_SOFTWARE_XCB_H
#define EWL_ENGINE_EVAS_SOFTWARE_XCB_H

#include "ewl_base.h"
#include <Ecore_X.h>
#include <Evas.h>
#include <Evas_Engine_Software_X11.h>

#define EWL_ENGINE_EVAS_SOFTWARE_XCB(engine) \
                ((Ewl_Engine_Evas_Software_Xcb *)engine)

typedef struct Ewl_Engine_Evas_Software_Xcb Ewl_Engine_Evas_Software_Xcb;
struct Ewl_Engine_Evas_Software_Xcb
{
        Ewl_Engine engine;
};

#endif

