#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Clock.h"

using namespace std;

namespace Elmxx {

Clock::Clock (Evasxx::Object &parent)
{
  o = elm_clock_add (parent.obj ());
  
  elmInit ();
}

Clock::~Clock () {}

Clock *Clock::factory (Evasxx::Object &parent)
{
  return new Clock (parent);
}

void Clock::setTime (const Clock::Time &time)
{
  elm_clock_time_set (o, time.hrs, time.min, time.sec);
}

const Clock::Time Clock::getTime ()
{
  Time time;
  elm_clock_time_get (o, &time.hrs, &time.min, &time.sec);
  return time;
}

void Clock::setEdit (bool edit)
{
  elm_clock_edit_set (o, edit);
}

void Clock::setShowAmPm (bool am_pm)
{
  elm_clock_show_am_pm_set (o, am_pm);
}

void Clock::setShowSeconds (bool seconds)
{
  elm_clock_show_seconds_set (o, seconds);
}

} // end namespace Elmxx
