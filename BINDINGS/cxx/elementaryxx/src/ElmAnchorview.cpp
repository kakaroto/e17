#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmAnchorview.h"

using namespace std;

namespace efl {

ElmAnchorview::ElmAnchorview (EvasObject &parent)
{
  o = elm_anchorview_add (parent.obj ());
  
  elmInit ();
}

ElmAnchorview::~ElmAnchorview () {}

ElmAnchorview *ElmAnchorview::factory (EvasObject &parent)
{
  return new ElmAnchorview (parent);
}

void ElmAnchorview::setText (const std::string &text)
{
  elm_anchorview_text_set (o, text.c_str ());
}

void ElmAnchorview::setHoverParent (EvasObject &parent)
{
  elm_anchorview_hover_parent_set (o, parent.obj ());
}

void ElmAnchorview::setHoverStyle (const std::string &style)
{
  elm_anchorview_hover_style_set (o, style.c_str ());
}

void ElmAnchorview::endHover ()
{
  elm_anchorview_hover_end (o);
}

} // end namespace efl
