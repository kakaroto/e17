#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmSeparator.h"

using namespace std;

namespace efl {

ElmSeparator::ElmSeparator (EvasObject &parent)
{
  o = elm_separator_add (parent.obj ());
  
  elmInit ();
}

ElmSeparator::~ElmSeparator () {}

ElmSeparator *ElmSeparator::factory (EvasObject &parent)
{
  return new ElmSeparator (parent);
}

void ElmSeparator::setOrientation (ElmSeparator::Orientation orient)
{
  switch (orient)
  {
    case Horizontal:
      elm_separator_horizontal_set (o, true);
      break;
    case Vertical:
      elm_separator_horizontal_set (o, false);
      break;
  }
}

ElmSeparator::Orientation ElmSeparator::getOrientation ()
{
  bool horizontal = elm_separator_horizontal_get (o);

  switch (horizontal)
  {
    case true:
      return Horizontal;
      break;
    case false:
      return Vertical;
      break;
  }
}

} // end namespace efl
