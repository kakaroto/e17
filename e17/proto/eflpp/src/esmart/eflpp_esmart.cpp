#include "eflpp_esmart.h"

#include <iostream>
#include <assert.h>
using namespace std;

namespace efl {

//===============================================================================================
// EvasEsmart
//===============================================================================================
EvasEsmart::EvasEsmart(EvasCanvas *canvas, const char *type, const char *name )
    :EvasObject( canvas )
{
	printf ("EvasEsmart constructor\n");
  //selfEsmartPointer = this;
}

EvasEsmart::~EvasEsmart()
{
}

Evas_Object *EvasEsmart::newEsmart( const char *name )
{
  Evas_Object *evasobj;

  evasobj = evas_object_smart_add(canvas()->obj(), getEsmart(name));

  evas_object_smart_data_set(evasobj, this);

  return evasobj;
}

Evas_Smart *EvasEsmart::getEsmart( const char *name )
{
  Evas_Smart *smart = evas_smart_new (name,
                          wrap_add,
                          wrap_del,
                          NULL, //deprecated
                          NULL, //deprecated
                          NULL, //deprecated
                          NULL, //deprecated
                          NULL, //deprecated
                          wrap_move,
                          wrap_resize,
                          wrap_show,
                          wrap_hide,
                          wrap_color_set,
                          wrap_clip_set,
                          wrap_clip_unset,
                          NULL
                         );

  return smart;
}

// C wrapper helpers

void EvasEsmart::wrap_add( Evas_Object *o ) 
{
  //EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  //es->addHandler();
}

void EvasEsmart::wrap_del(Evas_Object *o) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->delHandler();
}

void EvasEsmart::wrap_move(Evas_Object *o, Evas_Coord x, Evas_Coord y) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->moveHandler( x, y );
}

void EvasEsmart::wrap_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->resizeHandler( w, h );
}

void EvasEsmart::wrap_show(Evas_Object *o) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->showHandler();
}

void EvasEsmart::wrap_hide(Evas_Object *o) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->hideHandler();
}

void EvasEsmart::wrap_color_set(Evas_Object *o, int r, int g, int b, int a) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->colorSetHandler( r, g, b, a );
}

void EvasEsmart::wrap_clip_set(Evas_Object *o, Evas_Object *clip) 
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->clipSetHandler( clip );
}

void EvasEsmart::wrap_clip_unset(Evas_Object *o)
{
  EvasEsmart *es = static_cast<EvasEsmart*> (evas_object_smart_data_get(o));
  es->clipUnsetHandler();
}

} // end namespace efl
