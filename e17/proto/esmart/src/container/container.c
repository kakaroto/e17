/**************************************************************************
 * An evas smart container object 
 * 
 *  
 * 
 *
 ***************************************************************************/

#include <Evas.h>
#include <math.h>
#include "Esmart_Container.h"
#include "container_private.h"

int _container_scroll_timer(void *data);

/*** external API ***/

void esmart_container_sort(Evas_Object *container, int (*func)(void*,void*))
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;
    
  cont->elements = evas_list_sort(cont->elements, evas_list_count(cont->elements),
				func);
  _container_elements_fix(cont);
}
void esmart_container_direction_set(Evas_Object *container, int direction)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  if (cont->direction == direction) return;

  cont->direction = direction;
  _container_elements_fix(cont);
}

int  esmart_container_direction_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->direction;
}

void esmart_container_padding_set(Evas_Object *container, double l, double r,
                             double t, double b)
{
  Container *cont;
  
  cont = _container_fetch(container);

  if (cont->padding.l == l &&
      cont->padding.r == r &&
      cont->padding.t == t &&
      cont->padding.b == b)
    return;


  cont->padding.l = l;
  cont->padding.r = r;
  cont->padding.t = t;
  cont->padding.b = b;

  _container_elements_fix(cont);
}

void esmart_container_padding_get(Evas_Object *container, double *l, double *r,
                             double *t, double *b)
{
  Container *cont;
  
  cont = _container_fetch(container);

  if (l) *l = cont->padding.l;
  if (r) *r = cont->padding.r;
  if (t) *t = cont->padding.t;
  if (b) *b = cont->padding.b;
}

void esmart_container_scroll(Evas_Object *container, int val)
{
  Container *cont;
  double size, length, pad, max_scroll;
  
  if(!(cont = _container_fetch(container)))
    return;

  length = esmart_container_elements_length_get(container);
  size = cont->direction ? cont->h : cont->w;

  /* don't scroll unless the elements exceed the size of the container */
  if (length <= size)
    return;

  pad = cont->direction ? cont->padding.t + cont->padding.b :
                              cont->padding.l + cont->padding.r;
  max_scroll = size - length - pad;

  cont->scroll_offset += val;
  
  /* don't scroll beyond the top/bottom */
  if (cont->scroll_offset < max_scroll)
    cont->scroll_offset = max_scroll;
  else if (cont->scroll_offset > 0)
    cont->scroll_offset = 0;
  
  _container_elements_changed(cont);
  _container_elements_fix(cont);
}

void esmart_container_scroll_offset_set(Evas_Object *container, int scroll_offset)
{
  Container *cont;
  
  cont = _container_fetch(container);

  if (cont->scroll_offset == scroll_offset) return;
  cont->scroll_offset = scroll_offset;

  _container_elements_changed(cont);
  _container_elements_fix(cont);
}

int esmart_container_scroll_offset_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);

  return cont->scroll_offset;
}


void esmart_container_alignment_set(Evas_Object *container, 
                               Container_Alignment align)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  if (cont->align == align) return;
  
  cont->align = align;

  _container_elements_fix(cont);
  
}

Container_Alignment esmart_container_alignment_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->align;
}

void esmart_container_fill_policy_set(Evas_Object *container, 
                               Container_Fill_Policy fill)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  if (cont->fill == fill) return;
  
  cont->fill = fill;

  _container_elements_changed(cont);
  _container_elements_fix(cont);
  
}

Container_Fill_Policy esmart_container_fill_policy_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->fill;
}

void esmart_container_spacing_set(Evas_Object *container, int spacing)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->spacing = spacing;

  _container_elements_changed(cont);
  _container_elements_fix(cont);
}

int esmart_container_spacing_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->spacing;
}

void esmart_container_move_button_set(Evas_Object *container, int move_button)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  if (cont->move_button == move_button) return;

  cont->move_button = move_button;
}

int  esmart_container_move_button_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->move_button;
}

void esmart_container_callback_order_change_set(Evas_Object *container, void (*func)(void *data), void *data)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->cb_order_change = func;
  cont->data_order_change = data;
}

