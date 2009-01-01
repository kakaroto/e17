#ifndef EFLPP_EVASIMAGE_H
#define EFLPP_EVASIMAGE_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_countedptr.h>
#include "eflpp_evasobject.h"
#include "eflpp_evascanvas.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace efl {

/**
 * Wraps an Evas Image Object
 */
class EvasImage : public EvasObject
{
  public:
    EvasImage( EvasCanvas* canvas, const char* name = 0 );
    EvasImage( const char* filename, EvasCanvas* canvas, const char* name = 0 );
    EvasImage( int x, int y, const char* filename, EvasCanvas* canvas, const char* name = 0 );
    EvasImage( Evas_Object* object, EvasCanvas* canvas, const char* name = 0 );
    virtual ~EvasImage();

   /**
    * Sets the @filename containing the image data.
    * If the image data resides in an .eet or .edb you probably
    * want to supply a @key specifying the image name.
    * @returns true, if the image could be loaded, false, otherwise.
    * @see evas_object_image_file_set
    */
    bool setFile( const char* path, const char* key = 0 );
    void setFill( int x, int y, int width, int height );
    void setFill( int x, int y, const Size& );

    /* Size */
    virtual void resize( int width, int height, bool ff = true );
    virtual void resize( const Size& size, bool ff = true );
    virtual const Size size() const;
    const Size& trueSize();

    /// Sets the size of the image to be display by the given image object.
    void setImageSize (int w, int h);

    /// Retrieves the size of the image displayed by the given image object.
    void getImageSize (int &w, int &h);

    /* Border */
    void setBorder( int left, int right, int top, int bottom );

    /// Sets the raw image data.
    void setData (void *data);

    /// Retrieves the raw image data.
    void *getData (bool for_writing);

    /// Replaces an image object's internal image data buffer.
    void setDataCopy (void *data);

    /// Update a rectangle after putting data into the image.
    void addDataUpdate (int x, int y, int w, int h);

    static void setResourcePath( const char* path ) { EvasImage::_resourcePath = (char*) path; };
    static const char* resourcePath()               { return EvasImage::_resourcePath; };

  private:
    static char* _resourcePath;
    Size _size; // true size
};

} // end namespoace efl

#endif // EFLPP_EVASIMAGE_H
