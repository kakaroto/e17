/**************************************************************************
 * An evas smart container object 
 * 
 *  
 * 
 *
 ***************************************************************************/

#include <Evas.h>
#include "container.h"

/* smart object handlers */
void _container_add(Evas_Object *obj);
void _container_del(Evas_Object *obj);
void _container_layer_set(Evas_Object *obj, int l);
void _container_raise(Evas_Object *obj);
void _container_lower(Evas_Object *obj);
void _container_stack_above(Evas_Object *obj, Evas_Object *above);
void _container_stack_below(Evas_Object *obj, Evas_Object *below);
void _container_move(Evas_Object *obj, double x, double y);
void _container_resize(Evas_Object *obj, double w, double h);
void _container_show(Evas_Object *obj);
void _container_hide(Evas_Object *obj);
void _container_color_set(Evas_Object *obj, int r, int g, int b, int a);
void _container_clip_set(Evas_Object *obj, Evas_Object *clip);
void _container_clip_unset(Evas_Object *obj);

Evas_Smart *_container_smart_get();
Container *_container_fetch(Evas_Object *obj);
Container_Element *_container_element_new(Container *cont, Evas_Object *obj);
void _container_elements_fix(Container *cont);
double _container_elements_length_get(Container *cont);
void _cb_container(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void _cb_element_move(void *data, Evas *e, Evas_Object *obj, void *event_info);

/* keep a global copy of this, so it only has to be created once */
static Evas_Smart *smart;


/*** external API ***/

Evas_Object *
e_container_new(Evas *evas)
{
  Evas_Object *container;

  container = evas_object_smart_add(evas, _container_smart_get());

  return container;
}


void e_container_direction_set(Evas_Object *container, int direction)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->direction = direction;
  _container_elements_fix(cont);
}

int  e_container_direction_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->direction;
}

void e_container_padding_set(Evas_Object *container, double l, double r,
                             double t, double b)
{
  Container *cont;
  
  cont = _container_fetch(container);

  cont->padding.l = l;
  cont->padding.r = r;
  cont->padding.t = t;
  cont->padding.b = b;

  _container_elements_fix(cont);
}

void e_container_padding_get(Evas_Object *container, double *l, double *r,
                             double *t, double *b)
{
  Container *cont;
  
  cont = _container_fetch(container);

  if (l) *l = cont->padding.l;
  if (r) *r = cont->padding.r;
  if (t) *t = cont->padding.t;
  if (b) *b = cont->padding.b;
}

void e_container_fit_x_set(Evas_Object *container, int fit_x)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->fit_x = fit_x;

  _container_elements_fix(cont);
}

int  e_container_fit_x_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->fit_x;
}

void e_container_fit_y_set(Evas_Object *container, int fit_y)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->fit_y = fit_y;

  _container_elements_fix(cont);
}

int  e_container_fit_y_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->fit_y;
}

void e_container_scroll_set(Evas_Object *container, int scroll)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->scroll = scroll;

  _container_elements_fix(cont);
}

int  e_container_scroll_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->scroll;
}

void e_container_spacing_set(Evas_Object *container, int spacing)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->spacing = spacing;

  _container_elements_fix(cont);
}

int e_container_spacing_get(Evas_Object *container)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return 0;

  return cont->spacing;
}

/*** element API ***/

void e_container_element_append(Evas_Object *container, Evas_Object *element)
{
  Container *cont;
  Container_Element *el = NULL;
  
  cont = _container_fetch(container);
  if (!cont) return;

  el = _container_element_new(cont, element);
  if (!el) return;

  cont->elements = evas_list_append(cont->elements, el);
}

void e_container_element_prepend(Evas_Object *container, Evas_Object *element)
{
  Container *cont;
  Container_Element *el;
  
  cont = _container_fetch(container);
  if (!cont) return;

  el = _container_element_new(cont, element);
  if (!el) return;

  cont->elements = evas_list_prepend(cont->elements, el);
}

void e_container_element_append_relative(Evas_Object *container,
                                         Evas_Object *element,
                                         Evas_Object *relative)
{
  Container *cont;
  Container_Element *el, *rel;
  
  cont = _container_fetch(container);
  if (!cont) return;

  el = _container_element_new(cont, element);
  if (!el) return;

  rel = evas_object_data_get(relative, "Container_Element");
  if (!rel) return;

  cont->elements = evas_list_append_relative(cont->elements, el, rel);
}

void e_container_element_prepend_relative(Evas_Object *container,
                                          Evas_Object *element,
                                          Evas_Object *relative)
{
  Container *cont;
  Container_Element *el, *rel;
  
  cont = _container_fetch(container);
  if (!cont) return;

  el = _container_element_new(cont, element);
  if (!el) return;

  rel = evas_object_data_get(relative, "Container_Element");
  if (!rel) return;

  cont->elements = evas_list_prepend_relative(cont->elements, el, rel);
}

