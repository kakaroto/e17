#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <Evas.h>
#include <Ecore.h>

#include "../../Esmart_Container.h"
#include "../../esmart_container_private.h"

#define CONTAINER_ZOOMING 0
#define CONTAINER_ZOOMED 1
#define CONTAINER_UNZOOMING 2
#define CONTAINER_UNZOOMED 3

static double zoom = 0.0;
static double zoom_factor = 2.0;
static double zoom_duration = 0.2;
static Evas_Object *grabber = NULL;
static Ecore_Timer *zoom_timer = NULL;
static int zoom_state = CONTAINER_UNZOOMED;

static int _container_scroll_timer (void *data);
static void engage_zoom_in (Container * cont);
static void engage_zoom_out (Container * cont);
static int engage_zoom_in_slave (void *data);
static int engage_zoom_out_slave (void *data);

static void
_engage_mouse_grabber_mouse_move_cb (void *data, Evas * evas,
				     Evas_Object * obj, void *ev)
{
  fprintf (stderr, "Mouse move area\n");
}

static void
_engage_mouse_grabber_mouse_in_cb (void *data, Evas * evas, Evas_Object * obj,
				   void *ev)
{
  fprintf (stderr, "Mouse in area\n");
  if (data)
    engage_zoom_in ((Container *) data);
}

static void
_engage_mouse_grabber_mouse_out_cb (void *data, Evas * evas,
				    Evas_Object * obj, void *ev)
{
  fprintf (stderr, "Mouse out area\n");
  if (data)
    engage_zoom_out ((Container *) data);
}

static void
_engage_grabber_init (Container * cont)
{
  Evas_Object *obj = NULL;
  obj = evas_object_rectangle_add (cont->evas);
  evas_object_color_set (obj, 255, 255, 255, 0);
  evas_object_layer_set (obj, 9999);
  evas_object_repeat_events_set (obj, 1);
  evas_object_show (obj);

  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_IN,
				  _engage_mouse_grabber_mouse_in_cb, cont);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_OUT,
				  _engage_mouse_grabber_mouse_out_cb, cont);
  evas_object_event_callback_add (obj, EVAS_CALLBACK_MOUSE_MOVE,
				  _engage_mouse_grabber_mouse_move_cb, cont);
  grabber = obj;
}

