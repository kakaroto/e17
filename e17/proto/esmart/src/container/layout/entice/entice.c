#include <Evas.h>
#include <Ecore.h>
#include <math.h>
#include "../../Esmart_Container.h"

static int _entice_current = 0;
static int _entice_scroll_timer (void *data);

void
_entice_layout (Container * cont)
{
  Container_Element *el;
  Evas_List *center_el = NULL;
  Evas_List *l;
  double sc;
  double space;
  int d;
  int i = 0, im_no = 0, list_length;
  Evas_Coord center;
  double w, h;
  double ey = 0.0, ex = 0.0;

  if (!(center_el = evas_list_nth_list (cont->elements, _entice_current)))
    return;

  if (cont->direction)
    {
      center = cont->y + (cont->h / 2.0) + cont->padding.t;
      el = (Container_Element *) center_el->data;
      w = cont->w - (cont->padding.l + cont->padding.r);
      h = w * el->orig_h / el->orig_w;
      evas_object_resize (el->obj, w, h);
      evas_object_show (el->obj);
      evas_object_move (el->obj, cont->x + ((cont->w - w) / 2),
			center - (h / 2));


      i = im_no = _entice_current - 1;
      ey = center - (h / 2);
      for (l = center_el->prev; l && (i >= 0); l = l->prev, i--)
	{

	  el = l->data;
	  if (!el->obj)
	    continue;

	  w = cont->w - (cont->padding.l + cont->padding.r);
	  h = w * el->orig_h / el->orig_w;

	  d = im_no - i;
	  if (d < 0)
	    d = -d;
	  sc = 1.0 / (1.0 + (((double) d) * 0.2));
	  if (sc < 0.333333)
	    sc = 0.333333;
	  space = h * sc;
	  w *= sc;
	  h *= sc;

	  evas_object_resize (el->obj, w, h);
	  if (!strcmp (evas_object_type_get (el->obj), "image"))
	    evas_object_image_fill_set (el->obj, 0, 0, w, h);

	  ey -= cont->spacing + space;
	  evas_object_move (el->obj, cont->x + ((cont->w - w) / 2), ey);
	  evas_object_show (el->obj);

	  if (ey < cont->y)
	    {
	      for (l = l->prev; l; l = l->prev)
		{
		  el = (Container_Element *) l->data;
		  evas_object_hide (el->obj);
		}
	      break;
	    }
	}
      i = im_no = _entice_current + 1;
      ey = center + 24;
      list_length = evas_list_count (cont->elements);
      for (l = center_el->next; l && (i <= list_length); l = l->next, i++)
	{

	  el = l->data;
	  if (!el->obj)
	    continue;

	  w = cont->w - (cont->padding.l + cont->padding.r);
	  h = w * el->orig_h / el->orig_w;

	  d = i - im_no;
	  sc = 1.0 / (1.0 + (((double) d) * 0.2));
	  if (sc < 0.333333)
	    sc = 0.333333;
	  space = h * sc;
	  w *= sc;
	  h *= sc;

	  evas_object_resize (el->obj, w, h);
	  if (!strcmp (evas_object_type_get (el->obj), "image"))
	    evas_object_image_fill_set (el->obj, 0, 0, w, h);

	  evas_object_move (el->obj, cont->x + ((cont->w - w) / 2), ey);
	  evas_object_show (el->obj);
	  ey += cont->spacing + space;

	  if (ey > cont->y + cont->h)
	    {
	      for (l = l->next; l; l = l->next)
		{
		  el = (Container_Element *) l->data;
		  evas_object_hide (el->obj);
		}
	      break;
	    }
	}
    }
  else
    {
      center = cont->x + (cont->w / 2.0) + cont->padding.l;
      el = (Container_Element *) center_el->data;
      h = cont->h - (cont->padding.t + cont->padding.b);
      w = h * el->orig_w / el->orig_h;
      evas_object_resize (el->obj, w, h);
      evas_object_move (el->obj, center - (h / 2),
			cont->y + ((cont->h - h) / 2));
      evas_object_show (el->obj);


      i = im_no = _entice_current - 1;
      ex = center - (w / 2);
      for (l = center_el->prev; l && (i >= 0); l = l->prev, i--)
	{

	  el = l->data;
	  if (!el->obj)
	    continue;

	  h = cont->h - (cont->padding.t + cont->padding.b);
	  w = h * el->orig_w / el->orig_h;

	  d = im_no - i;
	  if (d < 0)
	    d = -d;
	  sc = 1.0 / (1.0 + (((double) d) * 0.2));
	  if (sc < 0.333333)
	    sc = 0.333333;
	  space = w * sc;
	  w *= sc;
	  h *= sc;

	  evas_object_resize (el->obj, w, h);
	  if (!strcmp (evas_object_type_get (el->obj), "image"))
	    evas_object_image_fill_set (el->obj, 0, 0, w, h);

	  ex -= cont->spacing + space;
	  evas_object_move (el->obj, ex, cont->y + ((cont->h - h) / 2));
	  evas_object_show (el->obj);

	  if (ex < cont->x)
	    {
	      for (l = l->prev; l; l = l->prev)
		{
		  el = (Container_Element *) l->data;
		  evas_object_hide (el->obj);
		}
	      break;
	    }
	}
      i = im_no = _entice_current + 1;
      center = cont->x + (cont->w / 2.0) + cont->padding.l;
      el = (Container_Element *) center_el->data;
      h = cont->h - (cont->padding.t + cont->padding.b);
      w = h * el->orig_w / el->orig_h;
      ex = center + (w / 2);
      list_length = evas_list_count (cont->elements);
      for (l = center_el->next; l && (i <= list_length); l = l->next, i++)
	{

	  el = l->data;
	  if (!el->obj)
	    continue;

	  h = cont->h - (cont->padding.t + cont->padding.b);
	  w = h * el->orig_w / el->orig_h;

	  d = i - im_no;
	  sc = 1.0 / (1.0 + (((double) d) * 0.2));
	  if (sc < 0.333333)
	    sc = 0.333333;
	  space = w * sc;
	  w *= sc;
	  h *= sc;

	  evas_object_resize (el->obj, w, h);
	  if (!strcmp (evas_object_type_get (el->obj), "image"))
	    evas_object_image_fill_set (el->obj, 0, 0, w, h);

	  evas_object_move (el->obj, ex, cont->y + ((cont->h - h) / 2));
	  evas_object_show (el->obj);
	  ex += cont->spacing + space;

	  if (ex > cont->x + cont->w)
	    {
	      for (l = l->next; l; l = l->next)
		{
		  el = (Container_Element *) l->data;
		  evas_object_hide (el->obj);
		}
	      break;
	    }
	}
    }
}

