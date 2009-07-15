#include <ecorexx/EcoreApplication.h>
#include <evasxx/Evasxx.h>
#include <edjexx/Edjexx.h>
#include <textentry/EvasSmartTextEntry.h>
#include "../../common/searchFile.h"

#define WIDTH 300
#define HEIGHT 30

using namespace efl;

int main( int argc, const char **argv )
{
  /* Create the application object */
  EcoreApplication* app = new EcoreApplication( argc, argv, "Esmart Container Test" );

  /* Create the main window, a window with an embedded canvas */
  EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );

  EvasCanvas &evas = mw->getCanvas();

  evas.appendFontPath( searchDataDir () + "/fonts" );

  /* Add some objects to the canvas */
  EvasRectangle* rect = new EvasRectangle( evas, Rect (0, 0, WIDTH, HEIGHT) );
  rect->setColor( Color (200, 200, 200, 255) );
  rect->setLayer( 0 );
  rect->show();

  EvasEdje* edje = new EvasEdje( evas, searchEdjeFile ("esmart_text_entry_test.edj"), "text_entry" );

  edje->resize( Size (WIDTH, HEIGHT) );
  edje->setLayer( 1 );
  edje->show();

  EvasSmartTextEntry* passwd = new EvasSmartTextEntry( evas, Rect (0, 0, WIDTH, HEIGHT) );
  passwd->setEdjePart (edje, "text");
  passwd->setColor( Color (0, 0, 0, 255) );
  passwd->setLayer( 0 );
  passwd->setFocus (true);
  passwd->show();

  mw->show();

  /* Enter the application main loop */
  app->exec();

  /* Delete the application */
  delete app;

  return 0;
}
