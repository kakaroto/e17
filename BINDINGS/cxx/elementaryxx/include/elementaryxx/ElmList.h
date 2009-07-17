#ifndef ELM_LIST_H
#define ELM_LIST_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "clicked" - when the user double-clicked an item
 * "selected" - when the user selected an item
 * "unselected" - when the user unselected an item
 */
class ElmList : public ElmWidget
{
public:  
  static ElmList *factory (EvasObject &parent);

  void clear ();

  void go ();

  void setMultiSelect (bool multi);

  void setHorizontalMode (Elm_List_Mode mode);

  void setAlwaysSelectMode (bool alwaysSelect);

  const Eina_List *getItems (); // TODO: port Eina_List

  Elm_List_Item *getSelectedItem (); // TODO port Elm_List_Item

  const Eina_List *getSelectedItems (); // TODO: port Eina_List

  Elm_List_Item *append (const std::string &label, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
  
  Elm_List_Item *append (const std::string &label, const EvasObject &icon, const EvasObject &end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
  
private:
  ElmList (); // forbid standard constructor
  ElmList (const ElmList&); // forbid copy constructor
  ElmList (EvasObject &parent); // private construction -> use factory ()
  ~ElmList (); // forbid direct delete -> use ElmWidget::destroy()
};

#if 0
   EAPI Elm_List_Item *elm_list_item_append(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
   EAPI Elm_List_Item *elm_list_item_prepend(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
   EAPI Elm_List_Item *elm_list_item_insert_before(Evas_Object *obj, Elm_List_Item *before, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
   EAPI Elm_List_Item *elm_list_item_insert_after(Evas_Object *obj, Elm_List_Item *after, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
#endif

} // end namespace efl

#endif // ELM_LIST_H
