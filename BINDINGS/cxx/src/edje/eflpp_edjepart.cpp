#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_edjepart.h"
#include "eflpp_evasedje.h"

/* EFL */
#include <Edje.h>

/* STD */
#include <map>

namespace efl {

EdjePart::EdjePart( EvasEdje* parent, const char* partname )
    :_parent( parent), _partname( partname )
{
    Dout( dc::notice, " EdjePart::EdjePart( '" << _partname << "' ) constructing..." );
}

EdjePart::~EdjePart()
{
    Dout( dc::notice, "~EdjePart::EdjePart( '" << _partname << "' ) destructing..." );
}

Rect EdjePart::geometry() const
{
    int x; int y; int w; int h;
    edje_object_part_geometry_get( _parent->obj(), _partname, &x, &y, &w, &h );
    return Rect( x, y, w, h );
}


void EdjePart::setText( const char* text )
{
    edje_object_part_text_set( _parent->obj(), _partname, text );
}

const char* EdjePart::text() const
{
    return edje_object_part_text_get( _parent->obj(), _partname );
}


void EdjePart::swallow( EvasObject* object )
{
    edje_object_part_swallow( _parent->obj(), _partname, object->obj() );
}

void EdjePart::unswallow( EvasObject* object )
{
    edje_object_part_unswallow( _parent->obj(), object->obj() );
}

CountedPtr <EvasObject> EdjePart::swallow()
{
    Evas_Object *eo = edje_object_part_swallow_get( _parent->obj(), _partname );
  
    EvasObject *ret_o = EvasObject::wrap (eo);
  
    return CountedPtr <EvasObject> (ret_o);
}

/*const EvasObject* EdjePart::getObject ( const char* name )
{
  return EvasObject::objectLink( edje_object_part_object_get( static_cast <const Evas_Object*> (_parent->obj()), name ) );
}*/

} // end namespace efl
