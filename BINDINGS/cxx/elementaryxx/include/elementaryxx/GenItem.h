#ifndef ELMXX_GEN_ITEM_H
#define ELMXX_GEN_ITEM_H

/* EFL */
#include <Elementary.h>

/* EFLxx */
#include <evasxx/Evasxx.h>
#include "GenListColumnConstructor.h"

namespace Elmxx {

/* forward declarations */
class Gen;
class GenDataModel;

// TODO: currently this has many genlist functions inside. I'll fix this correct after the C API is finished!
class GenItem
{
public:
  friend class Gen;
  friend class GenList;

  virtual ~GenItem ();
  
  void clearSubItems ();
  void setSelected (bool selected);
  bool getSelected () const;
  void setExpanded (bool expanded);
  bool getExpanded () const;
  void setDisabled (bool disabled);
  bool getDisabled () const;
  void setDisplayOnly (bool displayOnly);
  bool getDisplayOnly () const;
  void show ();
  void bringIn ();
  void showTop ();
  void bringInTop ();
  void showMiddle ();
  void bringInMiddle ();
  void update ();
  
  const Evasxx::Object *getEvasObject ();

  static GenItem *wrap (Elm_Genlist_Item &item, GenDataModel &model);

  static GenItem *objectLink (const Elm_Genlist_Item *item);
  
private:
  GenItem (Elm_Genlist_Item *item);

  const void *getData ();
  void setData (const void *data);
  
  Elm_Gen_Item *mItem;
  GenDataModel *mDataModel;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_ITEM_H

