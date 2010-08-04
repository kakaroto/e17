#ifndef EWL_ENGINE_EVAS_SOFTWARE_GDI_H
#define EWL_ENGINE_EVAS_SOFTWARE_GDI_H

#include "ewl_base.h"
#include <Ecore_Win32.h>
#include <Evas.h>
#include <Evas_Engine_Software_Gdi.h>

#define EWL_ENGINE_EVAS_SOFTWARE_GDI(engine) \
                ((Ewl_Engine_Evas_Software_Gdi *)engine)

typedef struct Ewl_Engine_Evas_Software_Gdi Ewl_Engine_Evas_Software_Gdi;
struct Ewl_Engine_Evas_Software_Gdi
{
        Ewl_Engine engine;
};

#endif