double
esmart_container_elements_length_get(Evas_Object *container)
{
  Container *cont;
  Evas_List *l;
  double length = 0;

  cont = _container_fetch(container);
  if (!cont) return 0;

  //_container_elements_fix(cont);
    
  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;
    double w, h;

    evas_object_geometry_get(el->obj, NULL, NULL, &w, &h);

    length += cont->direction ? h : w;
    length += cont->spacing; 
  }

  /* subtract off extra spacing from last element */
  length -= cont->spacing;

  return length;
}

void
esmart_container_scroll_start(Evas_Object *container, double velocity)
{
  Container *cont = _container_fetch(container);
  if (!cont) return;
  
  if (cont->plugin && cont->plugin->scroll_start)
    cont->plugin->scroll_start(cont, velocity);
}

void
esmart_container_scroll_stop(Evas_Object *container)
{
  Container *cont;

  cont = _container_fetch(container);
  if (!cont) return;

  if (cont->plugin && cont->plugin->scroll_stop)
    cont->plugin->scroll_stop(cont);
}

void
esmart_container_scroll_to(Evas_Object *container, Evas_Object *element)
{
  Container *cont;
  Container_Element *el;
  
  cont = _container_fetch(container);
  if (!cont) return;

  el = evas_object_data_get(element, "Container_Element");

  if (cont->plugin && cont->plugin->scroll_to)
    cont->plugin->scroll_to(cont, el);
}

double
esmart_container_elements_orig_length_get(Evas_Object *container)
{
  Container *cont;
  Evas_List *l;
  double length = 0;

  cont = _container_fetch(container);
  if (!cont) return 0.0;

  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;
    double w, h;

    evas_object_geometry_get(el->obj, NULL, NULL, &w, &h);

    length += cont->direction ? el->orig_h : el->orig_w;
  }

  return length;
}


/**************** internal  functions *******************/

Container_Element *
_container_element_new(Container *cont, Evas_Object *obj)
{
  Container_Element *el;
  double w, h;

  if (!obj) return NULL;

  el = calloc(1, sizeof(Container_Element));

  el->obj = obj;
  evas_object_data_set(obj, "Container_Element", el); 
  evas_object_show(obj);
 
  evas_object_geometry_get(obj, NULL, NULL, &w, &h);
  el->orig_w = w;
  el->orig_h = h;

  el->grabber = evas_object_rectangle_add(evas_object_evas_get(obj));
  evas_object_repeat_events_set(el->grabber, 1);
  evas_object_color_set(el->grabber, 0, 0, 0, 0);
  evas_object_show(el->grabber);
  
  el->container = cont;
  evas_object_clip_set(el->obj, cont->clipper);
  evas_object_clip_set(el->grabber, cont->clipper);

  evas_object_layer_set(el->obj, evas_object_layer_get(cont->obj));
  evas_object_layer_set(el->grabber, evas_object_layer_get(cont->obj));

  evas_object_event_callback_add(el->grabber, EVAS_CALLBACK_MOUSE_DOWN, _cb_element_down, el);
  evas_object_event_callback_add(el->grabber, EVAS_CALLBACK_MOUSE_UP, _cb_element_up, el);
  evas_object_event_callback_add(el->grabber, EVAS_CALLBACK_MOUSE_MOVE, _cb_element_move, el);

  return el;
}

void
_container_elements_changed(Container *cont)
{
  int r, g, b;
  evas_object_color_get(cont->clipper, &r, &g, &b, NULL);
  if(evas_list_count(cont->elements) > 0)
      evas_object_color_set(cont->clipper, r, g, b, cont->clipper_orig_alpha);
  else
      evas_object_color_set(cont->clipper, r, g, b, 0);
  if (cont->plugin && cont->plugin->changed)
    cont->plugin->changed(cont);
}
void
_container_elements_fix(Container *cont)
{
  if (cont->plugin && cont->plugin->layout)
    cont->plugin->layout(cont);
}



