#include <Evas.h>
#include <Ecore.h>
#include <math.h>
#include "../../container.h"

static int _entice_current = 0;
static int _entice_scroll_timer(void *data);

void
_entice_layout(Container *cont)
{
  Evas_List *l;
  int i, im_no;
  double pos;
  double ey = cont->y + cont->padding.t + cont->scroll_offset;

  im_no = _entice_current;
  i = 1;
  pos = 0.0;
  evas_object_color_set(cont->clipper, 255, 255, 255, 255);
  for (l=cont->elements; l; l = l->next, i++)
  {
    Container_Element *el;
    double w, h;
    double sc;
    double space;
    int d;

    el = l->data;
    if (!el->obj)
      continue;

    w = cont->w - (cont->padding.l + cont->padding.r);
    h = w * el->orig_h / el->orig_w;

    d = im_no - i;
    if (d < 0)
      d = -d;
    sc = 1.0 / (1.0 + (((double)d) * 0.2));
    if (sc < 0.333333)
      sc = 0.333333;
    w *= sc;
    h *= sc;
    space = 48 * sc;

    evas_object_resize(el->obj, w, h);
    if (!strcmp(evas_object_type_get(el->obj), "image"))
      evas_object_image_fill_set(el->obj, 0, 0, w, h);
      
    evas_object_move(el->obj,
                     cont->x + ((cont->w - w) / 2),
		     ey);

    ey += h + cont->spacing;
  }
  
}

void
_entice_scroll_start(Container *cont, double velocity)
{
  Scroll_Data *data;
  double length, size;

  length = e_container_elements_length_get(cont->obj);
  data = calloc(1, sizeof(Scroll_Data));
  data->velocity = velocity;
  data->start_time = ecore_time_get();
  data->cont = cont;
  data->length = length;
 
  cont->scroll_timer = ecore_timer_add(.02, _entice_scroll_timer, data);
}

void
_entice_scroll_stop(Container *cont)
{
  /* FIXME: decelerate on stop? */
  if (cont->scroll_timer)
  {
    ecore_timer_del(cont->scroll_timer);  
    cont->scroll_timer = NULL;
  }
}

void
_entice_scroll_to(Container *cont, Container_Element *el)
{
    if(cont && el)
    {
	Evas_List *l;
	_entice_current = 1;
	for(l = cont->elements; l; l = l->next, _entice_current++)
	{
	    if(el == l->data) return;
	}
    }
}

void
_entice_shutdown()
{
}

int
plugin_init(Container_Layout_Plugin *p)
{
  p->layout = _entice_layout;
  p->scroll_start = _entice_scroll_start;
  p->scroll_stop = _entice_scroll_stop;
  p->scroll_to = _entice_scroll_to;
  p->shutdown = _entice_shutdown;

  return 1;
}

int
_entice_scroll_timer(void *data)
{
  Scroll_Data *sd = data;
  double dt, dx, size, pad, max_scroll;
 
  dt = ecore_time_get() - sd->start_time;
  dx = 10 * (1 - exp(-dt)); 

  sd->cont->scroll_offset += dx * sd->velocity;
  
  size = sd->cont->direction ? sd->cont->h : sd->cont->w;
  pad = sd->cont->direction ? sd->cont->padding.t + sd->cont->padding.b :
                              sd->cont->padding.l + sd->cont->padding.r;
  max_scroll = size - sd->length - pad;

  if (sd->cont->scroll_offset < max_scroll)
    sd->cont->scroll_offset = max_scroll;
  
  else if (sd->cont->scroll_offset > 0)
    sd->cont->scroll_offset = 0;

  _entice_layout(sd->cont);
  return 1;
}
