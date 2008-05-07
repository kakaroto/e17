/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ENGINE_EVAS_SOFTWARE_16_X11_H
#define EWL_ENGINE_EVAS_SOFTWARE_16_X11_H

#include "ewl_base.h"
#include <Ecore_X.h>
#include <Evas.h>
#include <Evas_Engine_Software_16_X11.h>

#define EWL_ENGINE_EVAS_SOFTWARE_16_X11(engine) \
                ((Ewl_Engine_Evas_Software_16_X11 *)engine)

typedef struct Ewl_Engine_Evas_Software_16_X11 Ewl_Engine_Evas_Software_16_X11;
struct Ewl_Engine_Evas_Software_16_X11
{
        Ewl_Engine engine;
};

#endif

