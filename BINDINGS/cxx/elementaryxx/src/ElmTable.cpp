#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmTable.h"

using namespace std;

namespace efl {

ElmTable::ElmTable (EvasObject &parent)
{
  o = elm_table_add (parent.obj ());
  
  elmInit ();
}

ElmTable::~ElmTable () {}

ElmTable *ElmTable::factory (EvasObject &parent)
{
  return new ElmTable (parent);
}

void ElmTable::setHomogenous (bool homogenous)
{
  elm_table_homogenous_set (o, homogenous);
}

void ElmTable::pack (const EvasObject &subobj, const Rect &rect)
{
  elm_table_pack (o, subobj.obj (), rect.x (), rect.y (), rect.width (), rect.height ());
}

} // end namespace efl
