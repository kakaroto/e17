#include <eflpp_sys.h>
#include <eflpp_evas.h>
#include <eflpp_ecore.h>
#include <eflpp_edje.h>
#include <eflpp_etk.h>

#include <etk/Etk.h>

#define WIDTH 240
#define HEIGHT 320

#include <iostream>
using std::cout;
using std::endl;

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

using namespace efl;

int main( int argc, const char **argv )
{
    /* Create the application object */
    EcoreApplication* app = new EcoreApplication( argc, argv, "Simple Ecore Test" );

    /* Create the main window, a window with an embedded canvas */
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );

    EvasCanvas* evas = mw->canvas();

    evas->appendFontPath( PACKAGE_DATA_DIR "/fonts" );

    /* Add some objects to the canvas */
#if 1
    EvasEdje* edje = new EvasEdje( 0, 0, PACKAGE_DATA_DIR "/edjes/angstrom-bootmanager.edj", "background", evas );
    edje->resize( WIDTH, HEIGHT );
    Size s = edje->minimalSize();
    edje->setLayer( 1 );
    edje->show();
#else
    EvasRectangle* b = new EvasRectangle( 50, 50, 100, 100, evas );
    b->show();
#endif

    EtkEmbed* embed = new EtkEmbed( evas );
    EvasEtk* evasetk = new EvasEtk( embed, "evasetk" );

    embed->setFocus( true );
    evasetk->move( 5, 5 );
    evasetk->resize( 20, 20 );
    evasetk->setLayer( 2 );
    evasetk->show();
    embed->show();

    EtkButton* button = new EtkButton( "Hello World!" );
    embed->add( button );
    button->show();
    embed->show();

    //ewl_callback_append(EWL_WIDGET( embed->obj() ), EWL_CALLBACK_CONFIGURE, move_embed_contents_cb, vbox->obj());

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

