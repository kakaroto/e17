#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_ecore_window.h"
#include <eflpp_evascanvas.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>

//===============================================================================================
// EcoreEvasWindow
//===============================================================================================

namespace efl {

#define ECORE_EVAS_CALLBACK( name, func )  \
static void name( Ecore_Evas* ee )         \
{                                          \
  eApp->mainWindow()->func();              \
}

ECORE_EVAS_CALLBACK( callback_resize_dispatcher, resizeEvent )
ECORE_EVAS_CALLBACK( callback_move_dispatcher, moveEvent )
ECORE_EVAS_CALLBACK( callback_show_dispatcher, showEvent )
ECORE_EVAS_CALLBACK( callback_hide_dispatcher, hideEvent )
ECORE_EVAS_CALLBACK( callback_delete_request_dispatcher, deleteRequestEvent )
ECORE_EVAS_CALLBACK( callback_destroy_dispatcher, destroyEvent )
ECORE_EVAS_CALLBACK( callback_focus_in_dispatcher, focusInEvent )
ECORE_EVAS_CALLBACK( callback_focus_out_dispatcher, focusOutEvent )
ECORE_EVAS_CALLBACK( callback_mouse_in_dispatcher, mouseInEvent )
ECORE_EVAS_CALLBACK( callback_mouse_out_dispatcher, mouseOutEvent )
ECORE_EVAS_CALLBACK( callback_pre_render_dispatcher, preRenderEvent )
ECORE_EVAS_CALLBACK( callback_post_render_dispatcher, postRenderEvent )
  
EcoreEvasWindow::EcoreEvasWindow()
                :Trackable( "EcoreEvasWindow" )
{
    ecore_evas_init ();
}


EcoreEvasWindow::~EcoreEvasWindow()
{
    Dout( dc::notice, "EcoreEvasWindow::~EcoreEvasWindow" );
    delete _canvas;
}

void EcoreEvasWindow::setEventEnabled( Event e, bool on )
{
    switch( e )
    {
        case Resize: ecore_evas_callback_resize_set( _ee, on? &callback_resize_dispatcher:0 ); break;
        case Move: ecore_evas_callback_move_set( _ee, on? &callback_move_dispatcher:0 ); break;
        case Show: ecore_evas_callback_show_set( _ee, on? &callback_show_dispatcher:0 ); break;
        case Hide: ecore_evas_callback_hide_set( _ee, on? &callback_hide_dispatcher:0 ); break;
        case DeleteRequest: ecore_evas_callback_delete_request_set( _ee, on? &callback_delete_request_dispatcher:0 ); break;
        case Destroy: ecore_evas_callback_destroy_set( _ee, on? &callback_destroy_dispatcher:0 ); break;
        case FocusIn: ecore_evas_callback_focus_in_set( _ee, on? &callback_focus_in_dispatcher:0 ); break;
        case FocusOut: ecore_evas_callback_focus_out_set( _ee, on? &callback_focus_out_dispatcher:0 ); break;
        case MouseIn: ecore_evas_callback_mouse_in_set( _ee, on? &callback_mouse_in_dispatcher:0 ); break;
        case MouseOut: ecore_evas_callback_mouse_out_set( _ee, on? &callback_mouse_out_dispatcher:0 ); break;
        case PreRender: ecore_evas_callback_pre_render_set( _ee, on? &callback_pre_render_dispatcher:0 ); break;
        case PostRender: ecore_evas_callback_post_render_set( _ee, on? &callback_post_render_dispatcher:0 ); break;
        default: assert( 0 );
    }
}

void EcoreEvasWindow::resizeEvent()
{
    int x, y, w, h;
    ecore_evas_geometry_get( _ee, &x, &y, &w, &h );
    Dout( dc::notice, "EcoreEvasWindow::resizeEvent( " << x << "," << y << "*" << w << "," << h << " )" );
    Dout( dc::notice, " - evas viewport size = " << _canvas->viewport() );
    Dout( dc::notice, " - evas output   size = " << _canvas->size() );
    //FIXME: Resize manually if not fullscreen
}

void EcoreEvasWindow::moveEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::moveEvent()" );
}

void EcoreEvasWindow::showEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::showEvent()" );
}

void EcoreEvasWindow::hideEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::hideEvent()" );
}


void EcoreEvasWindow::deleteRequestEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::deleteRequestEvent()" );
    if ( canClose() ) eApp->quit();
}

void EcoreEvasWindow::destroyEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::destroyEvent()" );
}


void EcoreEvasWindow::focusInEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::focusInEvent()" );
}

void EcoreEvasWindow::focusOutEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::focusOutEvent()" );
}

