#ifndef ELM_LISTITEM_H
#define ELM_LISTITEM_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include <evasxx/EvasObject.h> // TODO: forward declaration?

// TODO: think about List Design!!

namespace efl {

class ElmListItem
{
public:
  virtual ~ElmListItem ();

  static ElmListItem *factory ();
  
  void setSelected (bool selected);

  void show ();

  void* getData ();

  EvasObject *getIcon (); //FIXME

  EvasObject *getEnd (); //FIXME

  EvasObject *getBase (); //FIXME

  const std::string getLabel ();

  ElmListItem *prev (); //FIXME

  ElmListItem *next (); //FIXME

private:
  ElmListItem (); // forbid standard constructor
  ElmListItem (const ElmListItem&); // forbid copy constructor
  //ElmListItem (EvasObject &parent); // private construction -> use factory ()
  
  Elm_List_Item *mItem;
};

#if 0


   EAPI void         elm_list_item_del(Elm_List_Item *item);
   EAPI void         elm_list_item_del_cb_set(Elm_List_Item *item, void (*func)(void *data, Evas_Object *obj, void *event_info));
   EAPI const void  *elm_list_item_data_get(const Elm_List_Item *item);
   EAPI Evas_Object *elm_list_item_icon_get(const Elm_List_Item *item);
   EAPI Evas_Object *elm_list_item_end_get(const Elm_List_Item *item);
   EAPI Evas_Object *elm_list_item_base_get(const Elm_List_Item *item);
   EAPI const char  *elm_list_item_label_get(const Elm_List_Item *item);
   EAPI Elm_List_Item *elm_list_item_prev(const Elm_List_Item *it);
   EAPI Elm_List_Item *elm_list_item_next(const Elm_List_Item *it);

#endif

} // end namespace efl

#endif // ELM_LISTITEM_H
