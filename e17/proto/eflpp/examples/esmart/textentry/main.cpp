#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_esmart.h>

#define WIDTH 240
#define HEIGHT 320

using namespace efl;

int main( int argc, const char **argv )
{
    /* Create the application object */
    EcoreApplication* app = new EcoreApplication( argc, argv, "Esmart Container Test" );

    /* Create the main window, a window with an embedded canvas */
    EcoreEvasWindow* mw = new EcoreEvasWindow( WIDTH, HEIGHT );

    EvasCanvas* evas = mw->canvas();

    evas->appendFontPath( PACKAGE_DATA_DIR "/fonts" );

    /* Add some objects to the canvas */
    EvasRectangle* rect = new EvasRectangle( 0, 0, WIDTH, HEIGHT, evas );
    rect->setColor( 200, 200, 200, 255 );
    rect->setLayer( 0 );
    rect->show();

    EvasEsmartTextEntry* passwd = new EvasEsmartTextEntry( 50, 50, 100, 10, evas );
    passwd->setColor( 0, 0, 0, 255 );
    passwd->setLayer( 10 );
    passwd->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

