/* 
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2 
 */
/**************************************************************************
 * draggies.c : a rectangle to add to your borderless/shaped ecore_evas app
 * that will handle moving the ecore evas, as if it were a border.
 ***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "Esmart_Draggies.h"


static Evas_Smart *_esmart_dragable_object_smart_get ();
static Evas_Object *esmart_dragable_object_new (Evas * evas);
static void _esmart_dragable_object_add (Evas_Object * o);
static void _esmart_dragable_object_del (Evas_Object * o);
static void _esmart_dragable_object_move (Evas_Object * o, Evas_Coord x,
					  Evas_Coord y);
static void _esmart_dragable_object_resize (Evas_Object * o, Evas_Coord w,
					    Evas_Coord h);
static void _esmart_dragable_object_show (Evas_Object * o);
static void _esmart_dragable_object_hide (Evas_Object * o);
static void _esmart_dragable_object_color_set (Evas_Object * o, int r, int g,
					       int b, int a);
static void _esmart_dragable_object_clip_set (Evas_Object * o,
					      Evas_Object * clip);
static void _esmart_dragable_object_clip_unset (Evas_Object * o);

/*==========================================================================
 * The actual code ethat handles the moving of the window
 *========================================================================*/
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

/*==========================================================================
 * The two external functions, new and button set
 *========================================================================*/
EAPI Evas_Object *
esmart_draggies_new (Ecore_Evas * ee)
{
  Evas_Object *o = NULL;
  Evas_Object *result = NULL;
  Esmart_Draggies *data = NULL;

  if (!ee)
    return (NULL);
  if ((result = esmart_dragable_object_new (ecore_evas_get (ee))))
    {
       if ((data = evas_object_smart_data_get (result)))
	 {
	    data->ee = ee;
	    o = evas_object_rectangle_add (ecore_evas_get (ee));
	    evas_object_color_set (o, 255, 255, 255, 0);
	    evas_object_repeat_events_set (o, 1);
	    evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
		  _mouse_down_cb, data);
	    evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_UP,
		  _mouse_up_cb, data);
	    evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_MOVE,
		  _mouse_move_cb, data);
	    data->obj = o;

	    evas_object_smart_member_add(o, result);
	 }
    }
  return (result);
}

EAPI void
esmart_draggies_button_set (Evas_Object * o, int button)
{
  Esmart_Draggies *data = NULL;
  if ((data = evas_object_smart_data_get (o)))
    {
      data->button = button;
    }
}

EAPI void
esmart_draggies_event_callback_add (Evas_Object * o, Evas_Callback_Type type,
				    void (*func) (void *data, Evas * e,
						  Evas_Object * obj,
						  void *ev),
				    const void *user_data)
{
  Esmart_Draggies *data = NULL;
  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_event_callback_add (data->obj, type, func, user_data);
    }
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/

static Evas_Object *
esmart_dragable_object_new (Evas * evas)
{
  Evas_Object *esmart_dragable_object;

  esmart_dragable_object =
    evas_object_smart_add (evas, _esmart_dragable_object_smart_get ());

  return esmart_dragable_object;
}

static Evas_Smart *
_esmart_dragable_object_smart_get ()
{
  static Evas_Smart *smart = NULL;

  if (!smart)
    { 
       static const Evas_Smart_Class sc = 
	 {
	    "esmart_dragable_object",
	    EVAS_SMART_CLASS_VERSION,
	    _esmart_dragable_object_add,
	    _esmart_dragable_object_del,
	    _esmart_dragable_object_move,
	    _esmart_dragable_object_resize,
	    _esmart_dragable_object_show,
	    _esmart_dragable_object_hide,
	    _esmart_dragable_object_color_set,
	    _esmart_dragable_object_clip_set,
	    _esmart_dragable_object_clip_unset, 
	    NULL,
	    NULL,
	    NULL
	 };

       smart = evas_smart_class_new(&sc);
    }

  return smart;
}

static void
_esmart_dragable_object_add (Evas_Object * o)
{
  Esmart_Draggies *data = NULL;

  data = malloc (sizeof (Esmart_Draggies));
  memset (data, 0, sizeof (Esmart_Draggies));
  evas_object_smart_data_set (o, data);
}

static void
_esmart_dragable_object_del (Evas_Object * o)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      data->ee = NULL;
      if (data->obj)
	evas_object_del (data->obj);
      free (data);
    }
}

static void
_esmart_dragable_object_move (Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_move (data->obj, x, y);
    }
}

static void
_esmart_dragable_object_resize (Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_resize (data->obj, w, h);
    }
}

static void
_esmart_dragable_object_show (Evas_Object * o)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_show (data->obj);
    }
}

static void
_esmart_dragable_object_hide (Evas_Object * o)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_hide (data->obj);
    }
}

static void
_esmart_dragable_object_color_set (Evas_Object * o, int r, int g, int b,
				   int a)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_color_set (data->obj, r, g, b, a);
    }
}

static void
_esmart_dragable_object_clip_set (Evas_Object * o, Evas_Object * clip)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_clip_set (data->obj, clip);
    }
}

static void
_esmart_dragable_object_clip_unset (Evas_Object * o)
{
  Esmart_Draggies *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_clip_unset (data->obj);
    }
}
