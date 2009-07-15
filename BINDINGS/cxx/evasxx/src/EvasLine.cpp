#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasCanvas.h"
#include "../include/evasxx/EvasLine.h"

namespace efl {

EvasLine::EvasLine( EvasCanvas &canvas )
{
  o = evas_object_line_add( canvas.obj() );
  init( );
}

EvasLine::EvasLine( EvasCanvas &canvas, const Rect &rect )
{
  o = evas_object_line_add( canvas.obj() );
  init();

  setGeometry (rect);
}

EvasLine::EvasLine( EvasCanvas &canvas, const Point &pos1, const Point &pos2 )
{
  o = evas_object_line_add( canvas.obj() );
  init();

  setGeometry (pos1, pos2);
}

EvasLine::~EvasLine()
{
  evas_object_del( o );
}

void EvasLine::setGeometry( const Point &pos1, const Point &pos2 )
{
  evas_object_line_xy_set( o, pos1.x (), pos1.y (), pos2.x (), pos2.y () );
}

void EvasLine::setGeometry( const Rect &rect )
{
  evas_object_line_xy_set( o, rect.x (), rect.y (), rect.x () + rect.width (), rect.y () + rect.height () );
}

} // end namespace efl
