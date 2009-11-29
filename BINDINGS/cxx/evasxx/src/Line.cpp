#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Canvas.h"
#include "../include/evasxx/Line.h"

using namespace Eflxx;

namespace Evasxx {

Line::Line( Canvas &canvas )
{
  o = evas_object_line_add( canvas.obj() );
  init( );
}

Line::Line( Canvas &canvas, const Eflxx::Rect &rect )
{
  o = evas_object_line_add( canvas.obj() );
  init();

  setGeometry (rect);
}

Line::Line( Canvas &canvas, const Eflxx::Point &pos1, const Eflxx::Point &pos2 )
{
  o = evas_object_line_add( canvas.obj() );
  init();

  setGeometry (pos1, pos2);
}

Line::~Line()
{
  evas_object_del( o );
}

void Line::setGeometry( const Eflxx::Point &pos1, const Eflxx::Point &pos2 )
{
  evas_object_line_xy_set( o, pos1.x (), pos1.y (), pos2.x (), pos2.y () );
}

void Line::setGeometry( const Eflxx::Rect &rect )
{
  evas_object_line_xy_set( o, rect.x (), rect.y (), rect.x () + rect.width (), rect.y () + rect.height () );
}

} // end namespace Evasxx

