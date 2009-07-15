#ifndef EVAS_RECTANGLE_H
#define EVAS_RECTANGLE_H

#include "EvasObject.h"

namespace efl
{

/* Forward declarations */
class EvasCanvas;

/*
 * Wraps an Evas Rectangle Object
 */
class EvasRectangle : public EvasObject
{
public:
  /*!
   * Constructor to create a EvasRectangle with size zero 
   * and position zero.
   */
  EvasRectangle( EvasCanvas &canvas );
  
  /*!
   * Constructor to create a EvasRectangle at a specific geometry.
   */
  EvasRectangle( EvasCanvas &canvas, const Rect &rect );
  
  /*!
   * Constructor to create a EvasRectangle at Point (0, 0)
   * and a given size.
   *
   */
  EvasRectangle( EvasCanvas &canvas, const Size &size );
  
  virtual ~EvasRectangle();
};

} // end namespace efl

#endif // EVAS_RECTANGLE_H
