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
	    drag->clicked = 0;
	}
    }
}
static void
_mouse_down_cb (void *data, Evas * evas, Evas_Object * obj, void *ev)
{
  Esmart_Draggies *drag = NULL;
  Evas_Event_Mouse_Down *e = NULL;
  if ((drag = (Esmart_Draggies *) data))
    {
      if ((e = (Evas_Event_Mouse_Down *) ev))
	{
	  if (e->button == drag->button)
	    {
	      drag->first = drag->clicked = 1;
	      drag->cx = e->canvas.x;
	      drag->cy = e->canvas.y;
	      switch (drag->type)
		{
		  /*
		     case DRAGGIES_TYPE_RESIZE_L:
		     ecore_evas_gravity_set(drag->ee, 4);
		     break;
		     case DRAGGIES_TYPE_RESIZE_R:
		     ecore_evas_gravity_set(drag->ee, 6);
		     break;
		     case DRAGGIES_TYPE_RESIZE_T:
		     ecore_evas_gravity_set(drag->ee, 2);
		     break;
		     case DRAGGIES_TYPE_RESIZE_B:
		     ecore_evas_gravity_set(drag->ee, 8);
		     break;
		     case DRAGGIES_TYPE_RESIZE_TR:
		     ecore_evas_gravity_set(drag->ee, 3);
		     break;
		     case DRAGGIES_TYPE_RESIZE_TL:
		     ecore_evas_gravity_set(drag->ee, 1);
		     break;
		     case DRAGGIES_TYPE_RESIZE_BL:
		     ecore_evas_gravity_set(drag->ee, 7);
		     break;
		     case DRAGGIES_TYPE_RESIZE_BR:
		     ecore_evas_gravity_set(drag->ee, 9);
		     break;
		     default:
		     ecore_evas_gravity_set(drag->ee, 1);
		     break;
		   */
		}
	    }
	}
    }
}
static void
_mouse_move_cb (void *data, Evas * evas, Evas_Object * obj, void *ev)
{
  int x, y, w, h;
  Esmart_Draggies *drag = NULL;
  Evas_Event_Mouse_Move *e = NULL;

  if ((drag = (Esmart_Draggies *) data))
    {
      if ((e = (Evas_Event_Mouse_Move *) ev))
	{
	  if (drag->clicked)
	    {
	      double dx, dy;
	      Evas_Coord ox, oy, ow, oh;

	      ecore_evas_geometry_get (drag->ee, &x, &y, &w, &h);
	      dx = (drag->cx - e->cur.canvas.x);
	      dy = (drag->cy - e->cur.canvas.y);
	      if ((dx < 1) && (dy < 1))
		return;

	      drag->cx = e->cur.canvas.x;
	      drag->cy = e->cur.canvas.y;
	      switch (drag->type)
		{
		case DRAGGIES_TYPE_RESIZE_L:
		  ecore_evas_move_resize (drag->ee, x + (int) dx, y,
					  w - (int) dx, h);
		  break;
		case DRAGGIES_TYPE_RESIZE_R:
		  ecore_evas_resize (drag->ee, w - (int) dx, h);
		  drag->cx -= dx;
		  break;
		case DRAGGIES_TYPE_RESIZE_T:
		  ecore_evas_move_resize (drag->ee, x, y - (int) dy,
					  w, h + (int) dy);
		  break;
		case DRAGGIES_TYPE_RESIZE_B:
		  ecore_evas_resize (drag->ee, w, h - (int) dy);
		  drag->cy -= dy;
		  break;
		case DRAGGIES_TYPE_RESIZE_TL:
		  ecore_evas_move_resize (drag->ee, x - (int) dx, y -
					  (int) dy, w + (int) dx,
					  h + (int) dy);
		  break;
		case DRAGGIES_TYPE_RESIZE_TR:
		  ecore_evas_move_resize (drag->ee, x, y - (int) dy, w -
					  (int) dx, h + (int) dy);
		  drag->cy += dy;
		  drag->cx += dx;
		  break;
		case DRAGGIES_TYPE_RESIZE_BL:
		  ecore_evas_move_resize (drag->ee, x - (int) dx,
					  y - (int) dy,
					  w - (int) dx, h - (int) dy);
		  break;
		case DRAGGIES_TYPE_RESIZE_BR:
		  /* Gravity */
		  ecore_evas_resize (drag->ee, w - (int) dx, h - (int) dy);
		  drag->cy -= dy;
		  drag->cx -= dx;
		  break;
		  /* default: is Moving */
		default:
		  ecore_evas_move (drag->ee, x - (int) dx, y - (int) dy);
		  if (!drag->first)
		    {
		      drag->cx = e->cur.canvas.x + dx;
		      drag->cy = e->cur.canvas.y + dy;
		    }
		  else
		    {
		      drag->cx = e->cur.canvas.x;
		      drag->cy = e->cur.canvas.y;
		      drag->first = 0;
		    }
		  break;
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
void
esmart_draggies_type_set (Evas_Object * o, Esmart_Draggies_Type type)
{
  Esmart_Draggies *drag = NULL;
  if (o)
    {
      if ((drag = (Esmart_Draggies *) evas_object_data_get (o, "Dragger")))
	{
	  drag->type = type;
	}
    }
}
