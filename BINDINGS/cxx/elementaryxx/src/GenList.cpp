#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* Project */
#include "../include/elementaryxx/GenList.h"
#include "../include/elementaryxx/GenListItem.h"
#include "../include/elementaryxx/GenItem.h"
#include "../include/elementaryxx/GenListColumnConstructor.h"
#include "../include/elementaryxx/GenListColumnSelector.h"
#include "localUtil.h"

/* STD */
#include <cassert>

using namespace std;

namespace Elmxx {

GenList::GenList (Evasxx::Object &parent)
{
  o = elm_genlist_add (parent.obj ());
  
  elmInit ();
}

GenList::~GenList ()
{
  delete_stl_container <std::list <GenListColumnSelector*>, GenListColumnSelector*> (mInternalSelList);
  delete_stl_container <std::list <GenListColumnConstructor*>, GenListColumnConstructor*> (mInternalConstructList);
}

GenList *GenList::factory (Evasxx::Object &parent)
{
  return new GenList (parent);
}

void GenList::setMultiSelect (bool multi)
{
  elm_genlist_multi_select_set (o, multi);
}

bool GenList::getMultiSelect ()
{
  return elm_genlist_multi_select_get (o);
}

void GenList::setHorizontal (Elm_List_Mode mode)
{
  elm_genlist_horizontal_set (o, mode);
}

Elm_List_Mode GenList::getHorizontal ()
{
  return elm_genlist_horizontal_get (o);
}

void GenList::setCompressMode (bool compress)
{
  elm_genlist_compress_mode_set (o, compress);
}

bool GenList::getCompressMode ()
{
  return elm_genlist_compress_mode_get (o);
}

void GenList::setHomogeneous (bool homogeneous)
{
  elm_genlist_homogeneous_set (o, homogeneous);
}

bool GenList::getHomogeneous ()
{
  return elm_genlist_homogeneous_get (o);
}

void GenList::setBlockCount  (int n)
{
  elm_genlist_block_count_set (o, n);
}

int GenList::getBlockCound ()
{
  return elm_genlist_block_count_get (o);
}

void GenList::setDataModel (GenDataModel &model)
{
  mModel = &model;
}

void GenList::gl_sel (void *data, Evas_Object *obj, void *event_info)
{
  GenListColumnSelector *selection = static_cast <GenListColumnSelector*> (data);
  assert (selection);
  GenList *gl = selection->mGenList;
  assert (gl);
  Evasxx::Object *eo = Evasxx::Object::objectLink (obj);
  assert (eo);
  gl->glSelected (*selection, *eo, event_info);
}

void GenList::glSelected (GenListColumnSelector &selection, const Evasxx::Object &eo, void *event_info)
{
  signalSelect.emit (selection, eo, event_info);
}

/* operations to add items */

GenItem *GenList::append (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection)
{
  insertInternal (construction, GenList::Append, parent, flags, selection);
}

GenItem *GenList::prepend (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection)
{
  insertInternal (construction, GenList::Prepend, parent, flags, selection);
}

GenItem *GenList::insertBefore (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection)
{
  insertInternal (construction, GenList::InsertBefore, parent, flags, selection);
}

GenItem *GenList::insertAfter (GenListColumnConstructor *construction, const GenItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection)
{
  insertInternal (construction, GenList::InsertAfter, parent, flags, selection);
}

GenItem *GenList::insertInternal (GenListColumnConstructor *construction, GenList::InsertOperation op, const GenItem *opItem, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection)
{
  assert (mModel);
  
  Elm_Genlist_Item *gli;
  bool internalConstruction = false;
  bool internalSelection = false;
  
  if (!construction)
  {
    // create internal construction object if construction==NULL was given and delete if after adding
    // this is needed to provide the user an easy API to add type save data to item append callbacks
    internalConstruction = true;
    construction = new GenListColumnConstructor ();  
  }
  
  if (!selection)
  {
    // create internal construction object if selection==NULL was given and delete if after adding
    // this is needed to provide the user an easy API to add type save data to item append callbacks
    internalSelection = true;
    selection = new GenListColumnSelector ();  
  }
  
  construction->mDataModel = mModel;
  selection->mGenList = this;

  switch (op)
  {
    case Append:
      gli = elm_genlist_item_append (o, &mModel->mGLIC,
                                     construction /* item data */,
                                     opItem ? opItem->mItem : NULL /* parent */,
                                     flags,
                                     GenList::gl_sel/* func */,
                                     selection /* func data */);
      break;
      
    case Prepend:
      gli = elm_genlist_item_prepend (o, &mModel->mGLIC,
                                     construction /* item data */,
                                     opItem ? opItem->mItem : NULL /* parent */,
                                     flags,
                                     GenList::gl_sel/* func */,
                                     selection /* func data */);
      break;
      
    case InsertBefore:
      gli = elm_genlist_item_insert_before (o, &mModel->mGLIC,
                                            construction /* item data */,
                                            opItem ? opItem->mItem : NULL /* parent */,
                                            NULL,
                                            flags,
                                            GenList::gl_sel/* func */,
                                            selection /* func data */);
      break;

    case InsertAfter:
      gli = elm_genlist_item_insert_after (o, &mModel->mGLIC,
                                            construction /* item data */,
                                            opItem ? opItem->mItem : NULL /* parent */,
                                            NULL,
                                            flags,
                                            GenList::gl_sel/* func */,
                                            selection /* func data */);
      break;
  }

  GenListItem *item = GenListItem::wrap (*gli, *mModel); // TODO: generic!!
  
  construction->mGenItem = item;

  //EAPI const void *
  //elm_genlist_item_data_get(const Elm_Genlist_Item *it)
  // -> returns: GenListColumnConstructor *construction
  // 1. add GenItem* to construction

  if (internalConstruction)
  {
    mInternalConstructList.push_back (construction);
  }
  if (internalSelection)
  {
    mInternalSelList.push_back (selection);
  }
  
  return item;
}

void GenList::del (GenItem &item)
{
  elm_genlist_item_del (item.mItem);
}

GenItem *GenList::getItemSelected () const
{
  Elm_Genlist_Item *item = elm_genlist_selected_item_get (o);

  if (!item)
    return NULL;
  
  const GenListColumnConstructor *construction = static_cast <const GenListColumnConstructor*> (elm_genlist_item_data_get (item));
  
  return construction->mGenItem;
}

GenItem *GenList::getItemAtXY (const Eflxx::Point &pos, int &posret) const
{
  Elm_Genlist_Item *item = elm_genlist_at_xy_item_get (o, pos.x (), pos.y (), &posret);

  if (!item)
    return NULL;
  
  const GenListColumnConstructor *construction = static_cast <const GenListColumnConstructor*> (elm_genlist_item_data_get (item));
  
  return construction->mGenItem;
}

GenItem *GenList::getItemFirst () const
{
  Elm_Genlist_Item *item = elm_genlist_first_item_get (o);

  if (!item)
    return NULL;

  const GenListColumnConstructor *construction = static_cast <const GenListColumnConstructor*> (elm_genlist_item_data_get (item));
  
  return construction->mGenItem;
}

GenItem *GenList::getItemLast () const
{
  Elm_Genlist_Item *item = elm_genlist_last_item_get (o);

  if (!item)
    return NULL;

  const GenListColumnConstructor *construction = static_cast <const GenListColumnConstructor*> (elm_genlist_item_data_get (item));
  
  return construction->mGenItem;
}

} // end namespace Elmxx