void e_container_element_remove(Evas_Object *container, Evas_Object *element)
{
  Container *cont;
  Container_Element *el;
  
  cont = _container_fetch(container);
  if (!cont) return;

  el = evas_object_data_get(element, "Container_Element");
  cont->elements = evas_list_remove(cont->elements, el);
}

Evas_List *e_container_elements_get(Evas_Object *container)
{
  Container *cont;
  Evas_List *l, *ll = NULL;
  
  cont = _container_fetch(container);
  if (!cont) return;

  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    ll = evas_list_append(ll, el->obj); 
  }

  return ll;
}

void e_container_callback_order_change_set(Evas_Object *container, void (*func)(void *data), void *data)
{
  Container *cont;
  
  cont = _container_fetch(container);
  if (!cont) return;

  cont->cb_order_change = func;
  cont->data_order_change = data;
}


/*** internal  functions ***/

Container *
_container_fetch(Evas_Object *obj)
{
  Container *cont;
  char *type;

  type = (char *)evas_object_type_get(obj);
  if (!type) return NULL;
  if (strcmp(type, "container")) return NULL;
  cont = evas_object_smart_data_get(obj); 
  return cont;
}

Container_Element *
_container_element_new(Container *cont, Evas_Object *obj)
{
  Container_Element *el;

  if (!obj) return NULL;

  el = calloc(1, sizeof(Container_Element));

  el->obj = obj;
  evas_object_data_set(obj, "Container_Element", el);
  evas_object_show(obj);

  el->grabber = evas_object_rectangle_add(evas_object_evas_get(obj));
  evas_object_repeat_events_set(el->grabber, 1);
  evas_object_color_set(el->grabber, 0, 0, 0, 0);
  evas_object_show(el->grabber);
  
  el->container = cont;
  evas_object_clip_set(el->obj, cont->clipper);
  evas_object_clip_set(el->grabber, cont->clipper);

  evas_object_event_callback_add(el->grabber, EVAS_CALLBACK_MOUSE_DOWN, _cb_element_down, el);
  evas_object_event_callback_add(el->grabber, EVAS_CALLBACK_MOUSE_UP, _cb_element_up, el);
  evas_object_event_callback_add(el->grabber, EVAS_CALLBACK_MOUSE_MOVE, _cb_element_move, el);


  return el;
}

void
_container_elements_fix(Container *cont)
{
  Evas_List *l;
  double ax, ay, aw, ah; // element area geom
  double ix, iy;

  evas_object_geometry_get(cont->grabber, &ax, &ay, &aw, &ah);

  /* adjust for padding */
  ax += cont->padding.l;
  ay += cont->padding.t;
  aw -= cont->padding.l + cont->padding.r;
  ah -= cont->padding.t + cont->padding.b;

  if (aw == 0 || ah == 0)
    return;

  ix = ax;
  iy = ay;

  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;
    double ew, eh; // element size

    if(!el)
    {
      continue;
    }
    evas_object_geometry_get(el->obj, NULL, NULL, &ew, &eh);

    evas_object_resize(el->grabber, ew, eh);

    /* vertical */
    if (cont->direction)
    {
      if (cont->fit_x)
      {
      }
      if (cont->fit_y)
      {
      }

      /* FIXME: do other alignments also */
      ix = ax + (aw - ew) / 2;
      evas_object_move(el->obj, ix, iy);
      evas_object_move(el->grabber, ix, iy);
      iy += eh + cont->spacing;
    }

    /* horizontal */
    else
    {
      if (cont->fit_x)
      {
      }
      if (cont->fit_y)
      {
      }

      /* FIXME: do other alignments also */
      iy = ay + (ah - eh) / 2;
      evas_object_move(el->obj, ix, iy);
      evas_object_move(el->grabber, ix, iy);
      ix += ew + cont->spacing;
    }
  }


  
}

