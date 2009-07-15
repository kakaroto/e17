#ifndef EVASLINE_H
#define EVASLINE_H

#include "EvasObject.h"

namespace efl
{

/* Forward declarations */
class EvasCanvas;

/**
 * Wraps an Evas Line Object
 */
class EvasLine : public EvasObject
{
public:
  EvasLine( EvasCanvas &canvas );
  /*!
   *
   * @param rect The EvasLine rectangle with Point and Size,
   */
  EvasLine( EvasCanvas &canvas, const Rect &rect );
  
  /*!
   * @param pos1 The EvasLine start point.
   * @param pos2 The EvasLine end point.
   */
  EvasLine( EvasCanvas &canvas, const Point &pos1, const Point &pos2 );
  
  virtual ~EvasLine();

  /*!
   * \brief Set the EvasLine geometry.
   *
   * This sets the geometry of the EvasObject automatic.
   * Please notice that the move() and resize() function does
   * only change the size of the EvasObject and not the EvasLine.
   * So please use setGeometry to change the geometry.
   *
   * @param rect The EvasLine rectangle with Point and Size,
   */
  virtual void setGeometry( const Rect &rect );
  
  /*!
   * \brief Set the EvasLine geometry.
   *
   * This sets the geometry of the EvasObject automatic.
   * Please notice that the move() and resize() function does
   * only change the size of the EvasObject and not the EvasLine.
   * So please use setGeometry to change the geometry.
   *
   * @param pos1 The EvasLine start point.
   * @param pos2 The EvasLine end point.
   */
  virtual void setGeometry( const Point &pos1, const Point &pos2 );
};

} // end namespace efl

#endif // EVASLINE_H
