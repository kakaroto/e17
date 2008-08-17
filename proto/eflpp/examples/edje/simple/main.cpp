#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_edje.h>

#define WIDTH 240
#define HEIGHT 320

#include <assert.h>

using namespace efl;

int main( int argc, const char **argv )
{
    /* Create the application object */
    EcoreApplication* app = new EcoreApplication( argc, argv, "Simple Edje Test" );

    /* Create the main window, a window with an embedded canvas */
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );

    EvasCanvas* evas = mw->canvas();

    evas->appendFontPath( PACKAGE_DATA_DIR "fonts" );

    /* Add some objects to the canvas */

    EvasEdje* edje = new EvasEdje( 0, 0, PACKAGE_DATA_DIR "/edjes/simple.edj", "test", evas );

    edje->resize( WIDTH, HEIGHT );
    Size s = edje->minimalSize();
    edje->setLayer( 0 );
    edje->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

