#include <ecorexx/EcoreApplication.h>
#include <evasxx/Evasxx.h>
#include <group/EvasSmartGroup.h>
#include "../../common/searchFile.h"

#include <memory>

#define WIDTH 240
#define HEIGHT 320

using namespace efl;
using namespace std;

int main( int argc, const char **argv )
{
  /* Create the application object */
  EcoreApplication app ( argc, argv, "Esmart Group Test");

  Size size (WIDTH, HEIGHT);

  /* Create the main window, a window with an embedded canvas */
  EcoreEvasWindowSoftwareX11 mw ( size );

  EvasCanvas &evas = mw.getCanvas();

  evas.appendFontPath( searchDataDir () + "/fonts" );

  /* Add some objects to the canvas */
  EvasRectangle rect ( evas, size );
  rect.setColor( Color (200, 200, 200, 255) );
  rect.setLayer( 0 );
  rect.show();

  EvasRectangle buttonbackground ( evas );
  buttonbackground.setColor( Color (255, 0, 0, 255) );
  buttonbackground.setLayer( 10 );
  buttonbackground.setGeometry( Rect (10, 10, 20, 20) );
  EvasText buttontext ( evas, "Vera", 14, "Click on this button" );
  buttontext.setColor( Color (0, 0, 0, 255) );
  buttontext.setLayer( 11 );

  EvasSmartGroup vbox (evas, Rect (50, 50, 200, 200) );
  //vbox->resize( 100, 100 );
  //vbox->setLayer( 20 );
  vbox.append( &buttonbackground );
  vbox.append( &buttontext );
  vbox.show();

  mw.show();

  /* Enter the application main loop */
  app.exec();

  return 0;
}
