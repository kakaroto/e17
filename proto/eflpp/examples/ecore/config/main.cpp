#include <eflpp_ecore.h>
#include <eflpp_ecoreconfig.h>
using namespace efl;

#include <iostream>
using namespace std;

int main( int argc, const char **argv )
{
    // using the EcoreConfigClass directly
    EcoreConfig c( "test" );
    c.setValue( "aString", string( "foo" ) );
    c.setValue( "anInt", 42 );
    c.setValue( "aBool", true );

    cout << "the string is = " << c.getString( "aString", "defaultString" ) << endl;
    cout << "the integer is = " << c.getInt( "anInt", 100 ) << endl;
    cout << "the boolean is = " << c.getBool( "aBool", false ) << endl;

    cout << "something not there is = " << c.getString( "not here", "not here" ) << endl;

    // using the application ecore config class
    EcoreApplication app( argc, argv, "This is my Application" );
    int runcount = app.config()->getInt( "runcount", 0 );
    cout << "this application runs for the " << ++runcount << "th time" << endl;
    app.config()->setValue( "runcount", runcount );

    return 0;
}
