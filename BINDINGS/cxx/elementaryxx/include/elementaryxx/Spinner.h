#ifndef ELMXX_SPINNER_H
#define ELMXX_SPINNER_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "changed" - when the spinner value changes
 * "delay,changed" - when the spinner value changed, but a small time after a change (use this if you only want to respond to a change once the spinner is held still for a short while).
 */
class Spinner : public Object
{
public:
  static Spinner *factory (Evasxx::Object &parent);

  void setLabelFormat (const std::string &format);
  const string getLabelFormat ();
  void setMinMax (double min, double max);
  void setStep (double step);
  void setValue (double val);
  double getValue ();
  void setWrap (bool wrap);
  
private:
  Spinner (); // forbid standard constructor
  Spinner (const Spinner&); // forbid copy constructor
  Spinner (Evasxx::Object &parent); // private construction -> use factory ()
  ~Spinner (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_SPINNER_H
