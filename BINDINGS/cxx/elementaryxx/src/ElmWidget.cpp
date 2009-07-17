#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmWidget.h"

using namespace std;

namespace efl {

ElmWidget::ElmWidget () {}

ElmWidget::~ElmWidget () {}  

void ElmWidget::elmInit ()
{
  init (); // call init() from EvasObject
  
  signalHandleFree.connect (sigc::mem_fun (this, &ElmWidget::freeSignalHandler));
}

void ElmWidget::destroy ()
{
  evas_object_del (o);

  // do a suicide as the delete operator isn't public available
  // the reason is that the C design below is a suicide design :-(
  delete (this);
}

void ElmWidget::freeSignalHandler ()
{
  //cout << "freeSignalHandler()" << endl;
  delete (this);
  // !!!ATTENTION!!!
  // suicide for a C++ object is dangerous, but allowed
  // the simple rule is that no member functions or member variables are allowed to access
}

} // end namespace efl
