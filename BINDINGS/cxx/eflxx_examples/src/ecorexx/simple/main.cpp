#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include "../../common/searchFile.h"

#define WIDTH 320
#define HEIGHT 240

#include <iostream>

using namespace std;
using namespace Eflxx;

int main( int argc, const char **argv )
{
  /* Create the application object */
  Ecorexx::Application* app = new Ecorexx::Application( argc, argv, "Simple Ecore Test" );

  /* Create the main window, a window with an embedded canvas */
  Ecorexx::EvasWindowSoftwareX11* mw = new Ecorexx::EvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );

  /* Create some objects on the canvas */
  Evasxx::Canvas &evas = mw->getCanvas();
  
  Evasxx::Image* image = new Evasxx::Image( evas, searchPixmapFile ("panel.png") );
  //image->setFilled (true);
  image->resize( evas.getSize() );
  image->setFill( image->getImageSize() ); // FIXME
  image->show();
  image->setFocus( true );
  
  cout << "Evas size: " << evas.getSize() << endl;
  cout << "Image size: " << image->getImageSize() << endl;
  cout << "Object size: " << image->getSize() << endl;

  Evasxx::Image* shadow = new Evasxx::Image( evas, searchPixmapFile ("panel_shadow.png") );
  //image->setFilled (true);
  shadow->resize( evas.getSize() );
  shadow->setFill( image->getImageSize() ); // FIXME
  shadow->show();

  Evasxx::Image* logo = new Evasxx::Image( evas, Point (50, 50), searchPixmapFile ("boing-shadow.png") );
  //image->setFilled (true);
  logo->resize( logo->getImageSize() );
  logo->setFill( logo->getImageSize() ); // FIXME
  logo->show();

  Evasxx::Text* text = new Evasxx::Text( evas, searchFontFile ("Gentium.ttf"), 20, Point (10, 10), "" );
  text->setText( "Commodore AMIGA rulez ..." );
  text->setLayer( 5 );
  text->show();

  mw->show ();

  /* Enter the application main loop */
  app->exec();

  /* Delete the application */
  delete app;

  return 0;
}
