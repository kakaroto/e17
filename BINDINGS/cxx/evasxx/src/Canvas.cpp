#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Object.h"
#include "../include/evasxx/Canvas.h"

using namespace std;
using namespace Eflxx;

namespace Evasxx {

Canvas::Canvas()
  :Eflxx::Trackable( "Canvas" ),
  mFree (true)
{
  AllocTag( this, "Canvas" );
  Dout( dc::notice, "Canvas::Canvas - creating new Evas" );
  o = evas_new();
}

Canvas::Canvas( Evas* evas ) :
  Eflxx::Trackable( "Canvas" ),
  mFree (false)
{
  AllocTag( this, "Canvas" );
  Dout( dc::notice, "Canvas::Canvas - attaching to Evas" );
  o = evas;
}

Canvas::Canvas( const Eflxx::Size &size ) :
  Eflxx::Trackable( "Canvas" ),
  mFree (true)
{
  AllocTag( this, "Canvas" );
  Dout( dc::notice, "Canvas::Canvas - creating new Evas" );
  o = evas_new();
  resize( size );
  setViewport( Rect (0, 0, size.width (), size.height ()));
}

Canvas::~Canvas()
{
  if (mFree)
  {
    Dout( dc::notice, "Canvas::~Canvas - freeing Evas" );
    evas_free (o);
  }
}

int Canvas::lookupRenderMethod( const std::string &method )
{
  return evas_render_method_lookup( method.c_str () );
}

bool Canvas::setOutputMethod( const std::string &method )
{
  int id = lookupRenderMethod( method );
  if ( id )
  {
    evas_output_method_set( o, id );
    return true;
  }
  return false;
}

void Canvas::resize( const Size &size )
{
  evas_output_size_set( o, size.width (), size.height () );
}

Size Canvas::getSize() const
{
  int width;
  int height;
  evas_output_size_get( o, &width, &height );
  return Size( width, height );
}

Rect Canvas::getGeometry() const
{
  int width;
  int height;
  evas_output_size_get( o, &width, &height );
  return Rect( 0, 0, width, height );
}

void Canvas::setViewport( const Rect &rect )
{
  evas_output_viewport_set( o, rect.x (), rect.y (), rect.width (), rect.height () );
}

Rect Canvas::getViewport() const
{
  int x;
  int y;
  int width;
  int height;
  evas_output_viewport_get( o, &x, &y, &width, &height );
  return Rect( x, y, width, height );
}

void Canvas::appendFontPath( const std::string &path )
{
  evas_font_path_append( o, path.c_str () );
}

void Canvas::clearFontPath()
{
  evas_font_path_clear( o );
}

const Eina_List* Canvas::getFontPath()
{
  return evas_font_path_list( o );
}

void Canvas::prependFontPath( const std::string &path )
{
  evas_font_path_prepend( o, path.c_str () );
}

int Canvas::getFontCache() const
{
  return evas_font_cache_get( o );
}

void Canvas::flushFontCache()
{
  evas_font_cache_flush( o );
}

void Canvas::setFontCache( unsigned int size )
{
  evas_font_cache_set( o, size );
}

int Canvas::getImageCache() const
{
  return evas_image_cache_get( o );
}

void Canvas::flushImageCache()
{
  evas_image_cache_flush( o );
}

void Canvas::reloadImageCache()
{
  evas_image_cache_reload( o );
}

void Canvas::setImageCache( unsigned int size )
{
  evas_image_cache_set( o, size );
}

Object* Canvas::focusedObject() const
{
  return Object::objectLink( evas_focus_get( o ) );
}

Object* Canvas::objectAtTop() const
{
  return Object::objectLink( evas_object_top_get( o ) );
}

Object* Canvas::objectAtBottom() const
{
  return Object::objectLink( evas_object_bottom_get( o ) );
}

Canvas *Canvas::wrap (Evas_Object *o)
{
  return new Canvas (evas_object_evas_get (o));
}

Canvas *Canvas::wrap (Evas *evas)
{
  return new Canvas (evas);
}

} // end namespace Evasxx

