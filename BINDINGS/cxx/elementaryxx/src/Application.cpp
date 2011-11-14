#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Application.h"

/* EFL */
#include <Elementary.h>

using namespace std;

namespace Elmxx {

Application::Application (int argc, const char **argv)
{
  elm_init (argc, const_cast <char**> (argv));
}

Application::~Application ()
{
  elm_shutdown ();
}

void Application::run ()
{
  elm_run ();
}

void Application::exit ()
{
  elm_exit ();
}

double Application::getScale ()
{
  return elm_scale_get ();
}

void Application::setScale (double scale)
{
  elm_scale_set (scale);
}

void Application::flushAll ()
{
  elm_all_flush ();
}
  
} // end namespace Elmxx
