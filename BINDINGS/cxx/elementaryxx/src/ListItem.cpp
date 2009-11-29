#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ListItem.h"

using namespace std;

namespace Elmxx {

ListItem::ListItem ()
{

}

ListItem::~ListItem () {}

ListItem *ListItem::factory ()
{
  return new ListItem ();
}

void ListItem::setSelected (bool selected)
{
  elm_list_item_selected_set (mItem, selected);
}

void ListItem::show ()
{
  elm_list_item_show (mItem);
}

void* ListItem::getData ()
{

}

Evasxx::Object *ListItem::getIcon ()
{

}

Evasxx::Object *ListItem::getEnd ()
{

}

Evasxx::Object *ListItem::getBase ()
{

}

const std::string ListItem::getLabel ()
{

}

ListItem *ListItem::prev ()
{

}

ListItem *ListItem::next ()
{

}

} // end namespace Elmxx
