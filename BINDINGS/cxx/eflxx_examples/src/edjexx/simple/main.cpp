#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include <edjexx/Edjexx.h>
#include "../../common/searchFile.h"

#include <memory>

#define WIDTH 240
#define HEIGHT 320

using namespace Eflxx;
using namespace std;

int main( int argc, const char **argv )
{
  // Create the application object
  Ecorexx::Application app (argc, argv, "Simple Edje Test");

  Size s (WIDTH, HEIGHT);

  // Create the main window, a window with an embedded canvas
  Ecorexx::EvasWindowSoftwareX11 mw (s);

  Evasxx::Canvas &evas = mw.getCanvas();

  evas.appendFontPath( searchDataDir () + "/fonts" );

  // Add some objects to the canvas

  cout << "edje: " << searchEdjeFile ("simple.edj") << endl;
  Edjexx::Object edje (evas, Point (0, 0), searchEdjeFile ("simple.edj"), "test");

  edje.resize( s );

  edje.setLayer( 0 );
  edje.show();

  mw.show();

  // Enter the application main loop
  app.exec();

  return 0;
}

