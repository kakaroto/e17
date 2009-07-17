#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmBox.h"

using namespace std;

namespace efl {

ElmBox::ElmBox (EvasObject &parent)
{
  o = elm_box_add (parent.obj ());
  
  elmInit ();
}

ElmBox::~ElmBox () {}

ElmBox *ElmBox::factory (EvasObject &parent)
{
  return new ElmBox (parent);
}

void ElmBox::setHorizontal (bool horizontal)
{
  elm_box_horizontal_set (o, horizontal);
}

void ElmBox::setHomogenous (bool homogenous)
{
  elm_box_homogenous_set (o, homogenous);
}

void ElmBox::packStart (const EvasObject &subobj)
{
  elm_box_pack_start (o, subobj.obj ());
}

void ElmBox::packEnd (const EvasObject &subobj)
{
  elm_box_pack_end (o, subobj.obj ());
}

void ElmBox::packBefore (const EvasObject &subobj, const EvasObject &before)
{
  elm_box_pack_before (o, subobj.obj (), before.obj ());
}

void ElmBox::packAfter (const EvasObject &subobj, const EvasObject &after)
{
  elm_box_pack_after (o, subobj.obj (), after.obj ());
}

} // end namespace efl
