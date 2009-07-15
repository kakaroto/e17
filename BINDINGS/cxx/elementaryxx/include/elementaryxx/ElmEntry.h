#ifndef ELM_ENTRY_H
#define ELM_ENTRY_H

/* STL */
#include <string>

/* EFL */
#include <Elementary.h>

/* ELFxx */
#include "ElmWidget.h"

namespace efl {

/*!
 * smart callbacks called:
 * "changed" - the text content changed
 * "selection,start" - the user started selecting text
 * "selection,changed" - the user modified the selection size/location
 * "selection,cleared" - the user cleared the selection
 * "selection,paste" - the user rrequested a paste of text
 * "selection,copy" - the user copied the text
 * "selection,cut" - the user cut the text
 * "cursor,changed" - the cursor changed position
 * "anchor,clicked" - achor called was clicked | event_info = Elm_Entry_Anchor_Info
 * "activated" - when the enter key is pressed (useful for single line)
 */   
class ElmEntry : public ElmWidget
{
public:
  virtual ~ElmEntry ();
  
  static ElmEntry *factory (EvasObject &parent);

  void setSingleLine (bool singleLine);

  void setPassword (bool password);
  
  void setText (const std::string &entry);
  
  const std::string getText () const;
  
  const std::string getSelection () const;
  
  void insertText (const std::string &entry);
  
  void setLineWrap (bool wrap);
  
  void setEditable (bool editable);
  
  void selectNone ();
  
  void selectAll ();
  
  static const std::string markupToUtf8 (const std::string &str);
  
  static const std::string utf8ToMarkup (const std::string &str);
  
private:
  ElmEntry (); // forbid standard constructor
  ElmEntry (const ElmEntry&); // forbid copy constructor
  ElmEntry (EvasObject &parent); // private construction -> use factory ()
};

} // end namespace efl

#endif // ELM_ENTRY_H
