#ifndef ELMXX_CHECK_H
#define ELMXX_CHECK_H

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
class Check : public Object
{
public:
  static Check *factory (Evasxx::Object &parent);

  void setLabel (const std::string &label);
  
  void setIcon (const Evasxx::Object &icon);
  
  void setState (bool state);
  
  bool getState () const;
  
private:
  Check (); // forbid standard constructor
  Check (const Check&); // forbid copy constructor
  Check (Evasxx::Object &parent); // private construction -> use factory ()
  ~Check (); // forbid direct delete -> use Object::destroy()
};

#if 0
   TODO
   EAPI void         elm_check_state_pointer_set(Evas_Object *obj, Evas_Bool *statep);
#endif

} // end namespace Elmxx

#endif // ELMXX_CHECK_H
