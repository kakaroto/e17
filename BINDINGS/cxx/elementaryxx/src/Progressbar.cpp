#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Progressbar.h"

using namespace std;

namespace Elmxx {

Progressbar::Progressbar (Evasxx::Object &parent)
{
  o = elm_progressbar_add (parent.obj ());
  
  elmInit ();
}

Progressbar::~Progressbar () {}

Progressbar *Progressbar::factory (Evasxx::Object &parent)
{
  return new Progressbar (parent);
}

void Progressbar::setLabel (const std::string &label)
{
  elm_object_text_set (o, label.c_str ());
}

void Progressbar::setIcon (Evasxx::Object &icon)
{
  elm_progressbar_icon_set (o, icon.obj ());
}

void Progressbar::setSpanSize (Evas_Coord size)
{
  elm_progressbar_span_size_set (o, size);
}

void Progressbar::setOrientation (Progressbar::Orientation orient)
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

void Progressbar::setInverted (bool inverted)
{
  elm_progressbar_inverted_set (o, inverted);
}

void Progressbar::setPulse (bool pulse)
{
  elm_progressbar_pulse_set (o, pulse);
}

void Progressbar::pulse (bool state)
{
  elm_progressbar_pulse (o, state);
}

void Progressbar::setUnitFormat (const std::string &format)
{
  elm_progressbar_unit_format_set (o, format.c_str ());
}

void Progressbar::hideUnitFormat ()
{
  elm_progressbar_unit_format_set (o, NULL);
}

void Progressbar::setValue (double val)
{
  elm_progressbar_value_set (o, val);
}

double Progressbar::getValue ()
{
  return elm_progressbar_value_get (o);
}

} // end namespace Elmxx
