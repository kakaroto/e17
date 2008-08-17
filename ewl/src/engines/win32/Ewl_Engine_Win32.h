/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef __EWL_ENGINE_WIN32_H__
#define __EWL_ENGINE_WIN32_H__

#include "ewl_base.h"
#include <Ecore_Win32.h>

#define EWL_ENGINE_WIN32(engine) ((Ewl_Engine_Win32 *)engine)
typedef struct Ewl_Engine_Win32 Ewl_Engine_Win32;
struct Ewl_Engine_Win32
{
        Ewl_Engine engine;
};

#endif /* __EWL_ENGINE_X11_H__ */

