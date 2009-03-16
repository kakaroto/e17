/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ENGINE_XCB_H
#define EWL_ENGINE_XCB_H

#include "ewl_base.h"
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include <Ecore_Input.h>

#define EWL_ENGINE_XCB(engine) ((Ewl_Engine_Xcb *)engine)
typedef struct Ewl_Engine_Xcb Ewl_Engine_Xcb;
struct Ewl_Engine_Xcb
{
        Ewl_Engine engine;
};

#endif

