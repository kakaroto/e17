#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Image.h"
#include "../include/evasxx/Canvas.h"

using namespace std;
using namespace Eflxx;

namespace Evasxx {

Image::Image( Canvas &canvas, bool filled )
{
  if (filled)
  {
    o = evas_object_image_filled_add( canvas.obj() );
  }
  else
  {
    o = evas_object_image_add( canvas.obj() );
  }

  init();
}

Image::Image( Canvas  &canvas, const std::string &filename, bool filled )
{
  if (filled)
  {
    o = evas_object_image_filled_add( canvas.obj() );
  }
  else
  {
    o = evas_object_image_add( canvas.obj() );
  }

  init();

  setFile( filename );
}

Image::Image( Canvas &canvas, const Point &pos, const std::string &filename, bool filled )
{
  if (filled)
  {
    o = evas_object_image_filled_add( canvas.obj() );
  }
  else
  {
    o = evas_object_image_add( canvas.obj() );
  }

  init();

  setFile( filename );
  move( pos );
}

Image::~Image()
{
  evas_object_del( o );
}

int Image::setFile( const std::string &path )
{
  evas_object_image_file_set( o, path.c_str(), NULL );

  return evas_object_image_load_error_get(o);
}

int Image::setFile( const std::string &path, const std::string &key )
{
  evas_object_image_file_set( o, path.c_str(), key.c_str() );

  return evas_object_image_load_error_get(o);
}

void Image::getFile( std::string &outPath, std::string &outKey ) const
{
  const char *path = NULL;
  const char *key = NULL;

  evas_object_image_file_get( o, &path, &key);

  outPath = path;
  outKey = key;
}

void Image::setFill( const Rect &rect )
{
  evas_object_image_fill_set( o, rect.x(), rect.y(), rect.width(), rect.height() );
}

void Image::setFill( const Size &size )
{
  evas_object_image_fill_set( o, 0, 0, size.width(), size.height() );
}


const Rect Image::getFill() const
{
  Evas_Coord x, y, w, h;
  evas_object_image_fill_get( o, &x, &y, &w ,&h );
  Rect rect( x, y, w, h );;
  return rect;
}

void Image::setFilled(bool setting)
{
  evas_object_image_filled_set( o, setting);
}

bool Image::getFilled() const
{
  return evas_object_image_filled_get( o );
}   

void Image::resize( const Size& size, bool fill )
{
  Dout( dc::notice, *this << " Image::resize current size is " << size.width() << ", " << size.height() );
  Object::resize( size, fill );
  if ( fill ) setFill( Rect (0, 0, size.width(), size.height() ));
}

void Image::setImageSize( const Size &size )
{
  evas_object_image_size_set (o, size.width(), size.height() );
}

const Size Image::getImageSize() const
{
  Evas_Coord w, h;
  evas_object_image_size_get (o, &w, &h);
  Size size( w, h );;
  return size;
}

void Image::setBorder( const Padding &border )
{
  evas_object_image_border_set( o, border.left (), border.right (), border.top (), border.bottom () );
}

const Padding Image::getBorder( ) const
{
  Evas_Coord l, r, t, b;
  evas_object_image_border_get( o, &l, &r, &t, &b );
  return Padding (l, r, t, b);
}

void Image::setFillBorderCenter (Evas_Border_Fill_Mode fill)
{
  evas_object_image_border_center_fill_set( o, fill );
}

Evas_Border_Fill_Mode Image::getFillBorderCenter() const
{
  return evas_object_image_border_center_fill_get( o );
}

void Image::setData( void *data )
{
  evas_object_image_data_set( o, data );
}

void *Image::getData( bool for_writing )
{
  return evas_object_image_data_get( o, for_writing );
}

void Image::setDataCopy( void *data )
{
  evas_object_image_data_copy_set( o, data );
}

void Image::addDataUpdate( const Rect &r )
{
  evas_object_image_data_update_add( o, r.x(), r.y(), r.width(), r.height() );
}

void Image::setAlpha( bool has_alpha )
{
  evas_object_image_alpha_set( o, has_alpha );
}

bool Image::getAlpha() const
{
  return evas_object_image_alpha_get( o );
}

void Image::setScaleSmooth( bool smooth_scale )
{
  evas_object_image_smooth_scale_set( o, smooth_scale);
}

bool Image::getScaleSmooth() const
{
  return evas_object_image_smooth_scale_get( o );
}

int Image::getStride() const
{
  return evas_object_image_stride_get( o );
}

bool Image::save( const std::string &file, const std::string &key, const std::string &flags) const
{
  evas_object_image_save( o, file.c_str(), key.empty() ? NULL : key.c_str(), flags.c_str() );
}

void Image::setFillSpread (Evas_Fill_Spread spread)
{
  evas_object_image_fill_spread_set( o, spread);
}

int Image::getSpread () const
{
  return evas_object_image_fill_spread_get( o );
}

void *Image::convertData (Evas_Colorspace to_cspace)
{
  return evas_object_image_data_convert (o, to_cspace);
}

void Image::preload (bool cancel)
{
  evas_object_image_preload (o, cancel);
}

void Image::reload ()
{
  evas_object_image_reload (o);
}

void Image::setDirty (bool dirty)
{
  evas_object_image_pixels_dirty_set (o, dirty);

}

bool Image::getDirty ()
{
  return evas_object_image_pixels_dirty_get (o);
}

void Image::setDPI (double dpi)
{
  evas_object_image_load_dpi_set (o, dpi);

}

double Image::getDPI ()
{
  return evas_object_image_load_dpi_get (o);
}

void Image::setLoadSize (const Size &size)
{
  evas_object_image_load_size_set (o, size.width (), size.height ());
}

const Size Image::getLoadSize ()
{
  Evas_Coord w, h;
  evas_object_image_load_size_get (o, &w, &h);
  Size size (w, h);
  
  return size;
}

void Image::setLoadScaleDown (int scale_down)
{
  evas_object_image_load_scale_down_set (o, scale_down);

}

int Image::setLoadScaleDown ()
{
  return evas_object_image_load_scale_down_get (o);
}

void Image::setColorspace (Evas_Colorspace cspace)
{
  evas_object_image_colorspace_set (o, cspace);
}

Evas_Colorspace Image::getColorspace ()
{
  return evas_object_image_colorspace_get (o);
}

} // end namespace Evasxx

