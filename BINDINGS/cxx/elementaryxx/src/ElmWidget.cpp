#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmWidget.h"

using namespace std;

namespace efl {

ElmWidget::ElmWidget ()
{

}

ElmWidget::~ElmWidget ()
{
}

void ElmWidget::elmInit ()
{
  init (); // call init() from EvasObject
  
  signalHandleFree.connect (sigc::mem_fun (this, &ElmWidget::freeSignalHandler));
}

void ElmWidget::freeSignalHandler ()
{
  //cout << "freeSignalHandler()" << endl;
  mFree = false; // don't delete the C object. In this case it's yet deleted.
  delete (this);
  // !!!ATTENTION!!!
  // suicide for a C++ object is dangerous, but allowed
  // the simple rule is that no member functions or member variables are allowed to access
}

} // end namespace efl
