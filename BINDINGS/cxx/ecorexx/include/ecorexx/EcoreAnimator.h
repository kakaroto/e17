#ifndef ECORE_ANIMATOR_H
#define ECORE_ANIMATOR_H

/* EFL */
#include <Ecore.h>

namespace efl {

class EcoreAnimator
{
public:
  EcoreAnimator();
  virtual ~EcoreAnimator();

  virtual bool tick();
  static void setFrameTime( double frametime );
  static double frameTime();

private:
  Ecore_Animator* _ea;

  static int __dispatcher( void* data );
};

} // end namespace efl

#endif // ECORE_ANIMATOR_H
