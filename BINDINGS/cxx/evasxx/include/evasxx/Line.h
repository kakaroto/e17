#ifndef EVAS_LINE_H
#define EVAS_LINE_H

#include "Object.h"

namespace Evasxx
{

/* Forward declarations */
class Canvas;

/**
 * Wraps an Evas Line Object
 */
class Line : public Object
{
public:
  Line( Canvas &canvas );
  /*!
   *
   * @param rect The Line rectangle with Point and Size,
   */
  Line( Canvas &canvas, const Eflxx::Rect &rect );
  
  /*!
   * @param pos1 The Line start point.
   * @param pos2 The Line end point.
   */
  Line( Canvas &canvas, const Eflxx::Point &pos1, const Eflxx::Point &pos2 );
  
  virtual ~Line();

  /*!
   * \brief Set the Line geometry.
   *
   * This sets the geometry of the Object automatic.
   * Please notice that the move() and resize() function does
   * only change the size of the Object and not the Line.
   * So please use setGeometry to change the geometry.
   *
   * @param rect The Line rectangle with Point and Size,
   */
  virtual void setGeometry( const Eflxx::Rect &rect );
  
  /*!
   * \brief Set the Line geometry.
   *
   * This sets the geometry of the Object automatic.
   * Please notice that the move() and resize() function does
   * only change the size of the Object and not the Line.
   * So please use setGeometry to change the geometry.
   *
   * @param pos1 The Line start point.
   * @param pos2 The Line end point.
   */
  virtual void setGeometry( const Eflxx::Point &pos1, const Eflxx::Point &pos2 );
};

} // end namespace Evasxx

#endif // EVAS_LINE_H
