#ifndef ICONBAR_BG_H
#define ICONBAR_BG_H

#include <Evas.h>
#include <Ecore.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

Evas_Object * transparency_get_pixmap(Evas *evas, Evas_Object *old,
                        int x, int y, int w, int h);
#endif
