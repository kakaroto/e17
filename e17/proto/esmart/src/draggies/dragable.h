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
  Evas_Coord cx, cy, cw, ch;	/* current mouse x/y ecore_evas h/w */
  int clicked;			/* Whether the mouse is down now or not */
  int first;			/* Whether it's the first move or not */
  int button;			/* the button that handles dragging */
};

Evas_Object *esmart_draggies_new (Ecore_Evas * evas);
void esmart_draggies_free (Evas_Object * o);
void esmart_draggies_button_set (Evas_Object * o, int button);

#endif
