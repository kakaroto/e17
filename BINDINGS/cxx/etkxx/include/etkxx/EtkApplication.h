#ifndef ETK_APPLICATION_H
#define ETK_APPLICATION_H

/* STL */
#include <string>

/* EFL++ */
#include "EtkWindow.h"

#define etkApp EtkApplication::application() // TODO: needed?

namespace efl {

class EtkApplication
{
public:
  EtkApplication( int argc, const char** argv, const char* name );
  ~EtkApplication();

  static EtkApplication* application(); // TODO: needed?
  const char* name() const {
    return _name;
  };

  /* Main Window */
  void setMainWindow( EtkWindow* ); // TODO: needed?
  EtkWindow* mainWindow() const; // TODO: needed?

  /* Main Loop */
  void exec();
  void exit(); // TODO: possible to include in constructor/destructor?

private:
  const char* _name;
  static EtkApplication* _instance;
  EtkWindow* _mainWindow;

};


} // end namespace efl

#endif // ETK_APPLICATION_H
