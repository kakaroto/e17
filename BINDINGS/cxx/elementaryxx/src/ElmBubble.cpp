#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmBubble.h"

using namespace std;

namespace efl {

ElmBubble::ElmBubble (EvasObject &parent)
{
  o = elm_bubble_add (parent.obj ());
  
  elmInit ();
}

ElmBubble::~ElmBubble () {}

ElmBubble *ElmBubble::factory (EvasObject &parent)
{
  return new ElmBubble (parent);
}

void ElmBubble::setLabel (const std::string &label)
{
  elm_bubble_label_set (o, label.c_str ());
}

void ElmBubble::setInfo (const std::string &info)
{
  elm_bubble_info_set (o, info.c_str ());
}

void ElmBubble::setContent (const EvasObject &content)
{
  elm_bubble_content_set (o, content.obj ());
}

void ElmBubble::setIcon (const EvasObject &icon)
{
  elm_bubble_icon_set (o, icon.obj ());
}

void ElmBubble::setCorner (const std::string &corner)
{
  elm_bubble_corner_set (o, corner.c_str ());
}

} // end namespace efl
