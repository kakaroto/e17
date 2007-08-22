#include <eflpp_sys.h>
#include <eflpp_evas.h>
#include <eflpp_ecore.h>
#include <eflpp_edje.h>
#include <eflpp_ewl.h>

#define WIDTH 240
#define HEIGHT 320

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Ewl.h>

#include <iostream>
using std::cout;
using std::endl;

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

static void move_embed_contents_cb(Ewl_Widget *w, void *ev_data __UNUSED__,void *user_data)
{
    cout << "::move_embed_contents_cb " << endl;
    ewl_object_geometry_request(EWL_OBJECT(user_data), CURRENT_X(w), CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}

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

    EvasEdje* edje = new EvasEdje( 0, 0, PACKAGE_DATA_DIR "/edjes/angstrom-bootmanager.edj", "background", evas );
    edje->resize( WIDTH, HEIGHT );
    Size s = edje->minimalSize();
    edje->setLayer( 1 );
    edje->show();

    ewl_init( &argc, const_cast<char**>( argv ) );

    EwlEmbed* embed = new EwlEmbed();
    EvasEwl* evasewl = new EvasEwl( embed, evas, "evasewl" );
    evasewl->setGeometry( 25, 25, 100, 100 );

    embed->setFocus( true );
    evasewl->setLayer( 10 );
    evasewl->show();
    embed->show();

    EwlVBox* vbox = new EwlVBox();
    embed->appendChild( vbox );
    vbox->show();
    EwlButton* button = new EwlButton( "Hello World!" );
    vbox->appendChild( button );
    button->show();

    //ewl_callback_append(EWL_WIDGET( embed->obj() ), EWL_CALLBACK_CONFIGURE, move_embed_contents_cb, vbox->obj());

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