void EcoreEvasWindow::mouseInEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::mouseInEvent()" );
}

void EcoreEvasWindow::mouseOutEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::mouseOutEvent()" );
}

void EcoreEvasWindow::preRenderEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::preRenderEvent()" );
}

void EcoreEvasWindow::postRenderEvent()
{
    Dout( dc::notice, "EcoreEvasWindow::postRenderEvent()" );
}

bool EcoreEvasWindow::isEngineTypeSupported (EngineType et)
{
  return ecore_evas_engine_type_supported_get (static_cast <Ecore_Evas_Engine_Type> (et));
}

bool EcoreEvasWindow::canClose() const
{
    return true;
}

void EcoreEvasWindow::move( const Point& point )
{
    ecore_evas_move( _ee, point.x(), point.y() );
}

void EcoreEvasWindow::resize( const Size& size )
{
    ecore_evas_resize( _ee, size.width(), size.height() );
}

void EcoreEvasWindow::setGeometry( const Rect& rect )
{
    ecore_evas_move_resize( _ee, rect.x(), rect.y(), rect.width(), rect.height() );
}

Rect EcoreEvasWindow::geometry() const
{
    int x, y, width, height;
    ecore_evas_geometry_get( _ee, &x, &y, &width, &height );
    return Rect( x, y, width, height );
}

void EcoreEvasWindow::setRotation( int rot )
{
    ecore_evas_rotation_set( _ee, rot );
}

int EcoreEvasWindow::rotation() const
{
    return ecore_evas_rotation_get( _ee );
}

void EcoreEvasWindow::setShaped( bool b )
{
    ecore_evas_shaped_set( _ee, b );
}

bool EcoreEvasWindow::isShaped() const
{
    return ecore_evas_shaped_get( _ee );
}

void EcoreEvasWindow::setAlpha( bool alpha )
{
  ecore_evas_alpha_set( _ee, alpha );
}

bool EcoreEvasWindow::hasAlpha () const
{
  return ecore_evas_alpha_get( _ee );
}

void EcoreEvasWindow::show()
{
    ecore_evas_show( _ee );
}
void EcoreEvasWindow::hide()
{
    ecore_evas_hide( _ee );
}
void EcoreEvasWindow::raise()
{
    ecore_evas_raise( _ee );
}

void EcoreEvasWindow::lower()
{
    ecore_evas_lower( _ee );
}

void EcoreEvasWindow::setTitle( const char* title )
{
    ecore_evas_title_set( _ee, title );
}

const char* EcoreEvasWindow::title() const
{
    return ecore_evas_title_get( _ee );
}

void EcoreEvasWindow::setMinimumSize( const Size& size )
{
    ecore_evas_size_min_set( _ee, size.width(), size.height() );
}

Size EcoreEvasWindow::minimumSize() const
{
    int x;
    int y;
    ecore_evas_size_min_get( _ee, &x, &y );
    return Size( x, y );
}

void EcoreEvasWindow::setMaximumSize( const Size& size )
{
    ecore_evas_size_max_set( _ee, size.width(), size.height() );
}

Size EcoreEvasWindow::maximumSize() const
{
    int x;
    int y;
    ecore_evas_size_max_get( _ee, &x, &y );
    return Size( x, y );
}

void EcoreEvasWindow::setBaseSize( const Size& size )
{
    ecore_evas_size_base_set( _ee, size.width(), size.height() );
}

Size EcoreEvasWindow::baseSize() const
{
    int x;
    int y;
    ecore_evas_size_base_get( _ee, &x, &y );
    return Size( x, y );
}

void EcoreEvasWindow::setStepSize( const Size& size )
{
    ecore_evas_size_step_set( _ee, size.width(), size.height() );
}

Size EcoreEvasWindow::stepSize() const
{
    int x;
    int y;
    ecore_evas_size_step_get( _ee, &x, &y );
    return Size( x, y );
}

void EcoreEvasWindow::setCursor( const char* file, int layer, int hot_x, int hot_y )
{
    ecore_evas_cursor_set( _ee, file, layer, hot_x, hot_y );
}

void EcoreEvasWindow::setLayer( int layer )
{
    ecore_evas_layer_set( _ee, layer );
}

int EcoreEvasWindow::layer() const
{
    return ecore_evas_layer_get( _ee );
}

void EcoreEvasWindow::setFocus( int on )
{
    ecore_evas_focus_set( _ee, on );
}

int EcoreEvasWindow::hasFocus() const
{
    return ecore_evas_focus_get( _ee );
}

void EcoreEvasWindow::setIconified( int on )
{
    ecore_evas_iconified_set( _ee, on );
}

