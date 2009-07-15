#include <eflpp_common.h>
#include <eflpp_sys.h>
#include <eflpp_debug_internal.h>

#include "eflpp_ecore.h"
#include "eflpp_ecoreconfig.h"

/* STD */
#ifdef ENABLE_EFLPP_FB
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
#include <assert.h>
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
    _font = new EvasFont( "Vera", 12 );
    _fixedFont = new EvasFont( "VeraMono", 10 );
}

EcoreApplication::~EcoreApplication()
{
    delete _font;
    delete _fixedFont;

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

void EcoreApplication::setMainWindow( EcoreEvasWindow* mw )
{
    _mainWindow = mw;
}

EcoreEvasWindow* EcoreApplication::mainWindow() const
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

EvasFont EcoreApplication::font()
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
}

EcoreConfig* EcoreApplication::config()
{
    if ( !_config ) _config = new EcoreConfig( _binary );
    return _config;
}


//===============================================================================================
// EcoreAnimator
//===============================================================================================

EcoreAnimator::EcoreAnimator()
{
    Dout( dc::notice, "EcoreAnimator::EcoreAnimator() - current frametime is " << frameTime() );
    _ea = ecore_animator_add( &EcoreAnimator::__dispatcher, this );
}

EcoreAnimator::~EcoreAnimator()
{
    ecore_animator_del( _ea );
}

void EcoreAnimator::setFrameTime( double frametime )
{
    ecore_animator_frametime_set( frametime );
}

double EcoreAnimator::frameTime()
{
    return ecore_animator_frametime_get();
}

bool EcoreAnimator::tick()
{
    Dout( dc::notice, "EcoreAnimator[ " << this << " ]::tick()" );
    return true;
}

int EcoreAnimator::__dispatcher( void* data )
{
    EcoreAnimator* object = reinterpret_cast<EcoreAnimator*>( data );
    int result = object->tick();
    if ( !result ) delete object;
    return result;
}

//===============================================================================================
// EcoreTimer
//===============================================================================================

EcoreTimer::EcoreTimer( double seconds, bool singleshot )
    :_ss( singleshot )
{
    Dout( dc::notice, "EcoreTimer::EcoreTimer() - current frequency is " << seconds );
    _et = ecore_timer_add( seconds, &EcoreTimer::__dispatcher, this );
}

EcoreTimer::~EcoreTimer()
{
    ecore_timer_del( _et );
}

EcoreTimer* EcoreTimer::singleShot( double seconds, const EcoreTimer::Slot& slot )
{
    EcoreTimer* ecoretimer = new EcoreTimer( seconds, true );
    ecoretimer->timeout.connect( slot );
    return ecoretimer;
}

void EcoreTimer::setInterval( double seconds )
{
    ecore_timer_interval_set( _et, seconds );
}

void EcoreTimer::tick()
{
    Dout( dc::notice, "EcoreTimer[ " << this << " ]::tick()" );
}

int EcoreTimer::__dispatcher( void* data )
{
    EcoreTimer* object = reinterpret_cast<EcoreTimer*>( data );
    assert( object );
    object->timeout.emit( object );
    object->tick();
    bool singleshot = object->_ss;
    if ( singleshot ) delete object;
    return singleshot? 0:1;
}

} // end namespace efl
