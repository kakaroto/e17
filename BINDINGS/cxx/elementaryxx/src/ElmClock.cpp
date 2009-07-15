#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmClock.h"

using namespace std;

namespace efl {

ElmClock::ElmClock (EvasObject &parent)
{
  o = elm_clock_add (parent.obj ());
  
  elmInit ();
}

ElmClock::~ElmClock ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmClock *ElmClock::factory (EvasObject &parent)
{
  return new ElmClock (parent);
}

void ElmClock::setTime (int hrs, int min, int sec)
{
  elm_clock_time_set (o, hrs, min, sec);
}

void ElmClock::getTime (int &hrs, int &min, int &sec)
{
  elm_clock_time_get (o, &hrs, &min, &sec);
}

void ElmClock::setEdit (bool edit)
{
  elm_clock_edit_set (o, edit);
}

void ElmClock::setShowAmPm (bool am_pm)
{
  elm_clock_show_am_pm_set (o, am_pm);
}

void ElmClock::setShowSeconds (bool seconds)
{
  elm_clock_show_seconds_set (o, seconds);
}

} // end namespace efl
