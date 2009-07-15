#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/evasxx/EvasCanvas.h"
#include "../include/evasxx/EvasRectangle.h"

namespace efl {

EvasRectangle::EvasRectangle( EvasCanvas &canvas )
{
  o = evas_object_rectangle_add( canvas.obj() );
  init();
}

EvasRectangle::EvasRectangle( EvasCanvas &canvas, const Rect& rect )
{
  o = evas_object_rectangle_add( canvas.obj() );
  init();

  setGeometry( rect );
}

EvasRectangle::EvasRectangle( EvasCanvas &canvas, const Size &size )
{
  o = evas_object_rectangle_add( canvas.obj() );
  init();

  resize( size );
}

EvasRectangle::~EvasRectangle()
{
  evas_object_del( o );
}

} // end namespace efl
