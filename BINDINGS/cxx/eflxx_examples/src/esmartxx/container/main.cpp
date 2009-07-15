#include <ecorexx/EcoreApplication.h>
#include <evasxx/Evasxx.h>
#include <container/EvasSmartContainer.h>
#include "../../common/searchFile.h"

#include <memory>

#define WIDTH 240
#define HEIGHT 320

using namespace efl;
using namespace std;

int main( int argc, const char **argv )
{
  /* Create the application object */
  auto_ptr <EcoreApplication> app (new EcoreApplication( argc, argv, "Esmart Container Test" ));

  Size size (WIDTH, HEIGHT);

  /* Create the main window, a window with an embedded canvas */
  auto_ptr <EcoreEvasWindowSoftwareX11> mw (new EcoreEvasWindowSoftwareX11( size ));

  EvasCanvas &evas = mw->getCanvas();

  evas.appendFontPath( searchDataDir () + "/fonts" );

  /* Add some objects to the canvas */
  auto_ptr <EvasRectangle> rect (new EvasRectangle( evas, size ));
  rect->setColor( Color (200, 150, 100, 255) );
  rect->setLayer( 0 );
  rect->show();

  auto_ptr <EvasRectangle> buttonbackground (new EvasRectangle( evas ));
  buttonbackground->setColor( Color (0, 0, 255, 255) );
  buttonbackground->setLayer( 10 );
  buttonbackground->setGeometry( Rect (10, 10, 20, 20) );
  buttonbackground->show ();

  auto_ptr <EvasText> buttontext (new EvasText( evas, "Vera", 14, "Click on this button" ));
  buttontext->setColor( Color (0, 255, 0, 255) );
  buttontext->setLayer( 11 );
  buttontext->show ();

  auto_ptr <EvasSmartContainer> vbox (new EvasSmartContainer( evas, Point (50, 50), EvasSmartContainer::Vertical ));
  vbox->resize( Size (150, 100) );
  vbox->setLayer( 20 );
  vbox->setAlignment( EvasSmartContainer::Right );
  //vbox->setPadding( 10, 10, 10, 10 );
  //vbox->setSpacing( 0 );
  vbox->setFillPolicy( (EvasSmartContainer::FillPolicy) (EvasSmartContainer::None ) );
  vbox->append( &(*buttonbackground) );
  vbox->append( &(*buttontext) );
  vbox->show();

  //vbox->setScrollOffset( 1 );
  //vbox->setScrollPercent( 20 );
  //vbox->startScrolling( 2 );
  //vbox->scroll( 10 );

  mw->show();

  /* Enter the application main loop */
  app->exec();

  return 0;
}