int EcoreEvasWindow::isIconified() const
{
    return ecore_evas_iconified_get( _ee );
}

void EcoreEvasWindow::setBorderless( int on )
{
    ecore_evas_borderless_set( _ee, on );
}

int EcoreEvasWindow::isBorderless() const
{
    return ecore_evas_borderless_get( _ee );
}

void EcoreEvasWindow::setOverride( int on )
{
    ecore_evas_override_set( _ee, on );
}

int EcoreEvasWindow::isOverride() const
{
    return ecore_evas_override_get( _ee );
}

void EcoreEvasWindow::setMaximized( int on )
{
    ecore_evas_maximized_set( _ee, on );
}
int EcoreEvasWindow::isMaximized() const
{
    return ecore_evas_maximized_get( _ee );
}

void EcoreEvasWindow::setFullscreen( int on )
{
    ecore_evas_fullscreen_set( _ee, on );
}

int EcoreEvasWindow::isFullscreen() const
{
    return ecore_evas_fullscreen_get( _ee );
}

void EcoreEvasWindow::setAvoidDamage( Ecore_Evas_Avoid_Damage_Type value )
{
    ecore_evas_avoid_damage_set( _ee, value );
}

Ecore_Evas_Avoid_Damage_Type EcoreEvasWindow::AvoidDamage() const
{
    return ecore_evas_avoid_damage_get( _ee );
}

void EcoreEvasWindow::setWithdrawn( int on )
{
    ecore_evas_withdrawn_set( _ee, on );
}

int EcoreEvasWindow::isWithdrawn() const
{
    return ecore_evas_withdrawn_get( _ee );
}

void EcoreEvasWindow::setSticky( int on )
{
    ecore_evas_sticky_set( _ee, on );
}

int EcoreEvasWindow::isSticky() const
{
    return ecore_evas_sticky_get( _ee );
}

/* PRIVATE */
EcoreEvasWindow* EcoreEvasWindow::objectLink( Ecore_Evas* ee )
{
    return static_cast<EcoreEvasWindow*>( ecore_evas_data_get( ee, "obj_c++" ) );
}

EcoreEvasWindowSoftwareX11::EcoreEvasWindowSoftwareX11( int width, int height, const char* display )
                :EcoreEvasWindow()
{    
    Dout( dc::notice, "EcoreEvasWindow::EcoreEvasWindowSoftwareX11" );
    EcoreApplication::application()->setMainWindow( this );

    if ( ::getenv( "EFL_DISPLAY" ) ) display = ::getenv( "EFL_DISPLAY" );
    if ( ::getenv( "EFL_WIDTH" ) ) width = atoi( ::getenv( "EFL_WIDTH" ) );
    if ( ::getenv( "EFL_HEIGHT" ) ) height = atoi( ::getenv( "EFL_HEIGHT" ) );
  
    Dout( dc::notice, "- detected display string '" << ( display ? display:"<null>" ) << "' - starting X11 engine" );
    //FIXME: Should we care about positioning? 0, 0 for now
    _ee = ecore_evas_software_x11_new( const_cast<char*>( display ), 0, 0, 0, width, height );
    
    ecore_evas_title_set( _ee, eApp->name().c_str() );
    ecore_evas_borderless_set( _ee, 0 );
    ecore_evas_show( _ee );
    _canvas = new EvasCanvas( ecore_evas_get( _ee ) );

    /* Set up magic object back link */
    ecore_evas_data_set( _ee, "obj_c++", this );

    /* Set up default callbacks */
    setEventEnabled( Resize, true );
    setEventEnabled( DeleteRequest, true );
}

EcoreEvasWindowSoftwareX11::~EcoreEvasWindowSoftwareX11()
{
}

EcoreXWindow *EcoreEvasWindowSoftwareX11::getXWindow()
{
    Ecore_X_Window exw = ecore_evas_software_x11_window_get( _ee );
    EcoreXWindow *exwin = new EcoreXWindow( exw );
  
    return exwin;
}

