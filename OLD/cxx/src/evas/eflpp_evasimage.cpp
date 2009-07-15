#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_evasimage.h"

using namespace std;

namespace efl {

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
    // TODO: commented out because of file split. Find a better way! Maybe Exceptions?
   /* Dout( dc::notice, *this << " EvasImage::file_set" << " path=" << p << ( k ? k : "<none>" )
            << "(" << EVAS_LOAD_ERROR[errorcode] << ")" );
    if ( errorcode ) cerr << "ERROR: EvasImage::setFile( '" << p << "|" << ( k ? k : "<none>" ) << ") = " << EVAS_LOAD_ERROR[errorcode] << endl;
   */ return ( errorcode == 0 );
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

} // end namespace efl
