#include <eflxx/Eflxx.h>
#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>

#include <sigc++/sigc++.h>
#include <cmath>
#include <list>
#include <memory>

using namespace std;
using namespace Eflxx;

const unsigned int WIDTH = 600;
const unsigned int HEIGHT = 600;
const unsigned int STARS = 64;
const unsigned int speed = 15;
const double rotation = 0.0005;

static int width = 0;
static int height = 0;

class Star : public Evasxx::Line
{
  static double angle;
public:
  Star( Evasxx::Canvas &evas ) : Evasxx::Line( evas )
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

bool advance (Ecorexx::Timer &timer)
{
  for (StarfieldIterator it = starfield.begin(); it != starfield.end(); ++it )
  {
    Star* star = *it;
    star->advance( speed, rotation );
  }

  return true;
}

int main( int argc, const char **argv )
{
  auto_ptr <Ecorexx::Application> app (new Ecorexx::Application( argc, argv, "Ecore 3D Stars Test" ));
  auto_ptr <Ecorexx::EvasWindowSoftwareX11> mw (new Ecorexx::EvasWindowSoftwareX11( Size (WIDTH, HEIGHT) ));
  Evasxx::Canvas &evas = mw->getCanvas();
  
  Rect bg = evas.getGeometry();
  width = bg.width();
  height = bg.height();

  auto_ptr <Evasxx::Rectangle> r (new Evasxx::Rectangle( evas, evas.getGeometry() ));
  r->setColor( Color (0, 0, 0, 255) );
  r->setLayer( 0 );
  r->show();

  for ( unsigned int i = 0; i < STARS; ++i )
  {
    // FIXME: Memory leak, but ok for this example
    // better use CountedPtr or delete it at the end
    starfield.push_back( new Star( evas ) );
  }
  
  sigc::slot <bool, Ecorexx::Timer&> timerSlot = sigc::ptr_fun (&::advance);

  Ecorexx::Timer *timer = Ecorexx::Timer::factory (0.05, timerSlot);

  mw->show();

  /* Enter the application main loop */
  app->exec();

  timer->destroy ();
  
  return 0;
}

