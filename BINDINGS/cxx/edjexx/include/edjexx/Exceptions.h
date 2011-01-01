#ifndef EDJEXX_EXCEPTIONS_H
#define EDJEXX_EXCEPTIONS_H

#include <string>

namespace Edjexx {

class LoadException : public std::exception
{
public:
  LoadException (const std::string &filename, const std::string &groupname, int errorcode) 
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

class PartNotExistingException : public std::exception
{
public:
  PartNotExistingException (const std::string &partname) : txt (partname) {}
  const char* what () const throw ();

private:
  const std::string &txt;
};

class ExternalNotExistingException : public std::exception
{
public:
  ExternalNotExistingException (const std::string &partname) : txt (partname) {}
  const char* what () const throw ();

private:
  const std::string &txt;
};

class SwallowNotExistingException : public std::exception
{
public:
  SwallowNotExistingException (const std::string &partname) : txt (partname) {}
  const char* what () const throw ();

private:
  const std::string &txt;
};

} // end namespace Edjexx

#endif // EDJEXX_EXCEPTIONS_H
