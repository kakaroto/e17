#ifndef ELM_ANCHORBLOCK_H
#define ELM_ANCHORBLOCK_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*! 
 * smart callbacks called:
 * "anchor,clicked" - achor called was clicked | event_info = Elm_Entry_Anchorblock_Info
 */
class ElmAnchorblock : public ElmObject
{
public:
  static ElmAnchorblock *factory (EvasObject &parent);

private:
  ElmAnchorblock (); // forbid standard constructor
  ElmAnchorblock (const ElmAnchorblock&); // forbid copy constructor
  ElmAnchorblock (EvasObject &parent); // private construction -> use factory ()
  ~ElmAnchorblock (); // forbid direct delete -> use ElmWidget::destroy()
  
  void setText (const std::string &text);
  void setHoverParent (EvasObject &parent);
  void setHoverStyle (const std::string &style);
  void endHover ();   

};

} // end namespace efl

#endif // ELM_ANCHORBLOCK_H
