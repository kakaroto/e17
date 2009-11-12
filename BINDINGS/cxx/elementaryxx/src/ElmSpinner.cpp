#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmSpinner.h"

using namespace std;

namespace efl {

ElmSpinner::ElmSpinner (EvasObject &parent)
{
  o = elm_spinner_add (parent.obj ());
  
  elmInit ();
}

ElmSpinner::~ElmSpinner () {}

ElmSpinner *ElmSpinner::factory (EvasObject &parent)
{
  return new ElmSpinner (parent);
}

void ElmSpinner::setLabelFormat (const std::string &format)
{
  elm_spinner_label_format_set (o, format.c_str ());
}

const string ElmSpinner::getLabelFormat ()
{
  return elm_spinner_label_format_get (o);
}

void ElmSpinner::setMinMax (double min, double max)
{
  elm_spinner_min_max_set (o, min, max);
}

void ElmSpinner::setStep (double step)
{
  elm_spinner_step_set (o, step);
}

void ElmSpinner::setValue (double val)
{
  elm_spinner_value_set (o, val);
}

double ElmSpinner::getValue ()
{
  return elm_spinner_value_get (o);
}

void ElmSpinner::setWrap (bool wrap)
{

  elm_spinner_wrap_set (o, wrap);
}

} // end namespace efl
