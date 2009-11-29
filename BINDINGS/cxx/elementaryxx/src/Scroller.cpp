#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Scroller.h"

using namespace std;

namespace Elmxx {

Scroller::Scroller (Evasxx::Object &parent)
{
  o = elm_scroller_add (parent.obj ());
  
  elmInit ();
}

Scroller::~Scroller () {}

Scroller *Scroller::factory (Evasxx::Object &parent)
{
  return new Scroller (parent);
}


void Scroller::setContent (Evasxx::Object &child)
{
  elm_scroller_content_set (o, child.obj ());
}

void Scroller::limitMinContent (bool width, bool height)
{
  elm_scroller_content_min_limit (o, width, height);
}

void Scroller::showRegion (const Eflxx::Rect &rect)
{
  elm_scroller_region_show (o, rect.x (), rect.y (), rect.width (), rect.height ());
}

void Scroller::setPolicy (Elm_Scroller_Policy policyH, Elm_Scroller_Policy policyV)
{
  elm_scroller_policy_set (o, policyH, policyV);

}

const Eflxx::Rect Scroller::getRegion () const
{
  Evas_Coord x, y, w, h;
  elm_scroller_region_get (o, &x, &y, &w, &h);
  return Eflxx::Rect (x, y, w, h);
}

const Eflxx::Size Scroller::getChildSize () const
{
  Evas_Coord w, h;
  elm_scroller_child_size_get (o, &w, &h);
  return Eflxx::Size (w, h);
}

void Scroller::setBounce (bool hBounce, bool vBounce)
{
  elm_scroller_bounce_set (o, hBounce, vBounce);
}

} // end namespace Elmxx
