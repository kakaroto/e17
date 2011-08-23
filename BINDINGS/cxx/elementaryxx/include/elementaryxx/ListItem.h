#ifndef ELMXX_LIST_ITEM_H
#define ELMXX_LIST_ITEM_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include <evasxx/Object.h> // TODO: forward declaration?

// TODO: think about List Design!!

namespace Elmxx {

class ListItem
{
public:
  virtual ~ListItem ();

  static ListItem *factory ();
  
  void setSelected (bool selected);

  void show ();

  void* getData ();

  Evasxx::Object *getIcon (); //FIXME

  Evasxx::Object *getEnd (); //FIXME

  Evasxx::Object *getBase (); //FIXME

  const std::string getLabel ();

  ListItem *prev (); //FIXME

  ListItem *next (); //FIXME

private:
  ListItem (); // forbid standard constructor
  ListItem (const ListItem&); // forbid copy constructor
  //ListItem (Evasxx::Object &parent); // private construction -> use factory ()
  
  Elm_List_Item *mItem;
};

#if 0


   EAPI void         elm_list_item_del(Elm_List_Item *item);
   EAPI void         elm_list_item_del_cb_set(Elm_List_Item *item, void (*func)(void *data, Evas_Object *obj, void *event_info));
   EAPI const void  *elm_list_item_data_get(const Elm_List_Item *item);
   EAPI Evas_Object *elm_list_item_icon_get(const Elm_List_Item *item);
   EAPI Evas_Object *elm_list_item_end_get(const Elm_List_Item *item);
   EAPI Evas_Object *elm_list_item_object_get(const Elm_List_Item *item);
   EAPI const char  *elm_list_item_label_get(const Elm_List_Item *item);
   EAPI Elm_List_Item *elm_list_item_prev(const Elm_List_Item *it);
   EAPI Elm_List_Item *elm_list_item_next(const Elm_List_Item *it);

#endif

} // end namespace Elmxx

#endif // ELMXX_LIST_ITEM_H
