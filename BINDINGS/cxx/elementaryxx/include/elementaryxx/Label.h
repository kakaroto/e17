#ifndef ELMXX_LABEL_H
#define ELMXX_LABEL_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Label : public Object
{
public:
  static Label *factory (Evasxx::Object &parent);
  
  void setLabel (const std::string &label);

private:
  Label (); // forbid standard constructor
  Label (const Label&); // forbid copy constructor
  Label (Evasxx::Object &parent); // private construction -> use factory ()
  ~Label (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_LABEL_H
