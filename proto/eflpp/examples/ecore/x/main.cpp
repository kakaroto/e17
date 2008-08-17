#include <eflpp_ecore.h>
#include <eflpp_ecore_x_window.h>
#include <eflpp_evas.h>

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
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );
  
    // Open this window as 'Utility' window
    EcoreXWindow *exwin = mw->getXWindow();
    exwin->setNetWMWindowType ( EcoreXWindow::Utility );
    delete exwin;
  
    /* Create some objects on the canvas */
    EvasCanvas* evas = mw->canvas();

    EvasImage* image = new EvasImage( 0, 0, PACKAGE_DATA_DIR "/images/panel.png", evas );
    image->resize( evas->size() );
    image->setFill( 0, 0, image->trueSize() );
    image->show();
    image->setFocus( true );

    EvasImage* shadow = new EvasImage( 0, 0, PACKAGE_DATA_DIR "/images/panel_shadow.png", evas );
    shadow->resize( evas->size() );
    shadow->setFill( 0, 0, image->size() );
    shadow->show();

    EvasImage* logo = new EvasImage( 50, 50, PACKAGE_DATA_DIR "/images/boing-shadow.png", evas );
    logo->show();

    EvasText* text = new EvasText( PACKAGE_DATA_DIR "/fonts/Gentium.ttf", 20, 10, 10, "", evas );
    text->setText( "Commodore AMIGA rulez ..." );
    text->setLayer( 5 );
    text->show();
 

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}
