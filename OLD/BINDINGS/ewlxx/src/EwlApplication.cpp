#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/ewlxx/EwlApplication.h"

/* EFL */
#include <Ecore_Evas.h>

/* STD */
#include <iostream>
#include <cstring>

using namespace std;

namespace efl {

EwlApplication* EwlApplication::_instance = 0;

EwlApplication::EwlApplication( int argc, const char **argv, const char* name )
    :_name( name )
{
  if ( EwlApplication::_instance )
  {
    cout << "FATAL: EwlApplication object already created" << endl;
    ::exit( -1 );
  }
  cout << "EwlApplication::EwlApplication" << endl;
  ewl_init( &argc, const_cast<char**>( argv ) );
  EwlApplication::_instance = this;
}

EwlApplication::~EwlApplication()
{
  cout << "EwlApplication::~EwlApplication" << endl;
  ewl_shutdown();
}

EwlApplication* EwlApplication::application()
{
  if ( !EwlApplication::_instance )
  {
    cerr << "FATAL: EwlApplication object hasn't been created yet" << endl;
    ::exit( -1 );
  }
  else
  {
    return EwlApplication::_instance;
  }
}

void EwlApplication::setMainWindow( EwlWindow* mw )
{
  _mainWindow = mw;
  mw->setTitle( _name );
}

EwlWindow* EwlApplication::mainWindow() const
{
  return _mainWindow;
}

void EwlApplication::exec()
{
  cout << "EwlApplication::exec() - >>> jumping into main loop" << endl;
  ewl_main();
  cout << "EwlApplication::exec() - <<< returned from main loop" << endl;
}

void EwlApplication::exit()
{
  //ecore_main_loop_quit();
}

}
