#ifndef ELM_TEMPLATE_H
#define ELM_TEMPLATE_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 */
class ElmSeparator : public ElmObject
{
public:
  enum Orientation
  {
    Horizontal,
    Vertical
  };
  
  static ElmSeparator *factory (EvasObject &parent);
    
  void setOrientation (ElmSeparator::Orientation orient);
  ElmSeparator::Orientation getOrientation ();
  
private:
  ElmSeparator (); // forbid standard constructor
  ElmSeparator (const ElmSeparator&); // forbid copy constructor
  ElmSeparator (EvasObject &parent); // private construction -> use factory ()
  ~ElmSeparator (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace efl

#endif // ELM_TEMPLATE_H
