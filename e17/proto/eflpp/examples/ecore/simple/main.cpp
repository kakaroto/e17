#include <eflpp_ecore.h>
#include <eflpp_evas.h>

#define WIDTH 320
#define HEIGHT 240

#include <iostream>
using namespace std;

int main( int argc, const char **argv )
{
    /* Create the application object */
    efl::EcoreApplication* app = new efl::EcoreApplication( argc, argv, "Simple Ecore Test" );

    /* Create the main window, a window with an embedded canvas */
    efl::EcoreEvasWindowSoftwareX11* mw = new efl::EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );

    /* Create some objects on the canvas */
    efl::EvasCanvas* evas = mw->canvas();

    efl::EvasImage* image = new efl::EvasImage( 0, 0, PACKAGE_DATA_DIR "/images/panel.png", evas );
    image->resize( evas->size() );
    image->setFill( 0, 0, image->trueSize() );
    image->show();
    image->setFocus( true );

    efl::EvasImage* shadow = new efl::EvasImage( 0, 0, PACKAGE_DATA_DIR "/images/panel_shadow.png", evas );
    shadow->resize( evas->size() );
    shadow->setFill( 0, 0, image->size() );
    shadow->show();

    efl::EvasImage* logo = new efl::EvasImage( 50, 50, PACKAGE_DATA_DIR "/images/boing-shadow.png", evas );
    logo->show();

    efl::EvasText* text = new efl::EvasText( PACKAGE_DATA_DIR "/fonts/Gentium.ttf", 20, 10, 10, "", evas );
    text->setText( "Commodore AMIGA rulez ..." );
    text->setLayer( 5 );
    text->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}
