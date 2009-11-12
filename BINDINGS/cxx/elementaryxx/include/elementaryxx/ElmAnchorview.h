#ifndef ELM_ANCHORVIEW_H
#define ELM_ANCHORVIEW_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

/*!
 * smart callbacks called:
 * "anchor,clicked" - achor called was clicked | event_info = Elm_Entry_Anchorview_Info
 */
class ElmAnchorview : public ElmObject
{
public:
  static ElmAnchorview *factory (EvasObject &parent);

private:
  ElmAnchorview (); // forbid standard constructor
  ElmAnchorview (const ElmAnchorview&); // forbid copy constructor
  ElmAnchorview (EvasObject &parent); // private construction -> use factory ()
  ~ElmAnchorview (); // forbid direct delete -> use ElmWidget::destroy()
  
  void setText (const std::string &text);
  void setHoverParent (EvasObject &parent);
  void setHoverStyle (const std::string &style);
  void endHover ();    
};

} // end namespace efl

#endif // ELM_ANCHORVIEW_H
