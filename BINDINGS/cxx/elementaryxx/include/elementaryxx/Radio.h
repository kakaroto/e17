#ifndef ELMXX_RADIO_H
#define ELMXX_RADIO_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "changed" - the user toggled the state
 */
class Radio : public Object
{
public:  
  static Radio *factory (Evasxx::Object &parent);

  void setLabel (const std::string &label);
  
  void setIcon (const Evasxx::Object &icon);
  
  void addGroup (const Evasxx::Object &group);
  
  void setStateValue (int value);
  
  void setValue (int value);
  
  int getValue () const;
  
private:
  Radio (); // forbid standard constructor
  Radio (const Radio&); // forbid copy constructor
  Radio (Evasxx::Object &parent); // private construction -> use factory ()
  ~Radio (); // forbid direct delete -> use Object::destroy()
};

#if 0
  TODO
   EAPI void         elm_radio_value_pointer_set(Evas_Object *obj, int *valuep);
#endif

} // end namespace Elmxx

#endif // ELMXX_RADIO_H
