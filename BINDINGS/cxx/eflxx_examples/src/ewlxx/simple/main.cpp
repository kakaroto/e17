#include <eflxx/eflpp_sys.h>
#include <ewlxx/Ewlxx.h>

#include <iostream>

using namespace std;
using namespace efl;

int main( int argc, const char **argv )
{
  EwlApplication app( argc, argv, "Simple Ewl Demo Application" );

  EwlWindow mainWindow;
  app.setMainWindow( &mainWindow );
  mainWindow.resize( Size( 200, 100 ) );
  mainWindow.show();

  EwlVBox vbox;
  mainWindow.appendChild( &vbox );
  vbox.setFillPolicy( All );
  vbox.show();

  EwlButton button ( "Hello World" );
  vbox.appendChild( &button ); // TODO better support non pointer parameter?
  button.setAlignment( Center );
  button.show();

  cout << "calling main now" << endl;

  app.exec();

  return 0;
}

