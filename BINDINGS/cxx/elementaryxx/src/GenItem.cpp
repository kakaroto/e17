#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* Project */
#include "../include/elementaryxx/GenItem.h"
#include "../include/elementaryxx/GenDataModel.h"

/* STD */
#include <iostream>
#include <cassert>

using namespace std;

namespace Elmxx {

GenItem::GenItem (Elm_Genlist_Item *item) :
  mItem (item)
{

}
  
GenItem::~GenItem ()
{
  cout << "GenItem::~GenItem" << endl;
  elm_genlist_item_del (mItem);
}

void GenItem::clearSubItems ()
{
  elm_genlist_item_subitems_clear (mItem);
}

void GenItem::setSelected (bool selected)
{
  elm_gen_item_selected_set (mItem, selected);
}

bool GenItem::getSelected () const
{
  return elm_gen_item_selected_get (mItem);
}

void GenItem::setExpanded (bool expanded)
{
  elm_genlist_item_expanded_set (mItem, expanded);
}

bool GenItem::getExpanded () const
{
  return elm_genlist_item_expanded_get (mItem);
}

void GenItem::setDisabled (bool disabled)
{
  elm_genlist_item_disabled_set (mItem, disabled);
}

bool GenItem::getDisabled () const
{
  return elm_genlist_item_disabled_get (mItem);
}

void GenItem::setDisplayOnly (bool displayOnly)
{
  elm_genlist_item_display_only_set (mItem, displayOnly);
}

bool GenItem::getDisplayOnly () const
{
  return elm_genlist_item_display_only_get (mItem);
}

void GenItem::show ()
{
  elm_genlist_item_show (mItem);
}

void GenItem::bringIn ()
{
  elm_genlist_item_bring_in (mItem);
}

void GenItem::showTop ()
{
  elm_genlist_item_top_show (mItem);
}

void GenItem::bringInTop ()
{
  elm_genlist_item_top_bring_in (mItem);
}

void GenItem::showMiddle ()
{
  elm_genlist_item_middle_show (mItem);
}

void GenItem::bringInMiddle ()
{
  elm_genlist_item_middle_bring_in (mItem);
}

void GenItem::update ()
{
  elm_genlist_item_update (mItem);
}

const Evasxx::Object *GenItem::getEvasObject ()
{
  const Evas_Object *obj = elm_genlist_item_object_get (mItem);
  return Evasxx::Object::objectLink (obj);
}

GenItem *GenItem::wrap (Elm_Genlist_Item &item, GenDataModel &model)
{
  GenItem *genItem = new GenItem (&item);
  genItem->mDataModel = &model;
  //model.signalDel.connect (sigc::mem_fun (genItem, &GenItem::destroy));

  return genItem;
}

GenItem *GenItem::objectLink (const Elm_Genlist_Item *item)
{
  //GenItem *item2 = static_cast <GenItem*> (const_cast <void*> (elm_genlist_item_data_get(item)));
  //assert (item2);
  return NULL;//item2;
}

const void *GenItem::getData ()
{
  return elm_genlist_item_data_get (mItem);
}

void GenItem::setData (const void *data)
{
  elm_genlist_item_data_set (mItem, data);
}
  
} // end namespace Elmxx
