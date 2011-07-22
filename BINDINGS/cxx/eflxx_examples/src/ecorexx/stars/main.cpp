#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>

#include <sigc++/sigc++.h>
#include <iostream>
#include <list>
#include <memory>

using namespace Eflxx;
using namespace std;

#define WIDTH 320
#define HEIGHT 240

typedef pair<Evasxx::Line*, int> Star;
typedef list<Star*> Starfield;
typedef list<Star*>::iterator StarfieldIterator;

Starfield starfield;

int width = 0;

bool advance (Ecorexx::Timer &timer)
{
  for (StarfieldIterator it = starfield.begin(); it != starfield.end(); ++it )
  {
    Star* star = *it;
    Evasxx::Line* line = star->first;
    int speed = star->second;
    Rect g = line->getGeometry();
    line->setGeometry( Rect ((g.x()+speed ) % width, g.y(), 1, 0) );
  }

  return true;
}

int main( int argc, const char **argv )
{
  auto_ptr <Ecorexx::Application> app (new Ecorexx::Application( argc, argv, "Ecore Stars Test" ));
  auto_ptr <Ecorexx::EvasWindowSoftwareX11> mw (new Ecorexx::EvasWindowSoftwareX11( Size (WIDTH, HEIGHT) ));
  Evasxx::Canvas &evas = mw->getCanvas();

  Rect bg = evas.getGeometry();
  width = bg.width();

  Evasxx::Rectangle* r = new Evasxx::Rectangle( evas, bg );
  r->setColor( Color (0, 0, 0, 255) );
  r->setLayer( 0 );
  r->show();

  for ( int scanline = 0; scanline < bg.height(); ++scanline )
  {
    int xpos = (int) ((double)bg.width()*rand()/(RAND_MAX));
    int speed = 1+(int) (10.0*rand()/(RAND_MAX+1.0));
    Evasxx::Line* line = new Evasxx::Line( evas, Point (xpos, scanline), Point (xpos+1, scanline) );
    line->setColor( Color (50+speed*16, 50+speed*18, 50+speed*17, 255) );
    line->setLayer( 10 );
    line->show();

    starfield.push_back( new Star( line, speed ) );
  }

  sigc::slot <bool, Ecorexx::Timer&> timerSlot = sigc::ptr_fun (&::advance);

  Ecorexx::Timer *timer = Ecorexx::Timer::factory (0.08, timerSlot);
  
//  ( new Ecorexx::Timer( 0.08 ) )->timeout.connect( sigc::ptr_fun( ::advance ) );

  mw->show ();

  /* Enter the application main loop */
  app->exec();

  timer->destroy ();

  return 0;
}

