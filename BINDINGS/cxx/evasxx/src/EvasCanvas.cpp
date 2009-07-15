#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasCanvas.h"
#include <eflxx/eflpp_common.h>

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

EvasCanvas::EvasCanvas( const Size &size )
    :Trackable( "EvasCanvas" )
{
  AllocTag( this, "EvasCanvas" );
  Dout( dc::notice, "EvasCanvas::EvasCanvas - creating new Evas" );
  o = evas_new();
  resize( size );
  setViewport( Rect (0, 0, size.width (), size.height ()));
}

EvasCanvas::~EvasCanvas()
{
  Dout( dc::notice, "EvasCanvas::~Canvas - freeing Evas" );
  evas_free( o );
}

int EvasCanvas::lookupRenderMethod( const std::string &method )
{
  return evas_render_method_lookup( method.c_str () );
}

bool EvasCanvas::setOutputMethod( const std::string &method )
{
  int id = lookupRenderMethod( method );
  if ( id )
  {
    evas_output_method_set( o, id );
    return true;
  }
  return false;
}

void EvasCanvas::resize( const Size &size )
{
  evas_output_size_set( o, size.width (), size.height () );
}

Size EvasCanvas::getSize() const
{
  int width;
  int height;
  evas_output_size_get( o, &width, &height );
  return Size( width, height );
}

Rect EvasCanvas::getGeometry() const
{
  int width;
  int height;
  evas_output_size_get( o, &width, &height );
  return Rect( 0, 0, width, height );
}

void EvasCanvas::setViewport( const Rect &rect )
{
  evas_output_viewport_set( o, rect.x (), rect.y (), rect.width (), rect.height () );
}

Rect EvasCanvas::getViewport() const
{
  int x;
  int y;
  int width;
  int height;
  evas_output_viewport_get( o, &x, &y, &width, &height );
  return Rect( x, y, width, height );
}

void EvasCanvas::appendFontPath( const std::string &path )
{
  evas_font_path_append( o, path.c_str () );
}

void EvasCanvas::clearFontPath()
{
  evas_font_path_clear( o );
}

const Eina_List* EvasCanvas::getFontPath()
{
  return evas_font_path_list( o );
}

void EvasCanvas::prependFontPath( const std::string &path )
{
  evas_font_path_prepend( o, path.c_str () );
}

int EvasCanvas::getFontCache() const
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

int EvasCanvas::getImageCache() const
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
