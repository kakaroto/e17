#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/EdjePart.h"
#include "../include/edjexx/EvasEdje.h"
#include "../include/edjexx/EdjeBase.h"
#include "../include/edjexx/EdjeExceptions.h"

/* EFL */
#include <Edje.h>

using namespace std;

namespace efl {

EvasEdje::EvasEdje( EvasCanvas &canvas )
{
  // Initialize the Edje library
  EdjeBase::init ();
  
  o = edje_object_add( canvas.obj() );
  init();

#ifdef CWDEBUG
  edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
  edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
}

EvasEdje::EvasEdje( EvasCanvas &canvas, const std::string &filename, const std::string &groupname )
{
  // Initialize the Edje library
  EdjeBase::init ();
  
  o = edje_object_add( canvas.obj() );
  init();

#ifdef CWDEBUG
  edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
  edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
  setFile( filename, groupname );
}

EvasEdje::EvasEdje( EvasCanvas &canvas, const Point &pos, const std::string &filename, const std::string &groupname )
{
  // Initialize the Edje library
  EdjeBase::init ();
  
  o = edje_object_add( canvas.obj() );
  init();

#ifdef CWDEBUG
  edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
  edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
  setFile( filename, groupname );
  move (pos);
}

EvasEdje::EvasEdje( Evas_Object* object)
{
  o = object;
  mFree = false;
}

EvasEdje::~EvasEdje()
{
  //FIXME: Remove callbacks?
  evas_object_del( o );
  
  // Shut down the Edje library (if internal count is zero)
  EdjeBase::shutdown ();
}

void EvasEdje::setFile( const std::string &filename, const std::string &groupname )
{
  if (!edje_object_file_set( o, filename.c_str (), groupname.c_str () ))
  {
    throw EdjeLoadException (filename, groupname, edje_object_load_error_get(o)); 
  }
}

Size EvasEdje::getMinimalSize() const
{
  int w, h;
  edje_object_size_min_get( o, &w, &h );
  Dout( dc::notice, "size min get seems to be " << w << " x " << h );
  return Size( w, h );
}

Size EvasEdje::getMaximumSize() const
{
  int w, h;
  edje_object_size_max_get( o, &w, &h );
  Dout( dc::notice, "size max get seems to be " << w << " x " << h );
  return Size( w, h );
}

bool EvasEdje::hasPart( const std::string &partname ) const
{
  return edje_object_part_exists( o, partname.c_str () );
}

CountedPtr <EdjePart> EvasEdje::operator[]( const std::string &partname )
{
  return getPart( partname );
}

CountedPtr <EdjePart> EvasEdje::getPart( const std::string &partname )
{
  if ( hasPart( partname ) )
  {
    EdjePart* ep = new EdjePart( this, partname );
    return CountedPtr <EdjePart> (ep);
  }
  throw EdjePartNotExistingException (partname);
}

void EvasEdje::connect( const std::string &emission, const std::string &source, const EdjeSignalSlot& slot )
{
  EdjeSignalSignal* signal = new EdjeSignalSignal();
  AllocTag( signal, emission );
  signal->connect( slot );
  edje_object_signal_callback_add( o, emission.c_str (), source.c_str (), &_edje_signal_handler_callback, static_cast<void*>( signal ) );
}

void EvasEdje::emit( const std::string &emission, const std::string &source )
{
  edje_object_signal_emit( o, emission.c_str (), source.c_str () );
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
