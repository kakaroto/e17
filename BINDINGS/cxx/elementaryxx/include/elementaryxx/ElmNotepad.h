#ifndef ELM_NOTEPAD_H
#define ELM_NOTEPAD_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

class ElmNotepad : public ElmObject
{
public:
  static ElmNotepad *factory (EvasObject &parent);

  void setFile (const std::string &file, Elm_Text_Format format);
  
private:
  ElmNotepad (); // forbid standard constructor
  ElmNotepad (const ElmNotepad&); // forbid copy constructor
  ElmNotepad (EvasObject &parent); // private construction -> use factory ()
  ~ElmNotepad (); // forbid direct delete -> use ElmObject::destroy()
};

} // end namespace efl

#endif // ELM_NOTEPAD_H
