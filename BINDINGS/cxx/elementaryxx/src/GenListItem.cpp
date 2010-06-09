#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* Project */
#include "../include/elementaryxx/GenListItem.h"
#include "../include/elementaryxx/GenListDataModel.h"

/* STD */
#include <iostream>
#include <cassert>

using namespace std;

namespace Elmxx {

GenListItem::GenListItem (Elm_Genlist_Item *item) :
  mItem (item)
{

}
  
GenListItem::~GenListItem ()
{
  cout << "GenListItem::~GenListItem" << endl;
  elm_genlist_item_del (mItem);
}

void GenListItem::clearSubItems ()
{
  elm_genlist_item_subitems_clear (mItem);
}

void GenListItem::setSelected (bool selected)
{
  elm_genlist_item_selected_set (mItem, selected);
}

bool GenListItem::getSelected () const
{
  return elm_genlist_item_selected_get (mItem);
}

void GenListItem::setExpanded (bool expanded)
{
  elm_genlist_item_expanded_set (mItem, expanded);
}

bool GenListItem::getExpanded () const
{
  return elm_genlist_item_expanded_get (mItem);
}

void GenListItem::setDisabled (bool disabled)
{
  elm_genlist_item_disabled_set (mItem, disabled);
}

bool GenListItem::getDisabled () const
{
  return elm_genlist_item_disabled_get (mItem);
}

void GenListItem::setDisplayOnly (bool displayOnly)
{
  elm_genlist_item_display_only_set (mItem, displayOnly);
}

bool GenListItem::getDisplayOnly () const
{
  return elm_genlist_item_display_only_get (mItem);
}

void GenListItem::show ()
{
  elm_genlist_item_show (mItem);
}

void GenListItem::bringIn ()
{
  elm_genlist_item_bring_in (mItem);
}

void GenListItem::showTop ()
{
  elm_genlist_item_top_show (mItem);
}

void GenListItem::bringInTop ()
{
  elm_genlist_item_top_bring_in (mItem);
}

void GenListItem::showMiddle ()
{
  elm_genlist_item_middle_show (mItem);
}

void GenListItem::bringInMiddle ()
{
  elm_genlist_item_middle_bring_in (mItem);
}

void GenListItem::update ()
{
  elm_genlist_item_update (mItem);
}

const Evasxx::Object *GenListItem::getEvasObject ()
{
  const Evas_Object *obj = elm_genlist_item_object_get (mItem);
  return Evasxx::Object::objectLink (obj);
}

GenListItem *GenListItem::wrap (Elm_Genlist_Item &item, GenListDataModel &model)
{
  GenListItem *genItem = new GenListItem (&item);
  genItem->mDataModel = &model;
  //model.signalDel.connect (sigc::mem_fun (genItem, &GenListItem::destroy));

  return genItem;
}

GenListItem *GenListItem::objectLink (const Elm_Genlist_Item *item)
{
  //GenListItem *item2 = static_cast <GenListItem*> (const_cast <void*> (elm_genlist_item_data_get(item)));
  //assert (item2);
  return NULL;//item2;
}

const void *GenListItem::getData ()
{
  return elm_genlist_item_data_get (mItem);
}

void GenListItem::setData (const void *data)
{
  elm_genlist_item_data_set (mItem, data);
}
  
} // end namespace Elmxx
