#include <ecorexx/EcoreApplication.h>
#include <ecorexx/EcoreXWindow.h>
#include <evasxx/Evasxx.h>
#include "../../common/searchFile.h"

#define WIDTH 320
#define HEIGHT 240

#include <iostream>
using namespace std;
using namespace efl;

int main( int argc, const char **argv )
{
  /* Create the application object */
  EcoreApplication* app = new EcoreApplication( argc, argv, "Simple Ecore Test" );

  /* Create the main window, a window with an embedded canvas */
  EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );

  // Open this window as 'Utility' window
  EcoreXWindow *exwin = mw->getXWindow();
  exwin->setNetWMWindowType ( EcoreXWindow::Utility );
  delete exwin;

  /* Create some objects on the canvas */
  EvasCanvas &evas = mw->getCanvas();

  EvasImage* image = new EvasImage( evas, searchPixmapFile ("panel.png") );
  image->resize( evas.getSize() );
  image->setFill( image->getImageSize() );
  image->show();
  image->setFocus( true );

  EvasImage* shadow = new EvasImage( evas, searchPixmapFile ("panel_shadow.png") );
  shadow->resize( evas.getSize() );
  shadow->setFill( image->getImageSize() );
  shadow->show();

  EvasImage* logo = new EvasImage(evas, Point (50, 50), searchPixmapFile ("boing-shadow.png") );
  logo->resize( logo->getImageSize() );
  logo->setFill( logo->getImageSize() );
  logo->show();

  EvasText* text = new EvasText( evas, searchFontFile ("Gentium.ttf"), 20, Point (10, 10), "" );
  text->setText( "Commodore AMIGA rulez ..." );
  text->setLayer( 5 );
  text->show();

  mw->show();

  /* Enter the application main loop */
  app->exec();

  /* Delete the application */
  delete app;

  return 0;
}
