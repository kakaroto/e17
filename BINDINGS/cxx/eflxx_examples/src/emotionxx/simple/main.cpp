#include <ecorexx/EcoreApplication.h>
#include <evasxx/Evasxx.h>
#include <edjexx/Edjexx.h>
#include <emotionxx/EvasEmotion.h>
#include "../../common/searchFile.h"

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
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );
    EvasCanvas &evas = mw->getCanvas();
    evas.appendFontPath( searchDataDir () + "/fonts" );

    /* Create EvasEmotion object using xine engine */
    EvasEmotion* emotion = new EvasEmotion( evas, Rect (0, 0, 320, 240), argv[1], "xine" );
    emotion->setSmoothScale( 1 );
	
    emotion->setPlay( true );
    emotion->show();

    mw->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

