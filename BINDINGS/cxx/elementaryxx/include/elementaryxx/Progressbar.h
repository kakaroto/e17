#ifndef ELMXX_PROGRESSBAR_H
#define ELMXX_PROGRESSBAR_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 *
 * available item styles:
 * default
 * wheel (simple style, no text, no progression, only pulse is available)
 */
class Progressbar : public Object
{
public:
  enum Orientation
  {
    Horizontal,
    Vertical
  };
  
  static Progressbar *factory (Evasxx::Object &parent);
  
  void setLabel (const std::string &label);
  void setIcon (Evasxx::Object &icon);
  void setSpanSize (Evas_Coord size);
  void setOrientation (Progressbar::Orientation orient);
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
  Progressbar (); // forbid standard constructor
  Progressbar (const Progressbar&); // forbid copy constructor
  Progressbar (Evasxx::Object &parent); // private construction -> use factory ()
  ~Progressbar (); // forbid direct delete -> use ElmWidget::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_PROGRESSBAR_H
