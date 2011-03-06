#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFLxx */
#include <eflxx/Common.h>
#include <eflxx/System.h>
#include <eflxx/DebugInternal.h>
#include "../include/ecorexx/Application.h"
#include "../include/ecorexx/Timer.h"

/* STD */
#ifdef ENABLE_EFLPP_FB // TODO: configure support!
#include <linux/fb.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

namespace Ecorexx {

Application* Application::_instance = 0;



//===============================================================================================
// Application
//===============================================================================================

Application::Application( int argc, const char **argv, const string& name )
    : Eflxx::Trackable( "Application" ), _binary( argv[0] ), _name( name )
{
  if ( Application::_instance )
  {
    DoutFatal( dc::fatal, "ECoreApplication object already created" );
  }
  Dout( dc::notice, "Application::Application" );
  ecore_init();
  ecore_app_args_set( argc, argv );
  Application::_instance = this;

  //FIXME: read from configuration system
  //_font = new EvasFont( "Vera", 12 );
  //_fixedFont = new EvasFont( "VeraMono", 10 );
}

Application::~Application()
{
  //delete _font;
  //delete _fixedFont;

  Dout( dc::notice, "Application::~Application" );
  ecore_shutdown();
}

Application* Application::getInstance()
{
  if ( !Application::_instance )
  {
    DoutFatal( dc::core, "ECoreApplication object hasn't been created yet" );
  }
  else
  {
    return Application::_instance;
  }
}

std::string Application::getName() const 
{
  return _name;
};

void Application::setMainWindow( EvasWindow* mw )
{
  _mainWindow = mw;
}

EvasWindow* Application::getMainWindow() const
{
  return _mainWindow;
}

void Application::exec()
{
  Dout( dc::notice, "Application::exec() - >>> jumping into main loop" );
  ecore_main_loop_begin();
  Dout( dc::notice, "Application::exec() - <<< returned from main loop" );
}

void Application::processOneEvent()
{
  ecore_main_loop_iterate();
}

void Application::quit()
{
  ecore_main_loop_quit();
}

/*EvasFont Application::font()
{
    assert( _font );
    return *_font;
}

EvasFont Application::fixedFont()
{
    assert( _fixedFont );
    return *_fixedFont;
}

void Application::setFont( const EvasFont& font )
{
    delete _font;
    _font = new EvasFont( font );
}

void Application::setFixedFont( const EvasFont& font )
{
    delete _fixedFont;
    _fixedFont = new EvasFont( font );
}*/

} // end namespace Ecorexx
