#include "eflpp_edje.h"
#include <eflpp_debug_internal.h>

#include <iostream>
#include <cassert>
#include <string>

using namespace std;

namespace efl {

const char* PartNotExistingException::what () const throw ()
{
  static std::string s;
  s = "EvasEdje loaded with not existing part '";
  s += txt;
  s += "'.";
  return static_cast <const char*> (s.c_str ());
}
  
//===============================================================================================
// EvasEdje
//===============================================================================================
EvasEdje::EvasEdje( EvasCanvas* canvas, const char* name )
{
    o = edje_object_add( canvas->obj() );
    init( name ? name : "edje" );
  
#ifdef CWDEBUG
    edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
    edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
}

EvasEdje::EvasEdje( const char* filename, const char* groupname, EvasCanvas* canvas, const char* name )
{
    o = edje_object_add( canvas->obj() );
    init( name ? name : groupname );
  
#ifdef CWDEBUG
    edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
    edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
    setFile( filename, groupname );
}

EvasEdje::EvasEdje( int x, int y, const char* filename, const char* groupname, EvasCanvas* canvas, const char* name )
{
    o = edje_object_add( canvas->obj() );
    init( name ? name : groupname );
  
#ifdef CWDEBUG
    edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
    edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
    setFile( filename, groupname );
    move( x, y );
}

EvasEdje::EvasEdje( Evas_Object* object)
{
  o = object;
  mManaged = false;
}

bool EvasEdje::setFile( const char* filename, const char* groupname )
{
    edje_object_file_set( o, filename, groupname );
    int errorcode = edje_object_load_error_get(o);
    Dout( dc::notice, "EvasEdje::file_set" << " path=" << filename << " group=" << groupname << "(" << EVAS_LOAD_ERROR[errorcode] << ")" );
    if ( errorcode ) cerr << "ERROR: EvasEdje::setFile( '" << filename << "|" << groupname << ") = " << EVAS_LOAD_ERROR[errorcode] << endl;
    return ( errorcode == 0 );
}

Size EvasEdje::minimalSize() const
{
    int w, h;
    edje_object_size_min_get( o, &w, &h );
    Dout( dc::notice, "size min get seems to be " << w << " x " << h );
    return Size( w, h );
}

Size EvasEdje::maximumSize() const
{
    int w, h;
    edje_object_size_max_get( o, &w, &h );
    Dout( dc::notice, "size max get seems to be " << w << " x " << h );
    return Size( w, h );
}   

EvasEdje::~EvasEdje()
{
    //FIXME: Remove callbacks?
}

bool EvasEdje::hasPart( const char* partname ) const
{
    return edje_object_part_exists( o, partname );
}

CountedPtr <EdjePart> EvasEdje::operator[]( const char* partname )
{
    return part( partname );
}

CountedPtr <EdjePart> EvasEdje::part( const char* partname )
{
    if ( hasPart( partname ) )
    {
        EdjePart* ep = new EdjePart( this, partname );
        return CountedPtr <EdjePart> (ep);
    }
    throw PartNotExistingException (partname);
}

void EvasEdje::connect( const char* emission, const char* source, const EdjeSignalSlot& slot )
{
    EdjeSignalSignal* signal = new EdjeSignalSignal();
    AllocTag( signal, emission );
    signal->connect( slot );
    edje_object_signal_callback_add( o, emission, source, &_edje_signal_handler_callback, static_cast<void*>( signal ) );
}

void EvasEdje::emit( const char* emission, const char* source )
{
    edje_object_signal_emit( o, emission, source );
}

void EvasEdje::_edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg )
{
    Dout( dc::notice, "EvasEdje::_edje_message_handler_callback()" );
    //EvasEdje* instance = reinterpret_cast<EvasEdje*>( data );
}

void EvasEdje::_edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source )
{
    Dout( dc::notice, "EvasEdje::_edje_signal_handler_callback( " << (emission ? emission:"<null>") << ", " << (source ? source:"<null>") << " ) " );
    EdjeSignalSignal* signal = reinterpret_cast<EdjeSignalSignal*>( data );
    if ( signal ) signal->emit( emission, source );
    else Dout( dc::warning, "EvasEdje::_edje_signal_handler_callback() - got callback without valid signal" );
}

EvasEdje* EvasEdje::wrap( Evas_Object* o )
{
    return new EvasEdje( o );
}

//===============================================================================================
// EdjePart
//===============================================================================================

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
    const char *t = evas_object_type_get( eo );
    EvasObject *ret_o = NULL;
  
    if( !strcmp( t,"edje" ) )
    {
        ret_o = EvasEdje::wrap (eo);
    }
    // TODO: support other types
  
    return CountedPtr <EvasObject> (ret_o);
}

/*const EvasObject* EdjePart::getObject ( const char* name )
{
  return EvasObject::objectLink( edje_object_part_object_get( static_cast <const Evas_Object*> (_parent->obj()), name ) );
}*/


} // end namespace efl

