#include <evasxx/Evasxx.h>
#include <ecorexx/EcoreApplication.h>
#include "../../common/searchFile.h"

#include <cmath>
#include <iostream>

using namespace std;
using namespace efl;

const int WIDTH = 320;
const int HEIGHT = 320;
const double PI = 3.141;
const unsigned int NUMBALLS = 15;
const unsigned int BALLSIZE = 50;
const unsigned int BALLWIDTH = WIDTH-BALLSIZE;
const unsigned int BALLHEIGHT = HEIGHT-BALLSIZE;

class TimerApp : public EcoreApplication
{
public:

  TimerApp( int argc, const char** argv ) : EcoreApplication( argc, argv, "Ecore Timer Test" ),
      xoffset( 0 ), yoffset( PI/3 ),
      xstep( 2*PI/360 ), ystep( 2*PI/360 ),
      size( BALLSIZE ), direction( -1 ),
      alpha( 0 ), alphadirection( 1 ),
      xaddfactor( 1 ), yaddfactor( 1 )
  {
    EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( size );
    EvasCanvas &evas = mw->getCanvas();

    const Size size (WIDTH, HEIGHT);

    image = new EvasImage( evas, searchPixmapFile ("panel.png") );
    image->resize( size );
    image->setFill( Rect (0, 0, WIDTH, HEIGHT) );
    image->show();

    shadow = new EvasImage( evas, searchPixmapFile ("panel_shadow.png" ));
    shadow->resize( size );
    shadow->setFill( Rect (0, 0, WIDTH, HEIGHT) );
    shadow->show();
    shadow->setFocus( true );

    logo = new EvasImage( evas, searchPixmapFile ("e_logo.png") );
    logo->setColor( Color (255, 255, 255, 0) );
    logo->show();

    for ( unsigned int i = 0; i < NUMBALLS; ++i )
    {
      balls[i] = new EvasImage( evas, Point (20*i, 20*i), searchPixmapFile ("boing-shadow.png") );
      balls[i]->resize( Size (BALLSIZE, BALLSIZE), true );
      balls[i]->setColor( Color (255, 255, 255, 240) );
      balls[i]->show();
    }

    startTimer( 1.0 / 25 );

    mw->show();
  }

  EvasImage* image, *shadow, *logo;
  EvasImage* balls[NUMBALLS];
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

  virtual bool timerEvent()
  {

    logo->setColor( Color (255, 255, 255, alpha) );
    alpha += alphadirection;
    if ( alpha == 0 || alpha == 255 ) alphadirection *= -1;

    for ( unsigned int i = 0; i < NUMBALLS; ++i )
    {
      balls[i]->move( Point (BALLWIDTH  / 2 + sin( xoffset + xaddfactor*i*(2*PI/NUMBALLS) ) * BALLWIDTH/2,
                             BALLHEIGHT / 2 + cos( yoffset + yaddfactor*i*(2*PI/NUMBALLS) ) * BALLHEIGHT/2 ));
      balls[i]->resize( size, size );
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

    return true; // call me again, please
  }

};

int main( int argc, const char **argv )
{
  TimerApp* app = new TimerApp( argc, argv );
  app->exec();
  delete app;
  return 0;
}
