#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <Evas.h>
#include <Ecore.h>

#include "../../Esmart_Container.h"
#include "../../esmart_container_private.h"

static int _container_scroll_timer(void *data);

static void
_default_layout(Container *cont)
{
  Eina_List *l;
  double ax, ay, aw, ah; // element area geom
  double ix, iy, iw, ih; // new x, y, w, h
  double L; // length of all objects at original size (for nonhomog)
  int num; // number of elements
  double error = 0;
  int r,g,b;

  /* FIXME: add a 'changed' flag to prevent excessive recalcs */
  
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

  if (cont->direction) iy += cont->scroll.offset;
  else ix += cont->scroll.offset;

  L = esmart_container_elements_orig_length_get(cont->obj);
  num = eina_list_count(cont->elements);
  
  
  evas_object_color_get(cont->clipper, &r, &g, &b, NULL);

  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;
    Evas_Coord ew, eh; // old element size

    if(!el)
    {
      continue;
    }
    evas_object_geometry_get(el->obj, NULL, NULL, &ew, &eh);
    if (ew == 0) ew = el->orig_w;
    if (eh == 0) eh = el->orig_h;

    evas_object_resize(el->grabber, ew, eh);

    /* vertical */
    if (cont->direction)
    {
      if (cont->fill & CONTAINER_FILL_POLICY_FILL)
      {
        iw = aw;

        if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
          ih = (ah - cont->spacing * (num - 1) ) / num;
        else
          ih = el->orig_h * (ah - cont->spacing * (num - 1) ) / L;
          
      }
      else if (cont->fill & CONTAINER_FILL_POLICY_FILL_X)
      {
        if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
        {
          iw = aw;
          ih = eh * iw/ew;
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
          ih = (ah - cont->spacing * (num - 1) ) / num;
        else
          ih = el->orig_h * (ah - cont->spacing * (num - 1) ) / L;

        if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
          iw = ew * ih/eh;
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

      evas_object_move(el->obj, ix, iy);
      evas_object_resize(el->obj, iw, ih);
      if (!strcmp(evas_object_type_get(el->obj), "image"))
      {
        evas_object_image_fill_set(el->obj, 0, 0, iw, ih);
      }
      evas_object_move(el->grabber, ix, iy);
      evas_object_resize(el->grabber, iw, ih);

      iy += ih + cont->spacing;
    }

    /* horizontal */
    else
    {
      if (cont->fill & CONTAINER_FILL_POLICY_FILL)
      {
        ih = ah;
        
        if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
          iw = (aw - cont->spacing * (num - 1) ) / num;
        else
        {
          iw = el->orig_w * (aw - cont->spacing * (num - 1) ) / L;
        }
      }
      else if (cont->fill & CONTAINER_FILL_POLICY_FILL_X)
      {

        if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
        {
          iw = (aw - cont->spacing * (num - 1) ) / num;
        }
        else
        {
          iw = el->orig_w * (aw - cont->spacing * (num - 1) ) / L;
        }

        if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
          ih = eh * iw/ew;
        else
          ih = eh;
      }
      else if (cont->fill & CONTAINER_FILL_POLICY_FILL_Y)
      {
        if (cont->fill & CONTAINER_FILL_POLICY_KEEP_ASPECT)
        {
          ih = ah;
          iw = ew * ih/eh;
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

      error += iw - (int)iw; 

      iw = (int)iw;

      evas_object_move(el->obj, ix, iy);
      evas_object_resize(el->obj, iw, ih);
      if (!strcmp(evas_object_type_get(el->obj), "image"))
      {
        evas_object_image_fill_set(el->obj, 0, 0, iw, ih);
      }
      evas_object_move(el->grabber, ix, iy);
      evas_object_resize(el->grabber, iw, ih);
      ix += iw + cont->spacing;
    }

   
    evas_object_geometry_get(el->obj, NULL, NULL, &ew, &eh);
    evas_object_resize(el->grabber, ew, eh);
  }
}

static void _default_scroll_start(Container *cont, double velocity)
{
  double length, size;

  length = esmart_container_elements_length_get(cont->obj);
  size = cont->direction ? cont->h : cont->w;
   
  /* Stop other scrolling in process*/
  if (cont->scroll.timer)
  {
    ecore_timer_del(cont->scroll.timer);  
    cont->scroll.timer = NULL;
  }
   
  /* don't scroll unless the elements exceed the size of the container */
  if (length <= size)
    return;
  
  cont->scroll.velocity = velocity;
  cont->scroll.start_time = ecore_time_get();
 
  cont->scroll.timer = ecore_timer_add(.02, _container_scroll_timer, cont);
}

static void _default_scroll_stop(Container *cont)
{
  /* FIXME: decelerate on stop? */
  if (cont->scroll.timer)
  {
    ecore_timer_del(cont->scroll.timer);  
    cont->scroll.timer = NULL;
  }
}

static void _default_scroll_to(Container *cont, Container_Element *el)
{
  return;
}

static void
_default_shutdown()
{
  return;
}


/*** internal plugin functions ***/
static int
_container_scroll_timer(void *data)
{
  Container *cont = data;
  double dt, dx, size, pad, max_scroll;
 
  dt = ecore_time_get() - cont->scroll.start_time;
  dx = 10 * (1 - exp(-dt)); 

  cont->scroll.offset += dx * cont->scroll.velocity;
  
  size = cont->direction ? cont->h : cont->w;
  pad = cont->direction ? cont->padding.t + cont->padding.b :
                              cont->padding.l + cont->padding.r;
  max_scroll = size - cont->length - pad;

  if (cont->scroll.offset < max_scroll)
  {
    cont->scroll.offset = max_scroll;
    _default_scroll_stop(cont);
  } 
  else if (cont->scroll.offset > 0)
  {
    cont->scroll.offset = 0;
    _default_scroll_stop(cont);
  }
    
  _default_layout(cont);
  return 1;
}

EAPI int
plugin_init(Container_Layout_Plugin *p)
{
  p->layout = _default_layout;
  p->scroll_start = _default_scroll_start;
  p->scroll_stop = _default_scroll_stop;
  p->scroll_to = _default_scroll_to;
  p->shutdown = _default_shutdown;

  return 1;
}

