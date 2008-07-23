#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_edje.h>
#include <eflpp_emotion.h>
using namespace efl;

#define WIDTH 320
#define HEIGHT 240

#include <iostream>
using namespace std;

#include <assert.h>

int main( int argc, const char **argv )
{
    /* Create the application object */
    EcoreApplication* app = new EcoreApplication( argc, argv, "Simple Emotion Test" );
    if ( argc < 2 )
    {
        cerr << "Usage: " << argv[0] << "<video file>" << endl;
        return 1;
    }
    /* Create the main window, a window with an embedded canvas */
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );
    EvasCanvas* evas = mw->canvas();
    evas->appendFontPath( PACKAGE_DATA_DIR "/fonts" );

    /* Create EvasEmotion object using xine engine */
    EvasEmotion* emotion = new EvasEmotion( 0, 0, 320, 240, argv[1], "xine", evas );
    emotion->setSmoothScale( 1 );
	
    emotion->setPlay( true );
    emotion->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

