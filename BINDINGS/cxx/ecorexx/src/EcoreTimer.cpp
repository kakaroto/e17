#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFLxx */
#include "../include/ecorexx/EcoreTimer.h"

#include <cassert>

namespace efl {

EcoreTimer::EcoreTimer( double seconds, bool singleshot )
    :_ss( singleshot )
{
  Dout( dc::notice, "EcoreTimer::EcoreTimer() - current frequency is " << seconds );
  _et = ecore_timer_add( seconds, &EcoreTimer::__dispatcher, this );
  
  // TODO: find out why to use this function and the difference between ecore_time_get() and ecore_loop_time_get()
  //ecore_timer_loop_add (double in, int (*func) (void *data), const void *data);
}

EcoreTimer::~EcoreTimer()
{
  ecore_timer_del( _et );
}

EcoreTimer* EcoreTimer::singleShot( double seconds, const EcoreTimer::Slot& slot )
{
  EcoreTimer* ecoretimer = new EcoreTimer( seconds, true );
  ecoretimer->timeout.connect( slot );
  return ecoretimer;
}

void EcoreTimer::del ()
{
  assert (ecore_timer_del (_et));
}

void EcoreTimer::setInterval (double seconds)
{
  ecore_timer_interval_set (_et, seconds);
}
  
double EcoreTimer::getInterval ()
{
  return ecore_timer_interval_get (_et);
}

void EcoreTimer::freeze ()
{
  ecore_timer_freeze (_et);
}

void EcoreTimer::thaw ()
{
  ecore_timer_thaw (_et);
}

void EcoreTimer::delay (double add)
{
  ecore_timer_delay (_et, add);
}

double EcoreTimer::getPending ()
{
  return ecore_timer_pending_get (_et);
}

double EcoreTimer::getPrecision ()
{
  return ecore_timer_precision_get ();
}

void EcoreTimer::setPrecision (double precision)
{
  ecore_timer_precision_set (precision);
}

/*void EcoreTimer::tick()
{
  Dout( dc::notice, "EcoreTimer[ " << this << " ]::tick()" );
}*/

int EcoreTimer::__dispatcher( void* data )
{
  EcoreTimer* object = reinterpret_cast<EcoreTimer*>( data );
  assert( object );
  object->timeout.emit( object );
  //object->tick();
  bool singleshot = object->_ss;
  if ( singleshot ) delete object;
  return singleshot? 0:1;
}

} // end namespace efl