static void
_engage_layout (Container * cont)
{
  Eina_List *l;
  double ax, ay, aw, ah;	// element area geom
  double ix, iy, iw, ih;	// new x, y, w, h
  double L;			// length of all objects at original size (for nonhomog)
  int num;			// number of elements
  double error = 0;
  int r, g, b;

  /* FIXME: add a 'changed' flag to prevent excessive recalcs */
  if (!grabber)
    _engage_grabber_init (cont);

  evas_object_move (grabber, cont->x, cont->y);
  evas_object_resize (grabber, cont->w, cont->h);
//  evas_object_geometry_get(cont->grabber, &ax, &ay, &aw, &ah);
  ax = cont->x;
  ay = cont->y;
  aw = cont->w;
  ah = cont->h;

  /* adjust for padding */
  ax += cont->padding.l;
  ay += cont->padding.t;
  aw -= cont->padding.l + cont->padding.r;
  ah -= cont->padding.t + cont->padding.b;

  if (aw == 0 || ah == 0)
    return;

  ix = ax;
  iy = ay;

  if (cont->direction)
    iy += cont->scroll.offset;
  else
    ix += cont->scroll.offset;

  L = esmart_container_elements_orig_length_get (cont->obj);
  num = eina_list_count (cont->elements);


  evas_object_color_get (cont->clipper, &r, &g, &b, NULL);

  for (l = cont->elements; l; l = l->next)
    {
      Container_Element *el = l->data;
      Evas_Coord ew, eh;	// old element size

      if (!el)
	{
	  continue;
	}
      evas_object_geometry_get (el->obj, NULL, NULL, &ew, &eh);
      if (ew == 0)
	ew = el->orig_w;
      if (eh == 0)
	eh = el->orig_h;

      evas_object_resize (el->grabber, ew, eh);

      /* vertical */
      if (cont->direction)
	{
	  if (cont->fill & CONTAINER_FILL_POLICY_FILL)
	    {
	      iw = aw;

	      if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
		ih = (ah - cont->spacing * (num - 1)) / num;
	      else
		ih = el->orig_h * (ah - cont->spacing * (num - 1)) / L;

	    }
	  else if (cont->fill & CONTAINER_FILL_POLICY_FILL_X)
	    {
	      if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
		{
		  iw = aw;
		  ih = eh * iw / ew;
		}
	      else
		{
		  iw = aw;
		  ih = eh;
		}
	    }
	  else if (cont->fill & CONTAINER_FILL_POLICY_FILL_Y)
	    {
	      if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
		ih = (ah - cont->spacing * (num - 1)) / num;
	      else
		ih = el->orig_h * (ah - cont->spacing * (num - 1)) / L;

	      if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
		iw = ew * ih / eh;
	      else
		iw = ew;
	    }
	  else
	    {
	      iw = ew;
	      ih = eh;
	    }

	  if (cont->align == CONTAINER_ALIGN_LEFT)
	    ix = ax;
	  else if (cont->align == CONTAINER_ALIGN_CENTER)
	    ix = ax + (aw - iw) / 2;
	  else if (cont->align == CONTAINER_ALIGN_RIGHT)
	    ix = ax + aw - iw;

	  evas_object_move (el->obj, ix, iy);
	  evas_object_resize (el->obj, iw, ih);
	  if (!strcmp (evas_object_type_get (el->obj), "image"))
	    {
	      evas_object_image_fill_set (el->obj, 0, 0, iw, ih);
	    }
	  evas_object_move (el->grabber, ix, iy);
	  evas_object_resize (el->grabber, iw, ih);

	  iy += ih + cont->spacing;
	}

      /* horizontal */
      else
	{
	  if (cont->fill & CONTAINER_FILL_POLICY_FILL)
	    {
	      //printf("fill\n");
	      ih = ah;

	      if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
		iw = (aw - cont->spacing * (num - 1)) / num;
	      else
		{
		  //printf("nonhomog\n");
		  iw = el->orig_w * (aw - cont->spacing * (num - 1)) / L;
		}
	    }
	  else if (cont->fill & CONTAINER_FILL_POLICY_FILL_X)
	    {
	      //printf("fill x\n");

	      if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
		{
		  //printf ("homog\n");
		  iw = (aw - cont->spacing * (num - 1)) / num;
		}
	      else
		{
		  //printf("nonhomog - L: %f, ew: %f\n", L, ew);
		  iw = el->orig_w * (aw - cont->spacing * (num - 1)) / L;
		}

	      if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
		ih = eh * iw / ew;
	      else
		ih = eh;
	    }
	  else if (cont->fill & CONTAINER_FILL_POLICY_FILL_Y)
	    {
	      if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
		{
		  ih = ah;
		  iw = ew * ih / eh;
		}
	      else
		{
		  ih = ah;
		  iw = ew;
		}
	    }
	  else
	    {
	      iw = ew;
	      ih = eh;
	    }

	  if (cont->align == CONTAINER_ALIGN_TOP)
	    iy = ay;
	  else if (cont->align == CONTAINER_ALIGN_CENTER)
	    iy = ay + (ah - ih) / 2;
	  else if (cont->align == CONTAINER_ALIGN_BOTTOM)
	    iy = ay + ah - ih;

	  /* make sure that the elements fill the container exactly */
	  if (error >= 1)
	    {
	      iw++;
	      error -= 1;
	    }
	  else if (error <= -1)
	    {
	      iw--;
	      error += 1;
	    }

	  error += iw - (int) iw;

	  iw = (int) iw;

	  evas_object_move (el->obj, ix, iy);
	  evas_object_resize (el->obj, iw, ih);
	  if (!strcmp (evas_object_type_get (el->obj), "image"))
	    {
	      evas_object_image_fill_set (el->obj, 0, 0, iw, ih);
	    }
	  evas_object_move (el->grabber, ix, iy);
	  evas_object_resize (el->grabber, iw, ih);
	  ix += iw + cont->spacing;
	}


      evas_object_geometry_get (el->obj, NULL, NULL, &ew, &eh);
      evas_object_resize (el->grabber, ew, eh);
    }
}

