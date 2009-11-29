#ifndef ELMXX_PANEL_H
#define ELMXX_PANEL_H

/* STL */
#include <string>

/* ELFxx */
#include "Object.h"

namespace Elmxx {

class Panel : public Object
{
public:
  static Panel *factory (Evasxx::Object &parent);

private:
  Panel (); // forbid standard constructor
  Panel (const Panel&); // forbid copy constructor
  Panel (Evasxx::Object &parent); // private construction -> use factory ()
  ~Panel (); // forbid direct delete -> use ElmWidget::destroy()
  
public:
  void setOrientation (Elm_Panel_Orient orient);
  void setContent (Evasxx::Object &content);
};

} // end namespace Elmxx

#endif // ELMXX_PANEL_H
