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

void EcoreTimer::setInterval( double seconds )
{
  ecore_timer_interval_set( _et, seconds );
}

void EcoreTimer::tick()
{
  Dout( dc::notice, "EcoreTimer[ " << this << " ]::tick()" );
}

int EcoreTimer::__dispatcher( void* data )
{
  EcoreTimer* object = reinterpret_cast<EcoreTimer*>( data );
  assert( object );
  object->timeout.emit( object );
  object->tick();
  bool singleshot = object->_ss;
  if ( singleshot ) delete object;
  return singleshot? 0:1;
}

} // end namespace efl
