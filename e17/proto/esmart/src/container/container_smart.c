#include "container.h"
#include "container_private.h"

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

static Evas_Smart *smart = NULL;

/****** Public API *******/

Evas_Object *
e_container_new(Evas *evas)
{
  Evas_Object *container;

  container = evas_object_smart_add(evas, _container_smart_get());

  return container;
}

/***** private smart object handler functions **************/
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
  if((data->x == x) && (data->y == y))
      return;
  
//  evas_object_move(data->clipper, x+ data->padding.l,
//                          y + data->padding.t);
  evas_object_move(data->clipper, x, y);
  evas_object_move(data->grabber, x, y);

  data->x = x;
  data->y = y;

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

  data->w = w;
  data->h = h;

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
