#include <ecorexx/Ecorexx.h>

using namespace Eflxx;

#include <iostream>
using namespace std;

int main( int argc, const char **argv )
{
  // using the EcoreConfigClass directly
  Ecorexx::Config c( "test" );
  c.setValue( "aString", string( "foo" ) );
  c.setValue( "anInt", 42 );
  c.setValue( "aBool", true );

  cout << "the string is = " << c.getString( "aString", "defaultString" ) << endl;
  cout << "the integer is = " << c.getInt( "anInt", 100 ) << endl;
  cout << "the boolean is = " << c.getBool( "aBool", false ) << endl;

  cout << "something not there is = " << c.getString( "not here", "not here" ) << endl;

  // using the application ecore config class
  Ecorexx::Application app( argc, argv, "This is my Application" );
  int runcount = app.getConfig()->getInt( "runcount", 0 );
  cout << "this application runs for the " << ++runcount << "th time" << endl;
  app.getConfig()->setValue( "runcount", runcount );

  return 0;
}
