#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <eflpp_sys.h>
#include <eflpp_common.h>
#include <eflpp_evasimage.h>

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
