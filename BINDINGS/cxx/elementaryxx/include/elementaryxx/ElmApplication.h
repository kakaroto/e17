#ifndef ELM_APPLICATION_H
#define ELM_APPLICATION_H

/* STL */
#include <string>

/* EFL++ */

namespace efl {

class ElmApplication
{
public:
  ElmApplication (int argc, char **argv);
  virtual ~ElmApplication ();

  static void run ();
  static void exit ();
  
  // EAPI void         elm_need_efreet(void);
  // EAPI void         elm_need_e_dbus(void);
  
  double getScale ();
  void setScale (double scale);
  
   // EAPI Evas_Coord   elm_finger_size_get(void);
   // EAPI void         elm_finger_size_set(Evas_Coord size);

};

} // end namespace efl

#endif // ELM_APPLICATION_H