void
_entice_scroll_start (Container * cont, double velocity)
{
  Scroll_Data *data;
  double length;

  length = esmart_container_elements_length_get (cont->obj);
  data = calloc (1, sizeof (Scroll_Data));
  data->velocity = velocity;
  data->start_time = ecore_time_get ();
  data->cont = cont;
  data->length = length;

  cont->scroll_timer = ecore_timer_add (.02, _entice_scroll_timer, data);
}

void
_entice_scroll_stop (Container * cont)
{
  /* FIXME: decelerate on stop? */
  if (cont->scroll_timer)
    {
      ecore_timer_del (cont->scroll_timer);
      cont->scroll_timer = NULL;
    }
}

void
_entice_scroll_to (Container * cont, Container_Element * el)
{
  if (cont && el)
    {
      int i = 0;
      Evas_List *l;
      for (l = cont->elements; l; l = l->next, i++)
	{
	  if (el == l->data)
	    {
	      _entice_current = i;
	      _entice_layout (cont);
	      return;
	    }
	}
    }
}

void
_entice_shutdown ()
{
}

int
plugin_init (Container_Layout_Plugin * p)
{
  p->layout = _entice_layout;
  p->scroll_start = _entice_scroll_start;
  p->scroll_stop = _entice_scroll_stop;
  p->scroll_to = _entice_scroll_to;
  p->shutdown = _entice_shutdown;

  return 1;
}

int
_entice_scroll_timer (void *data)
{
  Scroll_Data *sd = data;
  double dt, dx, size, pad, max_scroll;

  dt = ecore_time_get () - sd->start_time;
  dx = 10 * (1 - exp (-dt));

  sd->cont->scroll_offset += dx * sd->velocity;

  size = sd->cont->direction ? sd->cont->h : sd->cont->w;
  pad = sd->cont->direction ? sd->cont->padding.t + sd->cont->padding.b :
    sd->cont->padding.l + sd->cont->padding.r;
  max_scroll = 48;
  // size - sd->length - pad;

  if (sd->cont->scroll_offset > max_scroll)
    {
      sd->cont->scroll_offset = 0;
      _entice_current--;
    }

  else if (sd->cont->scroll_offset < -max_scroll)
    {
      sd->cont->scroll_offset = 0;
      _entice_current++;
    }

  _entice_layout (sd->cont);
  return 1;
}
