#ifndef EFLPP_ESMART_GROUP_H
#define EFLPP_ESMART_GROUP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <eflpp_esmart.h>
#include <list>

namespace efl {

class EvasEsmartGroup : public EvasEsmart
{
public:
  EvasEsmartGroup( EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartGroup( int x, int y, EvasCanvas* canvas, const char* name = 0 );
  EvasEsmartGroup( int x, int y, int width, int height, EvasCanvas* canvas, const char* name = 0 );
  virtual ~EvasEsmartGroup();

  void add (EvasObject* object);
  void remove (EvasObject* object);

protected:
  // smart object handlers
  virtual void addHandler();
  virtual void delHandler();
  virtual void moveHandler( Evas_Coord x, Evas_Coord y );
  virtual void resizeHandler( Evas_Coord w, Evas_Coord h );
  virtual void showHandler();
  virtual void hideHandler();
  virtual void colorSetHandler( int r, int g, int b, int a );
  virtual void clipSetHandler( Evas_Object *clip );
  virtual void clipUnsetHandler();

private:
   std::list<EvasObject*> evasObjectList;
};

} // end namespace efl

#endif // EFLPP_ESMART_GROUP_H
