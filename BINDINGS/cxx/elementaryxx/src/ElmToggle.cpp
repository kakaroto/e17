#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmToggle.h"

using namespace std;

namespace efl {

ElmToggle::ElmToggle (EvasObject &parent)
{
  o = elm_toggle_add (parent.obj ());
  
  elmInit ();
}

ElmToggle::~ElmToggle () {}

ElmToggle *ElmToggle::factory (EvasObject &parent)
{
  return new ElmToggle (parent);
}

void ElmToggle::setLabel (const std::string &label)
{
  elm_toggle_label_set (o, label.c_str ());
}

void ElmToggle::setIcon (EvasObject &icon)
{
  elm_toggle_icon_set (o, icon.obj ());
}

void ElmToggle::setLabelsStates (const std::string &onLabel, const std::string &offLabel)
{
  elm_toggle_states_labels_set (o, onLabel.c_str (), offLabel.c_str ());
}

void ElmToggle::setState (bool state)
{
  elm_toggle_state_set (o, state);
}

bool ElmToggle::getState () const
{
  return elm_toggle_state_get (o);
}

/*void ElmToggle::setPointerState (bool statep)
{
  elm_toggle_state_pointer_set (Evas_Object *obj, Evas_Bool *statep)
}*/

} // end namespace efl
