#ifndef EWL_ENGINE_EVAS_XRENDER_X11_H
#define EWL_ENGINE_EVAS_XRENDER_X11_H

#include <Ewl.h>

#define EWL_ENGINE_EVAS_XRENDER_X11(engine) \
		((Ewl_Engine_Evas_Xrender_X11 *)engine)

typedef struct Ewl_Engine_Evas_Xrender_X11 Ewl_Engine_Evas_Xrender_X11;
struct Ewl_Engine_Evas_Xrender_X11
{
	Ewl_Engine engine;
};

#endif

