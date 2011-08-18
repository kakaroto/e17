#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Bubble.h"

using namespace std;

namespace Elmxx {

Bubble::Bubble (Evasxx::Object &parent)
{
  o = elm_bubble_add (parent.obj ());
  
  elmInit ();
}

Bubble::~Bubble () {}

Bubble *Bubble::factory (Evasxx::Object &parent)
{
  return new Bubble (parent);
}

void Bubble::setLabel (const std::string &label)
{
  elm_object_text_set (o, label.c_str ());
}

void Bubble::setInfo (const std::string &info)
{
  elm_bubble_info_set (o, info.c_str ());
}

void Bubble::setContent (const Evasxx::Object &content)
{
  elm_bubble_content_set (o, content.obj ());
}

void Bubble::setIcon (const Evasxx::Object &icon)
{
  elm_bubble_icon_set (o, icon.obj ());
}

void Bubble::setCorner (const std::string &corner)
{
  elm_bubble_corner_set (o, corner.c_str ());
}

} // end namespace Elmxx
