/**************************************************************************
 * draggies.c : a rectangle to add to your borderless/shaped ecore_evas app
 * that will handle moving or resizing the ecore evas, as if it were a
 * border.
 ***************************************************************************/
#include "dragable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>

static void
_mouse_up_cb (void *data, Evas * evas, Evas_Object * obj, void *ev)
{
  Esmart_Draggies *drag = NULL;
  Evas_Event_Mouse_Up *e = NULL;

  if ((drag = (Esmart_Draggies *) data))
    {
      if ((e = (Evas_Event_Mouse_Up *) ev))
	{
	  if (e->button == drag->button)
	    {
	      drag->clicked = 0;
	      drag->dx = drag->dy = 0;
	    }
	}
    }
}
static void
_mouse_down_cb (void *data, Evas * evas, Evas_Object * obj, void *ev)
{
  Esmart_Draggies *drag = NULL;
  Evas_Event_Mouse_Down *e = NULL;
  Ecore_X_Event_Mouse_Button_Down *evx = NULL;

  if ((drag = (Esmart_Draggies *) data))
    {
      if (ecore_event_current_type_get () == ECORE_X_EVENT_MOUSE_BUTTON_DOWN)
	{
	  if ((evx = ecore_event_current_event_get ()))
	    {
	      if ((e = (Evas_Event_Mouse_Down *) ev))
		{
		  if (e->button == drag->button)
		    {
		      drag->clicked = 1;
		      drag->dx = evx->x;
		      drag->dy = evx->y;
		    }
		}
	    }
	}
    }
}
static void
_mouse_move_cb (void *data, Evas * evas, Evas_Object * obj, void *ev)
{
  Esmart_Draggies *drag = NULL;
  Ecore_X_Event_Mouse_Move *evx = NULL;

  if ((drag = (Esmart_Draggies *) data))
    {
      if (ecore_event_current_type_get () == ECORE_X_EVENT_MOUSE_MOVE)
	{
	  if ((evx = ecore_event_current_event_get ()))
	    {
	      if (drag->clicked)
		{
		  int dx, dy;

		  dx = evx->root.x - drag->dx;
		  dy = evx->root.y - drag->dy;
		  ecore_evas_move (drag->ee, dx, dy);
		}
	    }
	}
    }
}

Evas_Object *
esmart_draggies_new (Ecore_Evas * ee)
{
  Evas_Object *o = NULL;
  Esmart_Draggies *data = NULL;

  if (!ee)
    return (NULL);

  data = malloc (sizeof (Esmart_Draggies));
  memset (data, 0, sizeof (Esmart_Draggies));

  data->ee = ee;

  o = evas_object_rectangle_add (ecore_evas_get (ee));
  evas_object_layer_set (o, 9999);
  evas_object_color_set (o, 255, 255, 255, 255);

  evas_object_data_set (o, "Dragger", data);
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				  _mouse_down_cb, data);
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_UP,
				  _mouse_up_cb, data);
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_MOVE,
				  _mouse_move_cb, data);
  return (o);
}

void
esmart_draggies_free (Evas_Object * o)
{
  Esmart_Draggies *drag = NULL;
  if (o)
    {
      if ((drag = (Esmart_Draggies *) evas_object_data_get (o, "Dragger")))
	{
	  drag->ee = NULL;
	  free (drag);
	}
      evas_object_del (o);
    }
}
void
esmart_draggies_button_set (Evas_Object * o, int button)
{
  Esmart_Draggies *drag = NULL;
  if (o)
    {
      if ((drag = (Esmart_Draggies *) evas_object_data_get (o, "Dragger")))
	{
	  drag->button = button;
	}
    }
}
