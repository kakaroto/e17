#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_esmart_container.h>

#define WIDTH 240
#define HEIGHT 320

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

    EvasRectangle* buttonbackground = new EvasRectangle( evas );
    buttonbackground->setColor( 255, 0, 0, 255 );
    buttonbackground->setLayer( 10 );
    buttonbackground->setGeometry( 10, 10, 20, 20 );
    EvasText* buttontext = new EvasText( "Vera", 14, "Click on this button", evas );
    buttontext->setColor( 0, 0, 0, 255 );
    buttontext->setLayer( 11 );

    EvasEsmartContainer* vbox = new EvasEsmartContainer( 50, 50, EvasEsmartContainer::Vertical, evas );
    vbox->resize( 150, 100 );
    vbox->setLayer( 20 );
    vbox->setAlignment( EvasEsmartContainer::Right );
    //vbox->setPadding( 10, 10, 10, 10 );
    //vbox->setSpacing( 0 );
    vbox->setFillPolicy( (EvasEsmartContainer::FillPolicy) (EvasEsmartContainer::None ) );
    vbox->append( buttonbackground );
    vbox->append( buttontext );
    vbox->show();

    //vbox->setScrollOffset( 1 );
    //vbox->setScrollPercent( 20 );
    //vbox->startScrolling( 2 );
    //vbox->scroll( 10 );

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

