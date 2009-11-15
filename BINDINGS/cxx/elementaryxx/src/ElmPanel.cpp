#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmPanel.h"

using namespace std;

namespace efl {

ElmPanel::ElmPanel (EvasObject &parent)
{
  o = elm_panel_add (parent.obj ());
  
  elmInit ();
}

ElmPanel::~ElmPanel () {}

ElmPanel *ElmPanel::factory (EvasObject &parent)
{
  return new ElmPanel (parent);
}

void ElmPanel::setOrientation (Elm_Panel_Orient orient)
{
  elm_panel_orient_set (o, orient);

}

void ElmPanel::setContent (EvasObject &content)
{
  elm_panel_content_set (o, content.obj ());
}

} // end namespace efl
