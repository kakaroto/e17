#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmSlider.h"

using namespace std;

namespace efl {

ElmSlider::ElmSlider (EvasObject &parent)
{
  o = elm_slider_add (parent.obj ());
  
  elmInit ();
}

ElmSlider::~ElmSlider () {}

ElmSlider *ElmSlider::factory (EvasObject &parent)
{
  return new ElmSlider (parent);
}

void ElmSlider::setLabel (const std::string &label)
{
  elm_slider_label_set (o, label.c_str ());
}

void ElmSlider::setIcon (const EvasObject &icon)
{
  elm_slider_icon_set (o, icon.obj ());
}

void ElmSlider::setSpanSize (Evas_Coord size)
{
  elm_slider_span_size_set (o, size);
}

void ElmSlider::setUnitFormat (const std::string &format)
{
  elm_slider_unit_format_set (o, format.c_str ());
}

void ElmSlider::setIndicatorFormat (const std::string &indicator)
{
  elm_slider_indicator_format_set (o, indicator.c_str ());
}

void ElmSlider::setOrientation (ElmSlider::Orientation orient)
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

void ElmSlider::setMinMax (double min, double max)
{
  elm_slider_min_max_set (o, min, max);
}

void ElmSlider::setValue (double val)
{
  elm_slider_value_set (o, val);
}

double ElmSlider::getValue () const
{
  return elm_slider_value_get (o);
}

void ElmSlider::setInverted (bool inverted)
{
  elm_slider_inverted_set (o, inverted);
}

} // end namespace efl
