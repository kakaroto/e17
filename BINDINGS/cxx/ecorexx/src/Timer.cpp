#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFLxx */
#include "../include/ecorexx/Timer.h"

#include <cassert>

namespace Ecorexx {

Timer::Timer (double seconds, sigc::slot<bool, Timer&> task, bool loop) :
  mETimer (NULL),
  mTask (task)  
{
  if (!loop)
  {
    mETimer = ecore_timer_add (seconds, Timer::dispatcherFunc, this);
  }
  else
  {
    mETimer = ecore_timer_loop_add (seconds, Timer::dispatcherFunc, this);
  }
}

Timer *Timer::factory (double seconds, sigc::slot<bool, Timer&> task, bool loop)
{
  return new Timer (seconds, task, loop);
}

Timer::~Timer()
{
}

void Timer::destroy ()
{
  assert (ecore_timer_del (mETimer));

  // !!!ATTENTION!!!
  // suicide for a C++ object is dangerous, but allowed
  // the simple rule is that no member functions or member variables are allowed to access after this point!
  delete (this);
}

void Timer::setInterval (double seconds)
{
  ecore_timer_interval_set (mETimer, seconds);
}
  
double Timer::getInterval ()
{
  return ecore_timer_interval_get (mETimer);
}

void Timer::freeze ()
{
  ecore_timer_freeze (mETimer);
}

void Timer::thaw ()
{
  ecore_timer_thaw (mETimer);
}

void Timer::delay (double add)
{
  ecore_timer_delay (mETimer, add);
}

double Timer::getPending ()
{
  return ecore_timer_pending_get (mETimer);
}

double Timer::getPrecision ()
{
  return ecore_timer_precision_get ();
}

void Timer::setPrecision (double precision)
{
  ecore_timer_precision_set (precision);
}

void Timer::dump ()
{
  ecore_timer_dump ();
}

Eina_Bool Timer::dispatcherFunc (void *data)
{
  Timer* eTimer = static_cast <Timer*>( data );
  assert (eTimer);

  bool ret = eTimer->mTask (*eTimer);

  if (!ret)
  {
    // do a suicide as the delete operator isn't public available
    // the reason is that the C design below is a suicide design :-(
    delete eTimer;
    return false;
  }
  
  return ret;
}

} // end namespace Ecorexx
