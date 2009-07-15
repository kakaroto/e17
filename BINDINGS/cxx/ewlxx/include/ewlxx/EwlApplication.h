#ifndef EFLXX_EWL_APPLICATION_H
#define EFLXX_EWL_APPLICATION_H

/* EFL++ */
#include "../include/ewlxx/EwlWindow.h"

/* EFL */
#include <Ewl.h>

#define ewlApp EwlApplication::application()

namespace efl {

/* forward declarations */

enum Orientation
{
  Horizontal = EWL_ORIENTATION_HORIZONTAL,
  Vertical   = EWL_ORIENTATION_VERTICAL,
};



class EwlApplication
{
public:
  EwlApplication( int argc, const char** argv, const char* name );
  ~EwlApplication();

  static EwlApplication* application();
  const char* name() const {
    return _name;
  };

  /* Main Window */
  void setMainWindow( EwlWindow* );
  EwlWindow* mainWindow() const;

  /* Main Loop */
  void exec();
  void exit();

private:
  const char* _name;
  static EwlApplication* _instance;
  EwlWindow* _mainWindow;
};

}
#endif // EFLXX_EWL_APPLICATION_H
