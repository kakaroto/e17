#include <eflxx/eflpp_sys.h>
#include <evasxx/Evasxx.h>
#include <ecorexx/EcoreApplication.h>
#include <edjexx/Edjexx.h>
#include <etkxx/Etkxx.h>
#include "../../common/searchFile.h"

#define WIDTH 240
#define HEIGHT 320

#include <iostream>

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

using namespace efl;
using namespace std;

int main( int argc, const char **argv )
{
  /* Create the application object */
  EcoreApplication app ( argc, argv, "Simple Ecore Test" );

  const Size size (WIDTH, HEIGHT);

  /* Create the main window, a window with an embedded canvas */
  EcoreEvasWindowSoftwareX11 mw ( size );

  EvasCanvas &evas = mw.getCanvas();

  evas.appendFontPath( searchDataDir () + "/fonts" );

  /* Add some objects to the canvas */
#if 1
  EvasEdje edje ( evas, searchEdjeFile ("angstrom-bootmanager.edj"), "background" );
  edje.resize( size );
  Size s = edje.getMinimalSize();
  edje.setLayer( 1 );
  edje.show();
#else
  EvasRectangle b ( evas, Rect (50, 50, 100, 100) );
  b.show();
#endif

  EtkEmbed embed ( evas );
  EvasEtk evasetk ( &embed, "evasetk" );

  embed.setFocus( true );
  evasetk.move( Point (5, 5) );
  evasetk.resize( Size (20, 20) );
  evasetk.setLayer( 2 );
  evasetk.show();
  embed.show();

  EtkButton button ( "Hello World!" );
  embed.add( &button );
  button.show();
  embed.show();

  //ewl_callback_append(EWL_WIDGET( embed->obj() ), EWL_CALLBACK_CONFIGURE, move_embed_contents_cb, vbox->obj());

  mw.show();

  /* Enter the application main loop */
  app.exec();

  return 0;
}

