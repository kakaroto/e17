#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Object.h"

/* STD */
#include <cassert>

using namespace std;

namespace Elmxx {

Object::Object () {}

Object::~Object () {}  

void Object::elmInit ()
{
  // check if there was a problem with object creation
  assert (o);
  
  init (); // call init() from Evasxx::Object
  
  signalHandleFree.connect (sigc::mem_fun (this, &Object::freeSignalHandler));
}

void Object::destroy ()
{
  evas_object_del (o);

  // do a suicide as the delete operator isn't public available
  // the reason is that the C design below is a suicide design :-(
  delete (this);
}

void Object::freeSignalHandler ()
{
  //cout << "freeSignalHandler()" << endl;
  delete (this);
  // !!!ATTENTION!!!
  // suicide for a C++ object is dangerous, but allowed
  // the simple rule is that no member functions or member variables are allowed to access after this point!
}

void Object::setScale (double scale)
{
  elm_object_scale_set (o, scale);
}

double Object::getScale ()
{
  return elm_object_scale_get (o);
}

void Object::setStyle (const std::string &style)
{
  elm_object_style_set (o, style.c_str ());
}

const std::string Object::getStyle ()
{
  const char *tmp = elm_object_style_get (o);
  return tmp ? tmp : string ();
}

void Object::setDisabled (bool disabled)
{
  elm_object_disabled_set (o, disabled);
}

bool Object::getDisabled ()
{
  return elm_object_disabled_get (o);
}

void Object::focus ()
{
  elm_object_focus_set (o, EINA_TRUE);
}

bool Object::checkWidget ()
{
  return elm_object_widget_check (o);
}

Eflxx::CountedPtr <Evasxx::Object> Object::getParentWidget ()
{
  Evas_Object *eo = elm_object_parent_widget_get (o);
 
  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

Eflxx::CountedPtr <Evasxx::Object> Object::getTopWidget ()
{
  Evas_Object *eo = elm_object_top_widget_get (o);
 
  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

const std::string Object::getWidgetType ()
{
  return elm_object_widget_type_get (o);
}

} // end namespace Elmxx
