#ifndef EVAS_IMAGE_H
#define EVAS_IMAGE_H

/* STL */
#include <string>

/* EFL++ */
#include <eflxx/Common.h>
#include "Object.h"

/* EFL */
#include <Evas.h>

using std::string;

namespace Evasxx {

/* Forward declaration */
class Canvas;

/*!
 * Wraps an Evas Image Object
 */
class Image : public Object
{
public:
  /*!
   * @brief Constructor for a new Image object on the given Canvas
   */
  Image( Canvas &canvas, bool filled = false );

  /*!
   * @brief Constructor for a new Image object on the given Canvas
   *
   * @param filename call setFile after creating
   */
  Image( Canvas &canvas, const std::string &filename, bool filled = false );

  /*!
   * @brief Constructor for a new Image object on the given Canvas
   *
   * @param filename call setFile after creating
   * @param x move object to x after creating
   * @param y move object to y after creating
   */
  Image( Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, bool filled = false );
  
  // TODO: evas copy constructor!

  virtual ~Image();

  /*!
   * @brief Sets the @path containing the image data.
    *
   * @returns EVAS_LOAD_ERROR_* (EVAS_LOAD_ERROR_NONE    == success)
   * @see evas_object_image_file_set
   * @see evas_object_image_load_error_get
   */
  int setFile( const std::string &path );

  /*!
   * @brief Sets the @path containing the image data.
   * If the image data resides in an .eet you probably
   * want to supply a @key specifying the image name.
    *
   * @returns EVAS_LOAD_ERROR_* (EVAS_LOAD_ERROR_NONE	 == success)
   * @see evas_object_image_file_set
   * @see evas_object_image_load_error_get
   */
  int setFile( const std::string &path, const std::string &key );

  /*!
   * @brief Gets the loaded image @path.
   * It gets a @key if loaded from a eet file.
   *
   * @param outPath The image path (OUT).
   * @param outKey The image key (OUT).
   */
  void getFile( std::string &outPath, std::string &outKey ) const;

  /*!
  * Sets the rectangle of the given image object that the image will
  * be drawn to.
  *
  * Note that the image will be tiled around this one rectangle. To have
  * only one copy of the image drawn, x and y must be 0 and width
  * and height need to be the width and height of the image object
  * respectively.
  *
  * The default values for the fill parameters is:
  * Rect (x = 0, y = 0, width = 32, height = 32)
  *
  * @param r The image rectangle.
  */
  void setFill( const Eflxx::Rect &rect );

  /*!
  * Sets the rectangle of the given image object that the image will
  * be drawn to.
  *
  * @see setFill( const Rect &rect )
  * But with Point (0, 0) to enable a easier interface to not tile the image
  * and draw only one image.
  *
  * @param r The image rectangle.
  */
  void setFill( const Eflxx::Size &size );
  
  /*!
   * Retrieves the dimensions of the rectangle of the given image object
   * that the image will be drawn to.
   *
   * See @ref evas_object_image_fill_set for more details.
   *
   * @param r Location to store the image in.
   */
  const Eflxx::Rect getFill() const;

  /*!
  * Sets if image fill property should track object size.
  *
  * If set to true, then every resize() will automatically
  * trigger call to setFill() with the new size so
  * image will fill the whole object area.
  *
  * @param setting whether to follow object size.
  *
  * @see addFilled()
  * @see setFill()
  */
  void setFilled(bool setting);
  
  /*!
   * Retrieves if image fill property is tracking object size.
   *
   * @return true if it is tracking, false if not and setFill()
   * must be called manually.
   */
  bool getFilled() const;
  
  /*!
   * @brief Resize geometry of the Image.
   *
   * @param fill Set if the image should be resized together with the Object container.
   */
  virtual void resize( const Eflxx::Size& size, bool fill = true );
  
  /*!
   * Sets the size of the given image object.
   *
   * This function will scale down or crop the image so that it is
   * treated as if it were at the given size. If the size given is
   * smaller than the image, it will be cropped. If the size given is
   * larger, then the image will be treated as if it were in the upper
   * left hand corner of a larger image that is otherwise transparent.
   *
   * @param size The new size of the image.
   */
  void setImageSize( const Eflxx::Size &size );
  
