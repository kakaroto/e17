#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkapplication.h"

/* EFL */
#include <etk/Etk.h>

using namespace std;

namespace efl {

EtkApplication* EtkApplication::_instance = 0;
  
EtkApplication::EtkApplication( int argc, const char **argv, const char* name )
    :_name( name )
{
    // TOOD: use Exceptions
    if ( EtkApplication::_instance )
    {
        cout << "FATAL: EtkApplication object already created" << endl;
            ::exit( -1 );
    }
    cout << "EtkApplication::EtkApplication" << endl;
    if ( !etk_init( argc, const_cast<char**>(argv) ) )
    {
            cout << "FATAL: Couldn't initialize Etk" << endl;
            ::exit( -1 );
    }
    EtkApplication::_instance = this;
}

EtkApplication::~EtkApplication()
{
    cout << "EtkApplication::~EtkApplication" << endl;
    etk_shutdown();
}

EtkApplication* EtkApplication::application()
{
    if ( !EtkApplication::_instance )
    {
        cerr << "FATAL: EtkApplication object hasn't been created yet" << endl;
            ::exit( -1 );
    }
    else
    {
        return EtkApplication::_instance;
    }
}

void EtkApplication::setMainWindow( EtkWindow* mw )
{
    _mainWindow = mw;
    mw->setTitle( _name );
}

EtkWindow* EtkApplication::mainWindow() const
{
    return _mainWindow;
}

void EtkApplication::exec()
{
    cout << "EtkApplication::exec() - >>> jumping into main loop" << endl;
    etk_main();
    cout << "EtkApplication::exec() - <<< returned from main loop" << endl;
}

void EtkApplication::exit()
{
    //ecore_main_loop_quit();
}

} // end namespace efl
