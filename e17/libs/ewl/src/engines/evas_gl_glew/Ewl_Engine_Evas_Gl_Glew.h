/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ENGINE_EVAS_GL_GLEW_H
#define EWL_ENGINE_EVAS_GL_GLEW_H

#include "ewl_base.h"
#include <Evas.h>
#include <Evas_Engine_GL_Glew.h>
#include <Ecore_Win32.h>

#define EWL_ENGINE_EVAS_GL_GLEW(engine) \
                ((Ewl_Engine_Evas_Gl_Glew *)engine)

typedef struct Ewl_Engine_Evas_Gl_Glew Ewl_Engine_Evas_Gl_Glew;
struct Ewl_Engine_Evas_Gl_Glew
{
        Ewl_Engine engine;
};

#endif

