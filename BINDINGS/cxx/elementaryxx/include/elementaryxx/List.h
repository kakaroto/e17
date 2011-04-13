#ifndef ELMXX_LIST_H
#define ELMXX_LIST_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "clicked,double" - when the user double-clicked an item
 * "selected" - when the user selected an item
 * "unselected" - when the user unselected an item
 */
class List : public Object
{
public:  
  static List *factory (Evasxx::Object &parent);

  void clear ();

  void go ();

  void setMultiSelect (bool multi);

  void setHorizontal (bool horizontal);

  void setAlwaysSelectMode (bool alwaysSelect);

  const Eina_List *getItems (); // TODO: port Eina_List

  Elm_List_Item *getSelectedItem (); // TODO port Elm_List_Item

  const Eina_List *getSelectedItems (); // TODO: port Eina_List

  Elm_List_Item *append (const std::string &label, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
  
  Elm_List_Item *append (const std::string &label, const Evasxx::Object &icon, const Evasxx::Object &end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
  
private:
  List (); // forbid standard constructor
  List (const List&); // forbid copy constructor
  List (Evasxx::Object &parent); // private construction -> use factory ()
  ~List (); // forbid direct delete -> use Object::destroy()
};

#if 0
   EAPI Elm_List_Item *elm_list_item_append(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
   EAPI Elm_List_Item *elm_list_item_prepend(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
   EAPI Elm_List_Item *elm_list_item_insert_before(Evas_Object *obj, Elm_List_Item *before, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
   EAPI Elm_List_Item *elm_list_item_insert_after(Evas_Object *obj, Elm_List_Item *after, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
#endif

} // end namespace Elmxx

#endif // ELMXX_LIST_H
