#ifndef ECORE_APPLICATION_H
#define ECORE_APPLICATION_H

/* EFLxx */
#include "EcoreEvasWindow.h"

/* EFL */
#include <Evas.h>
#include <Ecore_Config.h>
#include <Ecore_Data.h>
#include <Ecore_Ipc.h>
#include <Ecore_Txt.h>
#include <Ecore_Con.h>
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Ecore_Job.h>

/* STD */
#include <string>
#include <list>

#define eApp EcoreApplication::application()

/**
 * C++ Wrapper for the Enlightenment Core Library (Ecore)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

/* Forward declarations */
class EcoreEvasWindow;
class EcoreApplication;
class EcoreCallback;
class EcoreConfig;
class EcoreTimer;

class EcoreCallback
{
public:
  EcoreCallback( EcoreApplication* object ) : o(object) {};
  ~EcoreCallback() {};
private:
  EcoreApplication* o;
};

//===============================================================================================
// EcoreApplication
//===============================================================================================

class EcoreApplication : public Trackable
{
public:
  EcoreApplication( int argc, const char** argv, const std::string& name );
  virtual ~EcoreApplication();

  static EcoreApplication* application();
  
  std::string getName() const;

  /* Main Window */
  void setMainWindow( EcoreEvasWindow* );
  EcoreEvasWindow* getMainWindow() const;

  /* Main Loop */
  static void exec();
  static void processOneEvent();
  static void quit();

  /* Timer */
  void startTimer( double seconds );
  virtual void timerEvent( EcoreTimer* ) {};

  /* Font */
  /*EvasFont font();
  EvasFont fixedFont();
  void setFont( const EvasFont& );
  void setFixedFont( const EvasFont& );*/

  /* Configuration */
  EcoreConfig* getConfig();

private:
  const std::string _binary;
  std::string _name;
  static EcoreApplication* _instance;
  EcoreEvasWindow* _mainWindow;
  //EvasFont* _font;
  //EvasFont* _fixedFont;
  EcoreConfig* _config;

private:
  EcoreApplication();
  bool operator=( const EcoreApplication& );
  bool operator==( const EcoreApplication& );
};

} // end namespace efl

#endif // ECORE_APPLICATION_H
