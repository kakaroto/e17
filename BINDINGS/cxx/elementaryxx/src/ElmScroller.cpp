#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmScroller.h"

using namespace std;

namespace efl {

ElmScroller::ElmScroller (EvasObject &parent)
{
  o = elm_scroller_add (parent.obj ());
  
  elmInit ();
}

ElmScroller::~ElmScroller ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmScroller *ElmScroller::factory (EvasObject &parent)
{
  return new ElmScroller (parent);
}


void ElmScroller::setContent (EvasObject &child)
{
  elm_scroller_content_set (o, child.obj ());
}

void ElmScroller::limitMinContent (bool width, bool height)
{
  elm_scroller_content_min_limit (o, width, height);
}

void ElmScroller::showRegion (const Rect &rect)
{
  elm_scroller_region_show (o, rect.x (), rect.y (), rect.width (), rect.height ());
}

void ElmScroller::setPolicy (Elm_Scroller_Policy policyH, Elm_Scroller_Policy policyV)
{
  elm_scroller_policy_set (o, policyH, policyV);

}

const Rect ElmScroller::getRegion () const
{
  Evas_Coord x, y, w, h;
  elm_scroller_region_get (o, &x, &y, &w, &h);
  return Rect (x, y, w, h);
}

const Size ElmScroller::getChildSize () const
{
  Evas_Coord w, h;
  elm_scroller_child_size_get (o, &w, &h);
  return Size (w, h);
}

void ElmScroller::setBounce (bool hBounce, bool vBounce)
{
  elm_scroller_bounce_set (o, hBounce, vBounce);
}

} // end namespace efl
