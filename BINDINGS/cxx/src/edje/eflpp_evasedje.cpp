#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_evasedje.h"
#include "eflpp_edjepart.h"

/* EFL */
#include <Edje.h>

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
    
    // TODO: Exception handling?
  
    //Dout( dc::notice, "EvasEdje::file_set" << " path=" << filename << " group=" << groupname << "(" << EVAS_LOAD_ERROR[errorcode] << ")" );
    //if ( errorcode ) cerr << "ERROR: EvasEdje::setFile( '" << filename << "|" << groupname << ") = " << EVAS_LOAD_ERROR[errorcode] << endl;
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

} // end namespace efl
