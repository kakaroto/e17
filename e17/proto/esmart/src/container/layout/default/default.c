#include <Evas.h>
#include <Ecore.h>
#include <math.h>
#include "../../container.h"

int _container_scroll_timer(void *data);

void
_default_layout(Container *cont)
{
  Evas_List *l;
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

  if (cont->direction) iy += cont->scroll_offset;
  else ix += cont->scroll_offset;

  L = e_container_elements_orig_length_get(cont->obj);
  num = evas_list_count(cont->elements);
  
  
  evas_object_color_get(cont->clipper, &r, &g, &b, NULL);
  if(num > 0)
	  evas_object_color_set(cont->clipper, r,g,b, cont->clipper_orig_alpha);
  else
	  evas_object_color_set(cont->clipper, r,g,b, 0);

  for (l = cont->elements; l; l = l->next)
  {
    Container_Element *el = l->data;
    double ew, eh; // old element size

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
        //printf("fill\n");
        ih = ah;
        
        if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
          iw = (aw - cont->spacing * (num - 1) ) / num;
        else
        {
          //printf("nonhomog\n");
          iw = el->orig_w * (aw - cont->spacing * (num - 1) ) / L;
        }
      }
      else if (cont->fill & CONTAINER_FILL_POLICY_FILL_X)
      {
        //printf("fill x\n");

        if (cont->fill & CONTAINER_FILL_POLICY_HOMOGENOUS)
        {
          //printf ("homog\n");
          iw = (aw - cont->spacing * (num - 1) ) / num;
        }
        else
        {
          //printf("nonhomog - L: %f, ew: %f\n", L, ew);
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

void _default_scroll_start(Container *cont, double velocity)
{
  Scroll_Data *data;
  double length, size;

  length = e_container_elements_length_get(cont->obj);
  size = cont->direction ? cont->h : cont->w;

  /* don't scroll unless the elements exceed the size of the container */
  if (length <= size)
  {
    printf(" length smaller than size\n");
    return;
  }
  //printf("continue\n");
  data = calloc(1, sizeof(Scroll_Data));
  data->velocity = velocity;
  data->start_time = ecore_time_get();
  data->cont = cont;
  data->length = length;
 
  cont->scroll_timer = ecore_timer_add(.02, _container_scroll_timer, data);
}

void _default_scroll_stop(Container *cont)
{
  /* FIXME: decelerate on stop? */
  if (cont->scroll_timer)
  {
    ecore_timer_del(cont->scroll_timer);  
    cont->scroll_timer = NULL;
  }
}

void _default_scroll_to(Container *cont, Container_Element *el)
{
  return;
}

void
_default_shutdown()
{
}


/*** internal plugin functions ***/
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

  _default_layout(sd->cont);
  return 1;
}

int
plugin_init(Container_Layout_Plugin *p)
{
  p->layout = _default_layout;
  p->scroll_start = _default_scroll_start;
  p->scroll_stop = _default_scroll_stop;
  p->scroll_to = _default_scroll_to;
  p->shutdown = _default_shutdown;

  return 1;
}

