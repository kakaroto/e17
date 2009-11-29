#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Canvas.h"
#include "../include/evasxx/Polygon.h"

using namespace Eflxx;

namespace Evasxx {

//===============================================================================================
// Polygon
//===============================================================================================

Polygon::Polygon( Canvas &canvas )
{
  o = evas_object_polygon_add( canvas.obj() );
  init();
}

Polygon::~Polygon()
{
  evas_object_del( o );
}

void Polygon::addPoint( const Point &pos )
{
  evas_object_polygon_point_add( o, pos.x (), pos.y () );
}

void Polygon::clearPoints()
{
  evas_object_polygon_points_clear( o );
}

} // end namespace Evasxx

