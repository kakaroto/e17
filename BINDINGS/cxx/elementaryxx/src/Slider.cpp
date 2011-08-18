#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Slider.h"

using namespace std;

namespace Elmxx {

Slider::Slider (Evasxx::Object &parent)
{
  o = elm_slider_add (parent.obj ());
  
  elmInit ();
}

Slider::~Slider () {}

Slider *Slider::factory (Evasxx::Object &parent)
{
  return new Slider (parent);
}

void Slider::setLabel (const std::string &label)
{
  elm_object_text_set (o, label.c_str ());
}

void Slider::setIcon (const Evasxx::Object &icon)
{
  elm_slider_icon_set (o, icon.obj ());
}

void Slider::setSpanSize (Evas_Coord size)
{
  elm_slider_span_size_set (o, size);
}

void Slider::setUnitFormat (const std::string &format)
{
  elm_slider_unit_format_set (o, format.c_str ());
}

void Slider::setIndicatorFormat (const std::string &indicator)
{
  elm_slider_indicator_format_set (o, indicator.c_str ());
}

void Slider::setOrientation (Slider::Orientation orient)
{
  switch (orient)
  {
    case Horizontal:
      elm_slider_horizontal_set (o, true);
      break;
    case Vertical:
      elm_slider_horizontal_set (o, false);
      break;
  }
}

void Slider::setMinMax (double min, double max)
{
  elm_slider_min_max_set (o, min, max);
}

void Slider::setValue (double val)
{
  elm_slider_value_set (o, val);
}

double Slider::getValue () const
{
  return elm_slider_value_get (o);
}

void Slider::setInverted (bool inverted)
{
  elm_slider_inverted_set (o, inverted);
}

} // end namespace Elmxx
