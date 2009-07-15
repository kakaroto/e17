#ifndef EVASPOLYGON_H
#define EVASPOLYGON_H

#include "EvasObject.h"

namespace efl {

/* Forward declarations */
class EvasCanvas;

/*
 * Wraps an Evas Polygon Object
 */
class EvasPolygon : public EvasObject
{
public:
  EvasPolygon( EvasCanvas &canvas );
  virtual ~EvasPolygon();

  void addPoint( const Point &pos );
  void clearPoints();
};

} // end namespace efl

#endif // EVASPOLYGON_H
