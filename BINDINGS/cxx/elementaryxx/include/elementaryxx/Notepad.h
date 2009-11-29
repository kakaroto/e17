#ifndef ELMXX_NOTEPAD_H
#define ELMXX_NOTEPAD_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Notepad : public Object
{
public:
  static Notepad *factory (Evasxx::Object &parent);

  void setFile (const std::string &file, Elm_Text_Format format);
  
private:
  Notepad (); // forbid standard constructor
  Notepad (const Notepad&); // forbid copy constructor
  Notepad (Evasxx::Object &parent); // private construction -> use factory ()
  ~Notepad (); // forbid direct delete -> use Object::destroy()
};

} // end namespace Elmxx

#endif // ELMXX_NOTEPAD_H
