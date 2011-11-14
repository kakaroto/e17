#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/Part.h"
#include "../include/edjexx/Object.h"
#include "../include/edjexx/Base.h"
#include "../include/edjexx/Exceptions.h"

/* EFL */
#include <Edje.h>

using namespace std;

namespace Edjexx {

Object::Object( Evasxx::Canvas &canvas ) :
  mFree (true)
{
  // Initialize the Edje library
  Base::init ();
  
  o = edje_object_add( canvas.obj() );
  init();

#ifdef CWDEBUG
  edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
  edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
}

Object::Object( Evasxx::Canvas &canvas, const std::string &filename, const std::string &groupname ) :
  mFree (true)
{
  // Initialize the Edje library
  Base::init ();
  
  o = edje_object_add( canvas.obj() );
  init();

#ifdef CWDEBUG
  edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
  edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
  setFile( filename, groupname );
}

Object::Object( Evasxx::Canvas &canvas, const Eflxx::Point &pos, const std::string &filename, const std::string &groupname ) :
  mFree (true)
{
  // Initialize the Edje library
  Base::init ();
  
  o = edje_object_add( canvas.obj() );
  init();

#ifdef CWDEBUG
  edje_object_message_handler_set( o, &_edje_message_handler_callback, 0 );
  edje_object_signal_callback_add( o, "*", "*", &_edje_signal_handler_callback, 0 );
#endif
  setFile( filename, groupname );
  move (pos);
}

Object::Object( Evas_Object* object) :
  mFree (false)
{
  o = object;
}

Object::~Object()
{
  if (mFree)
  {
    cout << "delete Object" << endl;
	  disconnectAll ();
    evas_object_del( o );
  }
}

void Object::deleteAllParts ()
{
	Eflxx::delete_stl_container (mPartList);
}

void Object::setFile( const std::string &filename, const std::string &groupname )
{
  if (!edje_object_file_set( o, filename.c_str (), groupname.c_str () ))
  {
    throw LoadException (filename, groupname, edje_object_load_error_get(o)); 
  }
}

Eflxx::Size Object::getMinimalSize() const
{
  int w, h;
  edje_object_size_min_get( o, &w, &h );
  Dout( dc::notice, "size min get seems to be " << w << " x " << h );
  return Eflxx::Size( w, h );
}

Eflxx::Size Object::getMaximumSize() const
{
  int w, h;
  edje_object_size_max_get( o, &w, &h );
  Dout( dc::notice, "size max get seems to be " << w << " x " << h );
  return Eflxx::Size( w, h );
}

bool Object::hasPart (const std::string &partname) const
{
  return edje_object_part_exists( o, partname.c_str () );
}

Part &Object::operator[]( const std::string &partname )
{
  return getPart( partname );
}

Part &Object::getPart (const std::string &partname)
{
  if (hasPart (partname))
  {
    Part *p = NULL;
    map <std::string, Part*>::iterator partFound = mPartList.find (partname);

    if (partFound != mPartList.end ()) // found
    {
      p = partFound->second;
    }
    else // not found
    {
      p = new Part (this, partname);
      mPartList[partname] = p;
    }

    return *p;
  }

  throw PartNotExistingException (partname);
}

void Object::connect (const std::string &emission, const std::string &source, const SignalSlot& slot)
{
  SignalSignal* signal = new SignalSignal();
	mSignalList[std::pair <std::string, std::string> (emission, source)] = signal;
  AllocTag( signal, emission );
  signal->connect( slot );
  edje_object_signal_callback_add( o, emission.c_str (), source.c_str (), &_edje_signal_handler_callback, static_cast<void*>( signal ) );
}

void Object::disconnect (const std::string &emission, const std::string &source)
{
	SignalSignal *signal = mSignalList[std::pair <std::string, std::string> (emission, source)];
  edje_object_signal_callback_del (o, emission.c_str (), source.c_str (), _edje_signal_handler_callback);
	delete signal;
}

void Object::disconnectAll ()
{
	for (std::map <std::pair <std::string, std::string>, SignalSignal*>::iterator s_it = mSignalList.begin ();
	     s_it != mSignalList.end ();
	     ++s_it)
	{
		std::pair <std::string, std::string> emission_source = (*s_it).first;
		SignalSignal *signal = (*s_it).second;

		edje_object_signal_callback_del (o, emission_source.first.c_str (), emission_source.second.c_str (),
		                                 _edje_signal_handler_callback);
		delete signal;
	}
}

void Object::emit( const std::string &emission, const std::string &source )
{
  edje_object_signal_emit( o, emission.c_str (), source.c_str () );
}

void Object::_edje_message_handler_callback( void* data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg )
{
  Dout( dc::notice, "Object::_edje_message_handler_callback()" );
  //Object* instance = reinterpret_cast<Object*>( data );
}

void Object::_edje_signal_handler_callback( void *data, Evas_Object *obj, const char *emission, const char *source )
{
  Dout( dc::notice, "Object::_edje_signal_handler_callback( " << (emission ? emission:"<null>") << ", " << (source ? source:"<null>") << " ) " );
  SignalSignal* signal = reinterpret_cast<SignalSignal*>( data );
  if ( signal ) signal->emit( emission, source );
  else Dout( dc::warning, "Object::_edje_signal_handler_callback() - got callback without valid signal" );
}

Object* Object::wrap( Evas_Object* o )
{
  return new Edjexx::Object( o );
}

} // end namespace Edjexx