void
_container_element_move(Container_Element *el)
{
  Container *cont = el->container;
  Container_Element *el2;
  Evas_List *l;
  Evas_List *prev;
  double x, y, w, h;
  double sx, sy, sw, sh, fx, fy, fw, fh;

  int not_found = 0;

  prev = evas_list_prev(evas_list_find_list(cont->elements, el));
  cont->elements = evas_list_remove(cont->elements, el);

  /* get start and finish icon geometry */
  el2 = evas_list_data(cont->elements);
  evas_object_geometry_get(el2->obj, &sx, &sy, &sw, &sh);
  el2 = evas_list_data(evas_list_last(cont->elements));
  evas_object_geometry_get(el2->obj, &fx, &fy, &fw, &fh);

#if 1 
  for (l = cont->elements; l; l = evas_list_next(l))
  {
    not_found = 0;
    el2 = evas_list_data(l);

    evas_object_geometry_get(el2->obj, &x, &y, &w, &h);

    /* horizontal */
    if (cont->direction == 0)
    {
      if (el->current.x < sx)
      {
        cont->elements = evas_list_prepend(cont->elements, el);
        break;
      }
      else if (el->current.x >= x + w/2 && el->current.x <= x + w)
      {
        cont->elements = evas_list_append_relative(cont->elements, el, el2);
        break;
      }
      else if (el->current.x >= x && el->current.x <= x + w/2)
      {
        cont->elements = evas_list_prepend_relative(cont->elements, el, el2);
        break;
      }
      else if (el->current.x > fx + fw)
      {
        cont->elements = evas_list_append(cont->elements, el);
        break;
      }
      else
        not_found = 1;
    }

    /* vertical */
    else
    {
      if (el->current.y < sy)
      {
        cont->elements = evas_list_prepend(cont->elements, el);
        break;
      }
      else if (el->current.y >= y + h/2 && el->current.y <= y + h)
      {
        cont->elements = evas_list_append_relative(cont->elements, el, el2);
        break;
      }
      else if ((el->current.y >= y) && (el->current.y <= y + h/2))
      {
        cont->elements = evas_list_prepend_relative(cont->elements, el, el2);
        break;
      }
      else if (el->current.y > fy + fh)
      {
        cont->elements = evas_list_append(cont->elements, el);
        break;
      }
      else
        not_found = 1;
    }
  }
#endif
  if (not_found)
  {
    if (prev)
      evas_list_append_relative(cont->elements, el, prev->data);
    else
      evas_list_prepend(cont->elements, el);
  }
  _container_elements_fix(cont);
//  write_out_order(ib);
  if (cont->cb_order_change)
  {
    (cont->cb_order_change)(cont->data_order_change);
  }
  
}


void
_cb_container(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
}

void
_cb_element_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *info;
  Container_Element *el;
  double x, y;

  info = event_info;

  if (info->button != 2) return;

  el = (Container_Element *)data;

  evas_object_geometry_get(el->obj, &x, &y, NULL, NULL);

  el->down.x = info->canvas.x;
  el->down.y = info->canvas.y;
  el->delta.x = x - el->down.x;
  el->delta.y = y - el->down.y;
  el->mouse_down = info->button;
}

void
_cb_element_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Up *info;
  Container_Element *el;

  info = event_info;
  el = (Container_Element *)data;

  el->mouse_down = 0;
    
  if (el->dragging)
  {
    el->dragging = 0;
    _container_element_move(el);
    _container_elements_fix(el->container);
  }
}

void
_cb_element_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Move *info;
  Container_Element *el;

  info = event_info;
  el = (Container_Element *)data;

  if (el->container->move_button == 0) return;

  if (el->mouse_down == el->container->move_button && !el->dragging)
  {
    if (abs(info->cur.canvas.x - el->down.x) >= 3 ||
        abs(info->cur.canvas.y - el->down.y) >= 3)
    {
      el->dragging = 1;
    }
  }
  if (el->dragging)
  {
    el->current.x = info->cur.canvas.x;
    el->current.y = info->cur.canvas.y;

    evas_object_move(el->obj, el->current.x + el->delta.x,
                              el->current.y + el->delta.y);
  }
}


int
_container_scroll_timer(void *data)
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

  _container_elements_fix(sd->cont);
  return 1;
}
