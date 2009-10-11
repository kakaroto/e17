#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmApplication.h"

/* EFL */
#include <Elementary.h>

using namespace std;

namespace efl {

ElmApplication::ElmApplication (int argc, char **argv)
{
  elm_init (argc, argv);
}

ElmApplication::~ElmApplication ()
{
  elm_shutdown ();
}

void ElmApplication::run ()
{
  elm_run ();
}

void ElmApplication::exit ()
{
  elm_exit ();
}

double ElmApplication::getScale ()
{
  return elm_scale_get ();
}

void ElmApplication::setScale (double scale)
{
  elm_scale_set (scale);
}
  
} // end namespace efl
