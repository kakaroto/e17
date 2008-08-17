#include <eflpp_ecore.h>
#include <eflpp_evas.h>

#include <cmath>
#include <iostream>
using namespace std;

const int WIDTH = 320;
const int HEIGHT = 0;
const double PI = 3.141;
const unsigned int NUMBALLS = 15;
const unsigned int BALLSIZE = 50;
const unsigned int BALLWIDTH = WIDTH-BALLSIZE;
const unsigned int BALLHEIGHT = HEIGHT-BALLSIZE;

class TimerApp : public efl::EcoreApplication
{
public:

TimerApp( int argc, const char** argv ) : efl::EcoreApplication( argc, argv, "Ecore Timer Test" ),
                                          xoffset( 0 ), yoffset( PI/3 ),
                                          xstep( 2*PI/360 ), ystep( 2*PI/360 ),
                                          size( BALLSIZE ), direction( -1 ),
                                          alpha( 0 ), alphadirection( 1 ),
                                          xaddfactor( 1 ), yaddfactor( 1 )
{
    mw = new efl::EcoreEvasWindowSoftwareX11( WIDTH, HEIGHT );
    evas = mw->canvas();

    image = new efl::EvasImage( 0, 0, PACKAGE_DATA_DIR "/images/panel.png", evas );
    image->resize( WIDTH, HEIGHT );
    image->setFill( 0, 0, image->trueSize() );
    image->show();

    shadow = new efl::EvasImage( 0, 0, PACKAGE_DATA_DIR "/images/panel_shadow.png", evas );
    shadow->resize( WIDTH, HEIGHT );
    shadow->setFill( 0, 0, WIDTH, HEIGHT );
    shadow->show();
    shadow->setFocus( true );

    logo = new efl::EvasImage( 40, 20, PACKAGE_DATA_DIR "/images/e_logo.png", evas );
    logo->setColor( 255, 255, 255, 0 );
    logo->show();

    for( unsigned int i = 0; i < NUMBALLS; ++i )
    {
        balls[i] = new efl::EvasImage( 20*i, 20*i, PACKAGE_DATA_DIR "/images/boing-shadow.png", evas );
        balls[i]->resize( BALLSIZE, BALLSIZE, true );
        balls[i]->setColor( 255, 255, 255, 240 );
        balls[i]->show();
    }

    startTimer( 1.0 / 25 );
}

efl::EcoreEvasWindowSoftwareX11* mw;
efl::EvasCanvas* evas;
efl::EvasImage* image, *shadow, *logo;
efl::EvasImage* balls[NUMBALLS];
double xoffset; double yoffset;
double xstep; double ystep;
int size; int direction;
int alpha; int alphadirection;
double xaddfactor; double yaddfactor;

virtual bool timerEvent()
{

    logo->setColor( 255, 255, 255, alpha );
    alpha += alphadirection;
    if ( alpha == 0 || alpha == 255 ) alphadirection *= -1;

    for ( unsigned int i = 0; i < NUMBALLS; ++i )
    {
        balls[i]->move( BALLWIDTH  / 2 + sin( xoffset + xaddfactor*i*(2*PI/NUMBALLS) ) * BALLWIDTH/2,
                        BALLHEIGHT / 2 + cos( yoffset + yaddfactor*i*(2*PI/NUMBALLS) ) * BALLHEIGHT/2 );
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
