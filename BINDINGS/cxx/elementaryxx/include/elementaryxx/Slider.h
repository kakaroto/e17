#ifndef ELMXX_SLIDER_H
#define ELMXX_SLIDER_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {
  
/*!
 * smart callbacks called:
 * "changed" - when the slider value changes
 * "delay,changed" - when the slider value changed, but a small time after a 
 *                   change (use this if you only want to respond to a change 
 *                   once the slider is held still for a short while).
 */
class Slider : public Object
{
public:
  enum Orientation
  {
    Horizontal,
    Vertical
  };
  
  static Slider *factory (Evasxx::Object &parent);

  void setLabel (const std::string &label);
  
  void setIcon (const Evasxx::Object &icon);
  
  void setSpanSize (Evas_Coord size);
  
  void setUnitFormat (const std::string &format);
  
  void setIndicatorFormat (const std::string &indicator);
  
  void setOrientation (Slider::Orientation orient);
  
  void setMinMax (double min, double max);
  
  void setValue (double val);
  
  double getValue () const;
  
  void setInverted (bool inverted);

private:
  Slider (); // forbid standard constructor
  Slider (const Slider&); // forbid copy constructor
  Slider (Evasxx::Object &parent); // private construction -> use factory ()
  ~Slider (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_SLIDER_H