static void
_engage_scroll_start (Container * cont, double velocity)
{
  double length, size;

  length = esmart_container_elements_length_get (cont->obj);
  size = cont->direction ? cont->h : cont->w;

  /* don't scroll unless the elements exceed the size of the container */
  if (length <= size)
    {
      printf (" length smaller than size\n");
      return;
    }
  cont->scroll.velocity = velocity;
  cont->scroll.start_time = ecore_time_get ();

  cont->scroll.timer = ecore_timer_add (.02, _container_scroll_timer, cont);
}

static void
_engage_scroll_stop (Container * cont)
{
  /* FIXME: decelerate on stop? */
  if (cont->scroll.timer)
    {
      ecore_timer_del (cont->scroll.timer);
      cont->scroll.timer = NULL;
    }
}

static void
_engage_scroll_to (Container * cont, Container_Element * el)
{
  return;
}

static void
_engage_shutdown ()
{
  return;
}


/*** internal plugin functions ***/
static int
_container_scroll_timer (void *data)
{
  Container *cont = data;
  double dt, dx, size, pad, max_scroll;

  dt = ecore_time_get () - cont->scroll.start_time;
  dx = 10 * (1 - exp (-dt));

  cont->scroll.offset += dx * cont->scroll.velocity;

  size = cont->direction ? cont->h : cont->w;
  pad = cont->direction ? cont->padding.t + cont->padding.b :
    cont->padding.l + cont->padding.r;
  max_scroll = size - cont->length - pad;

  if (cont->scroll.offset < max_scroll)
    cont->scroll.offset = max_scroll;

  else if (cont->scroll.offset > 0)
    cont->scroll.offset = 0;

  _engage_layout (cont);
  return 1;
}

EAPI int
plugin_init (Container_Layout_Plugin * p)
{
  p->layout = _engage_layout;
  p->scroll_start = _engage_scroll_start;
  p->scroll_stop = _engage_scroll_stop;
  p->scroll_to = _engage_scroll_to;
  p->shutdown = _engage_shutdown;

  return 1;
}
static void
engage_zoom_in (Container * cont)
{
  if (zoom_timer)
    ecore_timer_del (zoom_timer);
  engage_zoom_in_slave (cont);
  zoom_timer = ecore_timer_add (0.02, engage_zoom_in_slave, cont);
  _engage_layout (cont);
}

static void
engage_zoom_out (Container * cont)
{
  if (zoom_timer)
    ecore_timer_del (zoom_timer);
  engage_zoom_out_slave (cont);
  zoom_timer = ecore_timer_add (0.02, engage_zoom_out_slave, cont);
  _engage_layout (cont);
}
static int
engage_zoom_in_slave (void *data)
{
  static double start_time;
  if (zoom_state == CONTAINER_UNZOOMED)
    start_time = ecore_time_get ();
  else if (zoom_state == CONTAINER_UNZOOMING)
    start_time = ecore_time_get () -
      (zoom - 1.0) / (zoom_factor - 1.0) * zoom_duration;
  zoom =
    (zoom_factor - 1.0) * ((ecore_time_get () - start_time) /
			   zoom_duration) + 1.0;
  fprintf (stderr, "zooming in %0.2f\n", zoom);
  _engage_layout ((Container *) data);
  if (zoom < zoom_factor)
    {
      zoom_state = CONTAINER_ZOOMING;
      return (1);
    }
  else
    {
      zoom = zoom_factor;
      zoom_state = CONTAINER_ZOOMED;
      zoom_timer = NULL;
    }
  return (0);
}

static int
engage_zoom_out_slave (void *data)
{
  static double start_time;

  if (zoom_state == CONTAINER_ZOOMED)
    start_time = ecore_time_get ();
  else if (zoom_state == CONTAINER_ZOOMING)
    start_time = ecore_time_get () -
      (zoom - 1.0) / (zoom_factor - 1.0) * zoom_duration;
  zoom =
    (zoom_factor - 1.0) * (1.0 - (ecore_time_get () - start_time) /
			   zoom_duration) + 1.0;
  fprintf (stderr, "zooming out %0.2f\n", zoom);
  _engage_layout ((Container *) data);
  if (zoom > 1.0)
    {
      zoom_state = CONTAINER_UNZOOMING;
      return (1);
    }
  else
    {
      zoom = 1.0;
      zoom_state = CONTAINER_UNZOOMED;
      zoom_timer = NULL;
    }
  return (0);
}
