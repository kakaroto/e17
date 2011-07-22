#ifndef ECORETIMER_H
#define ECORETIMER_H

/* EFLxx */
#include <eflxx/Common.h>

/* EFL */
#include <Ecore.h>

namespace Ecorexx {

class Timer
{
public:
  static Timer *factory (double seconds, sigc::slot<bool, Timer&> task, bool loop = false);

  void destroy ();
  
  void setInterval (double interval);
  
  double getInterval ();
  
  void freeze ();
  
  void thaw ();
  
  void delay (double add);
  
  double getPending ();

  static double getPrecision ();
    
  static void setPrecision (double precision);

  static void dump ();

private:
  Ecore_Timer *mETimer;
  sigc::slot<bool, Timer&> mTask;

  static Eina_Bool dispatcherFunc (void *data);

  Timer (); // allow no construction
  Timer (const Timer&); // forbid copy constructor
  Timer (double seconds, sigc::slot<bool, Timer&> task, bool loop);  // private construction -> use factory ()
  virtual ~Timer (); // forbid direct delete -> use destroy()
};

} // end namespace Ecorexx

#endif // ECORETIMER_H
