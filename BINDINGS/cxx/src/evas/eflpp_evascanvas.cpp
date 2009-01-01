#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_evascanvas.h"

using namespace std;

namespace efl {
  
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

} // end namespace efl
