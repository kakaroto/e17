#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasCanvas.h"
#include "../include/evasxx/EvasGradient.h"

namespace efl {

EvasGradient::EvasGradient( EvasCanvas &canvas )
{
  o = evas_object_gradient_add( canvas.obj() );
  init();
}

EvasGradient::EvasGradient( EvasCanvas &canvas, const Rect &rect )
{
  o = evas_object_gradient_add( canvas.obj() );
  init();

  setGeometry( rect );
}

EvasGradient::~EvasGradient()
{
  evas_object_del( o );
}

void EvasGradient::addColorStop( const Color &c, int delta )
{
  evas_object_gradient_color_stop_add( o, c.red (), c.green (), c.blue (), c.alpha (), delta );
}

void EvasGradient::addAlphaStop (int alpha, int delta)
{
  evas_object_gradient_alpha_stop_add (o, alpha, delta);
}

void EvasGradient::setColorData (void *color_data, int len, bool has_alpha)
{
  evas_object_gradient_color_data_set (o, color_data, len, has_alpha);
}

void EvasGradient::setAlphaData (void *alpha_data, int len)
{
  evas_object_gradient_alpha_data_set (o, alpha_data, len);
}

void EvasGradient::clear()
{
  evas_object_gradient_clear( o );
}

void EvasGradient::setFill (const Rect &rect)
{
  evas_object_gradient_fill_set (o, rect.x (), rect.y (), rect.width (), rect.height ());
}

const Rect EvasGradient::getFill ()
{
  Evas_Coord x, y, w, h;
  evas_object_gradient_fill_get (o, &x, &y, &w, &h);
  Rect rect (x, y, w, h);
  return rect;
}

void EvasGradient::setFillAngle (Evas_Angle angle)
{
  evas_object_gradient_fill_angle_set (o, angle);
}

Evas_Angle EvasGradient::getFillAngle ()
{
  return evas_object_gradient_fill_angle_get (o);
}

void EvasGradient::setFillSpread (int tile_mode)
{
  evas_object_gradient_fill_spread_set (o, tile_mode);

}

int EvasGradient::getFillSpread ()
{
  return evas_object_gradient_fill_spread_get (o);
}

void EvasGradient::setAngle( Evas_Angle angle )
{
  evas_object_gradient_angle_set( o, angle );
}

Evas_Angle EvasGradient::getAngle()
{
  return evas_object_gradient_angle_get( o );
}

void EvasGradient::setDirection (int direction)
{
  evas_object_gradient_direction_set (o, direction);
}

int EvasGradient::getDirection ()
{
  return evas_object_gradient_direction_get (o);
}

void EvasGradient::setOffset (float offset)
{
  evas_object_gradient_offset_set (o, offset);
}

float EvasGradient::getOffset ()
{
  return evas_object_gradient_offset_get (o);
}

} // end namespace efl
