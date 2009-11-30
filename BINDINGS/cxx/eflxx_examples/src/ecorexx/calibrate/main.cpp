#include <eflxx/System.h>
#include <evasxx/Evasxx.h>
#include <ecorexx/Ecorexx.h>
#include "calibrate.h"

using namespace Eflxx;

#define WIDTH 240
#define HEIGHT 320

int main( int argc, const char **argv )
{
  Ecorexx::Application app( argc, argv, "Ecore Calibration Example" );
  Ecorexx::EvasWindowSoftwareX11* mw = new Ecorexx::EvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );
  Evasxx::Canvas &evas = mw->getCanvas();
  Size s = evas.getSize();

  CalibrationRectangle* r = new CalibrationRectangle( evas, Rect (0, 0, s.width(), s.height()) );
  r->show(); // starts calibration

  mw->show();

  app.exec();
  return 0;
}
