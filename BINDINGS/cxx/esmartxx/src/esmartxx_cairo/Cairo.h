#ifndef ESMARTXX_CAIRO_H
#define ESMARTXX_CAIRO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <evasxx/Smart.h>

/* EFL */
#include <Esmart/Esmart_Cairo.h>

namespace Esmartxx {

class Cairo : public Evasxx::Smart
{
public:

public:
  Cairo( Evasxx::Canvas &canvas, const Eflxx::Size &size, bool alpha);
  Cairo( Evasxx::Canvas &canvas, const Eflxx::Point &pos, const Eflxx::Size &size, bool alpha);
  Cairo( Evasxx::Canvas &canvas, cairo_surface_t *cairo_surface);  // TODO:think about supporting cairomm
  
  virtual ~Cairo();

public:

  cairo_surface_t* getSurface ();
  
  bool setSurface (cairo_surface_t *cairo_surface);
  
  void setAutoFill (bool enable);
  
};

} // end namespace Esmartxx

#endif // ESMARTXX_CAIRO_H
