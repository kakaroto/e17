#ifndef ELMXX_GEN_LIST_ITEM_H
#define ELMXX_GEN_LIST_ITEM_H

/* EFL */
#include <Elementary.h>

/* EFLxx */
#include <evasxx/Evasxx.h>
#include "GenListColumnConstructor.h"
#include "GenItem.h"

namespace Elmxx {

/* forward declarations */
class Gen;
class GenDataModel;

class GenListItem : public GenItem
{
public:
  friend class Gen;
  friend class GenList;

  virtual ~GenListItem ();

  void clearSubItems ();
  void setSelected (bool selected);
  bool getSelected () const;
  void setExpanded (bool expanded);
  bool getExpanded () const;
  void setDisabled (bool disabled);
  bool getDisabled () const;
  void setSelectMode (Elm_Object_Select_Mode mode);
  Elm_Object_Select_Mode getSelectMode () const;
  void show (Elm_Genlist_Item_Scrollto_Type type);
  void bringIn (Elm_Genlist_Item_Scrollto_Type type);
  void update ();

  static GenListItem *wrap (Elm_Object_Item &item, GenDataModel &model);

  static GenListItem *objectLink (const Elm_Object_Item *item);


private:
  GenListItem (Elm_Object_Item *item);

  const void *getData ();
  void setData (void *data);

  Elm_Object_Item *mItem;
  GenDataModel *mDataModel;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_ITEM_H

