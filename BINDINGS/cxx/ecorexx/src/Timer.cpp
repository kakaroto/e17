#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFLxx */
#include "../include/ecorexx/Timer.h"

#include <cassert>

namespace Ecorexx {

Timer::Timer( double seconds, bool singleshot )
    :_ss( singleshot )
{
  Dout( dc::notice, "Timer::Timer() - current frequency is " << seconds );
  _et = ecore_timer_add( seconds, &Timer::__dispatcher, this );
  
  // TODO: find out why to use this function and the difference between ecore_time_get() and ecore_loop_time_get()
  //ecore_timer_loop_add (double in, int (*func) (void *data), const void *data);
}

Timer::~Timer()
{
  ecore_timer_del( _et );
}

/*Timer* Timer::singleShot( double seconds, const Timer::Slot& slot )
{
  Timer* ecoretimer = new Timer( seconds, true );
  ecoretimer->timeout.connect( slot );
}*/

void Timer::del ()
{
  assert (ecore_timer_del (_et));
}

void Timer::setInterval (double seconds)
{
  ecore_timer_interval_set (_et, seconds);
}
  
double Timer::getInterval ()
{
  return ecore_timer_interval_get (_et);
}

void Timer::freeze ()
{
  ecore_timer_freeze (_et);
}

void Timer::thaw ()
{
  ecore_timer_thaw (_et);
}

void Timer::delay (double add)
{
  ecore_timer_delay (_et, add);
}

double Timer::getPending ()
{
  return ecore_timer_pending_get (_et);
}

double Timer::getPrecision ()
{
  return ecore_timer_precision_get ();
}

void Timer::setPrecision (double precision)
{
  ecore_timer_precision_set (precision);
}

/*void Timer::tick()
{
  Dout( dc::notice, "Timer[ " << this << " ]::tick()" );
}*/

Eina_Bool Timer::__dispatcher( void* data )
{
  Timer* object = reinterpret_cast<Timer*>( data );
  assert( object );
  object->timeout.emit( );
  //object->tick();
  /*bool singleshot = object->_ss;
  if ( singleshot ) delete object;
  return singleshot? 0:1;*/
  return ECORE_CALLBACK_RENEW;
}

} // end namespace Ecorexx
