#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* STD */
#include <iostream>

/* EFLxx */
#include <evasxx/Canvas.h>
#include "../include/ecorexx/EvasWindowFB.h"

namespace Ecorexx {

// TODO: Possible without #ifdef stuff?
EvasWindowFB::EvasWindowFB( const Eflxx::Size &size, const char* display, int rotation ) :
  EvasWindow()
{
  Dout( dc::notice, "EvasWindow::EvasWindowFB" );
  Application::getInstance()->setMainWindow( this );

  /*if ( ::getenv( "EFL_DISPLAY" ) ) display = ::getenv( "EFL_DISPLAY" );
  if ( ::getenv( "EFL_WIDTH" ) ) size.width (atoi( ::getenv( "EFL_WIDTH" ) ));
  if ( ::getenv( "EFL_HEIGHT" ) ) size.height (atoi( ::getenv( "EFL_HEIGHT" ) ));
  if ( ::getenv( "EFL_ROTATION" ) ) rotation = atoi( ::getenv( "EFL_ROTATION" ) );*/

  if ( display /*&& ::strstr( display, "/dev/fb" )*/ )
  {
#ifdef ENABLE_EFLPP_FB
    int fb_dev_fd = ::open( display, O_RDONLY );
    if ( fb_dev_fd < 0 )
    {
      fprintf(stderr,"Can't open display '%s': %s\n", display, strerror(errno));
      exit( 1 );
    }

    struct fb_var_screeninfo fb_vinfo;
    struct fb_fix_screeninfo fb_finfo;

    // read VScreen info from fb
    if ( ioctl( fb_dev_fd, FBIOGET_VSCREENINFO, &fb_vinfo ) )
    {
      fprintf(stderr,"Can't get VSCREENINFO: %s\n", strerror(errno));
      exit(1);
    }

    // readFScreen info from fb
    if ( ioctl( fb_dev_fd, FBIOGET_FSCREENINFO, &fb_finfo ) )
    {
      fprintf(stderr,"Can't get FSCREENINFO: %s\n", strerror(errno));
      exit(1);
    }

    Dout( dc::notice, "- using display '" << display << "' [" << fb_finfo.id << "] - " << fb_vinfo.xres << "x" << fb_vinfo.yres << "@" << rotation );
    width = ( rotation % 180 ) ? fb_vinfo.yres : fb_vinfo.xres;
    height = ( rotation % 180 ) ? fb_vinfo.xres : fb_vinfo.yres;
    Dout( dc::notice, "- using size (after rotating) " << size.width () << "x" << size.height () );
#ifdef ECORE_FB_NO_ROTATION_BUG
    //FIXME EFL BUG: initial rotation is not taken into account for evas calculation
    _ee = ecore_evas_fb_new( const_cast<char*>( display ), rotation, 50, 50 );
#else
    _ee = ecore_evas_fb_new( const_cast<char*>( display ), 0, 50, 50 ); // start with rotation 0 to workaround bug
#endif
    ecore_evas_fullscreen_set( _ee, 1 ); // fullscreen is default to get auto resize on changing rotation
    ecore_evas_rotation_set( _ee, rotation ); // force resize
#else
    printf("FB engine not enabled\n");
#endif
  }

  ecore_evas_title_set( _ee, Application::getInstance()->getName().c_str() );
  ecore_evas_borderless_set( _ee, 0 );

  _canvas = Evasxx::Canvas::wrap (ecore_evas_get( _ee ));

  /* Set up magic object back link */
  ecore_evas_data_set( _ee, "obj_c++", this );

  /* Set up default callbacks */
  setEventEnabled( Resize, true );
  setEventEnabled( DeleteRequest, true );
}

EvasWindowFB::~EvasWindowFB()
{
}

} // end namespace Ecorexx
