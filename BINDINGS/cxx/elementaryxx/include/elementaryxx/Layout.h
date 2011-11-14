#ifndef ELMXX_LAYOUT_H
#define ELMXX_LAYOUT_H

/* STL */
#include <string>

/* EFL */
#include <edjexx/Edjexx.h>
#include "Object.h"

namespace Elmxx {

class Layout : public Object
{
public:
  static Layout *factory (Evasxx::Object &parent);
  
  bool setFile (const std::string &file);
  
  bool setFile (const std::string &file, const std::string &group);

  void setContent (const std::string &swallow, const Evasxx::Object &content);

  Eflxx::CountedPtr <Edjexx::Object> getEdje ();
  

private:
  Layout (); // forbid standard constructor
  Layout (const Layout&); // forbid copy constructor
  Layout (Evasxx::Object &parent); // private construction -> use factory ()
  ~Layout (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_LAYOUT_H
