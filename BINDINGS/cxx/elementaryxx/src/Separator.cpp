#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Separator.h"

using namespace std;

namespace Elmxx {

Separator::Separator (Evasxx::Object &parent)
{
  o = elm_separator_add (parent.obj ());
  
  elmInit ();
}

Separator::~Separator () {}

Separator *Separator::factory (Evasxx::Object &parent)
{
  return new Separator (parent);
}

void Separator::setOrientation (Separator::Orientation orient)
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

Separator::Orientation Separator::getOrientation ()
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

} // end namespace Elmxx
