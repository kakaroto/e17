#ifndef ELM_CHECK_H
#define ELM_CHECK_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 * "changed" - the user toggled the state
 */
class ElmCheck : public ElmObject
{
public:
  static ElmCheck *factory (EvasObject &parent);

  void setLabel (const std::string &label);
  
  void setIcon (const EvasObject &icon);
  
  void setState (bool state);
  
  bool getState () const;
  
private:
  ElmCheck (); // forbid standard constructor
  ElmCheck (const ElmCheck&); // forbid copy constructor
  ElmCheck (EvasObject &parent); // private construction -> use factory ()
  ~ElmCheck (); // forbid direct delete -> use ElmObject::destroy()
};

#if 0
   TODO
   EAPI void         elm_check_state_pointer_set(Evas_Object *obj, Evas_Bool *statep);
#endif

} // end namespace efl

#endif // ELM_CHECK_H
