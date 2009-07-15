#include <eflxx/eflpp_sys.h>
#include <etkxx/Etkxx.h>

#include <iostream>

using namespace std;
using namespace efl;

int main( int argc, const char **argv )
{
  EtkApplication app( argc, argv, "Simple Etk Demo Application" );

  EtkWindow mainWindow;
  app.setMainWindow( &mainWindow );
  mainWindow.resize( Size( 200, 100 ) );

  EtkButton button ( "Hello World" );
  button.setFromStock( ETK_STOCK_BOOKMARK_NEW );
  button.setStockSize( ETK_STOCK_BIG );
  mainWindow.add( &button );
  mainWindow.showAll();

  cout << "calling main now" << endl;

  // TODO: support c++ style signals
  //etk_signal_connect("delete_event", ETK_OBJECT(mainWindow->obj()), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
  etk_signal_connect("delete_event", ETK_OBJECT(mainWindow.obj()), ETK_CALLBACK(etk_main_quit), NULL);


  etk_signal_connect("clicked", ETK_OBJECT(button.obj()), ETK_CALLBACK(etk_main_quit), NULL);

  app.exec();

  return 0;
}

