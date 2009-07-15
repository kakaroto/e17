#ifndef ECORETIMER_H
#define ECORETIMER_H

/* EFLxx */
#include <eflxx/eflpp_common.h>

/* EFL */
#include <Ecore.h>

namespace efl {

class EcoreTimer
{
  typedef sigc::signal<void,EcoreTimer*> Signal;
  typedef sigc::slot1<void,EcoreTimer*> Slot;

public:
  EcoreTimer( double seconds, bool singleshot = false );
  virtual ~EcoreTimer();

  virtual void tick();
  void setInterval( double );

  static EcoreTimer* singleShot( double seconds, const EcoreTimer::Slot& );

public: /* signals */
  EcoreTimer::Signal timeout;

private:
  Ecore_Timer* _et;
  bool _ss;

  static int __dispatcher( void* data );
};

} // end namespace efl

#endif // ECORETIMER_H