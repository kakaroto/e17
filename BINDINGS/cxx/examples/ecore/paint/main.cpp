#include <eflpp_sys.h>
#include <eflpp_debug.h>
#include <eflpp_ecore.h>
#include <eflpp_evas.h>
#include <eflpp_evasevent.h>
using namespace efl;

#include <iostream>
using namespace std;

#define WIDTH 240
#define HEIGHT 320

#define DOTSIZE 2

static int callback( void* data, int type, void *event )
{
    cout << "CALLBACK RECEIVED: type = " << type << ", pevent = " << event << "pdata = " << data << endl;
    return 0;
}

class PaintArea : public EvasRectangle
{
public:
    PaintArea( EvasCanvas* evas ) : EvasRectangle( evas ), mouseDown( false )
    {
        setLayer( 10 );
        setColor( 255, 255, 255, 0 ); // fully transparent, to catch the mouse events
        setFocus( true );
    }
    virtual ~PaintArea() { };

    virtual bool handleMouseUp( const EvasMouseUpEvent& e )
    {
        cout << "PA::mouseUp" << endl;
        mouseDown = false;
    }
    virtual bool handleMouseDown( const EvasMouseDownEvent& e )
    {
        cout << "PA::mouseDown" << endl;
        mouseDown = true;
        EvasObject* l = new EvasRectangle( e.data->canvas.x, e.data->canvas.y, 2, 2, canvas() );
        int b=200+(int) (55.0*rand()/(RAND_MAX+1.0));
        l->setColor( b, b, b, 255 );
        l->show();
    }
    virtual bool handleMouseMove( const EvasMouseMoveEvent& e )
    {
        cout << "PA::mouseMove" << endl;
        if ( mouseDown )
        {
            int width = 2 + abs( e.data->cur.canvas.x - e.data->prev.canvas.x );
            int height = 2 + abs( e.data->cur.canvas.y - e.data->prev.canvas.y );
            cout << "width = " << width << ", height = " << height << endl;
            EvasObject* l = new EvasRectangle( e.data->cur.canvas.x - width/2, e.data->cur.canvas.y - height/2, width, height, canvas() );
            int b=200+(int) (55.0*rand()/(RAND_MAX+1.0));
            l->setColor( b, b, b, 255 );
            l->show();
        }
    }
    virtual bool handleKeyUp( const EvasKeyUpEvent& e )
    {
        cout << "PA:::keyUp - released '" << e.data->keyname << "'" << endl;
        if ( strcmp( e.data->keyname, "Escape" ) == 0 )
        {
            eApp->quit();
        }
        if ( strcmp( e.data->keyname, "0x5b" ) == 0 )
        {
            int rot = eApp->mainWindow()->rotation();
            rot = rot ? 0: 270;
            eApp->mainWindow()->setRotation( rot );
            //eApp->mainWindow()->resize(
        }
        if ( strcmp( e.data->keyname, "j" ) == 0 )
        {
            Rect vp = eApp->mainWindow()->canvas()->viewport();
            eApp->mainWindow()->canvas()->setViewport( vp.x()+10, vp.y()+10, vp.width()-10, vp.height()-10 );
        }
        if ( strcmp( e.data->keyname, "k" ) == 0 )
        {
            Rect vp = eApp->mainWindow()->canvas()->viewport();
            eApp->mainWindow()->canvas()->setViewport( vp.x()-10, vp.y()-10, vp.width()+10, vp.height()+10 );
        }

    }
private:
    bool mouseDown;
};

int main( int argc, const char **argv )
{
    /* Create the application object */
    efl::EcoreApplication* app = new efl::EcoreApplication( argc, argv, "Ecore Paint Example" );

    /* Create the main window, a window with an embedded canvas */
    efl::EcoreEvasWindowSoftwareX11* mw = new efl::EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );
    mw->setAlpha (true);
  
    mw->show ();
  
    /* Create some objects on the canvas */
    efl::EvasCanvas* evas = mw->canvas();

    Size s = evas->size();
    cout << "PA: Size = " << s << endl;

    efl::EvasRectangle* bg = new efl::EvasRectangle( 0, 0, s.width(), s.height(), evas );
    bg->setColor( 50, 50, 50, 128 );
    bg->setLayer( 0 );
    bg->show();

    PaintArea* p = new PaintArea( evas );
    p->setGeometry( 0, 0, s.width(), s.height() );
    p->setLayer( 1 );
    p->show();

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}
