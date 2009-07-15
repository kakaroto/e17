#ifndef EDJEEXCEPTIONS_H
#define EDJEEXCEPTIONS_H

#include <string>

namespace efl {

class EdjeLoadException : public std::exception
{
public:
  EdjeLoadException (const std::string &filename, const std::string &groupname, int errorcode) 
    : mFilename (filename),
      mGroupname (groupname),
      mErrorcode (errorcode)
  {}
  
  const char* what () const throw ();

private:
  const std::string &mFilename;
  const std::string &mGroupname;
  int &mErrorcode;
};

class EdjePartNotExistingException : public std::exception
{
public:
  EdjePartNotExistingException (const std::string &partname) : txt (partname) {}
  const char* what () const throw ();

private:
  const std::string &txt;
};

} // end namespace efl

#endif // EDJEEXCEPTIONS_H
