#ifndef ELMXX_GEN_LIST_ITEM_H
#define ELMXX_GEN_LIST_ITEM_H

/* EFL */
#include <Elementary.h>

/* EFLxx */
#include <evasxx/Evasxx.h>

namespace Elmxx {

/* forward declarations */
class GenList;
  
class GenListItem
{
public:
  friend class GenList;

  virtual ~GenListItem ();

#if 0
   /* Genlist Item operation */
   EAPI Elm_Genlist_Item *elm_genlist_item_next_get(const Elm_Genlist_Item *item);
   EAPI Elm_Genlist_Item *elm_genlist_item_prev_get(const Elm_Genlist_Item *item);
   EAPI Evas_Object      *elm_genlist_item_genlist_get(const Elm_Genlist_Item *item);
   EAPI Elm_Genlist_Item *elm_genlist_item_parent_get(const Elm_Genlist_Item *it);
#endif
  
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

  static GenListItem *wrap (const Elm_Genlist_Item *item);

  static GenListItem *objectLink (const Elm_Genlist_Item *item);
  
private:
  GenListItem (const Elm_Genlist_Item *item);
  
  Elm_Genlist_Item *mItem;
};

} // end namespace Elmxx

#endif // ELMXX_GEN_LIST_ITEM_H
