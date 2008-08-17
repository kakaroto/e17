#ifndef BOOTMANAGER_WIN_H
#define BOOTMANAGER_WIN_H

#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_edje.h>
using namespace efl;

class BootManagerApp;

class BootManagerWin : public EcoreEvasWindow
{
  public:
    BootManagerWin( int height, int width );
    ~BootManagerWin();

  void slotCalibrationDone();

  private:
    BootManagerApp* _app;
    EvasEdje* background;
    EvasEdje* buttons;
};

#endif


