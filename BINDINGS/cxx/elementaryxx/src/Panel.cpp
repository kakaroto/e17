#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Panel.h"

using namespace std;

namespace Elmxx {

Panel::Panel (Evasxx::Object &parent)
{
  o = elm_panel_add (parent.obj ());
  
  elmInit ();
}

Panel::~Panel () {}

Panel *Panel::factory (Evasxx::Object &parent)
{
  return new Panel (parent);
}

void Panel::setOrientation (Elm_Panel_Orient orient)
{
  elm_panel_orient_set (o, orient);

}

void Panel::setContent (Evasxx::Object &content)
{
  elm_panel_content_set (o, content.obj ());
}

} // end namespace Elmxx
