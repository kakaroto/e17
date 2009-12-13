#ifndef EINAXX_BASE_H
#define EINAXX_BASE_H

/* EFL */
#include <Eina.h>

namespace Einaxx {

class Application
{
public:
  static int getHamsterCount ();
 	
  static int init ();

  static int shutdown ();

  static int initThreads ();

  static int shutdownThreads ();
};

} // end namespace Einaxx

#endif // EINAXX_BASE_H
