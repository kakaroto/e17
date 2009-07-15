#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmCheck.h"

using namespace std;

namespace efl {

ElmCheck::ElmCheck (EvasObject &parent)
{
  o = elm_check_add (parent.obj ());
  
  elmInit ();
}

ElmCheck::~ElmCheck ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmCheck *ElmCheck::factory (EvasObject &parent)
{
  return new ElmCheck (parent);
}

void ElmCheck::setLabel (const std::string &label)
{
  elm_check_label_set (o, label.c_str ());
}

void ElmCheck::setIcon (const EvasObject &icon)
{
   elm_check_icon_set (o, icon.obj ());
}

void ElmCheck::setState (bool state)
{
  elm_check_state_set (o, state);
}

bool ElmCheck::getState () const
{
  return elm_check_state_get (o);
}

} // end namespace efl
