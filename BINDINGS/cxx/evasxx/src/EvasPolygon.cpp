#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasCanvas.h"
#include "../include/evasxx/EvasPolygon.h"

namespace efl {

//===============================================================================================
// Polygon
//===============================================================================================

EvasPolygon::EvasPolygon( EvasCanvas &canvas )
{
  o = evas_object_polygon_add( canvas.obj() );
  init();
}

EvasPolygon::~EvasPolygon()
{
  evas_object_del( o );
}

void EvasPolygon::addPoint( const Point &pos )
{
  evas_object_polygon_point_add( o, pos.x (), pos.y () );
}

void EvasPolygon::clearPoints()
{
  evas_object_polygon_points_clear( o );
}

} // end namespace efl
