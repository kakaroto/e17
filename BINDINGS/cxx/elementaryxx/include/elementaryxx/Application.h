#ifndef ELMXX_APPLICATION_H
#define ELMXX_APPLICATION_H

/* STL */
#include <string>

/* EFL++ */

namespace Elmxx {

class Application
{
public:
  Application (int argc, const char **argv);
  virtual ~Application ();

  static void run ();
  static void exit ();
  
  // EAPI void         elm_need_efreet(void);
  // EAPI void         elm_need_e_dbus(void);
  
  static double getScale ();
  static void setScale (double scale);
  
   // EAPI Evas_Coord   elm_finger_size_get(void);
   // EAPI void         elm_finger_size_set(Evas_Coord size);

  /*!
   * Flush all caches & dump all data that can be to lean down to use less memory
   */
  static void flushAll ();
};

} // end namespace Elmxx

#endif // ELMXX_APPLICATION_H
