#include "container.h"
#include "container_private.h"

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

  _container_elements_fix(cont);
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

  _container_elements_fix(cont);
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

  _container_elements_fix(cont);
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

  _container_elements_fix(cont);
}

void e_container_element_remove(Evas_Object *container, Evas_Object *element)
{
  Container *cont;
  Container_Element *el;
  int old_length;
  
  cont = _container_fetch(container);
  if (!cont) return;

  old_length = e_container_elements_length_get(container);

  el = evas_object_data_get(element, "Container_Element");
  cont->elements = evas_list_remove(cont->elements, el);

  _container_elements_fix(cont);
  _container_scale_scroll(cont, old_length);
}

Evas_List *e_container_elements_get(Evas_Object *container)
{
  Container *cont;
  Evas_List *l, *ll = NULL;
  
  cont = _container_fetch(container);
  if (!cont) return NULL;

  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;

    ll = evas_list_append(ll, el->obj); 
  }

  return ll;
}

