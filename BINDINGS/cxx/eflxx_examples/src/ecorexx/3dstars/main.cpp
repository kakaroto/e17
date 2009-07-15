#include <eflxx/eflpp_sys.h>
#include <eflxx/eflpp_debug.h>
#include <ecorexx/EcoreApplication.h>
#include <ecorexx/EcoreTimer.h>
#include <evasxx/Evasxx.h>

#include <sigc++/sigc++.h>
#include <cmath>
#include <list>
#include <memory>

using namespace std;
using namespace efl;

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 600;
const unsigned int STARS = 64;
const unsigned int speed = 15;
const double rotation = 0.0005;

static int width = 0;
static int height = 0;

class Star : public EvasLine
{
  static double angle;
public:
  Star( EvasCanvas &evas ) : EvasLine( evas )
  {
    setLayer( 10 );
    reset();
    draw();
    show ();
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
    setColor( Color (255, 255, 255, 255) );
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
      cout << "before: " << getGeometry () << endl;
      setGeometry( Rect (x, y, 1, 0) );
      cout << "after: " << getGeometry () << endl << endl;
      setColor( Color ((int)(300-_z/5), (int)(300-_z/5), (int)(300-_z/5), 255) );
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
  auto_ptr <EcoreApplication> app (new EcoreApplication( argc, argv, "Ecore 3D Stars Test" ));
  auto_ptr <EcoreEvasWindowSoftwareX11> mw (new EcoreEvasWindowSoftwareX11( Size (WIDTH, HEIGHT) ));
  EvasCanvas &evas = mw->getCanvas();
  
  Rect bg = evas.getGeometry();
  width = bg.width();
  height = bg.height();

  auto_ptr <EvasRectangle> r (new EvasRectangle( evas, evas.getGeometry() ));
  r->setColor( Color (0, 0, 0, 255) );
  r->setLayer( 0 );
  r->show();

  for ( unsigned int i = 0; i < STARS; ++i )
  {
    // FIXME: Memory leak, but ok for this example
    // better use CountedPtr or delete it at the end
    starfield.push_back( new Star( evas ) );
  }

  // FIXME: Memory leak, but ok for this example
  // better use CountedPtr or delete it at the end
  (new EcoreTimer( 0.05 ) )->timeout.connect( sigc::ptr_fun( ::advance ) );

  mw->show();

  /* Enter the application main loop */
  app->exec();

  return 0;
}

