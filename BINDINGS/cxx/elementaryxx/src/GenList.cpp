#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* Project */
#include "../include/elementaryxx/GenList.h"
#include "../include/elementaryxx/GenListItem.h"
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
}

GenList *GenList::factory (Evasxx::Object &parent)
{
  return new GenList (parent);
}

void GenList::clear ()
{
  elm_genlist_clear (o);
}

void GenList::setMultiSelect (bool multi)
{
  elm_genlist_multi_select_set (o, multi);
}

bool GenList::getMultiSelect ()
{
  return elm_genlist_multi_select_get (o);
}

void GenList::setHorizontalMode (Elm_List_Mode mode)
{
  elm_genlist_horizontal_mode_set (o, mode);
}

Elm_List_Mode GenList::getHorizontalMode ()
{
  return elm_genlist_horizontal_mode_get (o);
}

void GenList::setAlwaysSelectMode (bool alwaysSelect)
{
  elm_genlist_always_select_mode_set (o, alwaysSelect);
}

bool GenList::getAlwaysSelectMode ()
{
  return elm_genlist_always_select_mode_get (o);
}

void GenList::setNoSelectMode (bool noSelect)
{
  elm_genlist_no_select_mode_set (o, noSelect);
}

bool GenList::getNoSelectMode ()
{
  return elm_genlist_no_select_mode_get (o);
}

void GenList::setCompressMode (bool compress)
{
  elm_genlist_compress_mode_set (o, compress);
}

bool GenList::getCompressMode ()
{
  return elm_genlist_compress_mode_get (o);
}

void GenList::setBounce (bool hBounce, bool vBounce)
{
  elm_genlist_bounce_set (o, hBounce, vBounce);
}

void GenList::getBounce (bool &hBounceOut, bool &vBounceOut)
{
  Eina_Bool h, v;
  elm_genlist_bounce_get (o, &h, &v);
  hBounceOut = h;
  vBounceOut = v;
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

void GenList::setDataModel (GenListDataModel &model)
{
  mModel = &model;
}

void GenList::gl_sel (void *data, Evas_Object *obj, void *event_info)
{
  GenListColumnSelector *selection = (GenListColumnSelector*) data;
  assert (selection);
  GenList *gl = selection->mGenList;
  assert (gl);
  Evasxx::Object *eo = Evasxx::Object::objectLink (obj);
  assert (eo);
  gl->glSelected (*eo, event_info);
}

void GenList::glSelected (Evasxx::Object &eo, void *event_info)
{
  cout << "GenList::glSelected" << endl;
  // FIXME: this call seems to segfault after the list is constructed after 2 or 3 seconds
  //signalSelect.emit (eo, event_info);
}

/* operations to add items */

void GenList::append (GenListColumnConstructor *construction, GenListItem *parent, Elm_Genlist_Item_Flags flags, GenListColumnSelector *selection)
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

  gli = elm_genlist_item_append (o, &mModel->mGLIC,
                                 construction /* item data */,
                                 parent ? parent->mItem : NULL /* parent */,
                                 flags,
                                 GenList::gl_sel/* func */,
                                 selection /* func data */);

  if (internalConstruction)
  {
    delete construction;
  }
  if (internalSelection)
  {
      mInternalSelList.push_back (selection);
  }
}

} // end namespace Elmxx
