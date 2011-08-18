#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Toggle.h"

using namespace std;

namespace Elmxx {

Toggle::Toggle (Evasxx::Object &parent)
{
  o = elm_toggle_add (parent.obj ());
  
  elmInit ();
}

Toggle::~Toggle () {}

Toggle *Toggle::factory (Evasxx::Object &parent)
{
  return new Toggle (parent);
}

void Toggle::setLabel (const std::string &label)
{
  elm_object_text_set (o, label.c_str ());
}

void Toggle::setIcon (Evasxx::Object &icon)
{
  elm_toggle_icon_set (o, icon.obj ());
}

void Toggle::setLabelsStates (const std::string &onLabel, const std::string &offLabel)
{
  elm_toggle_states_labels_set (o, onLabel.c_str (), offLabel.c_str ());
}

void Toggle::setState (bool state)
{
  elm_toggle_state_set (o, state);
}

bool Toggle::getState () const
{
  return elm_toggle_state_get (o);
}

/*void Toggle::setPointerState (bool statep)
{
  elm_toggle_state_pointer_set (Evas_Object *obj, Evas_Bool *statep)
}*/

} // end namespace Elmxx
