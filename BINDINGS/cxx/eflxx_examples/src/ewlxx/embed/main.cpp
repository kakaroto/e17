#include <eflxx/eflpp_sys.h>
#include <evasxx/Evasxx.h>
#include <ecorexx/EcoreApplication.h>
#include <edjexx/Edjexx.h>
#include <ewlxx/Ewlxx.h>
#include "../../common/searchFile.h"

/*
#include <Ewlxx.h>
#include <Ecorexx.h>
#include <Eflxx_Common.h>*/

#include <iostream>

#define WIDTH 240
#define HEIGHT 320

using namespace std;
using namespace efl;

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

static void move_embed_contents_cb(Ewl_Widget *w, void *ev_data __UNUSED__,void *user_data)
{
  cout << "::move_embed_contents_cb " << endl;
  ewl_object_geometry_request(EWL_OBJECT(user_data), CURRENT_X(w), CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}

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

  EdjeObject edje ( evas, searchEdjeFile ("angstrom-bootmanager.edj"), "background" );
  edje.resize( size );
  Size s = edje.getMinimalSize();
  edje.setLayer( 1 );
  edje.show();

  EwlEmbed embed;
  EvasEwl evasewl ( evas, &embed, "evasewl" ); // TODO:embed as reference parameter in lib
  evasewl.setGeometry( Rect (25, 25, 100, 100) );

  embed.setFocus( true );
  evasewl.setLayer( 10 );
  evasewl.show();
  embed.show();

  EwlVBox vbox;
  embed.appendChild( &vbox ); // TODO: vbox as reference parameter in lib
  vbox.show();
  EwlButton button ( "Hello World!" );
  vbox.appendChild( &button );
  button.show();

  //ewl_callback_append(EWL_WIDGET( embed->obj() ), EWL_CALLBACK_CONFIGURE, move_embed_contents_cb, vbox->obj());

  /* Enter the application main loop */
  app.exec();

  return 0;
}

