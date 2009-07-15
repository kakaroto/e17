#include <eflxx/eflpp_sys.h>
#include <eflxx/eflpp_common.h>
#include <eflxx/eflpp_debug.h>

#include <iostream>

using namespace efl;
using namespace std;

class DebugGuard
{
public:
    DebugGuard( const char* fn ) : _fn( fn ) { eflDout( dc::notice, "Entering " << _fn ); };
    ~DebugGuard() { eflDout( dc::notice, "Leaving " << _fn ); };
    private:
    const char* _fn;
};

int test2( int bar )
{
    DebugGuard g( __PRETTY_FUNCTION__ );
    return bar;
}

int test( int foo )
{
    DebugGuard g( __PRETTY_FUNCTION__ );
    return test2( -foo );
}

int main(int argc, char **argv, char **envv)
{
    eflDebug( "EFL_DEBUG_DEMO: Using eflDebug - This is a test..." );
    eflDout( dc::notice, "EFL_DEBUG_DEMO: Using eflDout - Debug Output" );

    Rect r( 10, 10, 50, 50 );
    Color c( 10, 20, 30, 40 );
    Point p( -50, -30 );
    Size s( 640, 480 );

    //cout << r << c << p << s << endl;

    eflDout( dc::notice, "This is a value " << test( 20 ) );

    eflForAllDebugChannels( "EFL_DEBUG_DEMO: Test..." );
    eflDoutFatal( dc::fatal, "EFL_DEBUG_DEMO: Using eflDoutFatal(dc::fatal) - A fatal error occured... exiting." );
    eflDoutFatal( dc::core, "EFL_DEBUG_DEMO: Using eflDoutFatal(dc::core) - A fatal error occured... exiting." );

    return 0;
}
