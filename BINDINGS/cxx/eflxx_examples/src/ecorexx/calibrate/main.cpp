#include <eflxx/eflpp_sys.h>
#include <evasxx/Evasxx.h>
#include <ecorexx/EcoreApplication.h>
#include "calibrate.h"

using namespace efl;

#define WIDTH 240
#define HEIGHT 320

int main( int argc, const char **argv )
{
  EcoreApplication app( argc, argv, "Ecore Calibration Example" );
  EcoreEvasWindowSoftwareX11* mw = new EcoreEvasWindowSoftwareX11( Size (WIDTH, HEIGHT) );
  EvasCanvas &evas = mw->getCanvas();
  Size s = evas.getSize();

  CalibrationRectangle* r = new CalibrationRectangle( evas, Rect (0, 0, s.width(), s.height()) );
  r->show(); // starts calibration

  mw->show();

  app.exec();
  return 0;
}
