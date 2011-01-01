#include "../include/edjexx/Exceptions.h"

#include <sstream> 

using namespace std;

namespace Edjexx {

const char* LoadException::what () const throw ()
{
  static std::ostringstream os;
  os << "File load error: filename: '"
     << mFilename
     << "' groupname: '"
     << mGroupname
     << "' errorcode: "
     << mErrorcode
     << ".";
  return static_cast <const char*> (os.str ().c_str ());
}

const char* PartNotExistingException::what () const throw ()
{
  std::ostringstream os;
  os << "Edje part not existing: '";
  os << txt.c_str (); // TODO: why is c_str() needed! -> rework Exception concept!
  os << "'.";
  return static_cast <const char*> (os.str ().c_str ());
}

const char* ExternalNotExistingException::what () const throw ()
{
  std::ostringstream os;
  os << "Edje external object not existing:  '";
  os << txt.c_str (); // TODO: why is c_str() needed! -> rework Exception concept!
  os << "'.";
  return static_cast <const char*> (os.str ().c_str ());
}

const char* SwallowNotExistingException::what () const throw ()
{
  std::ostringstream os;
  os << "Edje swallow object not existing:  '";
  os << txt.c_str (); // TODO: why is c_str() needed! -> rework Exception concept!
  os << "'.";
  return static_cast <const char*> (os.str ().c_str ());
}

} // end namespace Edjexx
