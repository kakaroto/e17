#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Table.h"

using namespace std;

namespace Elmxx {

Table::Table (Evasxx::Object &parent)
{
  o = elm_table_add (parent.obj ());
  
  elmInit ();
}

Table::~Table () {}

Table *Table::factory (Evasxx::Object &parent)
{
  return new Table (parent);
}

void Table::setHomogeneous (bool homogeneous)
{
  elm_table_homogeneous_set (o, homogeneous);
}

void Table::setHomogenous (bool homogenous)
{
  elm_table_homogeneous_set (o, homogenous);
}

void Table::pack (const Evasxx::Object &subobj, const Eflxx::Rect &rect)
{
  elm_table_pack (o, subobj.obj (), rect.x (), rect.y (), rect.width (), rect.height ());
}

} // end namespace Elmxx
