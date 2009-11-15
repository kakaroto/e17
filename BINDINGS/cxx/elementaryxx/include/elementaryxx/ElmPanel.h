#ifndef ELM_PANEL_H
#define ELM_PANEL_H

/* STL */
#include <string>

/* ELFxx */
#include "ElmObject.h"

namespace efl {

class ElmPanel : public ElmObject
{
public:
  static ElmPanel *factory (EvasObject &parent);

private:
  ElmPanel (); // forbid standard constructor
  ElmPanel (const ElmPanel&); // forbid copy constructor
  ElmPanel (EvasObject &parent); // private construction -> use factory ()
  ~ElmPanel (); // forbid direct delete -> use ElmWidget::destroy()
  
  void setOrientation (Elm_Panel_Orient orient);
  void setContent (EvasObject &content);
};

} // end namespace efl

#endif // ELM_PANEL_H
