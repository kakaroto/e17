#include <Evas.h>
#include <Ecore.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#ifndef _ECLIPSE_H
#define _ECLIPSE_H
#include "eclipse.h"
#endif


Evas_Object * transparency_get_pixmap(Evas *evas, Evas_Object *old,
				      int x, int y, int w, int h);
Evas_Object *transparency_get_pixmap_with_windows(Evas *evas, Evas_Object *old, int x, int y, int w, int h);
