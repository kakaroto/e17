#ifndef EVAS_RECTANGLE_H
#define EVAS_RECTANGLE_H

#include "Object.h"

namespace Evasxx
{

/* Forward declarations */
class Canvas;

/*
 * Wraps an Evas Rectangle Object
 */
class Rectangle : public Object
{
public:
  /*!
   * Constructor to create a Rectangle with size zero 
   * and position zero.
   */
  Rectangle( Canvas &canvas );
  
  /*!
   * Constructor to create a Rectangle at a specific geometry.
   */
  Rectangle( Canvas &canvas, const Eflxx::Rect &rect );
  
  /*!
   * Constructor to create a Rectangle at Point (0, 0)
   * and a given size.
   *
   */
  Rectangle( Canvas &canvas, const Eflxx::Size &size );
  
  virtual ~Rectangle();
};

} // end namespace Evasxx

#endif // EVAS_RECTANGLE_H
