#ifndef ELM_SPINNER_H
#define ELM_SPINNER_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 * "changed" - when the spinner value changes
 * "delay,changed" - when the spinner value changed, but a small time after a change (use this if you only want to respond to a change once the spinner is held still for a short while).
 */
class ElmSpinner : public ElmObject
{
public:
  static ElmSpinner *factory (EvasObject &parent);

private:
  ElmSpinner (); // forbid standard constructor
  ElmSpinner (const ElmSpinner&); // forbid copy constructor
  ElmSpinner (EvasObject &parent); // private construction -> use factory ()
  ~ElmSpinner (); // forbid direct delete -> use ElmWidget::destroy()

  void setLabelFormat (const std::string &format);
  const string getLabelFormat ();
  void setMinMax (double min, double max);
  void setStep (double step);
  void setValue (double val);
  double getValue ();
  void setWrap (bool wrap);
};

} // end namespace efl

#endif // ELM_SPINNER_H
