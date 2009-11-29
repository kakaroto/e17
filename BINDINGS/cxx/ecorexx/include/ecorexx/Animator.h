#ifndef ECOREXX_ANIMATOR_H
#define ECOREXX_ANIMATOR_H

/* EFL */
#include <Ecore.h>

namespace Ecorexx {

class Animator
{
public:
  Animator();
  virtual ~Animator();

  virtual bool tick();
  static void setFrameTime( double frametime );
  static double frameTime();

private:
  Ecore_Animator* _ea;

  static int __dispatcher( void* data );
};

} // end namespace Ecorexx

#endif // ECOREXX_ANIMATOR_H
