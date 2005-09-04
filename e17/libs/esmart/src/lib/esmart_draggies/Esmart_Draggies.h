#ifndef ESMART_DRAGGIES_H
#define ESMART_DRAGGIES_H

#include <Evas.h>
#include <Ecore_Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

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
void esmart_draggies_event_callback_add (Evas_Object * o,
					 Evas_Callback_Type type,
					 void (*func) (void *data, Evas * e,
						       Evas_Object * o,
						       void *ev),
					 const void *data);

#ifdef __cplusplus
}
#endif
#endif
