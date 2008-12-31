#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <eflpp_sys.h>
#include <eflpp_common.h>

#include "eflpp_evas.h"
#include "eflpp_evastextblockstyle.h"
#include "eflpp_ecore.h"

/* EFL */
extern "C" {
#include <Edje.h>
#ifdef EFL_SUPPORT_ESMART
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Text_Entry.h>
#endif
#ifdef EFL_SUPPORT_EMOTION
#include <Emotion.h>
#endif
}

/* STD */
#include <assert.h>
#include <cmath>
#include <iostream>
using std::cerr;
using std::endl;

namespace efl {

char* EvasImage::_resourcePath = 0;

//===============================================================================================
// Canvas
//===============================================================================================

EvasCanvas::EvasCanvas()
    :Trackable( "EvasCanvas" )
{
    AllocTag( this, "EvasCanvas" );
    Dout( dc::notice, "EvasCanvas::EvasCanvas - creating new Evas" );
    o = evas_new();
}

EvasCanvas::EvasCanvas( Evas* evas )
    :Trackable( "EvasCanvas" )
{
    AllocTag( this, "EvasCanvas" );
    Dout( dc::notice, "EvasCanvas::EvasCanvas - attaching to Evas" );
    o = evas;
}

EvasCanvas::EvasCanvas( int width, int height )
    :Trackable( "EvasCanvas" )
{
    AllocTag( this, "EvasCanvas" );
    Dout( dc::notice, "EvasCanvas::EvasCanvas - creating new Evas" );
    o = evas_new();
    resize( width, height );
    setViewport( 0, 0, width, height );
}

EvasCanvas::~EvasCanvas()
{
    Dout( dc::notice, "EvasCanvas::~Canvas - freeing Evas" );
    evas_free( o );
}

int EvasCanvas::lookupRenderMethod( const char* method )
{
    return evas_render_method_lookup( method );
}

bool EvasCanvas::setOutputMethod( const char* method )
{
    int id = lookupRenderMethod( method );
    if ( id )
    {
        evas_output_method_set( o, id );
        return true;
    }
    return false;
}

void EvasCanvas::resize( int width, int height )
{
    evas_output_size_set( o, width, height );
}

Size EvasCanvas::size() const
{
    int width;
    int height;
    evas_output_size_get( o, &width, &height );
    return Size( width, height );
}

Rect EvasCanvas::geometry() const
{
    int width;
    int height;
    evas_output_size_get( o, &width, &height );
    return Rect( 0, 0, width, height );
}

void EvasCanvas::setViewport( int x, int y, int width, int height )
{
    evas_output_viewport_set( o, x, y, width, height );
}

Rect EvasCanvas::viewport() const
{
    int x;
    int y;
    int width;
    int height;
    evas_output_viewport_get( o, &x, &y, &width, &height );
    return Rect( x, y, width, height );
}

void EvasCanvas::appendFontPath( const char* path )
{
    evas_font_path_append( o, path );
}

void EvasCanvas::clearFontPath()
{
    evas_font_path_clear( o );
}

const Evas_List* EvasCanvas::fontPath()
{
    return evas_font_path_list( o );
}

void EvasCanvas::prependFontPath( const char* path )
{
    evas_font_path_prepend( o, path );
}

int EvasCanvas::fontCache()
{
    return evas_font_cache_get( o );
}

void EvasCanvas::flushFontCache()
{
    evas_font_cache_flush( o );
}

void EvasCanvas::setFontCache( unsigned int size )
{
    evas_font_cache_set( o, size );
}

int EvasCanvas::imageCache()
{
    return evas_image_cache_get( o );
}

void EvasCanvas::flushImageCache()
{
    evas_image_cache_flush( o );
}

void EvasCanvas::reloadImageCache()
{
    evas_image_cache_reload( o );
}

void EvasCanvas::setImageCache( unsigned int size )
{
    evas_image_cache_set( o, size );
}

EvasObject* EvasCanvas::focusedObject() const
{
    return EvasObject::objectLink( evas_focus_get( o ) );
}

EvasObject* EvasCanvas::objectAtTop() const
{
    return EvasObject::objectLink( evas_object_top_get( o ) );
}

EvasObject* EvasCanvas::objectAtBottom() const
{
    return EvasObject::objectLink( evas_object_bottom_get( o ) );
}

//===============================================================================================
// Line
//===============================================================================================

EvasLine::EvasLine( EvasCanvas* canvas, const char* name )
{
    o = evas_object_line_add( canvas->obj() );
    init( name ? name : "line" );
}

EvasLine::EvasLine( int x1, int y1, int x2, int y2, EvasCanvas* canvas, const char* name )
{
    o = evas_object_line_add( canvas->obj() );
    init( name ? name : "line" );

    evas_object_line_xy_set( o, x1, y1, x2, y2 );
}

EvasLine::~EvasLine()
{
    evas_object_del( o );
}

void EvasLine::setGeometry( int x, int y, int width, int height )
{
    evas_object_line_xy_set( o, x, y, x+width, y+height );
}

//===============================================================================================
// EvasRectangle
//===============================================================================================

EvasRectangle::EvasRectangle( EvasCanvas* canvas, const char* name )
{
    o = evas_object_rectangle_add( canvas->obj() );
    init( name ? name : "rectangle" );
}

EvasRectangle::EvasRectangle( int x, int y, int width, int height, EvasCanvas* canvas, const char* name )
{
    o = evas_object_rectangle_add( canvas->obj() );
    init( name ? name : "rectangle" );

    setGeometry( x, y, width, height );
}

EvasRectangle::EvasRectangle( const Rect& r, EvasCanvas* canvas, const char* name )
{
    o = evas_object_rectangle_add( canvas->obj() );
    init( name ? name : "rectangle" );

  setGeometry( r );
}

EvasRectangle::~EvasRectangle()
{
    evas_object_del( o );
}

//===============================================================================================
// Polygon
//===============================================================================================

EvasPolygon::EvasPolygon( EvasCanvas* canvas, const char* name )
{
    o = evas_object_polygon_add( canvas->obj() );
    init( name ? name : "polygon" );
}

EvasPolygon::~EvasPolygon()
{
    evas_object_del( o );
}

void EvasPolygon::addPoint( int x, int y )
{
    evas_object_polygon_point_add( o, x, y );
}

void EvasPolygon::clearPoints()
{
    evas_object_polygon_points_clear( o );
}


//===============================================================================================
// Text
//===============================================================================================

EvasText::EvasText( EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );

    setFont( EvasFont() );
}

EvasText::EvasText( int x, int y, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );;

    move( x, y );
    setFont( EvasFont() );
    setText( text );
}

EvasText::EvasText( const char* font, int size, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );

    setFont( font, size );
    setText( text );
}

EvasText::EvasText( const char* font, int size, int x, int y, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_text_add( canvas->obj() );
    init( name ? name : "text" );

    move( x, y );
    setFont( font, size );
    setText( text );
}

EvasText::EvasText( EvasText* ao, EvasCanvas* canvas )
{
    Dout( dc::notice, *this << " EvasText::EvasText copy constructor" );
    o = evas_object_text_add( canvas->obj() );
    init( "text" );

    setGeometry( ao->geometry() );
    setLayer( ao->layer() );
    setText( ao->text() );
    setClip( ao->clip() );
    setFont( ao->font() );
    setColor( ao->color() );
    setVisible( ao->isVisible() );
}

const Size EvasText::size() const
{
    int width = horizontalAdvance();
    int height = verticalAdvance();
    Dout( dc::notice, *this << " EvasText::size() - width = " << width << ", height = " << height );

    return Size( width, height );
}

void EvasText::setFontSource( const char* source )
{
    evas_object_text_font_source_set( o, source );
}

EvasFont EvasText::font() const
{
    const char* name;
    Evas_Font_Size size;
    evas_object_text_font_get( o, &name, &size );
    return EvasFont( name, size );
}

void EvasText::setFont( const char* font, Evas_Font_Size size )
{
    evas_object_text_font_set( o, font, size );
}

void EvasText::setFont( const EvasFont& f )
{
    setFont( f.name(), f.size() );
}

void EvasText::setText( const char* text )
{
    evas_object_text_text_set( o, text );
}

const char* EvasText::text() const
{
    return evas_object_text_text_get( o );
}

int EvasText::ascent() const
{
    return evas_object_text_ascent_get( o );
}

int EvasText::descent() const
{
    return evas_object_text_descent_get( o );
}

int EvasText::maxAscent() const
{
    return evas_object_text_max_ascent_get( o );
}

int EvasText::maxDescent() const
{
    return evas_object_text_max_descent_get( o );
}

int EvasText::horizontalAdvance() const
{
    return evas_object_text_horiz_advance_get( o );
}

int EvasText::verticalAdvance() const
{
    return evas_object_text_vert_advance_get( o );
}

int EvasText::inset() const
{
    return evas_object_text_inset_get( o );
}


EvasText::~EvasText()
{
    evas_object_del( o );
}

//===============================================================================================
// Textblock
//===============================================================================================

EvasTextblock::EvasTextblock( EvasCanvas* canvas, const char* name )
{
    o = evas_object_textblock_add( canvas->obj() );
    init( name ? name : "textblock" );
}

EvasTextblock::EvasTextblock( int x, int y, const char* text, EvasCanvas* canvas, const char* name )
{
    o = evas_object_textblock_add( canvas->obj() );
    init( name ? name : "textblock" );

    move( x, y );
    setText( text );
}

void EvasTextblock::setStyle( const EvasTextblockStyle* style )
{
    evas_object_textblock_style_set( o, style->o );
}

void EvasTextblock::setText( const char* text )
{
    evas_object_textblock_text_markup_set( o, text );
}

void EvasTextblock::clear()
{
    evas_object_textblock_clear( o );
}

