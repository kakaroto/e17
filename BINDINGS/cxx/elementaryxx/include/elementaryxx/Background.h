#ifndef ELMXX_BACKGROUND_H
#define ELMXX_BACKGROUND_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Background : public Object
{
public:
  static Background *factory (Evasxx::Object &parent);
  
  void setFile (const std::string &file);
  
  /*!
   * FIXME: do bool return here? 
   * @see Image::setFile
   */
  void setFile (const std::string &file, const std::string &group);
  
private:
  Background (); // forbid standard constructor
  Background (const Background&); // forbid copy constructor
  Background (Evasxx::Object &parent); // private construction -> use factory ()
  ~Background (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_BACKGROUND_H
