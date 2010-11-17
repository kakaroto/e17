#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/List.h"

using namespace std;

namespace Elmxx {

List::List (Evasxx::Object &parent)
{
  o = elm_list_add (parent.obj ());
  
  elmInit ();
}

List::~List () {}

List *List::factory (Evasxx::Object &parent)
{
  return new List (parent);
}

void List::clear ()
{
  elm_list_clear (o);
}

void List::go ()
{
  elm_list_go (o);
}

void List::setMultiSelect (bool multi)
{
  elm_list_multi_select_set (o, multi);
}

void List::setHorizontal (bool horizontal)
{
  elm_list_horizontal_set (o, horizontal);
}

void List::setAlwaysSelectMode (bool alwaysSelect)
{
  elm_list_always_select_mode_set (o, alwaysSelect);
}

const Eina_List *List::getItems ()
{
  return elm_list_items_get (o);
}

Elm_List_Item *List::getSelectedItem ()
{
  return elm_list_selected_item_get (o);
}

const Eina_List *List::getSelectedItems ()
{
  return elm_list_selected_items_get (o);
}

Elm_List_Item *List::append (const std::string &label, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data)
{
  return elm_list_item_append (o, label.c_str (), NULL, NULL, func, data);
}

Elm_List_Item *List::append (const std::string &label, const Evasxx::Object &icon, const Evasxx::Object &end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data)
{
  return elm_list_item_append (o, label.c_str (), icon.obj (), end.obj (), func, data);
}

} // end namespace Elmxx
