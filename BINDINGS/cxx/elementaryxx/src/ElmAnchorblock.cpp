#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmAnchorblock.h"

using namespace std;

namespace efl {

ElmAnchorblock::ElmAnchorblock (EvasObject &parent)
{
  o = elm_anchorblock_add (parent.obj ());
  
  elmInit ();
}

ElmAnchorblock::~ElmAnchorblock () {}

ElmAnchorblock *ElmAnchorblock::factory (EvasObject &parent)
{
  return new ElmAnchorblock (parent);
}

void ElmAnchorblock::setText (const std::string &text)
{
  elm_anchorblock_text_set (o, text.c_str ());
}

void ElmAnchorblock::setHoverParent (EvasObject &parent)
{
  elm_anchorblock_hover_parent_set (o, parent.obj ());
}

void ElmAnchorblock::setHoverStyle (const std::string &style)
{
  elm_anchorblock_hover_style_set (o, style.c_str ());
}

void ElmAnchorblock::endHover ()
{
  elm_anchorblock_hover_end (o);
}

} // end namespace efl
