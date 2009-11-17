#ifndef ECORETIMER_H
#define ECORETIMER_H

/* EFLxx */
#include <eflxx/eflpp_common.h>

/* EFL */
#include <Ecore.h>

namespace efl {

class EcoreTimer
{
  typedef sigc::signal <void,EcoreTimer*> Signal;
  typedef sigc::signal <void,EcoreTimer*> Loop;
  typedef sigc::slot1  <void,EcoreTimer*> Slot;

public:
  EcoreTimer( double seconds, bool singleshot = false );
  virtual ~EcoreTimer();

  //virtual void tick();

  static EcoreTimer* singleShot( double seconds, const EcoreTimer::Slot& ); // TODO: CountedPtr
  
  void del ();
  
  void setInterval (double interval);
  
  double getInterval ();
  
  void freeze ();
  
  void thaw ();
  
  void delay (double add);
  
  double getPending ();

  static double getPrecision ();
    
  static void setPrecision (double precision);

  
public: /* signals */
  EcoreTimer::Signal timeout;
  EcoreTimer::Loop loop;
  
private:
  Ecore_Timer* _et;
  bool _ss;

  static int __dispatcher( void* data );
};

} // end namespace efl

#endif // ECORETIMER_H
