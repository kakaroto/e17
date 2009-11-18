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
  enum Orientation
  {
    Horizontal,
    Vertical
  };
  
  static ElmProgressbar *factory (EvasObject &parent);
  
  void setLabel (const std::string &label);
  void setIcon (EvasObject &icon);
  void setSpanSize (Evas_Coord size);
  void setOrientation (ElmProgressbar::Orientation orient);
  void setInverted (bool inverted);
  void setPulse (bool pulse);
  void pulse (bool state);
  
  /*!
   * Set the format string of the unit area
   *
   * Sets the format string for the unit text. The unit text is provided a 
   * floating point value, so the unit text can display up to 1 floating point 
   * falue. Note that this is optional. Use a format string such as 
   * "%1.2f meters" for example.
   *
   * @param format The format string for the units display
   *
   * @ingroup Progressbar
   */
  void setUnitFormat (const std::string &format);
  
  /*
   * Hide the unit area display
   */
  void hideUnitFormat ();
  
  void setValue (double val);
  double getValue ();
  
private:
  ElmProgressbar (); // forbid standard constructor
  ElmProgressbar (const ElmProgressbar&); // forbid copy constructor
  ElmProgressbar (EvasObject &parent); // private construction -> use factory ()
  ~ElmProgressbar (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace efl

#endif // ELM_PROGRESSBAR_H
