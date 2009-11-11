#include "../include/edjexx/EdjeExceptions.h"

#include <sstream> 

using namespace std;

namespace efl {

const char* EdjeLoadException::what () const throw ()
{
  static std::ostringstream os;
  os << "EdjeObject file load error: filename: '"
     << mFilename
     << "' groupname: '"
     << mGroupname
     << "' errorcode: "
     << mErrorcode
     << ".";
  return static_cast <const char*> (os.str ().c_str ());
}

const char* EdjePartNotExistingException::what () const throw ()
{
  std::ostringstream os;
  os << "EdjeObject loaded with not existing part '";
  os << txt.c_str (); // TODO: why is c_str() needed! -> rework Exception concept!
  os << "'.";
  return static_cast <const char*> (os.str ().c_str ());
}

} // end namespace efl
