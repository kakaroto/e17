#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Button.h"

using namespace std;

namespace Elmxx {

Button::Button (Evasxx::Object &parent)
{
  o = elm_button_add (parent.obj ());
  
  elmInit ();
}

Button::~Button () {}

Button *Button::factory (Evasxx::Object &parent)
{
  return new Button (parent);
}

void Button::setLabel (const std::string &label)
{
  elm_object_text_set (o, label.c_str ());
}

void Button::setIcon (const Evasxx::Object &icon)
{
  elm_button_icon_set (o, icon.obj ());
}

} // end namespace Elmxx
