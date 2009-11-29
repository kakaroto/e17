#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Anchorblock.h"

using namespace std;

namespace Elmxx {

Anchorblock::Anchorblock (Evasxx::Object &parent)
{
  o = elm_anchorblock_add (parent.obj ());
  
  elmInit ();
}

Anchorblock::~Anchorblock () {}

Anchorblock *Anchorblock::factory (Evasxx::Object &parent)
{
  return new Anchorblock (parent);
}

void Anchorblock::setText (const std::string &text)
{
  elm_anchorblock_text_set (o, text.c_str ());
}

void Anchorblock::setHoverParent (Evasxx::Object &parent)
{
  elm_anchorblock_hover_parent_set (o, parent.obj ());
}

void Anchorblock::setHoverStyle (const std::string &style)
{
  elm_anchorblock_hover_style_set (o, style.c_str ());
}

void Anchorblock::endHover ()
{
  elm_anchorblock_hover_end (o);
}

} // end namespace Elmxx
