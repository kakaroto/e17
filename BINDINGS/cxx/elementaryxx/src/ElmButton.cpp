#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmButton.h"

using namespace std;

namespace efl {

ElmButton::ElmButton (EvasObject &parent)
{
  o = elm_button_add (parent.obj ());
  
  elmInit ();
}

ElmButton::~ElmButton () {}

ElmButton *ElmButton::factory (EvasObject &parent)
{
  return new ElmButton (parent);
}

void ElmButton::setLabel (const std::string &label)
{
  elm_button_label_set (o, label.c_str ());
}

void ElmButton::setIcon (const EvasObject &icon)
{
  elm_button_icon_set (o, icon.obj ());
}

} // end namespace efl
