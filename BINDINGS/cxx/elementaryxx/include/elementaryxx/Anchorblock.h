#ifndef ELMXX_ANCHORBLOCK_H
#define ELMXX_ANCHORBLOCK_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

/*! 
 * smart callbacks called:
 * "anchor,clicked" - achor called was clicked | event_info = Elm_Entry_Anchorblock_Info
 */
class Anchorblock : public Object
{
public:
  static Anchorblock *factory (Evasxx::Object &parent);

private:
  Anchorblock (); // forbid standard constructor
  Anchorblock (const Anchorblock&); // forbid copy constructor
  Anchorblock (Evasxx::Object &parent); // private construction -> use factory ()
  ~Anchorblock (); // forbid direct delete -> use ElmWidget::destroy()
  
  void setText (const std::string &text);
  void setHoverParent (Evasxx::Object &parent);
  void setHoverStyle (const std::string &style);
  void endHover ();   
};

} // end namespace Elmxx

#endif // ELMXX_ANCHORBLOCK_H