EvasTextblock::~EvasTextblock()
{
    evas_object_del( o );
}

//===============================================================================================
// Image
//===============================================================================================

EvasImage::EvasImage( EvasCanvas* canvas, const char* name )
{
    o = evas_object_image_add( canvas->obj() );
    init( name ? name : "image" );

    _size = size ();
    resize( _size );
}

EvasImage::EvasImage( const char* filename, EvasCanvas* canvas, const char* name )
{
    o = evas_object_image_add( canvas->obj() );
    init( name ? name : filename );

    setFile( filename );
    _size = size();
    resize( _size );
}

EvasImage::EvasImage( int x, int y, const char* filename, EvasCanvas* canvas, const char* name )
{
    o = evas_object_image_add( canvas->obj() );
    init( name ? name : filename );

    setFile( filename );
    move( x, y );
    _size = size();
    resize( _size );
}

// attach temporary disabled
#if 0
EvasImage::EvasImage( Evas_Object* object, EvasCanvas* canvas, const char* name )
{
    o = evas_object_image_add( canvas->obj() );
    init( name ? name : type );

    o = object;
    _size = size();
}
#endif

EvasImage::~EvasImage()
{
    evas_object_del( o );
}

bool EvasImage::setFile( const char* path, const char* key )
{
    const char* p = path;
    const char* k = key;

    if ( ( EvasImage::_resourcePath ) && ( path[0] != '/' ) )
    {
        p = EvasImage::_resourcePath;
        k = path;
    }
    evas_object_image_file_set( o, p, k );
    int errorcode = evas_object_image_load_error_get(o);
    Dout( dc::notice, *this << " EvasImage::file_set" << " path=" << p << ( k ? k : "<none>" )
            << "(" << EVAS_LOAD_ERROR[errorcode] << ")" );
    if ( errorcode ) cerr << "ERROR: EvasImage::setFile( '" << p << "|" << ( k ? k : "<none>" ) << ") = " << EVAS_LOAD_ERROR[errorcode] << endl;
    return ( errorcode == 0 );
}

void EvasImage::setFill( int x, int y, int width, int height )
{
    Dout( dc::notice, *this << " EvasImage::fill_set" << " x=" << x << " y=" << y << " width=" << width << " height=" << height );
    evas_object_image_fill_set( o, x, y, width, height );
}

void EvasImage::setFill( int x, int y, const Size& size )
{
    setFill( x, y, size.width(), size.height() );
}

void EvasImage::resize( int width, int height, bool ff )
{
    EvasObject::resize( width, height );
    if ( ff ) setFill( 0, 0, width, height );
}

void EvasImage::resize( const Size& size, bool ff )
{
    Dout( dc::notice, *this << " EvasImage::resize current size is " << size.width() << ", " << size.height() );
    resize( size.width(), size.height(), ff );
}

const Size EvasImage::size() const
{
    int w, h;
    evas_object_image_size_get( o, &w, &h );
    return Size( w, h );
}

const Size& EvasImage::trueSize()
{
    return _size;
}

void EvasImage::setImageSize (int w, int h)
{
    evas_object_image_size_set (o, w, h);
}

void EvasImage::getImageSize (int &w, int &h)
{
    evas_object_image_size_get (o, &w, &h);
}

void EvasImage::setBorder( int left, int right, int top, int bottom )
{
    evas_object_image_border_set( o, left, right, top, bottom );
}


void EvasImage::setData (void *data)
{
    evas_object_image_data_set (o, data);
}

void *EvasImage::getData (bool for_writing)
{
    return evas_object_image_data_get (o, for_writing);
}

void EvasImage::setDataCopy (void *data)
{
    evas_object_image_data_copy_set (o, data);
}

void EvasImage::addDataUpdate (int x, int y, int w, int h)
{
    evas_object_image_data_update_add (o, x, y, w, h);
}

//===============================================================================================
// Gradient
//===============================================================================================

EvasGradient::EvasGradient( EvasCanvas* canvas, const char* name )
{
    o = evas_object_gradient_add( canvas->obj() );
    init( name ? name : "gradient" );
}

EvasGradient::EvasGradient( int x, int y, int width, int height, EvasCanvas* canvas, const char* name )
{
    o = evas_object_gradient_add( canvas->obj() );
    init( name ? name : "gradient" );

    setGeometry( x, y, width, height );
}

EvasGradient::~EvasGradient()
{
    evas_object_del( o );
}

void EvasGradient::addColorStop( int r, int g, int b, int a, int distance )
{
    evas_object_gradient_color_stop_add( o, r, g, b, a, distance );
}
#warning NEED_TO_UPDATE_GRADIENT
void EvasGradient::clear()
{
    evas_object_gradient_clear( o );
}

void EvasGradient::setAngle( Evas_Angle angle )
{
    evas_object_gradient_angle_set( o, angle );
}

Evas_Angle EvasGradient::angle()
{
    return evas_object_gradient_angle_get( o );
}

} // end namespace efl
