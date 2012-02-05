#ifndef ELMXX_BACKGROUND_H
#define ELMXX_BACKGROUND_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Background : public Object
{
public:
  static Background *factory (Evasxx::Object &parent);

  /**
   * Set the image file used for the background
   *
   * This sets the image file used in the background object. The image
   * will be stretched (retaining aspect if its an image file) to completely fill
   * the bg object. This may mean some parts are not visible.
   * 
   * @note  Once the image of @p obj is set, a previously set one will be deleted,
   * even if @p file is empty.
   * 
   * @ingroup Background
   */
  void setFile (const std::string &file);
  
  /**
   * Set the edje file/group used for the background
   * 
   * This sets the image file used in the background object. The edje
   * will be stretched (retaining aspect if its an image file) to completely fill
   * the bg object. This may mean some parts are not visible.
   * 
   * @note  Once the image of @p obj is set, a previously set one will be deleted,
   * even if @p file is empty.
   * 
   * @ingroup Background
   */
  void setFile (const std::string &file, const std::string &group);

#if 0
/** // TODO
 * Get the file (image or edje) used for the background
 *
 * @param obj The bg object
 * @param file The file path
 * @param group Optional key (group in Edje) within the file
 *
 * @ingroup @ingroup Background
 */
EAPI void                         elm_bg_file_get(const Evas_Object *obj, const char **file, const char **group);
#endif
  
  /**
   * Set the option used for the background image
   *
   * @param option The desired background option (TILE, SCALE)
   *
   * This sets the option used for manipulating the display of the background
   * image. The image can be tiled or scaled.
   *
   * @ingroup Background
   */
  void setOption(Elm_Bg_Option option);

  /**
   * Get the option used for the background image
   *
   * @return The desired background option (CENTER, SCALE, STRETCH or TILE)
   *
   * @ingroup Background
   */
  Elm_Bg_Option getOption();
  
  /**
   * Set the option used for the background color
   *
   * @param rgb
   *
   * This sets the RGB color used for the background rectangle. Its range goes
   * from 0 to 255. The alpha component is ignored by this function.
   *
   * @ingroup Background
   */
  void setColor(const Eflxx::Color rgb);
  
  /**
   * Get the option used for the background color
   *
   * @return The RGB color. The alpha component is ignored by this function.
   *
   * @ingroup Background
   */
  Eflxx::Color getColor();
  
  /**
   * Set the size of the pixmap representation of the image.
   *
   * This option just makes sense if an image is going to be set in the bg.
   *
   * @param s The new size of the image pixmap representation.
   *
   * This function sets a new size for pixmap representation of the given bg
   * image. It allows the image to be loaded already in the specified size,
   * reducing the memory usage and load time when loading a big image with load
   * size set to a smaller size.
   *
   * NOTE: this is just a hint, the real size of the pixmap may differ
   * depending on the type of image being loaded, being bigger than requested.
   *
   * @ingroup Background
   */
  void setLoadSize (Eflxx::Size s);
  
private:
  Background (); // forbid standard constructor
  Background (const Background&); // forbid copy constructor
  Background (Evasxx::Object &parent); // private construction -> use factory ()
  ~Background (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_BACKGROUND_H
