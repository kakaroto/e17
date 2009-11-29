#ifndef ELMXX_TEMPLATE_H
#define ELMXX_TEMPLATE_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 */
class Separator : public Object
{
public:
  enum Orientation
  {
    Horizontal,
    Vertical
  };
  
  static Separator *factory (Evasxx::Object &parent);
    
  void setOrientation (Separator::Orientation orient);
  Separator::Orientation getOrientation ();
  
private:
  Separator (); // forbid standard constructor
  Separator (const Separator&); // forbid copy constructor
  Separator (Evasxx::Object &parent); // private construction -> use factory ()
  ~Separator (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_TEMPLATE_H
