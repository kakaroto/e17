/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_ENGINE_EVAS_BUFFER_H
#define EWL_ENGINE_EVAS_BUFFER_H

#include "ewl_base.h"
#include <Evas.h>
#include <Evas_Engine_Buffer.h>

#define EWL_ENGINE_EVAS_BUFFER(engine) \
                ((Ewl_Engine_Evas_Buffer *)engine)

typedef struct Ewl_Engine_Evas_Buffer Ewl_Engine_Evas_Buffer;
struct Ewl_Engine_Evas_Buffer
{
        Ewl_Engine engine;
};

#endif

