#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/ecorexx/Animator.h"
#include <eflxx/Common.h>

namespace Ecorexx {

Animator::Animator()
{
  Dout( dc::notice, "Animator::Animator() - current frametime is " << frameTime() );
  _ea = ecore_animator_add( &Animator::__dispatcher, this );
}

Animator::~Animator()
{
  ecore_animator_del( _ea );
}

void Animator::setFrameTime( double frametime )
{
  ecore_animator_frametime_set( frametime );
}

double Animator::frameTime()
{
  return ecore_animator_frametime_get();
}

bool Animator::tick()
{
  Dout( dc::notice, "Animator[ " << this << " ]::tick()" );
  return true;
}

Eina_Bool Animator::__dispatcher( void* data )
{
  Animator* object = reinterpret_cast<Animator*>( data );
  int result = object->tick();
  if ( !result ) delete object;
  return result;
}

} // end namespace Ecorexx
