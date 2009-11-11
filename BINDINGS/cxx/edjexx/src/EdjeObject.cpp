#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/EdjePart.h"
#include "../include/edjexx/EdjeObject.h"
#include "../include/edjexx/EdjeBase.h"
#include "../include/edjexx/EdjeExceptions.h"

/* EFL */
#include <Edje.h>

using namespace std;

namespace efl {

EdjeObject::EdjeObject( EvasCanvas &canvas )
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

EdjeObject::EdjeObject( EvasCanvas &canvas, const std::string &filename, const std::string &groupname )
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

EdjeObject::EdjeObject( EvasCanvas &canvas, const Point &pos, const std::string &filename, const std::string &groupname )
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

EdjeObject::EdjeObject( Evas_Object* object)
{
  o = object;
  mFree = false;
}

EdjeObject::~EdjeObject()
{
  //FIXME: Remove callbacks?
  evas_object_del( o );
  
  // Shut down the Edje library (if internal count is zero)
  EdjeBase::shutdown ();
}

void EdjeObject::setFile( const std::string &filename, const std::string &groupname )
{
  if (!edje_object_file_set( o, filename.c_str (), groupname.c_str () ))
  {
    throw EdjeLoadException (filename, groupname, edje_object_load_error_get(o)); 
  }
}

Size EdjeObject::getMinimalSize() const
{
  int w, h;
  edje_object_size_min_get( o, &w, &h );
  Dout( dc::notice, "size min get seems to be " << w << " x " << h );
  return Size( w, h );
}

Size EdjeObject::getMaximumSize() const
{
  int w, h;
  edje_object_size_max_get( o, &w, &h );
  Dout( dc::notice, "size max get seems to be " << w << " x " << h );
  return Size( w, h );
}

bool EdjeObject::hasPart( const std::string &partname ) const
{
  return edje_object_part_exists( o, partname.c_str () );
}

CountedPtr <EdjePart> EdjeObject::operator[]( const std::string &partname )
{
  return getPart( partname );
}

CountedPtr <EdjePart> EdjeObject::getPart( const std::string &partname )
{
  if ( hasPart( partname ) )
  {
    EdjePart* ep = new EdjePart( this, partname );
    return CountedPtr <EdjePart> (ep);
  }
  throw EdjePartNotExistingException (partname);
}

void EdjeObject::connect( const std::string &emission, const std::string &source, const EdjeSignalSlot& slot )
{
  EdjeSignalSignal* signal = new EdjeSignalSignal();
  AllocTag( signal, emission );
  signal->connect( slot );
  edje_object_signal_callback_add( o, emission.c_str (), source.c_str (), &_edje_signal_handler_callback, static_cast<void*>( signal ) );
}

void EdjeObject::emit( const std::string &emission, const std::string &source )
{
  edje_object_signal_emit( o, emission.c_str (), source.c_str () );
}

void EdjeObject::_edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg )
{
  Dout( dc::notice, "EdjeObject::_edje_message_handler_callback()" );
  //EdjeObject* instance = reinterpret_cast<EdjeObject*>( data );
}

void EdjeObject::_edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source )
{
  Dout( dc::notice, "EdjeObject::_edje_signal_handler_callback( " << (emission ? emission:"<null>") << ", " << (source ? source:"<null>") << " ) " );
  EdjeSignalSignal* signal = reinterpret_cast<EdjeSignalSignal*>( data );
  if ( signal ) signal->emit( emission, source );
  else Dout( dc::warning, "EdjeObject::_edje_signal_handler_callback() - got callback without valid signal" );
}

EdjeObject* EdjeObject::wrap( Evas_Object* o )
{
  return new EdjeObject( o );
}

} // end namespace efl
