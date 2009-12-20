#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Cairo.h"

#include <iostream>
#include <assert.h>

using namespace std;

namespace Esmartxx {
  
Cairo::Cairo( Evasxx::Canvas &canvas, const Eflxx::Size &size, bool alpha)
{
  o = esmart_image_cairo_new( canvas.obj(), size.width (), size.height (), alpha );
  init();
}

Cairo::Cairo( Evasxx::Canvas &canvas, const Eflxx::Point &pos, const Eflxx::Size &size, bool alpha)
{
  o = esmart_image_cairo_new( canvas.obj(), size.width (), size.height (), alpha );
  init();

  move( pos );
}

Cairo::Cairo( Evasxx::Canvas &canvas, cairo_surface_t *cairo_surface)
{
  o = esmart_image_cairo_new_from_surface( canvas.obj(), cairo_surface );
  init();
}

Cairo::~Cairo()
{
  evas_object_del( o );
}

cairo_surface_t* Cairo::getSurface ()
{
  return esmart_image_cairo_surface_get (o);
}

bool Cairo::setSurface (cairo_surface_t *cairo_surface)
{
  return esmart_image_cairo_surface_set (o, cairo_surface);
}

void Cairo::setAutoFill (bool enable)
{
  esmart_image_cairo_fill_auto_set (o, enable);
}

void Cairo::setDirty ()
{
  evas_object_image_pixels_dirty_set (o, 1);
}

} // end namespace Esmartxx
