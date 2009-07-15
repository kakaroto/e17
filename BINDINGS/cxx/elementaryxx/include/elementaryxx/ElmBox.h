#ifndef ELM_BOX_H
#define ELM_BOX_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

class ElmBox : public ElmWidget
{
public:
  virtual ~ElmBox ();
  
  static ElmBox *factory (EvasObject &parent);

  void setHorizontal (bool horizontal);
  
  void setHomogenous (bool homogenous);
  
  void packStart (const EvasObject &subobj);
  
  void packEnd (const EvasObject &subobj);
  
  void packBefore (const EvasObject &subobj, const EvasObject &before);
  
  void packAfter (const EvasObject &subobj, const EvasObject &after);
  
private:
  ElmBox (); // forbid standard constructor
  ElmBox (const ElmBox&); // forbid copy constructor
  ElmBox (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_BOX_H
