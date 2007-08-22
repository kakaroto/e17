#include <eflpp_sys.h>
#include <eflpp_debug.h>
#include <eflpp_ecore.h>
#include <eflpp_evas.h>
using namespace efl;

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 600;
const unsigned int STARS = 64;
const unsigned int speed = 15;
const double rotation = 0.0005;

static int width = 0;
static int height = 0;

#include <sigc++/sigc++.h>
#include <cmath>
#include <list>
using std::pair;
using std::list;

class Star : public EvasLine
{
    static double angle;
    public:
        Star( EvasCanvas* evas ) : EvasLine( evas )
        {
            setLayer( 10 );
            reset();
            draw();
        }
        void reset()
        {
            _x = -500.0 + (1000.0*rand() / (RAND_MAX -500.0));
            _y = -500.0 + (1000.0*rand() / (RAND_MAX -500.0));
            _z = 256.0 + (1000.0*rand() / (RAND_MAX +256.0));
            eflDout( dc::notice, "Star @ " << _x << ", " << _y << ", " << _z );
        }
        void draw()
        {
            setColor( 255, 255, 255, 255 );
            int _xrot = _x * cos( angle ) - _y * sin( angle );
            int _yrot = _x * sin( angle ) + _y * cos( angle );
            int x = _xrot*256.0/_z + width/2;
            int y = _yrot*256.0/_z + height/2;
            if ( (x > width) || (y > height) || (x<0) || (y<0) )
            {
                eflDout( dc::notice, "Star ( " << x << ", " << y << ") left the screen @ zpos = " << _z );
                hide();
                reset();
            }
            else
            {
                setGeometry( x, y, 1, 0 );
                setColor( (int)(300-_z/5), (int)(300-_z/5), (int)(300-_z/5), 255 );
                show();
            }
        }
        void advance( int velocity, double rotation )
        {
             angle += rotation;
            _z -= speed;
            draw();
        }
        ~Star() {};
    private:
        double _x;
        double _y;
        double _z;
};

double Star::angle = 0;
typedef list<Star*> Starfield;
typedef list<Star*>::iterator StarfieldIterator;
Starfield starfield;

void advance( EcoreTimer* e )
{
    for (StarfieldIterator it = starfield.begin(); it != starfield.end(); ++it )
    {
        Star* star = *it;
        star->advance( speed, rotation );
    }
}

int main( int argc, const char **argv )
{
    EcoreApplication* app = new efl::EcoreApplication( argc, argv, "Ecore 3D Stars Test" );
    EcoreEvasWindowSoftwareX11* mw = new efl::EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );
    EvasCanvas* evas = mw->canvas();

    Rect bg = evas->geometry();
    width = bg.width();
    height = bg.height();

    EvasRectangle* r = new EvasRectangle( bg, evas );
    r->setColor( 0, 0, 0, 255 );
    r->setLayer( 0 );
    r->show();

    for ( unsigned int i = 0; i < STARS; ++i )
    {
        starfield.push_back( new Star( evas ) );
    }

    ( new EcoreTimer( 0.05 ) )->timeout.connect( sigc::ptr_fun( advance ) );

    /* Enter the application main loop */
    app->exec();

    /* Delete the application */
    delete app;

    return 0;
}

