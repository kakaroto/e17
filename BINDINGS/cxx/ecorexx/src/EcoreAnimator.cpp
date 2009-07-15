#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/ecorexx/EcoreAnimator.h"
#include <eflxx/eflpp_common.h>

namespace efl {

EcoreAnimator::EcoreAnimator()
{
  Dout( dc::notice, "EcoreAnimator::EcoreAnimator() - current frametime is " << frameTime() );
  _ea = ecore_animator_add( &EcoreAnimator::__dispatcher, this );
}

EcoreAnimator::~EcoreAnimator()
{
  ecore_animator_del( _ea );
}

void EcoreAnimator::setFrameTime( double frametime )
{
  ecore_animator_frametime_set( frametime );
}

double EcoreAnimator::frameTime()
{
  return ecore_animator_frametime_get();
}

bool EcoreAnimator::tick()
{
  Dout( dc::notice, "EcoreAnimator[ " << this << " ]::tick()" );
  return true;
}

int EcoreAnimator::__dispatcher( void* data )
{
  EcoreAnimator* object = reinterpret_cast<EcoreAnimator*>( data );
  int result = object->tick();
  if ( !result ) delete object;
  return result;
}

} // end namespace efl
