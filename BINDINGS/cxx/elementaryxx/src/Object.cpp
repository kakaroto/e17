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
  cout << "Object::destroy ()" << endl;

  // do a suicide as the delete operator isn't public available
  // the reason is that the C design below is a suicide design :-(
  //delete (this); // TODO: why does this make problems sometimes???
}

void Object::freeSignalHandler ()
{
  cout << "freeSignalHandler()" << endl;
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

void Object::setText (const std::string &label)
{
  elm_object_part_text_set(o, NULL, label.c_str());
}

void Object::setText (const std::string &part, const std::string &label)
{
  elm_object_part_text_set(o, part.c_str(), label.c_str());
}

const std::string Object::getText ()
{
  return elm_object_part_text_get(o, NULL);
}

const std::string Object::getText (const std::string &part)
{
  return elm_object_part_text_get(o, part.c_str());
}

void Object::setContent (const Evasxx::Object &content)
{
  elm_object_part_content_set(o, NULL, content.obj());
}

void Object::setContent (const std::string &part, const Evasxx::Object &content)
{
  elm_object_part_content_set(o, part.c_str(), content.obj());
}

Eflxx::CountedPtr <Evasxx::Object> Object::getContent ()
{
  Evas_Object *eo = elm_object_part_content_get(o, NULL);
 
  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

Eflxx::CountedPtr <Evasxx::Object> Object::getContent (const std::string &part)
{
  Evas_Object *eo = elm_object_part_content_get(o, part.c_str());
 
  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

void Object::unsetContent()
{
  /* ignore return */ elm_object_part_content_unset(o, NULL);
}

void Object::unsetContent(const std::string &part)
{
 /* ignore return */ elm_object_part_content_unset(o, part.c_str());
}

Eflxx::CountedPtr <Evasxx::Object> Object::findName(const std::string &name, int recurse)
{
  Evas_Object *eo = elm_object_name_find(o, name.c_str(), recurse);
 
  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

void Object::setInfoAccess(const std::string &txt)
{
  elm_object_access_info_set(o, txt.c_str());
}

} // end namespace Elmxx
