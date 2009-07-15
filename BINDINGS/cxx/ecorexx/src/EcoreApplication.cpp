#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFLxx */
#include <eflxx/eflpp_common.h>
#include <eflxx/eflpp_sys.h>
#include <eflxx/eflpp_debug_internal.h>
#include "../include/ecorexx/EcoreApplication.h"
#include "../include/ecorexx/EcoreConfig.h"
#include "../include/ecorexx/EcoreTimer.h"

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

namespace efl {

EcoreApplication* EcoreApplication::_instance = 0;



//===============================================================================================
// EcoreApplication
//===============================================================================================

EcoreApplication::EcoreApplication( int argc, const char **argv, const string& name )
    :Trackable( "EcoreApplication" ), _binary( argv[0] ), _name( name ), _config( 0 )
{
  if ( EcoreApplication::_instance )
  {
    DoutFatal( dc::fatal, "ECoreApplication object already created" );
  }
  Dout( dc::notice, "Application::Application" );
  ecore_init();
  ecore_app_args_set( argc, argv );
  EcoreApplication::_instance = this;

  //FIXME: read from configuration system
  //_font = new EvasFont( "Vera", 12 );
  //_fixedFont = new EvasFont( "VeraMono", 10 );
}

EcoreApplication::~EcoreApplication()
{
  //delete _font;
  //delete _fixedFont;

  Dout( dc::notice, "EcoreApplication::~EcoreApplication" );
  ecore_shutdown();
}

EcoreApplication* EcoreApplication::application()
{
  if ( !EcoreApplication::_instance )
  {
    DoutFatal( dc::core, "ECoreApplication object hasn't been created yet" );
  }
  else
  {
    return EcoreApplication::_instance;
  }
}

std::string EcoreApplication::getName() const 
{
  return _name;
};

void EcoreApplication::setMainWindow( EcoreEvasWindow* mw )
{
  _mainWindow = mw;
}

EcoreEvasWindow* EcoreApplication::getMainWindow() const
{
  return _mainWindow;
}

void EcoreApplication::exec()
{
  Dout( dc::notice, "EcoreApplication::exec() - >>> jumping into main loop" );
  ecore_main_loop_begin();
  Dout( dc::notice, "EcoreApplication::exec() - <<< returned from main loop" );
}

void EcoreApplication::processOneEvent()
{
  ecore_main_loop_iterate();
}

void EcoreApplication::quit()
{
  ecore_main_loop_quit();
}

void EcoreApplication::startTimer( double seconds )
{
  EcoreTimer* ecoretimer = new EcoreTimer( seconds );
  ecoretimer->timeout.connect( sigc::mem_fun( this, &EcoreApplication::timerEvent ) );
}

/*EvasFont EcoreApplication::font()
{
    assert( _font );
    return *_font;
}

EvasFont EcoreApplication::fixedFont()
{
    assert( _fixedFont );
    return *_fixedFont;
}

void EcoreApplication::setFont( const EvasFont& font )
{
    delete _font;
    _font = new EvasFont( font );
}

void EcoreApplication::setFixedFont( const EvasFont& font )
{
    delete _fixedFont;
    _fixedFont = new EvasFont( font );
}*/

EcoreConfig* EcoreApplication::getConfig()
{
  if ( !_config ) _config = new EcoreConfig( _binary );
  return _config;
}

} // end namespace efl
