#ifndef ESMART_DRAGGIES_H
#define ESMART_DRAGGIES_H

#include<Evas.h>
#include<Ecore.h>
#include<Ecore_Evas.h>


/**
 * How to use Esmart Draggies.
 */
typedef struct _Esmart_Draggies Esmart_Draggies;

struct _Esmart_Draggies
{
  Ecore_Evas *ee;		/* The ecore_evas that should move */
  int dx, dy;			/* offset from 0,0 of the window */
  int clicked;			/* Whether the mouse is down now or not */
  int button;			/* the button that handles dragging */
  Evas_Object *obj;		/* the rectangle that gets events */
};

Evas_Object *esmart_draggies_new (Ecore_Evas * evas);
void esmart_draggies_button_set (Evas_Object * o, int button);

#endif
