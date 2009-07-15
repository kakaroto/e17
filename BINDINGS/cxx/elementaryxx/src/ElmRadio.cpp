#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmRadio.h"

using namespace std;

namespace efl {

ElmRadio::ElmRadio (EvasObject &parent)
{
  o = elm_radio_add (parent.obj ());
  
  elmInit ();
}

ElmRadio::~ElmRadio ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmRadio *ElmRadio::factory (EvasObject &parent)
{
  return new ElmRadio (parent);
}


void ElmRadio::setLabel (const std::string &label)
{
  elm_radio_label_set (o, label.c_str ());
}

void ElmRadio::setIcon (const EvasObject &icon)
{
  elm_radio_icon_set (o, icon.obj ());
}

void ElmRadio::addGroup (const EvasObject &group)
{
  elm_radio_group_add (o, group.obj ());
}

void ElmRadio::setStateValue (int value)
{
  elm_radio_state_value_set (o, value);
}

void ElmRadio::setValue (int value)
{
  elm_radio_value_set (o, value);
}

int ElmRadio::getValue () const
{
  return elm_radio_value_get (o);
}

} // end namespace efl
