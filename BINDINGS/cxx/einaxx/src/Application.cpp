#include "../include/einaxx/Application.h"

namespace Einaxx {

int Application::getHamsterCount ()
{
  return eina_hamster_count ();
}

int Application::init ()
{
  return eina_init ();
}

int Application::shutdown ()
{
  return eina_shutdown ();
}

int Application::initThreads ()
{
  return eina_threads_init ();
}

int Application::shutdownThreads ()
{
  return eina_threads_shutdown ();
}
    
} // end namespace Einaxx
