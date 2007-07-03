#ifndef BOOTMANAGER_APP_H
#define BOOTMANAGER_APP_H

#include <eflpp_ecore.h>
using namespace efl;

class BootManagerApp : public EcoreApplication
{
  public:
    BootManagerApp( int argc, const char** argv, const char* title );
    ~BootManagerApp();

  public: /* slots */
    void suspend( const char*, const char* );
    void reboot();
};

#endif


