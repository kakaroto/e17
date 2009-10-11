#ifndef ELM_LABEL_H
#define ELM_LABEL_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

class ElmLabel : public ElmObject
{
public:
  static ElmLabel *factory (EvasObject &parent);
  
  void setLabel (const std::string &label);

private:
  ElmLabel (); // forbid standard constructor
  ElmLabel (const ElmLabel&); // forbid copy constructor
  ElmLabel (EvasObject &parent); // private construction -> use factory ()
  ~ElmLabel (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_LABEL_H
