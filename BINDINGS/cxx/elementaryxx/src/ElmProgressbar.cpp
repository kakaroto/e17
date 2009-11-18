#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmProgressbar.h"

using namespace std;

namespace efl {

ElmProgressbar::ElmProgressbar (EvasObject &parent)
{
  o = elm_progressbar_add (parent.obj ());
  
  elmInit ();
}

ElmProgressbar::~ElmProgressbar () {}

ElmProgressbar *ElmProgressbar::factory (EvasObject &parent)
{
  return new ElmProgressbar (parent);
}

void ElmProgressbar::setLabel (const std::string &label)
{
  elm_progressbar_label_set (o, label.c_str ());
}

void ElmProgressbar::setIcon (EvasObject &icon)
{
  elm_progressbar_icon_set (o, icon.obj ());
}

void ElmProgressbar::setSpanSize (Evas_Coord size)
{
  elm_progressbar_span_size_set (o, size);
}

void ElmProgressbar::setOrientation (ElmProgressbar::Orientation orient)
{
  switch (orient)
  {
    case Horizontal:
      elm_progressbar_horizontal_set (o, true);
      break;
    case Vertical:
      elm_progressbar_horizontal_set (o, false);
      break;
  }
}

void ElmProgressbar::setInverted (bool inverted)
{
  elm_progressbar_inverted_set (o, inverted);
}

void ElmProgressbar::setPulse (bool pulse)
{
  elm_progressbar_pulse_set (o, pulse);
}

void ElmProgressbar::pulse (bool state)
{
  elm_progressbar_pulse (o, state);
}

void ElmProgressbar::setUnitFormat (const std::string &format)
{
  elm_progressbar_unit_format_set (o, format.c_str ());
}

void ElmProgressbar::hideUnitFormat ()
{
  elm_progressbar_unit_format_set (o, NULL);
}

void ElmProgressbar::setValue (double val)
{
  elm_progressbar_value_set (o, val);
}

double ElmProgressbar::getValue ()
{
  return elm_progressbar_value_get (o);
}

} // end namespace efl
