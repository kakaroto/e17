#ifndef ELM_PROGRESSBAR_H
#define ELM_PROGRESSBAR_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 *
 * available item styles:
 * default
 * wheel (simple style, no text, no progression, only pulse is available)
 */
class ElmProgressbar : public ElmObject
{
public:
  static ElmProgressbar *factory (EvasObject &parent);

private:
  ElmProgressbar (); // forbid standard constructor
  ElmProgressbar (const ElmProgressbar&); // forbid copy constructor
  ElmProgressbar (EvasObject &parent); // private construction -> use factory ()
  ~ElmProgressbar (); // forbid direct delete -> use ElmWidget::destroy()
  
public:
  void setLabel (const std::string &label);
  void setIcon (EvasObject &icon);
  void setSpanSize (Evas_Coord size);
  void setHorizontal (bool horizontal);
  void setInverted (bool inverted);
  void setPulse (bool pulse);
  void pulse (bool state);
  void setUnitFormat (const std::string &format);
  void setValue (double val);
  double getValue ();
};

} // end namespace efl

#endif // ELM_PROGRESSBAR_H
