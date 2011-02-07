#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <iostream>

/* EFLxx */
#include <evasxx/Canvas.h>
#include "../include/ecorexx/EvasWindowSoftwareX11.h"
#include "../include/ecorexx/XWindow.h"

namespace Ecorexx {
  
EvasWindowSoftwareX11::EvasWindowSoftwareX11( const Eflxx::Size &size, const char* display ) : 
  EvasWindow(),
  mExwin (NULL)
{
  Dout( dc::notice, "EvasWindow::EvasWindowSoftwareX11" );
  Application::getInstance()->setMainWindow( this );

  /*if ( ::getenv( "EFL_DISPLAY" ) ) display = ::getenv( "EFL_DISPLAY" );
  if ( ::getenv( "EFL_WIDTH" ) ) size.width (atoi( ::getenv( "EFL_WIDTH" ) ));
  if ( ::getenv( "EFL_HEIGHT" ) ) size.height (atoi( ::getenv( "EFL_HEIGHT" ) ));*/

  Dout( dc::notice, "- detected display string '" << ( display ? display:"<null>" ) << "' - starting X11 engine" );
  //FIXME: Should we care about positioning? 0, 0 for now
  _ee = ecore_evas_software_x11_new( const_cast<char*>( display ), 0, 0, 0, size.width (), size.height () );

  ecore_evas_title_set( _ee, Application::getInstance()->getName().c_str() );
  ecore_evas_borderless_set( _ee, 0 );

  _canvas = Evasxx::Canvas::wrap ( ecore_evas_get( _ee ) );

  /* Set up magic object back link */
  ecore_evas_data_set( _ee, "obj_c++", this );

  /* Set up default callbacks */
  setEventEnabled( Resize, true );
  setEventEnabled( DeleteRequest, true );
}

EvasWindowSoftwareX11::~EvasWindowSoftwareX11()
{
}

XWindow *EvasWindowSoftwareX11::getXWindow()
{
  // lazy construction...
  if (!mExwin)
  {
    Ecore_X_Window exw = ecore_evas_software_x11_window_get( _ee );
    mExwin = new XWindow( exw );
  }

  return mExwin;
}

} // end namespace Ecorexx
