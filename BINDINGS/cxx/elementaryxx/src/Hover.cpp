#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Hover.h"

using namespace std;

namespace Elmxx {

Hover::Hover (Evasxx::Object &parent)
{
  o = elm_hover_add (parent.obj ());
  
  elmInit ();
}

Hover::~Hover () {}

Hover *Hover::factory (Evasxx::Object &parent)
{
  return new Hover (parent);
}

void Hover::setTarget (const Evasxx::Object &target)
{
  elm_hover_target_set (o, target.obj ());
}

void Hover::setParent (const Evasxx::Object &parent)
{
  elm_hover_parent_set (o, parent.obj ());
}

void Hover::setContent (const std::string &swallow, const Evasxx::Object &content)
{
  elm_hover_content_set (o, swallow.c_str (), content.obj ());
}

const string Hover::getBestContentLocation (Elm_Hover_Axis prefAxis) const
{
  return elm_hover_best_content_location_get (o, prefAxis);
}

} // end namespace Elmxx
