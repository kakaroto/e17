#ifndef ELMXX_ANCHORVIEW_H
#define ELMXX_ANCHORVIEW_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*!
 * smart callbacks called:
 * "anchor,clicked" - achor called was clicked | event_info = Elm_Entry_Anchorview_Info
 */
class Anchorview : public Object
{
public:
  static Anchorview *factory (Evasxx::Object &parent);

private:
  Anchorview (); // forbid standard constructor
  Anchorview (const Anchorview&); // forbid copy constructor
  Anchorview (Evasxx::Object &parent); // private construction -> use factory ()
  ~Anchorview (); // forbid direct delete -> use ElmWidget::destroy()
  
  void setText (const std::string &text);
  void setHoverParent (Evasxx::Object &parent);
  void setHoverStyle (const std::string &style);
  void endHover ();    
};

} // end namespace Elmxx

#endif // ELMXX_ANCHORVIEW_H
