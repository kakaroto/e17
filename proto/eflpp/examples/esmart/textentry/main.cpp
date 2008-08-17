#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_edje.h>
#include <eflpp_esmart_textentry.h>

#define WIDTH 300
#define HEIGHT 30

using namespace efl;

int main( int argc, const char **argv )
{
    /* Create the application object */
    EcoreApplication* app = new EcoreApplication( argc, argv, "Esmart Container Test" );

    /* Create the main window, a window with an embedded canvas */
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );

    EvasCanvas* evas = mw->canvas();

    evas->appendFontPath( PACKAGE_DATA_DIR "/fonts" );

    /* Add some objects to the canvas */
    EvasRectangle* rect = new EvasRectangle( 0, 0, WIDTH, HEIGHT, evas );
    rect->setColor( 200, 200, 200, 255 );
    rect->setLayer( 0 );
    rect->show();

    EvasEdje* edje = new EvasEdje( 0, 0, PACKAGE_DATA_DIR "/edjes/esmart_text_entry_test.edj", "text_entry", evas );

    edje->resize( WIDTH, HEIGHT );
    edje->setLayer( 1 );
    edje->show();

    EvasEsmartTextEntry* passwd = new EvasEsmartTextEntry( 0, 0, WIDTH, HEIGHT, evas );
    passwd->setEdjePart (edje, "text");
    passwd->setColor( 0, 0, 0, 255 );
    passwd->setLayer( 0 );
    passwd->setFocus (true);
    passwd->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}
