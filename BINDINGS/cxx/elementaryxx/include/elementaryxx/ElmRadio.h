#ifndef ELM_RADIO_H
#define ELM_RADIO_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "changed" - the user toggled the state
 */
class ElmRadio : public ElmWidget
{
public:
  virtual ~ElmRadio ();
  
  static ElmRadio *factory (EvasObject &parent);

  void setLabel (const std::string &label);
  
  void setIcon (const EvasObject &icon);
  
  void addGroup (const EvasObject &group);
  
  void setStateValue (int value);
  
  void setValue (int value);
  
  int getValue () const;
  
private:
  ElmRadio (); // forbid standard constructor
  ElmRadio (const ElmRadio&); // forbid copy constructor
  ElmRadio (EvasObject &parent); // private construction -> use factory ()
};

#if 0
  TODO
   EAPI void         elm_radio_value_pointer_set(Evas_Object *obj, int *valuep);
#endif

} // end namespace efl

#endif // ELM_RADIO_H
