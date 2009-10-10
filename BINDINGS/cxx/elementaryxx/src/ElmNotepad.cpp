#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmNotepad.h"

using namespace std;

namespace efl {

ElmNotepad::ElmNotepad (EvasObject &parent)
{
  o = elm_notepad_add (parent.obj ());
  
  elmInit ();
}

ElmNotepad::~ElmNotepad () {}

ElmNotepad *ElmNotepad::factory (EvasObject &parent)
{
  return new ElmNotepad (parent);
}

void ElmNotepad::setFile (const std::string &file, Elm_Text_Format format)
{
  elm_notepad_file_set (o, file.c_str (), format);
}

} // end namespace efl
