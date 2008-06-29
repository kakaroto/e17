/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ENGINE_EVAS_SOFTWARE_DDRAW_H
#define EWL_ENGINE_EVAS_SOFTWARE_DDRAW_H

#include "ewl_base.h"
#include <Ecore_Win32.h>
#include <Evas.h>
#include <Evas_Engine_Software_DDraw.h>

#define EWL_ENGINE_EVAS_SOFTWARE_DDRAW(engine) \
                ((Ewl_Engine_Evas_Software_DDraw *)engine)

typedef struct Ewl_Engine_Evas_Software_DDraw Ewl_Engine_Evas_Software_DDraw;
struct Ewl_Engine_Evas_Software_DDraw
{
        Ewl_Engine engine;
};

#endif

