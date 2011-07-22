#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include "../../common/searchFile.h"

#include <cmath>
#include <iostream>
#include <memory>

using namespace std;
using namespace Eflxx;

const int WIDTH = 320;
const int HEIGHT = 320;
const double PI = 3.141;
const unsigned int NUMBALLS = 15;
const unsigned int BALLSIZE = 50;
const unsigned int BALLWIDTH = WIDTH-BALLSIZE;
const unsigned int BALLHEIGHT = HEIGHT-BALLSIZE;

class TimerApp : public Ecorexx::Application
{
public:

  TimerApp( int argc, const char** argv )
    : Ecorexx::Application( argc, argv, "Ecore Timer Test" ),
      xoffset( 0 ), yoffset( PI/3 ),
      xstep( 2*PI/360 ), ystep( 2*PI/360 ),
      size( BALLSIZE ), direction( -1 ),
      alpha( 0 ), alphadirection( 1 ),
      xaddfactor( 1 ), yaddfactor( 1 ),
      ecoreTimer (NULL)
  {
    Ecorexx::EvasWindowSoftwareX11* mw = new Ecorexx::EvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );
    Evasxx::Canvas &evas = mw->getCanvas();

    image = new Evasxx::Image( evas, searchPixmapFile ("panel.png") );
    image->resize( evas.getSize() );
    //image->setFill( 0, 0, image->trueSize() ); //FIXME
    image->show();

    shadow = new Evasxx::Image( evas, searchPixmapFile ("panel_shadow.png") );
    shadow->resize( evas.getSize() );
    //shadow->setFill( 0, 0, evas.size() ); //FIXME
    shadow->show();
    shadow->setFocus( true );

    logo = new Evasxx::Image( evas, Point (40, 20), searchPixmapFile ("e_logo.png") );
    logo->setColor( Color (255, 255, 255, 0) );
    logo->show();

    for ( unsigned int i = 0; i < NUMBALLS; ++i )
    {
      balls[i] = new Evasxx::Image( evas, Point (20*i, 20*i), searchPixmapFile ("boing-shadow.png") );
      balls[i]->resize( Size (BALLSIZE, BALLSIZE), true );
      balls[i]->setColor( Color (255, 255, 255, 240) );
      balls[i]->show();
    }

    sigc::slot <bool, Ecorexx::Timer&> timerSlot = sigc::mem_fun (*this, &TimerApp::timerEvent);

    ecoreTimer = Ecorexx::Timer::factory (1.0 / 25, timerSlot);

    mw->show();
  }

  Evasxx::Image* image, *shadow, *logo;
  Evasxx::Image* balls[NUMBALLS];

  double xoffset;
  double yoffset;
  double xstep;
  double ystep;
  int size;
  int direction;
  int alpha;
  int alphadirection;
  double xaddfactor;
  double yaddfactor;
  Ecorexx::Timer* ecoreTimer;

  bool timerEvent (Ecorexx::Timer &timer)
  {

    logo->setColor( Color (255, 255, 255, alpha) );
    alpha += alphadirection;
    if ( alpha == 0 || alpha == 255 ) alphadirection *= -1;

    for ( unsigned int i = 0; i < NUMBALLS; ++i )
    {
      balls[i]->move( Point (BALLWIDTH  / 2 + sin( xoffset + xaddfactor*i*(2*PI/NUMBALLS) ) * BALLWIDTH/2,
                             BALLHEIGHT / 2 + cos( yoffset + yaddfactor*i*(2*PI/NUMBALLS) ) * BALLHEIGHT/2) );
      balls[i]->resize( Size (size, size) );
    }
    xoffset += xstep;
    yoffset += ystep;
    size += direction;
    if ( size == -5 || size == BALLSIZE+10 ) direction *= -1;
    if ( size == -5 )
    {
      xstep = 2*PI / (360.0*rand()/(RAND_MAX));
      ystep = 2*PI / (360.0*rand()/(RAND_MAX));
      xaddfactor = -2 + (4.0*rand()/(RAND_MAX));
      yaddfactor = -2 + (4.0*rand()/(RAND_MAX));
    }

    return true;
  }

  ~TimerApp ()
  {
    ecoreTimer->destroy ();
  }

};

int main( int argc, const char **argv )
{
  auto_ptr <TimerApp> app (new TimerApp( argc, argv ));
  app->exec();

  return 0;
}
