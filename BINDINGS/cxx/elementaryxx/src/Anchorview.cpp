#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Anchorview.h"

using namespace std;

namespace Elmxx {

Anchorview::Anchorview (Evasxx::Object &parent)
{
  o = elm_anchorview_add (parent.obj ());
  
  elmInit ();
}

Anchorview::~Anchorview () {}

Anchorview *Anchorview::factory (Evasxx::Object &parent)
{
  return new Anchorview (parent);
}

void Anchorview::setText (const std::string &text)
{
  elm_anchorview_text_set (o, text.c_str ());
}

void Anchorview::setHoverParent (Evasxx::Object &parent)
{
  elm_anchorview_hover_parent_set (o, parent.obj ());
}

void Anchorview::setHoverStyle (const std::string &style)
{
  elm_anchorview_hover_style_set (o, style.c_str ());
}

void Anchorview::endHover ()
{
  elm_anchorview_hover_end (o);
}

} // end namespace Elmxx
