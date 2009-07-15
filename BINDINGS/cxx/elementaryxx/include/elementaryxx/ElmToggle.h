#ifndef ELM_TOGGLE_H
#define ELM_TOGGLE_H

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
class ElmToggle : public ElmWidget
{
public:
  virtual ~ElmToggle ();
  
  static ElmToggle *factory (EvasObject &parent);

  void setLabel (const std::string &label);
  
  void setIcon (EvasObject &icon);
  
  void setLabelsStates (const std::string &onLabel, const std::string &offLabel); // TODO: Think about a better name?
  
  void setState (bool state); // TODO: maybe left/right as parameter or so?
  
  bool getState () const;
  
  // TODO: why pointer parameter for C type?
  //void setPointerState (bool statep);
  
private:
  ElmToggle (); // forbid standard constructor
  ElmToggle (const ElmToggle&); // forbid copy constructor
  ElmToggle (EvasObject &parent); // private construction -> use factory ()  
};

#if 0

;

#endif

} // end namespace efl

#endif // ELM_TOGGLE_H
