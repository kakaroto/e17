#include <ecorexx/Ecorexx.h>
#include <evasxx/Evasxx.h>
#include <edjexx/Edjexx.h>
#include <emotionxx/Emotionxx.h>
#include "../../common/searchFile.h"
#include <iostream>
#include <assert.h>

using namespace Eflxx;
using namespace std;

int main( int argc, const char **argv )
{
  /* Create the application object */
  Ecorexx::Application* app = new Ecorexx::Application( argc, argv, "Simple Emotion Test" );
  if ( argc < 2 )
  {
      cerr << "Usage: " << argv[0] << "<audio file>" << endl;
      return 1;
  }
  /* Create the main window, a window with an embedded canvas */
  Ecorexx::EvasWindowSoftwareX11* mw = new Ecorexx::EvasWindowSoftwareX11 (Size (0, 0));
  Evasxx::Canvas &evas = mw->getCanvas();

  /* Create Emotionxx::Object object using xine engine */
  Emotionxx::AudioObject* emotion = new Emotionxx::AudioObject (evas, argv[1], "xine" );

  emotion->setPlay( true );

  // don't show the window!

  /* Enter the application main loop */
  app->exec();

  /* Delete the application */
  delete app;

  return 0;
}

