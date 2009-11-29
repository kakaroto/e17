#ifndef EVAS_POLYGON_H
#define EVAS_POLYGON_H

#include "Object.h"

namespace Evasxx {

/* Forward declarations */
class Canvas;

/*
 * Wraps an Evas Polygon Object
 */
class Polygon : public Object
{
public:
  Polygon( Canvas &canvas );
  virtual ~Polygon();

  void addPoint( const Eflxx::Point &pos );
  void clearPoints();
};

} // end namespace Evasxx

#endif // EVAS_POLYGON_H

