#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Box.h"

using namespace std;

namespace Elmxx {

Box::Box (Evasxx::Object &parent)
{
  o = elm_box_add (parent.obj ());
  
  elmInit ();
}

Box::~Box () {}

Box *Box::factory (Evasxx::Object &parent)
{
  return new Box (parent);
}

void Box::setOrientation (Box::Orientation orient)
{
  switch (orient)
  {
    case Horizontal:
      elm_box_horizontal_set (o, true);
      break;
    case Vertical:
      elm_box_horizontal_set (o, false);
      break;
  }
}

void Box::setHomogeneous (bool homogeneous)
{
  elm_box_homogeneous_set (o, homogeneous);
}

void Box::setHomogenous (bool homogenous)
{
  elm_box_homogeneous_set (o, homogenous);
}

void Box::packStart (const Evasxx::Object &subobj)
{
  elm_box_pack_start (o, subobj.obj ());
}

void Box::packEnd (const Evasxx::Object &subobj)
{
  elm_box_pack_end (o, subobj.obj ());
}

void Box::packBefore (const Evasxx::Object &subobj, const Evasxx::Object &before)
{
  elm_box_pack_before (o, subobj.obj (), before.obj ());
}

void Box::packAfter (const Evasxx::Object &subobj, const Evasxx::Object &after)
{
  elm_box_pack_after (o, subobj.obj (), after.obj ());
}

} // end namespace Elmxx
