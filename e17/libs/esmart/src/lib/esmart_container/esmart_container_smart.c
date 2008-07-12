/* 
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2 
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <Ecore.h>

#include "Esmart_Container.h"
#include "esmart_container_private.h"

/* smart object handlers */
void _container_add(Evas_Object *obj);
void _container_del(Evas_Object *obj);
void _container_layer_set(Evas_Object *obj, int l);
void _container_raise(Evas_Object *obj);
void _container_lower(Evas_Object *obj);
void _container_stack_above(Evas_Object *obj, Evas_Object *above);
void _container_stack_below(Evas_Object *obj, Evas_Object *below);
void _container_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
void _container_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
void _container_show(Evas_Object *obj);
void _container_hide(Evas_Object *obj);
void _container_color_set(Evas_Object *obj, int r, int g, int b, int a);
void _container_clip_set(Evas_Object *obj, Evas_Object *clip);
void _container_clip_unset(Evas_Object *obj);

Evas_Smart *_container_smart_get();

static Evas_Smart *smart = NULL;

/****** Public API *******/

Evas_Object *
esmart_container_new(Evas *evas)
{
  Evas_Object *container;

  container = evas_object_smart_add(evas, _container_smart_get());
 
  {
    Container *cont = _container_fetch(container);
    if (!cont) printf("wtf! (%s)\n", evas_object_type_get(container));
  }
  /* load the default layout plugin */
  if (!esmart_container_layout_plugin_set(container, "default"))
  {
    evas_object_del(container);
    return NULL;
  }

  return container;
}

/***** private smart object handler functions **************/
Evas_Smart *
_container_smart_get()
{
   if (!smart)
     {
	static const Evas_Smart_Class sc =
	  {
	     "container",
	     EVAS_SMART_CLASS_VERSION,
	     _container_add,
	     _container_del,
	     _container_move,
	     _container_resize,
	     _container_show,
	     _container_hide,
	     _container_color_set,
	     _container_clip_set,
	     _container_clip_unset,
	     NULL
	  };

	smart = evas_smart_class_new(&sc);
     }

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
  evas_object_smart_member_add(data->clipper, obj);
  evas_object_repeat_events_set(data->clipper, 1);
  evas_object_color_set(data->clipper, 255, 255, 255, 0);
  
  data->clipper_orig_alpha = 255;

  data->grabber = evas_object_rectangle_add(data->evas);
  evas_object_color_set(data->grabber, 40, 40, 255, 0);
  evas_object_repeat_events_set(data->grabber, 1);
  evas_object_smart_member_add(data->grabber, obj);
  /*
  evas_object_clip_set(data->grabber, data->clipper);
  */

  evas_object_event_callback_add(data->grabber, EVAS_CALLBACK_MOUSE_DOWN, _cb_container, data);
  evas_object_event_callback_add(data->grabber, EVAS_CALLBACK_MOUSE_UP, _cb_container, data);
  evas_object_event_callback_add(data->grabber, EVAS_CALLBACK_MOUSE_MOVE, _cb_container, data);

}


void
_container_del(Evas_Object *obj)
{
  Container *data;

  data = _container_fetch(obj);
  
  esmart_container_empty(obj);
  _container_layout_plugin_free(data->plugin);

  evas_object_del(data->clipper);
  evas_object_del(data->grabber);
  if (data->scroll.timer) ecore_timer_del(data->scroll.timer);

  free(data);
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

  evas_object_stack_above(data->grabber, above);
  evas_object_stack_above(data->clipper, above);

  for (l = data->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    evas_object_stack_above(el->grabber, above);
    evas_object_stack_above(el->obj, above);
  }
  
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
_container_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
  Container *data;
  
  data = evas_object_smart_data_get(obj);
  if((data->x == x) && (data->y == y))
      return;
  evas_object_move(data->clipper, x, y);
  evas_object_move(data->grabber, x, y);

  data->x = x;
  data->y = y;
 
  _container_elements_changed(data);
  _container_elements_fix(data);
}

void
_container_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  Container *data;
  double old_length;
  
  data = evas_object_smart_data_get(obj);

  if (w == data->w && h == data->h) return;

  old_length = esmart_container_elements_length_get(obj);

  evas_object_resize(data->clipper, w, h);
  evas_object_resize(data->grabber, w, h);

  data->w = w;
  data->h = h;

  _container_elements_changed(data);
  _container_elements_fix(data);
  _container_scale_scroll(data, old_length);
  data->changed = 1;
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
  data->clipper_orig_alpha = a;
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
