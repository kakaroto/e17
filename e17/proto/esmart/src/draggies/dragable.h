#ifndef ESMART_DRAGGIES_H
#define ESMART_DRAGGIES_H

#include<Evas.h>
#include<Edje.h>
#include<Ecore_Evas.h>

/**
 * Different dragable types
 */
enum _Esmart_Draggies_Type
{
  DRAGGIES_TYPE_MOVER = 0,
  DRAGGIES_TYPE_RESIZE_L,
  DRAGGIES_TYPE_RESIZE_R,
  DRAGGIES_TYPE_RESIZE_T,
  DRAGGIES_TYPE_RESIZE_B,
  DRAGGIES_TYPE_RESIZE_BR,
  DRAGGIES_TYPE_RESIZE_BL,
  DRAGGIES_TYPE_RESIZE_TR,
  DRAGGIES_TYPE_RESIZE_TL
};
typedef enum _Esmart_Draggies_Type Esmart_Draggies_Type;

/**
 * How to use Esmart Draggies.
 *
 * Assign it to an edje part, that's dragable and unconfined
 */
typedef struct _Esmart_Draggies Esmart_Draggies;

struct _Esmart_Draggies
{
  Ecore_Evas *ee;		/* The ecore_evas that should move */
  Evas_Coord cx, cy, cw, ch;	/* current mouse x/y ecore_evas h/w */
  Esmart_Draggies_Type type;	/* Resizer or Dragger */
  int clicked;			/* Whether the mouse is down now or not */
  int first;			/* Whether the mouse is down now or not */
  int button;			/* the button that handles dragging */
};

Evas_Object *esmart_draggies_new (Ecore_Evas * evas);
void esmart_draggies_free (Evas_Object * o);
void esmart_draggies_type_set (Evas_Object * o, Esmart_Draggies_Type type);
void esmart_draggies_button_set (Evas_Object * o, int button);

#endif
