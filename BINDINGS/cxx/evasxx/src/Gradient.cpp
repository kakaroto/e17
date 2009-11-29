#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Canvas.h"
#include "../include/evasxx/Gradient.h"

using namespace Eflxx;

namespace Evasxx {

Gradient::Gradient( Canvas &canvas )
{
  o = evas_object_gradient_add( canvas.obj() );
  init();
}

Gradient::Gradient( Canvas &canvas, const Rect &rect )
{
  o = evas_object_gradient_add( canvas.obj() );
  init();

  setGeometry( rect );
}

Gradient::~Gradient()
{
  evas_object_del( o );
}

void Gradient::addColorStop( const Color &c, int delta )
{
  evas_object_gradient_color_stop_add( o, c.red (), c.green (), c.blue (), c.alpha (), delta );
}

void Gradient::addAlphaStop (int alpha, int delta)
{
  evas_object_gradient_alpha_stop_add (o, alpha, delta);
}

void Gradient::setColorData (void *color_data, int len, bool has_alpha)
{
  evas_object_gradient_color_data_set (o, color_data, len, has_alpha);
}

void Gradient::setAlphaData (void *alpha_data, int len)
{
  evas_object_gradient_alpha_data_set (o, alpha_data, len);
}

void Gradient::clear()
{
  evas_object_gradient_clear( o );
}

void Gradient::setFill (const Rect &rect)
{
  evas_object_gradient_fill_set (o, rect.x (), rect.y (), rect.width (), rect.height ());
}

const Rect Gradient::getFill ()
{
  Evas_Coord x, y, w, h;
  evas_object_gradient_fill_get (o, &x, &y, &w, &h);
  Rect rect (x, y, w, h);
  return rect;
}

void Gradient::setFillAngle (Evas_Angle angle)
{
  evas_object_gradient_fill_angle_set (o, angle);
}

Evas_Angle Gradient::getFillAngle ()
{
  return evas_object_gradient_fill_angle_get (o);
}

void Gradient::setFillSpread (int tile_mode)
{
  evas_object_gradient_fill_spread_set (o, tile_mode);

}

int Gradient::getFillSpread ()
{
  return evas_object_gradient_fill_spread_get (o);
}

void Gradient::setAngle( Evas_Angle angle )
{
  evas_object_gradient_angle_set( o, angle );
}

Evas_Angle Gradient::getAngle()
{
  return evas_object_gradient_angle_get( o );
}

void Gradient::setDirection (int direction)
{
  evas_object_gradient_direction_set (o, direction);
}

int Gradient::getDirection ()
{
  return evas_object_gradient_direction_get (o);
}

void Gradient::setOffset (float offset)
{
  evas_object_gradient_offset_set (o, offset);
}

float Gradient::getOffset ()
{
  return evas_object_gradient_offset_get (o);
}

} // end namespace Evasxx

