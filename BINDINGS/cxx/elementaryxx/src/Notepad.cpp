#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Notepad.h"

using namespace std;

namespace Elmxx {

Notepad::Notepad (Evasxx::Object &parent)
{
  o = elm_notepad_add (parent.obj ());
  
  elmInit ();
}

Notepad::~Notepad () {}

Notepad *Notepad::factory (Evasxx::Object &parent)
{
  return new Notepad (parent);
}

void Notepad::setFile (const std::string &file, Elm_Text_Format format)
{
  elm_notepad_file_set (o, file.c_str (), format);
}

} // end namespace Elmxx