EcoreEvasWindowGLX11::EcoreEvasWindowGLX11( int width, int height, const char* display )
                :EcoreEvasWindow()
{    
    Dout( dc::notice, "EcoreEvasWindow::EcoreEvasWindowGL" );
    EcoreApplication::application()->setMainWindow( this );

    if ( ::getenv( "EFL_DISPLAY" ) ) display = ::getenv( "EFL_DISPLAY" );
    if ( ::getenv( "EFL_WIDTH" ) ) width = atoi( ::getenv( "EFL_WIDTH" ) );
    if ( ::getenv( "EFL_HEIGHT" ) ) height = atoi( ::getenv( "EFL_HEIGHT" ) );
  
    Dout( dc::notice, "- detected display string '" << ( display ? display:"<null>" ) << "' - starting X11 engine" );
    //FIXME: Should we care about positioning? 0, 0 for now
    _ee = ecore_evas_gl_x11_new( const_cast<char*>( display ), 0, 0, 0, width, height );
    
    ecore_evas_title_set( _ee, eApp->name().c_str() );
    ecore_evas_borderless_set( _ee, 0 );
    ecore_evas_show( _ee );
    _canvas = new EvasCanvas( ecore_evas_get( _ee ) );

    /* Set up magic object back link */
    ecore_evas_data_set( _ee, "obj_c++", this );

    /* Set up default callbacks */
    setEventEnabled( Resize, true );
    setEventEnabled( DeleteRequest, true );
}

EcoreEvasWindowGLX11::~EcoreEvasWindowGLX11()
{
  
}

EcoreXWindow *EcoreEvasWindowGLX11::getXWindow()
{
    Ecore_X_Window exw = ecore_evas_gl_x11_window_get( _ee );
    EcoreXWindow *exwin = new EcoreXWindow( exw );
  
    return exwin;
}

EcoreEvasWindowXRenderX11::EcoreEvasWindowXRenderX11( int width, int height, const char* display )
                :EcoreEvasWindow()
{    
    Dout( dc::notice, "EcoreEvasWindow::EcoreEvasWindowGL" );
    EcoreApplication::application()->setMainWindow( this );

    if ( ::getenv( "EFL_DISPLAY" ) ) display = ::getenv( "EFL_DISPLAY" );
    if ( ::getenv( "EFL_WIDTH" ) ) width = atoi( ::getenv( "EFL_WIDTH" ) );
    if ( ::getenv( "EFL_HEIGHT" ) ) height = atoi( ::getenv( "EFL_HEIGHT" ) );
  
    Dout( dc::notice, "- detected display string '" << ( display ? display:"<null>" ) << "' - starting X11 engine" );
    //FIXME: Should we care about positioning? 0, 0 for now
    _ee = ecore_evas_xrender_x11_new( const_cast<char*>( display ), 0, 0, 0, width, height );
    
    ecore_evas_title_set( _ee, eApp->name().c_str() );
    ecore_evas_borderless_set( _ee, 0 );
    ecore_evas_show( _ee );
    _canvas = new EvasCanvas( ecore_evas_get( _ee ) );

    /* Set up magic object back link */
    ecore_evas_data_set( _ee, "obj_c++", this );

    /* Set up default callbacks */
    setEventEnabled( Resize, true );
    setEventEnabled( DeleteRequest, true );
}

EcoreEvasWindowXRenderX11::~EcoreEvasWindowXRenderX11()
{
  
}

EcoreXWindow *EcoreEvasWindowXRenderX11::getXWindow()
{
    Ecore_X_Window exw = ecore_evas_xrender_x11_window_get( _ee );
    EcoreXWindow *exwin = new EcoreXWindow( exw );
  
    return exwin;
}

// TODO: Possible without #ifdef stuff?
EcoreEvasWindowFB::EcoreEvasWindowFB( int width, int height, const char* display, int rotation )
                :EcoreEvasWindow()
{
    Dout( dc::notice, "EcoreEvasWindow::EcoreEvasWindowFB" );
    EcoreApplication::application()->setMainWindow( this );

    if ( ::getenv( "EFL_DISPLAY" ) ) display = ::getenv( "EFL_DISPLAY" );
    if ( ::getenv( "EFL_WIDTH" ) ) width = atoi( ::getenv( "EFL_WIDTH" ) );
    if ( ::getenv( "EFL_HEIGHT" ) ) height = atoi( ::getenv( "EFL_HEIGHT" ) );
    if ( ::getenv( "EFL_ROTATION" ) ) rotation = atoi( ::getenv( "EFL_ROTATION" ) );

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
        Dout( dc::notice, "- using size (after rotating) " << width << "x" << height );
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
    
    ecore_evas_title_set( _ee, eApp->name().c_str() );
    ecore_evas_borderless_set( _ee, 0 );
    ecore_evas_show( _ee );
    _canvas = new EvasCanvas( ecore_evas_get( _ee ) );

    /* Set up magic object back link */
    ecore_evas_data_set( _ee, "obj_c++", this );

    /* Set up default callbacks */
    setEventEnabled( Resize, true );
    setEventEnabled( DeleteRequest, true );
}

EcoreEvasWindowFB::~EcoreEvasWindowFB()
{
  
}

} // end namespace efl
