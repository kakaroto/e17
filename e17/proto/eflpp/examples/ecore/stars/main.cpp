#include <eflpp_ecore.h>
#include <eflpp_evas.h>
using namespace efl;

#define WIDTH 320
#define HEIGHT 240

#include <sigc++/sigc++.h>

#include <iostream>
#include <list>
using std::pair;
using std::cout;
using std::endl;
using std::list;

typedef pair<EvasLine*, int> Star;
typedef list<Star*> Starfield;
typedef list<Star*>::iterator StarfieldIterator;

Starfield starfield;

int width = 0;

void advance( EcoreTimer* e )
{
    for (StarfieldIterator it = starfield.begin(); it != starfield.end(); ++it )
    {
        Star* star = *it;
        EvasLine* line = star->first;
        int speed = star->second;
        Rect g = line->geometry();
        line->setGeometry( ( g.x()+speed ) % width, g.y(), 1, 0 );
    }

}

int main( int argc, const char **argv )
{
    EcoreApplication* app = new efl::EcoreApplication( argc, argv, "Ecore Stars Test" );
    EcoreEvasWindowSoftwareX11* mw = new efl::EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );
    EvasCanvas* evas = mw->canvas();

    Rect bg = evas->geometry();
    width = bg.width();

    EvasRectangle* r = new EvasRectangle( bg, evas );
    r->setColor( 0, 0, 0, 255 );
    r->setLayer( 0 );
    r->show();

    for ( int scanline = 0; scanline < bg.height(); ++scanline )
    {
        int xpos = (int) ((double)bg.width()*rand()/(RAND_MAX));
        int speed = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
        EvasLine* line = new EvasLine( xpos, scanline, xpos+1, scanline, evas );
        line->setColor( 50+speed*16, 50+speed*18, 50+speed*17, 255 );
        line->setLayer( 10 );
        line->show();

        starfield.push_back( new Star( line, speed ) );
    }

    ( new EcoreTimer( 0.08 ) )->timeout.connect( sigc::ptr_fun( advance ) );

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

