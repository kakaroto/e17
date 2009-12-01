#ifndef ELMXX_TOGGLE_H
#define ELMXX_TOGGLE_H

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
class Toggle : public Object
{
public:  
  static Toggle *factory (Evasxx::Object &parent);

  void setLabel (const std::string &label);
  
  void setIcon (Evasxx::Object &icon);
  
  void setLabelsStates (const std::string &onLabel, const std::string &offLabel); // TODO: Think about a better name?
  
  void setState (bool state); // TODO: maybe left/right as parameter or so?
  
  bool getState () const;
  
  // TODO: why pointer parameter for C type?
  //void setPointerState (bool statep);
  
private:
  Toggle (); // forbid standard constructor
  Toggle (const Toggle&); // forbid copy constructor
  Toggle (Evasxx::Object &parent); // private construction -> use factory ()  
  ~Toggle (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_TOGGLE_H
