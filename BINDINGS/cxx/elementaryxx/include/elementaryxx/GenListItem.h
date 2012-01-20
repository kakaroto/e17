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

  static GenListItem *wrap (Elm_Object_Item &item, GenDataModel &model);

  static GenListItem *objectLink (const Elm_Object_Item *item);


private:
  GenListItem (Elm_Object_Item *item);

  const void *getData ();
  void setData (const void *data);

  Elm_Object_Item *mItem;
  GenDataModel *mDataModel;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_ITEM_H

