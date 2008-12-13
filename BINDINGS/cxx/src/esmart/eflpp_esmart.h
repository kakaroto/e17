#ifndef ESMART_BASE
#define ESMART_BASE

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* EFL++ */
#include <eflpp_common.h>
#include <eflpp_evas.h>

/**
 * C++ Wrapper for the Enlightenment Smart Object Library (ESMART)
 *
 * @author Michael 'Mickey' Lauer <mickey@Vanille.de>
 */

namespace efl {

// TODO: Change to signal handling for callback functions...
class EvasEsmart : public EvasObject
{
public:
  EvasEsmart();
  virtual ~EvasEsmart();

protected:
  Evas_Object *newEsmart( EvasCanvas *canvas, const char *name );
  Evas_Smart *getEsmart( const char *name );

  // smart object handlers
  virtual void addHandler() = 0;
  virtual void delHandler() = 0;
  virtual void moveHandler( Evas_Coord x, Evas_Coord y ) = 0;
  virtual void resizeHandler( Evas_Coord w, Evas_Coord h ) = 0;
  virtual void showHandler() = 0;
  virtual void hideHandler() = 0;
  virtual void colorSetHandler( int r, int g, int b, int a ) = 0;
  // TODO: Evas_Object -> EvasObject?
  virtual void clipSetHandler( Evas_Object *clip ) = 0;
  virtual void clipUnsetHandler() = 0;

private:
  static void wrap_add(Evas_Object *o);
  static void wrap_del(Evas_Object *o);
  static void wrap_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
  static void wrap_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
  static void wrap_show(Evas_Object *o);
  static void wrap_hide( Evas_Object *o);
  static void wrap_color_set(Evas_Object *o, int r, int g, int b, int a);
  // TODO: Evas_Object -> EvasObject?
  static void wrap_clip_set(Evas_Object *o, Evas_Object *clip);
  static void wrap_clip_unset(Evas_Object *o);
  
  Evas_Smart_Class sc;

};

} // end namespace efl

#endif
