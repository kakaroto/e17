#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/Part.h"
#include "../include/edjexx/Object.h"
#include "../include/edjexx/ExternalParam.h"

/* STD */
#include <map>

namespace Edjexx {

Part::Part( Object* parent, const std::string &partname )
    :_parent( parent), _partname( partname )
{
  Dout( dc::notice, " Part::EdjePart( '" << _partname << "' ) constructing..." );
}

Part::~Part()
{
  Dout( dc::notice, "~Part::EdjePart( '" << _partname << "' ) destructing..." );
}

Eflxx::Rect Part::getGeometry() const
{
  int x;
  int y;
  int w;
  int h;
  edje_object_part_geometry_get( _parent->obj(), _partname.c_str (), &x, &y, &w, &h );
  return Eflxx::Rect( x, y, w, h );
}


void Part::setText( const std::string &text )
{
  edje_object_part_text_set( _parent->obj(), _partname.c_str (), text.c_str () );
}

const std::string Part::getText() const
{
  return edje_object_part_text_get( _parent->obj(), _partname.c_str () );
}


void Part::swallow( Evasxx::Object* object )
{
  edje_object_part_swallow (_parent->obj(), _partname.c_str (), object->obj());
}

void Part::unswallow( Evasxx::Object* object )
{
  edje_object_part_unswallow( _parent->obj(), object->obj() );
}

Eflxx::CountedPtr <Evasxx::Object> Part::getSwallow()
{
  Evas_Object *eo = edje_object_part_swallow_get (_parent->obj(), _partname.c_str ());

  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

Eflxx::CountedPtr <Evasxx::Object> Part::getExternalObject ()
{
  Evas_Object *eo = edje_object_part_external_object_get (_parent->obj(), _partname.c_str ());
 
  Evasxx::Object *ret_o = Evasxx::Object::wrap (eo);

  return Eflxx::CountedPtr <Evasxx::Object> (ret_o);
}

bool Part::setParam (Edjexx::ExternalParam *param)
{
  return edje_object_part_external_param_set (_parent->obj (), _partname.c_str (), param->getRaw ());
}

/*const Evasxx::Object* Part::getObject ( const char* name )
{
  return Evasxx::Object::objectLink( edje_object_part_object_get( static_cast <const Evas_Object*> (_parent->obj()), name ) );
}*/

} // end namespace Edjexx
