#ifndef ELMXX_BOX_H
#define ELMXX_BOX_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Box : public Object
{
public:
  static Box *factory (Evasxx::Object &parent);
  
private:
  Box (); // forbid standard constructor
  Box (const Box&); // forbid copy constructor
  Box (Evasxx::Object &parent); // private construction -> use factory ()
  ~Box (); // forbid direct delete -> use Object::destroy()
  
public:
  enum Orientation
  {
    Horizontal,
    Vertical
  };

  void setOrientation (Box::Orientation orient);
  
  void setHomogeneous (bool homogeneous);

  void setHomogenous (bool homogenous);
  
  void packStart (const Evasxx::Object &subobj);
  
  void packEnd (const Evasxx::Object &subobj);
  
  void packBefore (const Evasxx::Object &subobj, const Evasxx::Object &before);
  
  void packAfter (const Evasxx::Object &subobj, const Evasxx::Object &after);
};

} // end namespace Elmxx

#endif // ELMXX_BOX_H
