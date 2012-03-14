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

} // end namespace Elmxx
