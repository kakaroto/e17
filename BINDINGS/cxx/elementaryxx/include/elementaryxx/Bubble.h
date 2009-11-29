#ifndef ELMXX_BUBBLE_H
#define ELMXX_BUBBLE_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Bubble : public Object
{
public:
  static Bubble *factory (Evasxx::Object &parent);

  void setLabel (const std::string &label);
  
  void setInfo (const std::string &info);
  
  void setContent (const Evasxx::Object &content);
  
  void setIcon (const Evasxx::Object &icon);
  
  void setCorner (const std::string &corner);
  
private:
  Bubble (); // forbid standard constructor
  Bubble (const Bubble&); // forbid copy constructor
  Bubble (Evasxx::Object &parent); // private construction -> use factory ()
  ~Bubble (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_BUBBLE_H
