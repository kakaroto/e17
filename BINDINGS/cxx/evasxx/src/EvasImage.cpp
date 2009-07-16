#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasImage.h"
#include "../include/evasxx/EvasCanvas.h"

using namespace std;

namespace efl {

EvasImage::EvasImage( EvasCanvas &canvas, bool filled )
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

EvasImage::EvasImage( EvasCanvas  &canvas, const std::string &filename, bool filled )
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

EvasImage::EvasImage( EvasCanvas &canvas, const Point &pos, const std::string &filename, bool filled )
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

EvasImage::~EvasImage()
{
  evas_object_del( o );
}

int EvasImage::setFile( const std::string &path )
{
  evas_object_image_file_set( o, path.c_str(), NULL );

  return evas_object_image_load_error_get(o);
}

int EvasImage::setFile( const std::string &path, const std::string &key )
{
  evas_object_image_file_set( o, path.c_str(), key.c_str() );

  return evas_object_image_load_error_get(o);
}

void EvasImage::getFile( std::string &outPath, std::string &outKey ) const
{
  const char *path = NULL;
  const char *key = NULL;

  evas_object_image_file_get( o, &path, &key);

  outPath = path;
  outKey = key;
}

void EvasImage::setFill( const Rect &rect )
{
  evas_object_image_fill_set( o, rect.x(), rect.y(), rect.width(), rect.height() );
}

void EvasImage::setFill( const Size &size )
{
  evas_object_image_fill_set( o, 0, 0, size.width(), size.height() );
}


const Rect EvasImage::getFill() const
{
  Evas_Coord x, y, w, h;
  evas_object_image_fill_get( o, &x, &y, &w ,&h );
  Rect rect( x, y, w, h );;
  return rect;
}

void EvasImage::setFilled(bool setting)
{
  evas_object_image_filled_set( o, setting);
}

bool EvasImage::getFilled() const
{
  return evas_object_image_filled_get( o );
}   

void EvasImage::resize( const Size& size, bool fill )
{
  Dout( dc::notice, *this << " EvasImage::resize current size is " << size.width() << ", " << size.height() );
  EvasObject::resize( size, fill );
  if ( fill ) setFill( Rect (0, 0, size.width(), size.height() ));
}

void EvasImage::setImageSize( const Size &size )
{
  evas_object_image_size_set (o, size.width(), size.height() );
}

const Size EvasImage::getImageSize() const
{
  Evas_Coord w, h;
  evas_object_image_size_get (o, &w, &h);
  Size size( w, h );;
  return size;
}

void EvasImage::setBorder( const Padding &border )
{
  evas_object_image_border_set( o, border.left (), border.right (), border.top (), border.bottom () );
}

const Padding EvasImage::getBorder( ) const
{
  Evas_Coord l, r, t, b;
  evas_object_image_border_get( o, &l, &r, &t, &b );
  return Padding (l, r, t, b);
}

void EvasImage::setFillBorderCenter (Evas_Border_Fill_Mode fill)
{
  evas_object_image_border_center_fill_set( o, fill );
}

Evas_Border_Fill_Mode EvasImage::getFillBorderCenter() const
{
  return evas_object_image_border_center_fill_get( o );
}

void EvasImage::setData( void *data )
{
  evas_object_image_data_set( o, data );
}

void *EvasImage::getData( bool for_writing )
{
  return evas_object_image_data_get( o, for_writing );
}

void EvasImage::setDataCopy( void *data )
{
  evas_object_image_data_copy_set( o, data );
}

void EvasImage::addDataUpdate( const Rect &r )
{
  evas_object_image_data_update_add( o, r.x(), r.y(), r.width(), r.height() );
}

void EvasImage::setAlpha( bool has_alpha )
{
  evas_object_image_alpha_set( o, has_alpha );
}

bool EvasImage::getAlpha() const
{
  return evas_object_image_alpha_get( o );
}

void EvasImage::setScaleSmooth( bool smooth_scale )
{
  evas_object_image_smooth_scale_set( o, smooth_scale);
}

bool EvasImage::getScaleSmooth() const
{
  return evas_object_image_smooth_scale_get( o );
}

int EvasImage::getStride() const
{
  return evas_object_image_stride_get( o );
}

bool EvasImage::save( const std::string &file, const std::string &key, const std::string &flags) const
{
  evas_object_image_save( o, file.c_str(), key.empty() ? NULL : key.c_str(), flags.c_str() );
}

void EvasImage::setSpread (int tile_mode)
{
  evas_object_image_fill_spread_set( o, tile_mode);
}

int EvasImage::getSpread () const
{
  return evas_object_image_fill_spread_get( o );
}

void EvasImage::setTransform (EvasTransform &t)
{
  evas_object_image_fill_transform_set (o, &t.t);
}

const EvasTransform EvasImage::getTransform () const
{
  Evas_Transform t;
  evas_object_image_fill_transform_get (o, &t);
  EvasTransform et;
  et.t = t;
  
  return et;
}

void *EvasImage::convertData (Evas_Colorspace to_cspace)
{
  return evas_object_image_data_convert (o, to_cspace);
}

void EvasImage::preload (bool cancel)
{
  evas_object_image_preload (o, cancel);
}

void EvasImage::reload ()
{
  evas_object_image_reload (o);
}

void EvasImage::setDirty (bool dirty)
{
  evas_object_image_pixels_dirty_set (o, dirty);

}

bool EvasImage::getDirty ()
{
  return evas_object_image_pixels_dirty_get (o);
}

void EvasImage::setDPI (double dpi)
{
  evas_object_image_load_dpi_set (o, dpi);

}

double EvasImage::getDPI ()
{
  return evas_object_image_load_dpi_get (o);
}

void EvasImage::setLoadSize (const Size &size)
{
  evas_object_image_load_size_set (o, size.width (), size.height ());
}

const Size EvasImage::getLoadSize ()
{
  Evas_Coord w, h;
  evas_object_image_load_size_get (o, &w, &h);
  Size size (w, h);
  
  return size;
}

void EvasImage::setLoadScaleDown (int scale_down)
{
  evas_object_image_load_scale_down_set (o, scale_down);

}

int EvasImage::setLoadScaleDown ()
{
  return evas_object_image_load_scale_down_get (o);
}

void EvasImage::setColorspace (Evas_Colorspace cspace)
{
  evas_object_image_colorspace_set (o, cspace);
}

Evas_Colorspace EvasImage::getColorspace ()
{
  return evas_object_image_colorspace_get (o);
}

} // end namespace efl
