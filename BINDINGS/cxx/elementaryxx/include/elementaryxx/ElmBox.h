#ifndef ELM_BOX_H
#define ELM_BOX_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

class ElmBox : public ElmObject
{
public:
  static ElmBox *factory (EvasObject &parent);
  
private:
  ElmBox (); // forbid standard constructor
  ElmBox (const ElmBox&); // forbid copy constructor
  ElmBox (EvasObject &parent); // private construction -> use factory ()
  ~ElmBox (); // forbid direct delete -> use ElmObject::destroy()
  
public:
  enum Orientation
  {
    Horizontal,
    Vertical
  };

  void setOrientation (ElmBox::Orientation orient);
  
  void setHomogenous (bool homogenous);
  
  void packStart (const EvasObject &subobj);
  
  void packEnd (const EvasObject &subobj);
  
  void packBefore (const EvasObject &subobj, const EvasObject &before);
  
  void packAfter (const EvasObject &subobj, const EvasObject &after);
};

} // end namespace efl

#endif // ELM_BOX_H