double
_container_elements_length_get(Container *cont)
{
  Evas_List *l;
  double length = 0;

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
 
  if (el->mouse_down && !el->dragging)
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


/*** smart object handler functions ***/

Evas_Smart *
_container_smart_get()
{
  if (smart) return smart;

  smart = evas_smart_new ("container",
                          _container_add,
                          _container_del,
                          _container_layer_set,
                          _container_raise,
                          _container_lower,
                          _container_stack_above,
                          _container_stack_below,
                          _container_move,
                          _container_resize,
                          _container_show,
                          _container_hide,
                          _container_color_set,
                          _container_clip_set,
                          _container_clip_unset,
                          NULL
                          );

  return smart; 
}

void
_container_add(Evas_Object *obj)
{
  Container *data;
  
  data = calloc(1, sizeof(Container));
  if (!data) return;

  evas_object_smart_data_set(obj, data);
  data->obj = obj;
  data->evas = evas_object_evas_get(obj);

  data->clipper = evas_object_rectangle_add(data->evas);
  evas_object_smart_member_add(obj, data->clipper);
  //evas_object_color_set(data->clipper, 255, 40, 40, 20);
  evas_object_repeat_events_set(data->clipper, 1);

  data->grabber = evas_object_rectangle_add(data->evas);
  evas_object_color_set(data->grabber, 40, 40, 255, 0);
  evas_object_repeat_events_set(data->grabber, 1);
  evas_object_smart_member_add(obj, data->grabber);

  evas_object_event_callback_add(data->grabber, EVAS_CALLBACK_MOUSE_DOWN, _cb_container, data);
  evas_object_event_callback_add(data->grabber, EVAS_CALLBACK_MOUSE_UP, _cb_container, data);
  evas_object_event_callback_add(data->grabber, EVAS_CALLBACK_MOUSE_MOVE, _cb_container, data);
}


void
_container_del(Evas_Object *obj)
{
  Container *data;
  Evas_List *l;
  
  data = evas_object_smart_data_get(obj);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;
    evas_object_del(el->obj);
    evas_object_del(el->grabber);
  }
}

void
_container_layer_set(Evas_Object *obj, int layer)
{
  Container *data;
  Evas_List *l;
  
  data = evas_object_smart_data_get(obj);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    evas_object_layer_set(el->obj, layer);
    evas_object_layer_set(el->grabber, layer);
  }
   
  evas_object_layer_set(data->clipper, layer);
  evas_object_layer_set(data->grabber, layer);
  
}

void
_container_raise(Evas_Object *obj)
{
  Container *data;
  Evas_List *l;
  
  data = evas_object_smart_data_get(obj);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    evas_object_raise(el->obj);
    evas_object_raise(el->grabber);
  }

  evas_object_raise(data->clipper);
  evas_object_raise(data->grabber);
}

void
_container_lower(Evas_Object *obj)
{
  Container *data;
  Evas_List *l;
  
  data = evas_object_smart_data_get(obj);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    evas_object_lower(el->obj);
    evas_object_lower(el->grabber);
  }
  
  evas_object_lower(data->clipper);
  evas_object_lower(data->grabber);
}

void
_container_stack_above(Evas_Object *obj, Evas_Object *above)
{
  Container *data;
  Evas_List *l;
  
  data = evas_object_smart_data_get(obj);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    evas_object_stack_above(el->obj, above);
    evas_object_stack_above(el->grabber, above);
  }
  
  evas_object_stack_above(data->clipper, above);
  evas_object_stack_above(data->grabber, above);
}

void
_container_stack_below(Evas_Object *obj, Evas_Object *below)
{
  Container *data;
  Evas_List *l;
  
  data = evas_object_smart_data_get(obj);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    evas_object_stack_below(el->obj, below);
    evas_object_stack_below(el->grabber, below);
  }
  
  evas_object_stack_below(data->clipper, below);
  evas_object_stack_below(data->grabber, below);
}

void
_container_move(Evas_Object *obj, double x, double y)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

  
//  evas_object_move(data->clipper, x+ data->padding.l,
//                          y + data->padding.t);

  evas_object_move(data->clipper, x, y);
  evas_object_move(data->grabber, x, y);

  _container_elements_fix(data);
}

void
_container_resize(Evas_Object *obj, double w, double h)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

//  evas_object_resize(data->clipper, w - (data->padding.l + data->padding.r),
//                     h - (data->padding.t + data->padding.b));
  evas_object_resize(data->clipper, w, h);
  evas_object_resize(data->grabber, w, h);

  _container_elements_fix(data);
}

void
_container_show(Evas_Object *obj)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

  evas_object_show(data->clipper);
  evas_object_show(data->grabber);
}

void
_container_hide(Evas_Object *obj)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

  evas_object_hide(data->clipper);
  evas_object_hide(data->grabber);
}


void
_container_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

  evas_object_color_set(data->clipper, r, g, b, a);
}

void
_container_clip_set(Evas_Object *obj, Evas_Object *clip)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

  evas_object_clip_set(data->clipper, clip);
  evas_object_clip_set(data->grabber, clip);
}

void
_container_clip_unset(Evas_Object *obj)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);

  evas_object_clip_unset(data->clipper);
  evas_object_clip_unset(data->grabber);
}
