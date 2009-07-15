#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmListItem.h"

using namespace std;

namespace efl {

ElmListItem::ElmListItem ()
{

}

ElmListItem::~ElmListItem ()
{
  // delete?
}

ElmListItem *ElmListItem::factory ()
{
  return new ElmListItem ();
}

void ElmListItem::setSelected (bool selected)
{
  elm_list_item_selected_set (mItem, selected);
}

void ElmListItem::show ()
{
  elm_list_item_show (mItem);
}

void* ElmListItem::getData ()
{

}

EvasObject *ElmListItem::getIcon ()
{

}

EvasObject *ElmListItem::getEnd ()
{

}

EvasObject *ElmListItem::getBase ()
{

}

const std::string ElmListItem::getLabel ()
{

}

ElmListItem *ElmListItem::prev ()
{

}

ElmListItem *ElmListItem::next ()
{

}

} // end namespace efl
