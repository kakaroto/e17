#include <Evas.h>
#include <Ecore.h>
#include <math.h>
#include "../../container.h"

void
_entice_layout(Container *cont)
{
  Evas_List *l;
  int i, im_no;
  double pos;
  double ey = cont->y + cont->padding.t;

  i = 1;
  im_no = 0;
  printf("*** layout, entice style...\n");
/*
  for (l = cont->elements; l; l = l->next, i++)
  {
    if (l == current)
      im_no = i;
  }
*/
  im_no = 3;
  i = 1;
  pos = 0.0;
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
}

void
_entice_scroll_stop(Container *cont)
{
}

void
_entice_scroll_to(Container *cont, Container_Element *el)
{
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
