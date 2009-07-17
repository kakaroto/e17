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
};

} // end namespace efl

#endif // ELM_APPLICATION_H
