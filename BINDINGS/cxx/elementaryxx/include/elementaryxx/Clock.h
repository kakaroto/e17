#ifndef ELMXX_CLOCK_H
#define ELMXX_CLOCK_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "changed" - the user changed the time
 */
class Clock : public Object
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
  
  static Clock *factory (Evasxx::Object &parent);
  
  void setTime (const Clock::Time &time);
  
  const Clock::Time getTime ();

  void setEdit (bool edit);
  
  void setShowAmPm (bool am_pm);
  
  void setShowSeconds (bool seconds);
  
private:
  Clock (); // forbid standard constructor
  Clock (const Clock&); // forbid copy constructor
  Clock (Evasxx::Object &parent); // private construction -> use factory ()
  ~Clock (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_CLOCK_H
