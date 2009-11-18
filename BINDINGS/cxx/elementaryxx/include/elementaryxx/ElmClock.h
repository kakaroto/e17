#ifndef ELM_CLOCK_H
#define ELM_CLOCK_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 * "changed" - the user changed the time
 */
class ElmClock : public ElmObject
{
public:
  class Time
  {
  public:
    Time () :
      hrs (0), min (0), sec (0) {}
      
    Time (int hrs, int min, int sec) :
      hrs (this->hrs), min (this->min), sec (this->sec) {}
        
    int hrs;
    int min;
    int sec;
  };
  
  static ElmClock *factory (EvasObject &parent);
  
  void setTime (const ElmClock::Time &time);
  
  const ElmClock::Time getTime ();

  void setEdit (bool edit);
  
  void setShowAmPm (bool am_pm);
  
  void setShowSeconds (bool seconds);
  
private:
  ElmClock (); // forbid standard constructor
  ElmClock (const ElmClock&); // forbid copy constructor
  ElmClock (EvasObject &parent); // private construction -> use factory ()
  ~ElmClock (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_CLOCK_H