  /*!
   * Retrieves the size of the given image object.
   *
   * See @ref setSize for more details.
   *
   * @return The size of the image.
   */
  const Eflxx::Size getImageSize() const;

  /*!
   * Sets how much of each border of the given image object is not
   * to be scaled.
   *
   * When rendering, the image may be scaled to fit the size of the
   * image object. This function sets what area around the border of
   * the image is not to be scaled. This sort of function is useful for
   * widget theming, where, for example, buttons may be of varying
   * sizes, but the border size must remain constant.
   *
   * The units used for @p left, @p right, @p top and @p bottom are output units.
   *
   * @param border Padding of border that is not to be stretched.
   */
  void setBorder( const Eflxx::Padding &border );

  /*!
   * Retrieves how much of each border of the given image object is not to
   * be scaled.
   *
   * See @ref evas_object_image_border_set for more details.
   *
   * @return Padding of border that is not to be stretched.
   */
  const Eflxx::Padding getBorder( ) const;

  /*!
   * Sets if the center part of the given image object (not the border)
   * should be drawn.
   *
   * When rendering, the image may be scaled to fit the size of the
   * image object. This function sets if the center part of the scaled image
   * is to be drawn or left completely blank, or forced to be solid. Very useful
   * for frames and decorations.
   *
   * @param fill Fill mode of the middle.
   */
  void setFillBorderCenter (Evas_Border_Fill_Mode fill);

  /*!
   * Retrieves if the center of the given image object is to be drawn
   * or not.
   *
   * See @ref setFillBorderCenter for more details.
   *
   * @return Fill mode of the  center.
   */
  Evas_Border_Fill_Mode getFillBorderCenter() const;

  /*!
  * Sets the raw image data of the given image object.
  *
  * Note that the raw data must be of the same size and colorspace
  * of the image. If data is NULL the current image data will be freed.
  *
  * @param data The raw data, or NULL.
  */
  void setData (void *data);

  /*!
  * Get a pointer to the raw image data of the given image object.
  *
  * This function returns a pointer to an image object's internal pixel buffer,
  * for reading only or read/write. If you request it for writing, the image
  * will be marked dirty so that it gets redrawn at the next update.
  *
  * This is best suited when you want to modify an existing image,
  * without changing its dimensions.
  *
  * @param for_writing Whether the data being retrieved will be modified.
  * @return The raw image data.
  */
  void *getData (bool for_writing);

  /*!
  * Replaces the raw image data of the given image object.
  *
  * This function lets the application replace an image object's internal pixel
  * buffer with a user-allocated one. For best results, you should generally
  * first call setImageSize() with the width and height for the
  * new buffer.
  *
  * This call is best suited for when you will be using image data with
  * different dimensions than the existing image data, if any. If you only need
  * to modify the existing image in some fashion, then using
  * getImageSize() is probably what you are after.
  *
  * Note that the caller is responsible for freeing the buffer when finished
  * with it, as user-set image data will not be automatically freed when the
  * image object is deleted.
  *
  * See @ref getImageSize for more details.
  *
  * @param data The raw data.
  */
  void setDataCopy (void *data);

  /*!
  * To be documented.
  *
  * FIXME: To be fixed.
  *
  */
  /// Update a rectangle after putting data into the image.
  void addDataUpdate ( const Eflxx::Rect &r );

  /*!
  * To be documented.
  *
  * FIXME: To be fixed.
  *
  */
  void setAlpha( bool has_alpha );
  
  /*!
  * To be documented.
  *
  * FIXME: To be fixed.
  *
  */
  bool getAlpha() const;

  /*!
   * Sets whether to use of high-quality image scaling algorithm
   * of the given image object.
   *
   * When enabled, a higher quality image scaling algorithm is used when scaling
   * images to sizes other than the source image. This gives better results but
   * is more computationally expensive.
   *
   * @param smooth_scale Whether to use smooth scale or not.
   */
  void setScaleSmooth( bool smooth_scale );

  /*!
   * Retrieves whether the given image object is using use a high-quality
   * image scaling algorithm.
   *
   * See @ref setScaleSmooth for more details.
   *
   * @return Whether smooth scale is being used.
   */
  bool getScaleSmooth() const;

