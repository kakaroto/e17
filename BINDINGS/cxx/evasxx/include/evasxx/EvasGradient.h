#ifndef EVASGRADIENT_H
#define EVASGRADIENT_H

#include "EvasObject.h"

namespace efl {

/* Forward declarations */
class EvasCanvas;

/*
 * Wraps an Evas Gradient Object
 */
class EvasGradient : public EvasObject
{
public:
  EvasGradient( EvasCanvas &canvas );
  EvasGradient( EvasCanvas &canvas, const Rect &rect );
  
  virtual ~EvasGradient();
  
  /*!
   * Adds a color stop to the given evas gradient object.
   *
   * The @p delta parameter determines the proportion of the gradient
   * object that is to be set to the color.  For instance, if red is
   * added with @p delta set to 2, and green is added with @p
   * delta set to 1, two-thirds will be red or reddish and one-third
   * will be green or greenish.
   *
   * Colors are added from the top downwards.
   *
   * @param   color    The color.
   * @param   delta    Proportion of the gradient object that is this color.
   */
  void addColorStop( const Color &c, int delta );
  
  /*!
   * Adds an alpha stop to the given evas gradient object.
   *
   * The @p delta parameter determines the proportion of the gradient
   * object that is to be set to the alpha value.
   *
   * Alphas are added from the top downwards.
   *
   * @param   alpha    Alpha value.
   * @param   delta    Proportion of the gradient object that is this alpha.
   */
  void addAlphaStop (int alpha, int delta);
  
  /*!
   * Sets color data for the given evas gradient object.
   *
   * If data is so set, any existing gradient stops will be deleted,
   * The data is not copied, so if it was allocated, do not free it while it's set.
   *
   * @param   color_data The color data to be set. Should be in argb32 pixel format.
   * @param   len        The length of the data pointer - multiple of the pixel size.
   * @param   has_alpha  A flag indicating if the data has alpha or not.
   */
  void setColorData (void *color_data, int len, bool has_alpha);
  
  /*!
   * Sets alpha data for the given evas gradient object.
   *
   * If alpha data is so set, any existing gradient stops will be cleared,
   * The data is not copied, so if it was allocated, do not free it while it's set.
   *
   * @param   alpha_data  The alpha data to be set, in a8 format.
   * @param   len         The length of the data pointer - multiple of the pixel size.
   */
  void setAlphaData (void *alpha_data, int len);
  
  /*!
   * Deletes all stops set for the given evas gradient object or any set data.
   */
  void clear();
  
  // TODO
  //EAPI void              evas_object_gradient_type_set       (Evas_Object *obj, const char *type, const char *instance_params) EINA_ARG_NONNULL(1, 2);
  //EAPI void              evas_object_gradient_type_get       (const Evas_Object *obj, char **type, char **instance_params) EINA_ARG_NONNULL(1, 2);

  /*!
   * Sets the rectangle on the gradient object that the gradient will be
   * drawn to.
   *
   * Note that the gradient may be tiled around this one rectangle,
   * according to its spread value - restrict, repeat, or reflect.  To have
   * only one 'cycle' of the gradient drawn, the spread value must be set
   * to restrict, or x and y must be 0 and width and height need to be
   * the width and height of the gradient object respectively.
   *
   * The default values for the fill parameters is x = 0, y = 0,
   * width = 32 and height = 32.
   *
   * @param   rect   The coordinate Rect.
   */
  void setFill (const Rect &rect);
  
  /*!
   * Retrieves the dimensions of the rectangle on the gradient object that
   * the gradient will use as its fill rect.
   *
   * See @ref setFill for more details.
   *
   * @return The coordinate Rect.
   */
  const Rect getFill ();
   
  /*!
   * Sets the angle at which the given evas gradient object's fill sits clockwise
   * from vertical.
   *
   * @param   angle Angle in degrees.  Can be negative.
   */
  void setFillAngle (Evas_Angle angle);
  
  /*!
   * Retrieves the angle at which the given evas gradient object's fill sits
   * clockwise from the vertical.
   *
   * @return  The current angle if successful. @c 0.0 otherwise.
   */
  Evas_Angle getFillAngle ();
  
  /*!
   * Sets the tiling mode for the given evas gradient object's fill.
   *
   * @param   spread One of EVAS_TEXTURE_REFLECT, EVAS_TEXTURE_REPEAT,
   * EVAS_TEXTURE_RESTRICT, EVAS_TEXTURE_RESTRICT_REFLECT, EVAS_TEXTURE_RESTRICT_REPEAT,
   * or EVAS_TEXTURE_PAD.
   */
  void setFillSpread (int tile_mode);
  
  /*!
   * Retrieves the spread (tiling mode) for the given gradient object's fill.
   * 
   * @return  The current spread mode of the gradient object.
   */
  int getFillSpread ();
  
  /*!
   * Sets the angle at which the given evas gradient sits,
   * relative to whatever intrinsic orientation of the grad type.
   * Used mostly by 'linear' kinds of gradients.
   *
   * @param   angle Angle in degrees.  Can be negative.
   */
  void setAngle (Evas_Angle angle);

  /*!
   * Retrieves the angle at which the given evas gradient object sits
   * rel to its intrinsic orientation.
   *
   * @return  The current angle if successful. @c 0.0 otherwise.
   */
  Evas_Angle getAngle ();
  
  /*!
   * Sets the direction of the given evas gradient object's spectrum.
   * 
   * @param   direction Values are either 1 (the default) or -1.
   */
  void setDirection (int direction);
  
  /*!
   * Retrieves the evas gradient object's spectrum direction
   *
   * @return  The current gradient direction if successful. @c 1 otherwise.
   */
  int getDirection ();
  
  /*!
   * Sets the offset of the given evas gradient object's spectrum.
   *
   * @param   offset Values can be negative.
   */
  void setOffset (float offset);
  
  /*!
   * Retrieves the spectrum's offset
   *
   * @return  The current gradient offset if successful. @c 0.0 otherwise.
   */
  float getOffset ();
};

} // end namespace efl

#endif // EVASGRADIENT_H
 