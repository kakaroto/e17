#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Spinner.h"

using namespace std;

namespace Elmxx {

Spinner::Spinner (Evasxx::Object &parent)
{
  o = elm_spinner_add (parent.obj ());
  
  elmInit ();
}

Spinner::~Spinner () {}

Spinner *Spinner::factory (Evasxx::Object &parent)
{
  return new Spinner (parent);
}

void Spinner::setLabelFormat (const std::string &format)
{
  elm_spinner_label_format_set (o, format.c_str ());
}

const string Spinner::getLabelFormat ()
{
  return elm_spinner_label_format_get (o);
}

void Spinner::setMinMax (double min, double max)
{
  elm_spinner_min_max_set (o, min, max);
}

void Spinner::setStep (double step)
{
  elm_spinner_step_set (o, step);
}

void Spinner::setValue (double val)
{
  elm_spinner_value_set (o, val);
}

double Spinner::getValue ()
{
  return elm_spinner_value_get (o);
}

void Spinner::setWrap (bool wrap)
{

  elm_spinner_wrap_set (o, wrap);
}

} // end namespace Elmxx