  /*!
   * Retrieves the row stride of the given image object,
   *
   * The row stride is the number of units between the start of a
   * row and the start of the next row.
   *
   * @return The stride of the image.
   */
  int getStride() const;

  /*!
   * Save the given image object to a file.
   *
   * Note that you should pass the filename extension when saving.
   * If the file supports multiple data stored in it as eet,
   * you can specify the key to be used as the index of the
   * image in this file.
   *
   * You can specify some flags when saving the image.
   * Currently acceptable flags are quality and compress.
   * Eg.: "quality=100 compress=9"
   *
   * @param file The filename to be used to save the image.
   * @param key The image key in file (or empty string).
   * @param flags String containing the flags to be used.
   */
  bool save( const std::string &file, const std::string &key, const std::string &flags) const;
  
  /*!
   * Sets the tiling mode for the given evas image object's fill.
   * @param   spread One of EVAS_TEXTURE_REFLECT, EVAS_TEXTURE_REPEAT,
   * EVAS_TEXTURE_RESTRICT, or EVAS_TEXTURE_PAD.
   */
  void setFillSpread (Evas_Fill_Spread spread);
  
  /*!
  * Retrieves the spread (tiling mode) for the given image object's fill.
  * @return  The current spread mode of the image object.
  */
  int getSpread () const;
  
  /*!
  * Converts the raw image data of the given image object to the
  * specified colorspace.
  *
  * Note that this function does not modify the raw image data.
  * If the requested colorspace is the same as the image colorspace
  * nothing is done and NULL is returned. You should use
  * evas_object_image_colorspace_get() to check the current image
  * colorspace.
  *
  * See @ref getColorspace
  *
  * @param to_cspace The colorspace to which the image raw data will be converted.
  * @return data A newly allocated data in the format specified by to_cspace.
  */
  void *convertData (Evas_Colorspace to_cspace);
  
  /*!
   * Preload image in the background
   *
   * This function request the preload of the data image in the background. The
   * worked is queued before being processed.
   *
   * TODO: how in C++?
   * If image data is already loaded, it will callback
   * EVAS_CALLBACK_IMAGE_PRELOADED immediatelly and do nothing else.
   *
   * If cancel is set, it will remove the image from the workqueue.
   *
   * @param cancel false means add to the workqueue, true for remove it.
   */
  void preload (bool cancel);
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  void reload ();
  
  //TODO:
  //EAPI Eina_Bool         evas_object_image_pixels_import          (Evas_Object *obj, Evas_Pixel_Import_Source *pixels) EINA_ARG_NONNULL(1, 2);
  //EAPI void              evas_object_image_pixels_get_callback_set(Evas_Object *obj, void (*func) (void *data, Evas_Object *o), void *data) EINA_ARG_NONNULL(1, 2);
    
  /*!
   * Mark whether the given image object is dirty (needs to be redrawn).
   *
   * @param dirty Whether the image is dirty.
   */
  void setDirty (bool dirty);
  
  /*!
   * Retrieves whether the given image object is dirty (needs to be redrawn).
   *
   * @return Whether the image is dirty.
   */
  bool getDirty ();
  
  /*!
  * To be documented.
  *
  * FIXME: To be fixed.
  *
  */
  void setDPI (double dpi);
  
  /*!
  * To be documented.
  *
  * FIXME: To be fixed.
  *
  */
  double getDPI ();
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  void setLoadSize (const Eflxx::Size &size);
  
  const Eflxx::Size getLoadSize ();
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  void setLoadScaleDown (int scale_down);
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  int setLoadScaleDown ();
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  void setColorspace (Evas_Colorspace cspace);
  
  /*!
   * To be documented.
   *
   * FIXME: To be fixed.
   *
   */
  Evas_Colorspace getColorspace ();
  
};

} // end namespoace evas

#if 0

TODO:

EAPI void                 evas_object_image_native_surface_set  (Evas_Object *obj, Evas_Native_Surface *surf) EINA_ARG_NONNULL(1, 2);
EAPI Evas_Native_Surface *evas_object_image_native_surface_get  (const Evas_Object *obj) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;




#endif

#endif // EVAS_IMAGE_H
