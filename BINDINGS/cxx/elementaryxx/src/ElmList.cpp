#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmList.h"

using namespace std;

namespace efl {

ElmList::ElmList (EvasObject &parent)
{
  o = elm_list_add (parent.obj ());
  
  elmInit ();
}

ElmList::~ElmList ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmList *ElmList::factory (EvasObject &parent)
{
  return new ElmList (parent);
}

void ElmList::clear ()
{
  elm_list_clear (o);
}

void ElmList::go ()
{
  elm_list_go (o);
}

void ElmList::setMultiSelect (bool multi)
{
  elm_list_multi_select_set (o, multi);
}

void ElmList::setHorizontalMode (Elm_List_Mode mode)
{
  elm_list_horizontal_mode_set (o, mode);
}

void ElmList::setAlwaysSelectMode (bool alwaysSelect)
{
  elm_list_always_select_mode_set (o, alwaysSelect);
}

const Eina_List *ElmList::getItems ()
{
  return elm_list_items_get (o);
}

Elm_List_Item *ElmList::getSelectedItem ()
{
  return elm_list_selected_item_get (o);
}

const Eina_List *ElmList::getSelectedItems ()
{
  return elm_list_selected_items_get (o);
}

Elm_List_Item *ElmList::append (const std::string &label, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data)
{
  return elm_list_item_append (o, label.c_str (), NULL, NULL, func, data);
}

Elm_List_Item *ElmList::append (const std::string &label, const EvasObject &icon, const EvasObject &end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data)
{
  return elm_list_item_append (o, label.c_str (), icon.obj (), end.obj (), func, data);
}

} // end namespace efl
