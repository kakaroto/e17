#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmObject.h"

using namespace std;

namespace efl {

ElmObject::ElmObject () {}

ElmObject::~ElmObject () {}  

void ElmObject::elmInit ()
{
  init (); // call init() from EvasObject
  
  signalHandleFree.connect (sigc::mem_fun (this, &ElmObject::freeSignalHandler));
}

void ElmObject::destroy ()
{
  evas_object_del (o);

  // do a suicide as the delete operator isn't public available
  // the reason is that the C design below is a suicide design :-(
  delete (this);
}

void ElmObject::freeSignalHandler ()
{
  //cout << "freeSignalHandler()" << endl;
  delete (this);
  // !!!ATTENTION!!!
  // suicide for a C++ object is dangerous, but allowed
  // the simple rule is that no member functions or member variables are allowed to access
}

void ElmObject::setScale (double scale)
{
  elm_object_scale_set (o, scale);
}

double ElmObject::getScale ()
{
  return elm_object_scale_get (o);
}

void ElmObject::setStyle (const std::string &style)
{
  elm_object_style_set (o, style.c_str ());
}

const std::string ElmObject::getStyle ()
{
  const char *tmp = elm_object_style_get (o);
  return tmp ? tmp : string ();
}

void ElmObject::setDisabled (bool disabled)
{
  elm_object_disabled_set (o, disabled);
}

bool ElmObject::getDisabled ()
{
  return elm_object_disabled_get (o);
}

void ElmObject::focus ()
{
  elm_object_focus (o); 
}

} // end namespace efl
