#include <ecorexx/EcoreApplication.h>
#include <ecorexx/EcoreTimer.h>
#include <evasxx/Evasxx.h>

#include <sigc++/sigc++.h>
#include <iostream>
#include <list>

using namespace efl;
using namespace std;

#define WIDTH 320
#define HEIGHT 240

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
    Rect g = line->getGeometry();
    line->setGeometry( Rect ((g.x()+speed ) % width, g.y(), 1, 0) );
  }
}

int main( int argc, const char **argv )
{
  EcoreApplication* app = new EcoreApplication( argc, argv, "Ecore Stars Test" );
  EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );
  EvasCanvas &evas = mw->getCanvas();

  Rect bg = evas.getGeometry();
  width = bg.width();

  EvasRectangle* r = new EvasRectangle( evas, bg );
  r->setColor( Color (0, 0, 0, 255) );
  r->setLayer( 0 );
  r->show();

  for ( int scanline = 0; scanline < bg.height(); ++scanline )
  {
    int xpos = (int) ((double)bg.width()*rand()/(RAND_MAX));
    int speed = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
    EvasLine* line = new EvasLine( evas, Point (xpos, scanline), Point (xpos+1, scanline) );
    line->setColor( Color (50+speed*16, 50+speed*18, 50+speed*17, 255) );
    line->setLayer( 10 );
    line->show();

    starfield.push_back( new Star( line, speed ) );
  }

  ( new EcoreTimer( 0.08 ) )->timeout.connect( sigc::ptr_fun( ::advance ) );

  mw->show ();

  /* Enter the application main loop */
  app->exec();

  /* Delete the application */
  delete app;

  return 0;
}

