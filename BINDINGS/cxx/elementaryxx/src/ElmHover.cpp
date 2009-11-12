#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmHover.h"

using namespace std;

namespace efl {

ElmHover::ElmHover (EvasObject &parent)
{
  o = elm_hover_add (parent.obj ());
  
  elmInit ();
}

ElmHover::~ElmHover () {}

ElmHover *ElmHover::factory (EvasObject &parent)
{
  return new ElmHover (parent);
}

void ElmHover::setTarget (const EvasObject &target)
{
  elm_hover_target_set (o, target.obj ());
}

void ElmHover::setParent (const EvasObject &parent)
{
  elm_hover_parent_set (o, parent.obj ());
}

void ElmHover::setContent (const std::string &swallow, const EvasObject &content)
{
  elm_hover_content_set (o, swallow.c_str (), content.obj ());
}

const string ElmHover::getBestContentLocation (Elm_Hover_Axis prefAxis) const
{
  return elm_hover_best_content_location_get (o, prefAxis);
}

} // end namespace efl
