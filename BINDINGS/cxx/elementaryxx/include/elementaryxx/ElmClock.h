#ifndef ELM_CLOCK_H
#define ELM_CLOCK_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "changed" - the user changed the time
 */
class ElmClock : public ElmWidget
{
public:
  virtual ~ElmClock ();
  
  static ElmClock *factory (EvasObject &parent);
  
  void setTime (int hrs, int min, int sec); // TODO
  
  void getTime (int &hrs, int &min, int &sec); // TODO

  void setEdit (bool edit);
  
  void setShowAmPm (bool am_pm);
  
  void setShowSeconds (bool seconds);
  
private:
  ElmClock (); // forbid standard constructor
  ElmClock (const ElmClock&); // forbid copy constructor
  ElmClock (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_CLOCK_H
