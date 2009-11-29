#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/Canvas.h"
#include "../include/evasxx/Rectangle.h"

using namespace Eflxx;

namespace Evasxx {

Rectangle::Rectangle( Canvas &canvas )
{
  o = evas_object_rectangle_add( canvas.obj() );
  init();
}

Rectangle::Rectangle( Canvas &canvas, const Rect& rect )
{
  o = evas_object_rectangle_add( canvas.obj() );
  init();

  setGeometry( rect );
}

Rectangle::Rectangle( Canvas &canvas, const Size &size )
{
  o = evas_object_rectangle_add( canvas.obj() );
  init();

  resize( size );
}

Rectangle::~Rectangle()
{
  evas_object_del( o );
}

} // end namespace Evasxx

