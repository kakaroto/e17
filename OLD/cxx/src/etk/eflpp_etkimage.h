#ifndef EFLPP_ETKIMAGE_H
#define EFLPP_ETKIMAGE_H

/* STL */
#include <string>

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>
#include <eflpp_countedptr.h>
#include "eflpp_etkwidget.h"

/* EFL */
#include <etk/etk_image.h>

using std::string;

namespace efl {

class EtkImage : public EtkWidget
{
  public:
  /*
  Etk_Widget * 	etk_image_new (void)
    Creates a new empty image.
  Etk_Widget * 	etk_image_new_from_file (const char *filename, const char *key)
    Creates a new image and loads the image from an image file.
  Etk_Widget * 	etk_image_new_from_edje (const char *filename, const char *group)
    Creates a new image and loads the image from an edje-file.
  Etk_Widget * 	etk_image_new_from_stock (Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
    Creates a new image and loads the image corresponding to the stock id.
  Etk_Widget * 	etk_image_new_from_evas_object (Evas_Object *evas_object)
    Creates a new image from the given evas object.
  Etk_Widget * 	etk_image_new_from_data (int width, int height, void *data, Etk_Bool copy)
    Creates a new image from the given pixel data.
  */
    /**
     * @brief Loads the image from a file.
     * @param filename the path to the file to load
     * @param key the key to load (only used if the file is an Eet file, otherwise you can set it to NULL)
     */
    void setFromFile( const string &filename, const string &key );
    
    /**
     * @brief Gets the path to the file used by the image.
     * @param outFilename the location where to store the path to the loaded file
     * @param outKey the location where to store the key of the loaded image
     * @param false if the file is not loaded from an Eet file
     */
    void getFile( string &outFilename, string &outKey, bool eetLoaded );
    
    /**
     * @brief Loads the image from an edje file
     * @param filename the path to the edje-file to load
     * @param group the name of the edje-group to load
     */
    void setFromEdje (const string &filename, const string &group);
    
    /**
     * @brief Gets the filename and the group of the edje-object used for the image
     * @param outFilename the location to store the path to the edje-file used
     * @param outGroup the location to store the name of the edje-group used
     */
    void getEdje (string &outFilename, string &outGroup);
    
    /**
     * @brief Loads the image corresponding to the given stock-id
     * @param stock_id the stock-id corresponding to the icon to load
     * @param stock_size the size of the stock-icon
     */
    void setFromStock (Etk_Stock_Id stock_id, Etk_Stock_Size stock_size);
    
    /**
     * @brief Gets the stock-id used by the image
     * @return stock id used by the image
     */
    Etk_Stock_Id getStockId ();
    
    /**
     * @brief Gets the stock-size used by the image
     * @return stock size used by the image
     */
    Etk_Stock_Size getStockSize ();

    
    /**
     * @brief Loads the image from an Evas object
     * @param evas_object the Evas object to use. The object can be anything (image, edje object, emotion object, ...)
     */
    void setFromEvasObject (const EvasObject &evas_object);
    
    /**
     * @brief Gets the Evas object used by the image. You can call this function even if you have not explicitly set the
     * Evas object used by this image. For example, if you have loaded the image from a file, this function will return the
     * corresponding Evas image object. You should just be careful by manipulating it (don't use Edje functions on an image
     * object for example).
     * @return Returns the EvasObject of the image
     */
    CountedPtr <EvasObject> getEvasObject ();
  
    /**
     * @brief Sets the pixels of the image
     * @param width the width of the image
     * @param height the height of the image
     * @param data a pointer to the pixels: the pixels have to be stored in the premul'ed ARGB format
     * @param copy whether the pixels should be copied or not. If you decide not to copy the pixels, you have to make sure
     * the memory area where the pixels are stored is valid during all the lifetime of the image
     * @return Returns the new image widget
     */
    void setFromData (int width, int height, void *data, bool copy);
    
    /**
     * @brief Gets a pointer to the image pixels. This function only works if the image has been loaded from a file or if
     * you have explicitely set its pixels with etk_image_set_from_data().
     * @param for_writing whether or not you want to be able to modify the pixels of the image. If so, call
     * update() once you have finished.
     * @return Returns a pointer to the location of the pixels (stored in premul'ed ARGB format)
     * @note If the image is loaded from a file, it has to be realized. Otherwise it will return NULL
     */
    void *getData (bool for_writing);
    
    /**
     * @brief Gets the source of the image (file, edje-file, stock, Evas object or pixel data)
     * @return Returns the source of the image
     */
    Etk_Image_Source getSource ();
    
    /**
     * @brief Updates all the pixels of the image (to be called after you have modified the pixel buffer for example).
     * Same as rectUpdate(0, 0, image_width, image_height)
     */
    void update ();
    
    /**
     * @brief Updates a rectangle of the pixels of the image (to be called after you have modified the pixel buffer
     * for example). It only has effect on image loaded from a pixel buffer
     * @param x the x position of the top-left corner of the rectangle to update
     * @param y the y position of the top-left corner of the rectangle to update
     * @param w the width of the rectangle to update
     * @param h the height of the rectangle to update
     */
    void rectUpdate (int x, int y, int w, int h);
    
    /**
     * @brief Gets the native width of the image. If the image is loaded from a file or from given pixels, it returns the
     * native size of the image. If the image is loaded from an Edje file or a stock-id, it returns the minimum size of the
     * Edje object (a stock image is an Edje object). Otherwise, the returned size is 0x0
     * @return native width of the image
     */
    int getWidth ();
    
    /**
     * @brief Gets the native height of the image. If the image is loaded from a file or from given pixels, it returns the
     * native size of the image. If the image is loaded from an Edje file or a stock-id, it returns the minimum size of the
     * Edje object (a stock image is an Edje object). Otherwise, the returned size is 0x0
     * @return native height of the image
     */
    int getHeight ();
    
    /**
     * @brief Sets if the image should keep its aspect ratio when it is resized
     * @param keep_aspect if @a keep_aspect is true, the image will keep its aspect ratio when it is resized
     */
    void setAspect (bool keep_aspect);
    
    /**
     * @brief Gets whether the image keeps its aspect ratio when it is resized
     * @return Returns true if the image keeps its aspect ratio when it is resized
     */
    bool getKeepAspect ();
    
     /**
     * @brief Sets (forces) the aspect ratio of the image. You can use this function for example to set the aspect-ratio
     * when you set the image from an Evas object with etk_image_set_from_evas_object().
     * @param aspect_ratio the aspect ratio to set, or 0.0 to make Etk calculates automatically the aspect ratio
     */
    void setRatio (double aspect_ratio);
    
    /**
     * @brief Gets the aspect ratio you set to the image. If no aspect ratio has been set, it will return 0.0.
     * To know the native aspect ratio, call etk_image_size_get() to get the native size of the image and calculate the
     * aspect ratio from these values.
     * @return Returns the aspect ratio you set to the image, or 0.0 if no aspect ratio has been set
     */
    double getRatio ();
   
  /**
   * @brief Copies the image @a src_image to @a dest_image
   * @param dest_image the destination image
   * @param src_image the image to copy
   * @note If @a src_image is an edje image, the current state of the edje animation won't be copied
   */
    /*
   TODO: copy constructor:
   void 	etk_image_copy (Etk_Image *dest_image, Etk_Image *src_image)
   */
  
    /**
     * @brief C object wrapper factory method
     * Only for internal usage!
     */
    static EtkImage *wrap( Etk_Object* o );
  
  private:
    EtkImage( Etk_Object *o );
};

} // end namespace efl

#endif // EFLPP_ETKIMAGE_H
